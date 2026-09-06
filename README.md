# sdoctest

Bazel 위에서 **StrictDoc(`sdoc`)**와 **Sphinx + sphinx-needs**
([Eclipse S-CORE](https://github.com/eclipse-score)의 `docs-as-code`
저장소가 실제로 쓰는 도구)로 **같은 요구사항/아키텍처/구현/테스트**를 각각
작성해두고, 두 도구의 문서-추적성(traceability) 메커니즘을 **back-to-back으로
빌드·검증·비교**할 수 있게 만든 저장소입니다.

예제 대상은 6개의 C++ 모듈로 구성된 작은 신호처리 파이프라인입니다.

## C++ 예제 시스템 (6개 모듈)

```
SensorInput ──▶ MovingAverageFilter ──▶ Controller ──▶ RangeDiagnostics
 (ARCH-1)             (ARCH-2)          (ARCH-6)          (ARCH-4)
                                            │
                     MathUtils ◀───────────┤
                      (ARCH-3)              │
                                            ▼
                                          Logger
                                          (ARCH-5)
```

| 모듈 | 클래스/함수 | 요구사항 | 소스 | 테스트 |
|---|---|---|---|---|
| SensorInput | `SensorInput` | REQ-1 | `src/sensor_input/` | `tests/sensor_input_test.cpp` |
| SignalFilter | `MovingAverageFilter` | REQ-2 | `src/signal_filter/` | `tests/signal_filter_test.cpp` |
| MathUtils | `Add`, `Clamp` | REQ-3 | `src/math_utils/` | `tests/math_utils_test.cpp` |
| Diagnostics | `RangeDiagnostics` | REQ-4 | `src/diagnostics/` | `tests/diagnostics_test.cpp` |
| Logger | `Logger` | REQ-5 | `src/logger/` | `tests/logger_test.cpp` |
| Controller | `Controller` (위 5개를 통합) | REQ-6 | `src/controller/` | `tests/controller_test.cpp` |

`Controller`는 한 사이클마다 `SensorInput`에서 원시값을 읽고,
`MovingAverageFilter`로 평활화하고, `MathUtils::Clamp`로 클램핑한 뒤,
`RangeDiagnostics`로 범위를 검사하고, 결과를 `Logger`에 기록합니다 —
6개 모듈이 실제로 서로 의존하는 최소한의 통합 예제입니다. 테스트는
외부 프레임워크(gtest 등) 없이 `tests/test_util.h`의 작은 assert
헬퍼만으로 작성했습니다 (`-c opt` 빌드에서도 사라지지 않도록 `assert()`
대신 직접 만든 헬퍼를 씁니다).

## 구성

```
MODULE.bazel                Bzlmod 설정: rules_python + pip.parse (strictdoc, sphinx, sphinx-needs).
                             C++ 툴체인은 Bazel 내장 cc_library/cc_test를 그대로 사용 (별도 bazel_dep 불필요)
requirements.txt            두 문서 툴체인의 전이 의존성을 함께 고정한 pip 락 파일
strictdoc_config.py         StrictDoc 프로젝트 설정 (문서/소스 경로, 활성 기능)
BUILD.bazel                 //:strictdoc, //:sphinx-build (실행형) +
                             //:strictdoc_traceability_test, //:sphinx_needs_traceability_test,
                             //:traceability_tests (문서 검증형) +
                             6개 모듈의 cc_library/cc_test + //:cpp_module_tests

docs/                        --- StrictDoc 쪽 ---
  requirements.sdoc          REQ-1 ~ REQ-6 (소프트웨어 요구사항)
  architecture.sdoc          ARCH-1 ~ ARCH-6, RELATIONS: Parent -> REQ-*
                              (ARCH-6/Controller는 ARCH-1~5에도 Parent로 연결)

docs_needs/                  --- sphinx-needs 쪽 (동일 내용) ---
  conf.py                    extensions = ["sphinx_needs"]
  requirements.rst           REQ-1 ~ REQ-6 (.. req:: 지시어)
  architecture.rst           ARCH-1 ~ ARCH-6, :links: REQ-*, ARCH-6은 ARCH-1~5에도 링크 (.. spec:: 지시어)
  implementation.rst         IMPL-1 ~ IMPL-6, :links: ARCH-* (.. impl:: 지시어)
  verification.rst           TEST-1 ~ TEST-6, :links: IMPL-* (.. test:: 지시어)

src/<module>/<module>.h, .cpp   각 파일 상단에 # @relation(ARCH-N, scope=file) 로 StrictDoc과 연결된 구현
tests/<module>_test.cpp         # @relation(ARCH-N, scope=file, role=Test) 로 연결된 테스트
tests/test_util.h               테스트 전용 assert 헬퍼 (프레임워크 의존성 없음)

tests/strictdoc_traceability_test.py       StrictDoc 쪽 검증 py_test
tests/sphinx_needs_traceability_test.py    sphinx-needs 쪽 검증 py_test

.github/workflows/traceability.yml   PR/main 푸시마다 C++ 모듈 테스트 + 두 문서 도구를 모두 검증하는 CI
```

**StrictDoc 쪽** 추적 체인은 모듈마다 `REQ-N → ARCH-N → src/<module>/*
→ tests/<module>_test.cpp`로 이어지며, `docs/architecture.sdoc`의
`RELATIONS`(문서 간 Parent 관계)와 소스 코드의 `@relation(...)` 주석
(문서 ↔ 코드 관계) 두 메커니즘을 모두 사용합니다. `Controller`(ARCH-6)는
추가로 나머지 5개 ARCH 요소에도 `Parent` 관계를 걸어, 통합 의존성까지
그래프에 나타냅니다.

**sphinx-needs 쪽**은 같은 체인을 모듈마다 `REQ-N → ARCH-N → IMPL-N →
TEST-N`으로 표현합니다. `IMPL-N`/`TEST-N`은 실제 소스/테스트 파일 경로를
본문에 텍스트로만 적어두는데, 이것이 StrictDoc과의 핵심 차이입니다
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
# 6개 C++ 모듈 단위 테스트
bazel test //:cpp_module_tests --test_output=errors

# StrictDoc HTML 문서(요구사항/아키텍처/추적성 매트릭스/소스 커버리지) 생성
bazel run //:strictdoc -- export . --output-dir=/tmp/sdoc-html

# sphinx-needs HTML 문서(요구사항/아키텍처/needs 테이블) 생성
bazel run //:sphinx-build -- -W -b html docs_needs /tmp/needs-html

# 두 도구의 추적성 검증을 back-to-back으로 실행
bazel test //:traceability_tests --test_output=errors

# 개별로 실행하고 싶다면
bazel test //:strictdoc_traceability_test --test_output=errors
bazel test //:sphinx_needs_traceability_test --test_output=errors

# 전체 (C++ 테스트 + 두 문서 도구)
bazel test //...
```

### 추적성이 실제로 검증되는지 확인하기

- **StrictDoc**: `docs/architecture.sdoc`에서 `ARCH-4`(Diagnostics)의
  `VALUE: REQ-4`를 존재하지 않는 `VALUE: REQ-999`로 바꾸고
  `bazel test //:strictdoc_traceability_test`를 실행하면, StrictDoc이
  `references parent requirement which doesn't exist` 오류를 내며 종료
  코드 1을 반환해 테스트가 실패합니다. (소스 파일의 `@relation(ARCH-4, ...)`
  을 존재하지 않는 UID로 바꾸거나, `RELATIONS`의 `File` 경로를 존재하지
  않는 파일로 바꾸는 경우도 동일하게 검증됩니다 — 후자는 `TYPE: File`
  관계를 쓸 때 해당됩니다.)

- **sphinx-needs**: `docs_needs/architecture.rst`에서 `ARCH-4`의
  `:links: REQ-4`를 `:links: REQ-999`로 바꾸고
  `bazel test //:sphinx_needs_traceability_test`를 실행하면,
  `Need 'ARCH-4' has unknown outgoing link 'REQ-999'` 경고가 `-W` 옵션
  때문에 에러로 승격되어 테스트가 실패합니다.

- **C++ 모듈 테스트**: 예를 들어 `src/diagnostics/diagnostics.cpp`의
  `<` / `>` 비교를 뒤집으면 `tests/diagnostics_test.cpp`의 assert가
  실패해 `bazel test //:diagnostics_test`가 실패합니다 — 요구사항 문서와
  별개로, 구현 자체의 정확성도 이 저장소에서 함께 검증됩니다.

세 경우 모두 원래 값으로 되돌리면 다시 통과합니다.

## 두 도구 비교 (StrictDoc vs sphinx-needs)

| 항목 | StrictDoc (`docs/`) | sphinx-needs (`docs_needs/`) |
|---|---|---|
| 문서 포맷 | 전용 `.sdoc` 텍스트 포맷 | Sphinx `.rst` + `sphinx_needs` 확장 지시어 |
| 요구사항 노드 | `[REQUIREMENT]` | `.. req::` / `.. spec::` / `.. impl::` / `.. test::` |
| 문서 간 관계 | `RELATIONS: TYPE: Parent VALUE: <UID>` | `:links: <UID>, <UID>` 옵션 |
| 소스코드 ↔ 문서 관계 | 소스 파일에 직접 `@relation(UID, scope=...)` 주석을 써서 **자동으로 검증**됨 (`TYPE: File`도 지원) | 기본 기능 없음 — 본문에 파일 경로를 텍스트로만 적음, 실제 파일 존재 여부는 검증되지 않음 |
| 깨진 관계 처리 | 파싱/빌드 단계에서 항상 **에러**(종료 코드 ≠ 0) | 기본은 **경고**일 뿐 빌드는 성공. `sphinx-build -W`로 경고를 에러로 승격해야 CI 게이트가 됨 |
| HTML 산출물 | 요구사항 트리, Traceability Matrix, Source Coverage 화면 | Sphinx HTML + needs 테이블/그래프(`needflow`, `needtable` 등, 이 저장소는 기본 화면만 사용) |
| Bazel 실행 타겟 | `//:strictdoc` (`py_console_script_binary`) | `//:sphinx-build` (`py_console_script_binary`) |
| Bazel 검증 타겟 | `//:strictdoc_traceability_test` | `//:sphinx_needs_traceability_test` |
| 여러 모듈 규모에서 체감 | 모듈이 늘어도 소스 마커만 추가하면 됨 — 문서와 코드가 어긋나면 빌드가 즉시 멈춤 | 모듈마다 REQ/ARCH/IMPL/TEST 4개 need를 사람이 다 맞춰 써야 함 — 6개 모듈만으로도 문서량이 StrictDoc보다 커짐 (`docs_needs/`가 `docs/`보다 파일 수·줄 수 모두 많음) |
| 대표 사용처 | StrictDoc 자체 프로젝트, 임베디드/안전 분야의 경량 요구사항 관리 | Eclipse S-CORE `docs-as-code`를 비롯한 Sphinx 기반 문서 파이프라인 |

**핵심 시사점**: StrictDoc은 소스코드 추적성(누가 이 요구사항을 구현/테스트
했는지)을 도구 차원에서 강제하고 깨지면 항상 빌드를 실패시키는 반면,
sphinx-needs는 문서 간 관계까지만 기본 제공하고 소스코드 연결은 프로젝트가
직접 컨벤션(예: Eclipse S-CORE의 `score_metamodel` 같은 커스텀 Sphinx
확장)을 만들어야 하며, 그 문서 간 관계조차 CI에서 강제하려면 `-W` 같은
옵션을 별도로 켜야 합니다. 6개 모듈로 규모를 키워보면 이 차이가 문서
유지보수 비용으로 바로 드러납니다: StrictDoc은 소스 마커만 관리하면
되지만, sphinx-needs는 `implementation.rst`/`verification.rst`를 소스
변경과 별도로 계속 손으로 맞춰줘야 합니다.

## 왜 이런 구조인가

Eclipse S-CORE는 자체 `docs-as-code` 저장소(Sphinx-needs 기반)에서
Bzlmod(`MODULE.bazel`) + `rules_python`으로 문서 툴체인을 hermetic하게
받아오고, `bazel run //:ide_support` 같은 타겟으로 개발 환경을, CI에서
문서 빌드/추적성 검사를 게이트로 두는 패턴을 씁니다. 이 저장소는 같은
Bazel 패턴을 그대로 두고, 그 위에 StrictDoc(`.sdoc`)과 sphinx-needs를
나란히 얹어 두 접근을 직접 비교할 수 있게 했습니다:

- `//:strictdoc`, `//:sphinx-build` — 각각 `py_console_script_binary`로
  노출한 실행형 타겟 (Eclipse S-CORE의 `//:ide_support`에 대응)
- `//:strictdoc_traceability_test`, `//:sphinx_needs_traceability_test` —
  각 도구의 종료 코드(StrictDoc)/경고-에러 승격(sphinx-needs `-W`)으로
  추적성 깨짐을 감지하는 `py_test` (Eclipse S-CORE CI의 traceability
  게이트에 대응)
- `//:traceability_tests` — 위 둘을 한 번에 실행하는 `test_suite`
- `//:cpp_module_tests` — Eclipse S-CORE 같은 실무 프로젝트에서 실제로
  추적해야 할 대상인, C++로 작성된 6개 SW 모듈의 단위 테스트

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
- sphinx-needs 쪽의 `implementation.rst`/`verification.rst`에 적힌
  `src/<module>/*`, `tests/<module>_test.cpp` 경로는 사람이 손으로 맞춰
  적은 것이며, Bazel 테스트가 실제 파일 존재를 검증하지 않습니다 (위
  비교표 참고). StrictDoc 쪽은 반대로 실제 파일 경로를 검증합니다.
- C++ 모듈 테스트는 googletest 같은 프레임워크 없이 `tests/test_util.h`의
  최소 assert 헬퍼만 사용합니다. 규모를 더 키우거나 실무에 맞추려면
  `bazel_dep(name = "googletest", ...)`를 추가하고 `cc_test`의 `deps`에
  `@googletest//:gtest_main`을 넣는 식으로 손쉽게 교체할 수 있습니다.
- 사내망 등에서 `bcr.bazel.build`(Bazel Central Registry)가 막혀 있다면
  `bazel test //... --registry=https://raw.githubusercontent.com/bazelbuild/bazel-central-registry/main/`
  처럼 GitHub 미러를 registry로 지정해 우회할 수 있습니다.
