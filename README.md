# sdoctest

[StrictDoc](https://github.com/strictdoc-project/strictdoc) 문서(`.sdoc`)와
Bazel을 결합해, [Eclipse S-CORE](https://github.com/eclipse-score) 프로젝트의
`docs-as-code` 저장소들이 쓰는 것과 같은 방식(Bzlmod + `rules_python` +
CI 게이트)으로 **요구사항 ↔ 아키텍처 ↔ 소스코드/테스트** 추적성(traceability)을
Bazel 빌드/테스트 대상으로 검증하는 초기 설정입니다.

## 구성

```
MODULE.bazel          Bzlmod 설정: rules_python + pip.parse로 strictdoc 설치
requirements.txt       strictdoc 0.29.0 및 전이 의존성을 고정한 pip 락 파일
strictdoc_config.py    StrictDoc 프로젝트 설정 (문서/소스 경로, 활성 기능)
BUILD.bazel            //:strictdoc (실행형), //:traceability_test (검증형) 타겟
docs/
  requirements.sdoc     REQ-1, REQ-2 (소프트웨어 요구사항)
  architecture.sdoc      ARCH-1, RELATIONS: Parent -> REQ-1, REQ-2
src/calculator.py       # @relation(ARCH-1, scope=file) 로 아키텍처에 연결된 구현
tests/test_calculator.py # @relation(ARCH-1, scope=file, role=Test) 로 연결된 테스트
.github/workflows/traceability.yml  PR/main 푸시마다 추적성을 검증하는 CI
```

추적 체인은 `REQ-* → ARCH-1 → src/calculator.py → tests/test_calculator.py`
로 이어지며, `docs/architecture.sdoc`의 `RELATIONS`(문서 간 Parent 관계)와
소스 코드의 `@relation(...)` 주석(문서 ↔ 코드 관계)이라는 StrictDoc의 두 가지
추적 메커니즘을 모두 사용합니다.

## 필요한 도구 설치

이 저장소를 빌드/테스트하는 데 필요한 것은 **Git**과 **Bazel**
(`.bazelversion`에 `7.4.1`로 고정)뿐입니다. Python 인터프리터와 StrictDoc
자체는 빌드 시점에 `rules_python`이 hermetic하게 받아오므로 로컬에 미리
설치할 필요가 없습니다.

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

3. (선택, 문제 발생 시) 경로 길이 제한 때문에 빌드가 실패하면 관리자 권한
   PowerShell에서 아래를 실행하고 재부팅하세요.
   ```powershell
   New-ItemProperty -Path "HKLM:\SYSTEM\CurrentControlSet\Control\FileSystem" `
     -Name "LongPathsEnabled" -Value 1 -PropertyType DWORD -Force
   ```

4. 설치 확인 (새 터미널에서):
   ```powershell
   bazel --version
   ```

### Linux (Ubuntu/Debian 기준, 다른 배포판도 패키지 매니저만 다를 뿐 동일)

1. **Git**
   ```bash
   sudo apt-get update && sudo apt-get install -y git
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
   ```

### (선택) 로컬 개발용 Python 가상환경

IDE 자동완성이나 `requirements.txt` 재생성처럼 Bazel 밖에서 StrictDoc을 직접
다루고 싶을 때만 필요하며, `bazel build`/`bazel test`를 실행하는 데는
필요하지 않습니다.

```bash
python3 -m venv .venv
source .venv/bin/activate        # Windows PowerShell: .venv\Scripts\Activate.ps1
pip install -r requirements.txt
```

## 사용법

가상환경 없이 Bazel만으로 재현됩니다 (Python 인터프리터와 strictdoc은
`rules_python`이 hermetic하게 받아옵니다).

```bash
# StrictDoc HTML 문서(요구사항/아키텍처/추적성 매트릭스/소스 커버리지) 생성
bazel run //:strictdoc -- export . --output-dir=/tmp/sdoc-html
open /tmp/sdoc-html/html/index.html

# 추적성 검증 (요구사항-아키텍처-소스 간 링크가 끊기면 실패)
bazel test //:traceability_test --test_output=errors

# 전체
bazel test //...
```

### 추적성이 실제로 검증되는지 확인하기

`docs/architecture.sdoc`의 `VALUE: REQ-2`를 존재하지 않는
`VALUE: REQ-999`로 바꾸고 다시 `bazel test //:traceability_test`를
실행하면, StrictDoc이 `references parent requirement which doesn't exist`
오류를 내며 종료 코드 1을 반환해 테스트가 실패하는 것을 볼 수 있습니다.
(`src/calculator.py`의 File 관계 경로를 존재하지 않는 파일로 바꾸는 경우도
동일하게 실패합니다.) 원래 값으로 되돌리면 다시 통과합니다.

## 왜 이런 구조인가

Eclipse S-CORE는 자체 `docs-as-code` 저장소(Sphinx-needs 기반)에서
Bzlmod(`MODULE.bazel`) + `rules_python`으로 문서 툴체인을 hermetic하게 받아오고,
`bazel run //:ide_support` 같은 타겟으로 개발 환경을, CI에서 문서 빌드/추적성
검사를 게이트로 두는 패턴을 씁니다. 이 저장소는 같은 패턴을 StrictDoc(`.sdoc`)
기준으로 재현한 것입니다:

- `//:strictdoc` — `py_console_script_binary`로 노출한 StrictDoc 실행형 타겟
  (Eclipse S-CORE의 `//:ide_support`에 대응하는 "툴 실행" 타겟)
- `//:traceability_test` — `strictdoc export`의 종료 코드로 추적성 깨짐을
  감지하는 `py_test` (Eclipse S-CORE CI의 traceability 게이트에 대응)

## 참고

- StrictDoc 문서: https://strictdoc.readthedocs.io/
- StrictDoc 예제(요구사항-소스 추적성): https://github.com/strictdoc-project/strictdoc-examples
- Eclipse S-CORE docs-as-code: https://github.com/eclipse-score/docs-as-code

## 알려진 제약

- `bazel test //:traceability_test`는 sandbox 안에서 매번 `strictdoc export`를
  전부 다시 실행하는 검증용 테스트이며, 결과 HTML을 영구 아티팩트로 남기려면
  `bazel run //:strictdoc -- export ...`를 별도로 실행해야 합니다.
- 사내망 등에서 `bcr.bazel.build`(Bazel Central Registry)가 막혀 있다면
  `bazel test //... --registry=https://raw.githubusercontent.com/bazelbuild/bazel-central-registry/main/`
  처럼 GitHub 미러를 registry로 지정해 우회할 수 있습니다.
