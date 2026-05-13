#!/usr/bin/env bash
set -euo pipefail

./spring/run.sh &
SPRING_PID=$!

./prism/run.sh &
PRISM_PID=$!

trap 'kill $SPRING_PID $PRISM_PID' SIGINT SIGTERM

wait