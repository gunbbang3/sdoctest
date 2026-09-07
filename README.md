# sdoctest

Bazel 위에서 **StrictDoc(`sdoc`)**와 **Sphinx + sphinx-needs**
([Eclipse S-CORE](https://github.com/eclipse-score)의 `docs-as-code`
저장소가 실제로 쓰는 도구)로 **같은 요구사항/아키텍처/구현/테스트**를 각각
작성해두고, 두 도구의 문서-추적성(traceability) 메커니즘을 **back-to-back으로
빌드·검증·비교**할 수 있게 만든 저장소입니다.

예제 대상은 7개의 C++/C 모듈로 구성된 작은 신호처리 파이프라인이며, 그중
하나(`speed_controller`)는 손으로 작성한 코드가 아니라 **Simulink
모델 기반 설계(Model-Based Design, MBD)로 생성된 코드**를 흉내 낸
모듈입니다. **각 SW 모듈은 자신의 구현·테스트·(두 형식의) 문서를 모두
자기 폴더 하위에 갖는 자기완결적 구조**(`modules/<module>/{src,tests,docs,docs_needs}`)
로 배치되어 있습니다.

## 예제 시스템 (7개 모듈)

```
SensorInput ──▶ MovingAverageFilter ──▶ Controller ──▶ RangeDiagnostics
 (ARCH-1)             (ARCH-2)          (ARCH-6)          (ARCH-4)
                                            │
                     MathUtils ◀───────────┤
                      (ARCH-3)              │
                                            ▼
                                          Logger
                                          (ARCH-5)

SpeedController (ARCH-7, Simulink MBD 생성 코드) ── 독립 서브시스템
```

| 모듈 | 클래스/함수 | 요구사항 | 모듈 폴더 | 작성 방식 |
|---|---|---|---|---|
| SensorInput | `SensorInput` | REQ-1 | `modules/sensor_input/` | 수기 작성 |
| SignalFilter | `MovingAverageFilter` | REQ-2 | `modules/signal_filter/` | 수기 작성 |
| MathUtils | `Add`, `Clamp` | REQ-3 | `modules/math_utils/` | 수기 작성 |
| Diagnostics | `RangeDiagnostics` | REQ-4 | `modules/diagnostics/` | 수기 작성 |
| Logger | `Logger` | REQ-5 | `modules/logger/` | 수기 작성 |
| Controller | `Controller` (위 5개를 통합) | REQ-6 | `modules/controller/` | 수기 작성 |
| SpeedController | `speed_controller_step()` 등 (PI 제어) | REQ-7 | `modules/speed_controller/` | **Simulink MBD 생성 코드** |

`Controller`는 한 사이클마다 `SensorInput`에서 원시값을 읽고,
`MovingAverageFilter`로 평활화하고, `MathUtils::Clamp`로 클램핑한 뒤,
`RangeDiagnostics`로 범위를 검사하고, 결과를 `Logger`에 기록합니다 —
6개 모듈이 실제로 서로 의존하는 최소한의 통합 예제입니다. 단위테스트
프레임워크로는 **GoogleTest**(`bazel_dep(name = "googletest")`, 각
모듈의 `cc_test`가 `@googletest//:gtest_main`에 링크)를 사용합니다.

### SpeedController — Simulink MBD 생성 코드 예제

`modules/speed_controller/src/`는 Simulink Embedded Coder가 실제로
만들어내는 생성 코드의 구조(배너 주석, `ExtU_`/`ExtY_`/`DW_`/`P_`
구조체, `<model>_initialize`/`_step`/`_terminate` 진입점, `DO NOT EDIT`
경고, `'<Root>/블록이름'` 형태의 블록 경로 주석)를 그대로 흉내 내어
손으로 작성한 것입니다. **이 저장소에는 MATLAB/Simulink 툴체인이 없어
실제로 생성된 코드가 아니며**, `modules/speed_controller/models/speed_controller.md`에
그 사실과 원본 블록 다이어그램(비례-적분 제어기 + 포화)을 설명해
두었습니다.

이 모듈이 중요한 이유는 **생성 코드는 손으로 건드리면 안 된다**는
제약 때문입니다: 모델을 다시 빌드하면 생성물이 통째로 새로 써지므로,
다른 6개 모듈처럼 소스 파일에 `@relation(...)` 주석을 직접 추가하는
방식은 쓸 수 없습니다. 그래서 `ARCH-7`은:

- **StrictDoc**: `modules/speed_controller/docs/architecture.sdoc`에서
  `TYPE: File`로 문서 쪽에서 생성 파일 경로
  (`modules/speed_controller/src/speed_controller.c`/`.h`)를 직접
  선언합니다 — StrictDoc이 그 경로의 실제 존재 여부까지 검증합니다.
- **sphinx-needs**: 동일한 패턴을 표현할 문서-쪽 "파일 존재 검증" 기능이
  없으므로, `modules/speed_controller/docs_needs/implementation.rst`의
  `IMPL-7`에는 그냥 텍스트로 경로를 적어 둘 뿐입니다 (다른 모듈의
  `IMPL-N`과 동일한 한계).
- 손으로 작성한 `modules/speed_controller/tests/speed_controller_test.cpp`만은
  다른 테스트들과 동일하게 `@relation(ARCH-7, scope=file, role=Test)`
  소스 마커를 씁니다 (테스트 코드는 생성물이 아니라 사람이 관리하는
  코드이기 때문입니다).

## 구성

```
MODULE.bazel                Bzlmod 설정: rules_python + pip.parse (strictdoc, sphinx, sphinx-needs) +
                             bazel_dep(googletest) (단위테스트 프레임워크).
                             C++ 툴체인 자체는 Bazel 내장 cc_library/cc_test를 그대로 사용 (별도 bazel_dep 불필요)
requirements.txt            두 문서 툴체인의 전이 의존성을 함께 고정한 pip 락 파일
strictdoc_config.py         StrictDoc 프로젝트 설정: include_doc_paths/include_source_paths가
                             모두 "/modules/" 하나만 가리키며, 그 아래를 재귀적으로 훑습니다
BUILD.bazel                 //:strictdoc, //:sphinx-build (실행형) +
                             //:strictdoc_traceability_test, //:sphinx_needs_traceability_test,
                             //:traceability_tests (문서 검증형) +
                             7개 모듈의 cc_library/cc_test + //:cpp_module_tests

modules/<name>/              모듈별 자기완결 폴더 (7개: sensor_input, signal_filter,
                              math_utils, diagnostics, logger, controller, speed_controller)
  src/                        구현 (.h/.cpp, speed_controller만 .c)
                               각 파일 상단에 # @relation(ARCH-N, scope=file) 로 StrictDoc과 연결
                               (단, speed_controller/src/*는 생성 코드라 마커 없음 -- 위 절 참고)
  tests/<name>_test.cpp       이 모듈만의 단위 테스트
                               # @relation(ARCH-N, scope=file, role=Test) 로 연결
  docs/                        --- 이 모듈의 StrictDoc 문서 ---
    requirements.sdoc          이 모듈의 REQ-N
    architecture.sdoc          이 모듈의 ARCH-N, RELATIONS: Parent -> REQ-N
                                 (controller는 ARCH-1~5에도 Parent로 연결,
                                  speed_controller는 TYPE: File로 생성 파일에 직접 연결)
  docs_needs/                  --- 이 모듈의 sphinx-needs 문서 (동일 내용) ---
    requirements.rst           REQ-N (.. req:: 지시어)
    architecture.rst           ARCH-N, :links: REQ-N (.. spec:: 지시어)
    implementation.rst         IMPL-N, :links: ARCH-N (.. impl:: 지시어)
    verification.rst           TEST-N, :links: IMPL-N (.. test:: 지시어)

modules/speed_controller/models/speed_controller.md   원본 Simulink 모델을 대신하는 설명 문서

modules/docs_needs/          모듈이 아닌, sphinx-needs 전용 최상위 색인 (아래 참고)
  conf.py                    extensions = ["sphinx_needs"], root_doc = "docs_needs/index"
  index.rst                  7개 모듈의 docs_needs/*.rst를 모아 하나의 toctree로 연결

tests/                        모듈에 속하지 않는, 저장소 전체를 다루는 공용 테스트 인프라
  strictdoc_traceability_test.py       StrictDoc 쪽 검증 py_test
  sphinx_needs_traceability_test.py    sphinx-needs 쪽 검증 py_test
                                        (7개 모듈의 cc_test 자체는 GoogleTest에 직접 링크하므로
                                         공용 헬퍼 파일은 따로 없습니다)

.github/workflows/traceability.yml   PR/main 푸시마다 C++ 모듈 테스트 + 두 문서 도구를 모두 검증하는 CI
```

**StrictDoc 쪽** 추적 체인은 모듈마다 `REQ-N → ARCH-N → modules/<name>/src/*
→ modules/<name>/tests/<name>_test.cpp`로, 모두 그 모듈 자신의 폴더
안에서 이어지며, `docs/architecture.sdoc`의 `RELATIONS`(문서 간 Parent
관계)와 소스 코드의 `@relation(...)` 주석(문서 ↔ 코드 관계) 두 메커니즘을
모두 사용합니다. `Controller`(ARCH-6, `modules/controller/docs/architecture.sdoc`)만
예외적으로 다른 5개 모듈 폴더에 있는 ARCH 요소들까지 `Parent` 관계로
가리켜 통합 의존성을 나타내고, `SpeedController`(ARCH-7)는 `TYPE: File`
관계로 생성 코드 파일 경로를 직접 검증합니다 (위 "SpeedController"
절 참고). StrictDoc은 `include_doc_paths`/`include_source_paths` 아래를
재귀적으로 훑어 프로젝트 전역에서 UID를 해석하므로, 이렇게 문서가
여러 모듈 폴더에 흩어져 있어도(그리고 서로 다른 폴더의 UID를 참조해도)
문제없이 동작합니다.

**sphinx-needs 쪽**은 같은 체인을 모듈마다 `REQ-N → ARCH-N → IMPL-N →
TEST-N`으로, 역시 그 모듈 자신의 `docs_needs/` 안에서 표현합니다.
다만 Sphinx는 "루트 문서가 소스 디렉터리 하위에 있어야 한다"는 제약이
있어 StrictDoc처럼 완전히 암묵적으로 동작하진 않습니다 — `modules/`
자체를 소스 디렉터리로, `modules/docs_needs/index.rst`를 루트 문서로
삼아 7개 모듈의 `docs_needs/*.rst`를 하나의 `toctree`로 모읍니다 (아래
"사용법"의 `-c` 옵션 참고). `IMPL-N`/`TEST-N`은 실제 소스/테스트 파일
경로를 본문에 텍스트로만 적어두는데, 이것이 StrictDoc과의 핵심 차이입니다
(아래 "두 도구 비교" 참고).

## 필요한 도구 설치

이 저장소를 빌드/테스트하는 데 필요한 것은 **Git**, **Bazel**
(`.bazelversion`에 `7.4.1`로 고정), 그리고 C++ 컴파일을 위한 **C/C++
툴체인**(Linux는 GCC 또는 Clang, Windows는 Visual Studio Build Tools)
뿐입니다. Python 인터프리터, StrictDoc, Sphinx, sphinx-needs는 모두
빌드 시점에 `rules_python`이 hermetic하게 받아오므로 로컬에 미리 설치할
필요가 없습니다.

### Windows

1. **Git**
   ```powershell
   winget install --id Git.Git -e
   ```
   (winget이 없다면 https://git-scm.com/download/win 에서 설치 프로그램을 받으세요.)

2. **Bazel (Bazelisk)** — Bazelisk는 `.bazelversion`에 적힌 버전을 자동으로
   내려받아 실행해 주는 공식 런처입니다. 아래 중 하나를 사용하세요.
   ```powershell
   winget install --id Bazel.Bazelisk -e
   # 또는
   choco install bazelisk
   # 또는
   scoop install bazelisk
   ```
   winget/choco/scoop을 쓰지 않는다면 GitHub 릴리스에서
   `bazelisk-windows-amd64.exe`를 받아 `bazel.exe`로 이름을 바꾼 뒤 PATH에
   등록된 폴더에 두어도 됩니다.
   https://github.com/bazelbuild/bazelisk/releases

3. **C++ 빌드 도구** — `cc_library`/`cc_test`를 컴파일하려면 MSVC가
   필요합니다. "Build Tools for Visual Studio"를 설치하고 워크로드에서
   **"C++를 사용한 데스크톱 개발"**을 선택하세요.
   ```powershell
   winget install --id Microsoft.VisualStudio.2022.BuildTools -e
   ```

4. (선택, 문제 발생 시) 경로 길이 제한 때문에 빌드가 실패하면 관리자 권한
   PowerShell에서 아래를 실행하고 재부팅하세요.
   ```powershell
   New-ItemProperty -Path "HKLM:\SYSTEM\CurrentControlSet\Control\FileSystem" `
     -Name "LongPathsEnabled" -Value 1 -PropertyType DWORD -Force
   ```

5. 설치 확인 (새 터미널에서):
   ```powershell
   bazel --version
   ```

### Linux (Ubuntu/Debian 기준, 다른 배포판도 패키지 매니저만 다를 뿐 동일)

1. **Git**과 **C++ 빌드 도구**
   ```bash
   sudo apt-get update && sudo apt-get install -y git build-essential
   ```

2. **Bazel (Bazelisk)** — 아래 중 편한 방법 하나를 선택하세요.
   - npm이 있다면:
     ```bash
     npm install -g @bazel/bazelisk
     ```
   - 또는 GitHub 릴리스에서 Bazelisk 바이너리를 직접 받기:
     ```bash
     sudo curl -fsSL -o /usr/local/bin/bazel \
       https://github.com/bazelbuild/bazelisk/releases/latest/download/bazelisk-linux-amd64
     sudo chmod +x /usr/local/bin/bazel
     ```
   - Bazelisk 대신 해당 버전의 Bazel 바이너리를 바로 받아도 됩니다
     (버전은 `.bazelversion` 참고):
     ```bash
     sudo curl -fsSL -o /usr/local/bin/bazel \
       https://github.com/bazelbuild/bazel/releases/download/7.4.1/bazel-7.4.1-linux-x86_64
     sudo chmod +x /usr/local/bin/bazel
     ```
     (이 방법은 사내망 등에서 Bazelisk가 쓰는 `releases.bazel.build`가 막혀
     있을 때도 `github.com`만 열려 있으면 동작합니다 — 이 저장소를 만들 때도
     같은 방식으로 검증했습니다.)

3. 설치 확인:
   ```bash
   bazel --version
   gcc --version
   ```

### (선택) 로컬 개발용 Python 가상환경

IDE 자동완성이나 `requirements.txt` 재생성처럼 Bazel 밖에서 StrictDoc/Sphinx를
직접 다루고 싶을 때만 필요하며, `bazel build`/`bazel test`를 실행하는 데는
필요하지 않습니다.

```bash
python3 -m venv .venv
source .venv/bin/activate        # Windows PowerShell: .venv\Scripts\Activate.ps1
pip install -r requirements.txt
```

## 사용법

가상환경 없이 Bazel만으로 재현됩니다.

```bash
# 7개 모듈 단위 테스트 (Simulink MBD 생성 코드 스타일의 speed_controller 포함)
bazel test //:cpp_module_tests --test_output=errors

# StrictDoc HTML 문서(요구사항/아키텍처/추적성 매트릭스/소스 커버리지) 생성
# -- 프로젝트 루트를 그대로 넘기면 strictdoc_config.py가 modules/ 아래를 재귀적으로 찾습니다.
bazel run //:strictdoc -- export . --output-dir=/tmp/sdoc-html

# sphinx-needs HTML 문서(요구사항/아키텍처/needs 테이블) 생성
# -- 소스 디렉터리는 modules/ (7개 모듈의 docs_needs/*.rst를 모두 포함),
#    -c로 conf.py가 있는 modules/docs_needs/를 별도 지정합니다.
bazel run //:sphinx-build -- -W -b html -c modules/docs_needs modules /tmp/needs-html

# 두 도구의 추적성 검증을 back-to-back으로 실행
bazel test //:traceability_tests --test_output=errors

# 개별로 실행하고 싶다면
bazel test //:strictdoc_traceability_test --test_output=errors
bazel test //:sphinx_needs_traceability_test --test_output=errors

# 전체 (C++ 테스트 + 두 문서 도구)
bazel test //...
```

### 추적성이 실제로 검증되는지 확인하기

- **StrictDoc**: `modules/diagnostics/docs/architecture.sdoc`에서
  `ARCH-4`의 `VALUE: REQ-4`를 존재하지 않는 `VALUE: REQ-999`로 바꾸고
  `bazel test //:strictdoc_traceability_test`를 실행하면, StrictDoc이
  `references parent requirement which doesn't exist` 오류를 내며 종료
  코드 1을 반환해 테스트가 실패합니다. (소스 파일의 `@relation(ARCH-4, ...)`
  을 존재하지 않는 UID로 바꾸거나, `RELATIONS`의 `File` 경로를 존재하지
  않는 파일로 바꾸는 경우도 동일하게 검증됩니다 — 후자는 `TYPE: File`
  관계를 쓸 때 해당됩니다.)

- **sphinx-needs**: `modules/diagnostics/docs_needs/architecture.rst`에서
  `ARCH-4`의 `:links: REQ-4`를 `:links: REQ-999`로 바꾸고
  `bazel test //:sphinx_needs_traceability_test`를 실행하면,
  `Need 'ARCH-4' has unknown outgoing link 'REQ-999'` 경고가 `-W` 옵션
  때문에 에러로 승격되어 테스트가 실패합니다.

- **C++ 모듈 테스트**: 예를 들어 `modules/diagnostics/src/diagnostics.cpp`의
  `<` / `>` 비교를 뒤집으면 `modules/diagnostics/tests/diagnostics_test.cpp`의
  GoogleTest `EXPECT_EQ`가 실패해 `bazel test //:diagnostics_test`가
  실패합니다 — 요구사항 문서와 별개로, 구현 자체의 정확성도 이 저장소에서
  함께 검증됩니다.

- **생성 코드의 `File` 관계**: `modules/speed_controller/docs/architecture.sdoc`에서
  `ARCH-7`의 `VALUE: modules/speed_controller/src/speed_controller.h`를
  존재하지 않는 경로로 바꾸고 `bazel test //:strictdoc_traceability_test`를
  실행하면, `references a file that does not exist` 오류로 테스트가
  실패합니다 — 소스에 마커를 넣을 수 없는 생성 코드에서도 StrictDoc은
  여전히 실제 파일 존재를 검증합니다. (sphinx-needs 쪽은
  `modules/speed_controller/docs_needs/implementation.rst`의 `IMPL-7`이
  같은 경로를 텍스트로만 담고 있어 이 검증 자체가 불가능합니다.)

네 경우 모두 원래 값으로 되돌리면 다시 통과합니다.

## 두 도구 비교 (StrictDoc vs sphinx-needs)

| 항목 | StrictDoc (`modules/<name>/docs/`) | sphinx-needs (`modules/<name>/docs_needs/`) |
|---|---|---|
| 문서 포맷 | 전용 `.sdoc` 텍스트 포맷 | Sphinx `.rst` + `sphinx_needs` 확장 지시어 |
| 요구사항 노드 | `[REQUIREMENT]` | `.. req::` / `.. spec::` / `.. impl::` / `.. test::` |
| 문서 간 관계 | `RELATIONS: TYPE: Parent VALUE: <UID>` | `:links: <UID>, <UID>` 옵션 |
| 소스코드 ↔ 문서 관계 | 소스 파일에 직접 `@relation(UID, scope=...)` 주석을 써서 **자동으로 검증**됨 (`TYPE: File`도 지원) | 기본 기능 없음 — 본문에 파일 경로를 텍스트로만 적음, 실제 파일 존재 여부는 검증되지 않음 |
| 깨진 관계 처리 | 파싱/빌드 단계에서 항상 **에러**(종료 코드 ≠ 0) | 기본은 **경고**일 뿐 빌드는 성공. `sphinx-build -W`로 경고를 에러로 승격해야 CI 게이트가 됨 |
| 모듈별 폴더 분리 대응 | `include_doc_paths=["/modules/"]` 하나로 하위 전체를 재귀 탐색 — 모듈 폴더를 추가/이동해도 설정 변경 불필요, 명시적 색인 문서도 불필요 | 루트 문서가 소스 디렉터리 하위에 있어야 한다는 제약 때문에, 모듈이 아닌 별도의 `modules/docs_needs/index.rst` + `toctree`로 모든 모듈의 문서를 일일이 나열해 모아야 함 — 모듈을 추가할 때마다 이 색인도 함께 고쳐야 함 |
| HTML 산출물 | 요구사항 트리, Traceability Matrix, Source Coverage 화면 | Sphinx HTML + needs 테이블/그래프(`needflow`, `needtable` 등, 이 저장소는 기본 화면만 사용) |
| Bazel 실행 타겟 | `//:strictdoc` (`py_console_script_binary`) | `//:sphinx-build` (`py_console_script_binary`) |
| Bazel 검증 타겟 | `//:strictdoc_traceability_test` | `//:sphinx_needs_traceability_test` |
| 여러 모듈 규모에서 체감 | 모듈이 늘어도 소스 마커만 추가하면 됨 — 문서와 코드가 어긋나면 빌드가 즉시 멈춤 | 모듈마다 REQ/ARCH/IMPL/TEST 4개 need를 사람이 다 맞춰 써야 함 — 7개 모듈만으로도 문서량이 StrictDoc보다 커짐 (`docs_needs/`가 `docs/`보다 파일 수·줄 수 모두 많음) |
| Simulink MBD 생성 코드(`speed_controller`) 처리 | `TYPE: File` 관계로 문서가 생성 파일 경로를 직접 검증(존재하지 않으면 빌드 실패) | 문서 쪽에 경로를 텍스트로만 적음 — 파일이 없어져도 빌드는 그대로 성공 |
| 대표 사용처 | StrictDoc 자체 프로젝트, 임베디드/안전 분야의 경량 요구사항 관리 | Eclipse S-CORE `docs-as-code`를 비롯한 Sphinx 기반 문서 파이프라인 |

**핵심 시사점**: StrictDoc은 소스코드 추적성(누가 이 요구사항을 구현/테스트
했는지)을 도구 차원에서 강제하고 깨지면 항상 빌드를 실패시키는 반면,
sphinx-needs는 문서 간 관계까지만 기본 제공하고 소스코드 연결은 프로젝트가
직접 컨벤션(예: Eclipse S-CORE의 `score_metamodel` 같은 커스텀 Sphinx
확장)을 만들어야 하며, 그 문서 간 관계조차 CI에서 강제하려면 `-W` 같은
옵션을 별도로 켜야 합니다. 모듈별로 폴더를 나눠 보면 이 차이가 한 번 더
드러납니다: StrictDoc은 `include_doc_paths` 하나로 폴더 구조와 무관하게
전역 추적을 유지하지만, sphinx-needs는 Sphinx의 "루트 문서는 소스 트리
안에 있어야 한다"는 제약 때문에 모듈이 아닌 별도의 최상위 색인
(`modules/docs_needs/index.rst`)을 두고 그 안에서 모든 모듈의 문서 경로를
일일이 나열해야 합니다. 7개 모듈로 규모를 키워보면 이 차이가 문서
유지보수 비용으로 바로 드러납니다: StrictDoc은 소스 마커만 관리하면
되지만, sphinx-needs는 `implementation.rst`/`verification.rst`를 소스
변경과 별도로 계속 손으로 맞춰줘야 합니다. 생성 코드(`speed_controller`)
차례가 되면 차이가 더 벌어집니다 — StrictDoc은 `TYPE: File`로 생성물의
실제 존재를 검증하지만, sphinx-needs는 그 검증 자체를 표현할 방법이
없습니다.

## 왜 이런 구조인가

Eclipse S-CORE는 자체 `docs-as-code` 저장소(Sphinx-needs 기반)에서
Bzlmod(`MODULE.bazel`) + `rules_python`으로 문서 툴체인을 hermetic하게
받아오고, `bazel run //:ide_support` 같은 타겟으로 개발 환경을, CI에서
문서 빌드/추적성 검사를 게이트로 두는 패턴을 씁니다. 이 저장소는 같은
Bazel 패턴을 그대로 두고, 그 위에 StrictDoc(`.sdoc`)과 sphinx-needs를
나란히 얹어 두 접근을 직접 비교할 수 있게 했습니다. 여기에 더해, 각 SW
모듈이 자신의 구현·테스트·문서를 한 폴더 아래 모두 갖는 구조
(`modules/<name>/`)로 배치해, 실제 멀티모듈 프로젝트에서 모듈을
추가/이동/삭제할 때 두 도구가 얼마나 잘 버티는지도 함께 드러나게 했습니다:

- `//:strictdoc`, `//:sphinx-build` — 각각 `py_console_script_binary`로
  노출한 실행형 타겟 (Eclipse S-CORE의 `//:ide_support`에 대응)
- `//:strictdoc_traceability_test`, `//:sphinx_needs_traceability_test` —
  각 도구의 종료 코드(StrictDoc)/경고-에러 승격(sphinx-needs `-W`)으로
  추적성 깨짐을 감지하는 `py_test` (Eclipse S-CORE CI의 traceability
  게이트에 대응)
- `//:traceability_tests` — 위 둘을 한 번에 실행하는 `test_suite`
- `//:cpp_module_tests` — Eclipse S-CORE 같은 실무 프로젝트에서 실제로
  추적해야 할 대상인, 7개 SW 모듈(수기 작성 6개 + Simulink MBD 생성
  코드 스타일 1개)의 단위 테스트

## 참고

- StrictDoc 문서: https://strictdoc.readthedocs.io/
- StrictDoc 예제(요구사항-소스 추적성): https://github.com/strictdoc-project/strictdoc-examples
- sphinx-needs 문서: https://sphinx-needs.readthedocs.io/
- Eclipse S-CORE docs-as-code: https://github.com/eclipse-score/docs-as-code

## 알려진 제약

- `//:*_traceability_test`는 sandbox 안에서 매번 문서를 전부 다시 빌드하는
  검증용 테스트이며, 결과 HTML을 영구 아티팩트로 남기려면
  `bazel run //:strictdoc -- export ...` / `bazel run //:sphinx-build -- ...`를
  별도로 실행해야 합니다.
- sphinx-needs 쪽의 각 모듈 `implementation.rst`/`verification.rst`에
  적힌 소스/테스트 파일 경로는 사람이 손으로 맞춰 적은 것이며, Bazel
  테스트가 실제 파일 존재를 검증하지 않습니다 (위 비교표 참고).
  StrictDoc 쪽은 반대로 실제 파일 경로를 검증합니다.
- C++ 모듈 테스트는 GoogleTest(`bazel_dep(name = "googletest", version =
  "1.17.0.bcr.2")`)를 사용합니다. 최신 `1.18.x`는 `abseil-cpp`가 요구하는
  버전이 `rules_python`이 끌어오는 `protobuf`의 `abseil-cpp` 요구 버전과
  `compatibility_level`이 달라 Bzlmod 해석이 실패해서, 둘 다와
  호환되는 `1.17.0.bcr.2`로 고정했습니다. GoogleTest는 C++17 이상을
  요구하므로 `.bazelrc`에 `--cxxopt=-std=c++17`/`--host_cxxopt=-std=c++17`도
  추가되어 있습니다.
- `modules/speed_controller/src/*`는 이 저장소에 MATLAB/Simulink가
  없어서 **실제로 Simulink Embedded Coder가 생성한 코드가 아닙니다** —
  Embedded Coder의 실제 출력 구조(배너 주석, `ExtU_`/`ExtY_`/`DW_`/`P_`
  구조체, 진입점 이름 규칙)를 손으로 재현한 것이며,
  `modules/speed_controller/models/speed_controller.md`에 이 사실과
  원본 블록 다이어그램을 명시해 두었습니다. 목적은 "생성 코드는 손으로
  건드릴 수 없다"는 제약 하에서 두 추적성 도구가 어떻게 다르게 대응하는지
  보여주는 것이지, 실제 MATLAB 산출물을 재현하는 것이 아닙니다.
- 사내망 등에서 `bcr.bazel.build`(Bazel Central Registry)가 막혀 있다면
  `bazel test //... --registry=https://raw.githubusercontent.com/bazelbuild/bazel-central-registry/main/`
  처럼 GitHub 미러를 registry로 지정해 우회할 수 있습니다.
