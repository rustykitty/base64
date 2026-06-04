#!/usr/bin/env bash

set -e

function getms() {
    date "+%s.%N"
}

function time2() {
    local t1="$(getms)"
    "$@"
    local t2="$(getms)"
    local td="$(echo "$t2 - $t1" | bc)"
    printf "%.3f\n" "$td" >&2
}

function errecho() {
    echo "$@" >&2
}

function eprintf() {
    printf "$@" >&2
}

cd "$(dirname "$0")"

type base64 >/dev/null || exit 1

coreutils_base64="base64 -w 0"
custom_base64="../main"

time="/usr/bin/env time --format %e"

eprintf "Encode tests\n"

eprintf "GNU coreutils, encode (stream): "
time2 $coreutils_base64 < "tmp/test_data" > coreutils.out

eprintf "Custom, encode (stream): "
time2 $custom_base64 < "tmp/test_data" > custom.out

eprintf "GNU coreutils, encode (file): "
time2 $coreutils_base64 "tmp/test_data" > coreutils.out

eprintf "Custom, encode (file): "
time2 $custom_base64 "tmp/test_data" > custom.out

eprintf "Decode tests\n"

$coreutils_base64 < "tmp/test_data" > tmp/encoded_data

eprintf "GNU coreutils, decode (stream): "
time2 $coreutils_base64 -d < tmp/encoded_data > "tmp/coreutils.out"

eprintf "Custom, decode (stream): "
time2 $custom_base64 -d < tmp/encoded_data > "tmp/custom.out"

eprintf "GNU coreutils, decode (file): "
time2 $coreutils_base64 -d tmp/encoded_data > "tmp/coreutils.out"

eprintf "Custom, decode (file): "
time2 $custom_base64 -d tmp/encoded_data > "tmp/custom.out"