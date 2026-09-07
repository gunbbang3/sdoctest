#!/usr/bin/env bash
# Runs every test target and both traceability-report genrules, then
# collects their outputs into build/ at the repository root -- Bazel
# itself can only write build outputs under bazel-bin (never back into
# the source tree), so this script is what "installs" a copy of them
# into a plain, repo-local build/ folder for local inspection or for CI
# to upload as a workflow artifact. See README.md ("산출물") for details.
#
# Collects whatever it can even if tests fail or a report fails to
# build, then exits non-zero at the end if anything did -- so `build/`
# still has the (partial) results to inspect on a red run.
#
# Usage: scripts/generate_reports.sh
set -uo pipefail

repo_root="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
cd "$repo_root"

rm -rf build
mkdir -p build/test-results build/reports/strictdoc build/reports/sphinx-needs

exit_code=0

echo "==> Running all tests (7 module unit tests + 2 traceability tests)"
bazel test //... --test_output=errors || exit_code=$?

echo "==> Collecting JUnit test-result XML into build/test-results/"
find -L bazel-testlogs -name 'test.xml' -exec cp --parents {} build/test-results/ \;

echo "==> Building the StrictDoc + sphinx-needs HTML traceability reports"
bazel build //:traceability_reports || exit_code=$?

echo "==> Extracting reports into build/reports/"
[ -f bazel-bin/strictdoc-report.tar.gz ] &&
  tar -xzf bazel-bin/strictdoc-report.tar.gz -C build/reports/strictdoc
[ -f bazel-bin/sphinx-needs-report.tar.gz ] &&
  tar -xzf bazel-bin/sphinx-needs-report.tar.gz -C build/reports/sphinx-needs

cat <<EOF

build/ now contains:
  build/test-results/bazel-testlogs/<target>/test.xml   (JUnit test results)
  build/reports/strictdoc/index.html                    (StrictDoc report, if it built)
  build/reports/sphinx-needs/docs_needs/index.html       (sphinx-needs report, if it built)
EOF

exit "$exit_code"
