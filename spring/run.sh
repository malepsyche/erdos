#!/usr/bin/env bash
set -euo pipefail

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
REPO_ROOT="$(cd "${SCRIPT_DIR}/.." && pwd)"

LOG_FILE="${1:-${REPO_ROOT}/spring/market/pipeline_log.bin}"

cd "${REPO_ROOT}"

bazel build -c opt //spring:runtime_main

./bazel-bin/spring/runtime_main "$LOG_FILE"