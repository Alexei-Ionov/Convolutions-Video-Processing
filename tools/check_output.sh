#!/usr/bin/env bash
set -eu

help() {
  echo "Expected arg: path to input.txt"
}

if [ -z "${1:-}" ]; then
  help
  exit 1
fi

passed=0
total=0
task_dirname="$(dirname ${1})"
while IFS= read -r task_dir; do
  diff_ret=0
  diff -q --binary ${task_dirname}/${task_dir}/out.bin ${task_dirname}/${task_dir}/ref.bin > /dev/null || diff_ret=$?

  if [ $diff_ret -ne 0 ]; then
    echo "${task_dir}: output does not match reference"
  else
    passed=$(($passed + 1))
  fi
  total=$(($total + 1))
done < <(tail -n +2 ${1})

echo "${task_dirname}: ${passed}/${total} tests passed"

if [ "$passed" -ne "$total" ]; then
  exit 1
fi
