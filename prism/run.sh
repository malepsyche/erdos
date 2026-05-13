#!/usr/bin/env bash
set -euo pipefail

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
REPO_ROOT="$(cd "${SCRIPT_DIR}/.." && pwd)"

LOG_BIN_FILE="${1:-${REPO_ROOT}/spring/market/pipeline_log.bin}"
LOG_TXT_FILE="${2:-${REPO_ROOT}/prism/pipeline_log.txt}"

cd "${REPO_ROOT}"

bazel build -c opt //prism:runtime_main

./bazel-bin/prism/runtime_main "$LOG_BIN_FILE" "$LOG_TXT_FILE"