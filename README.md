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
