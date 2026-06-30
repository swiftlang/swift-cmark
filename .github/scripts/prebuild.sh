#!/bin/bash
#
# This source file is part of the Swift.org open source project
#
# Copyright (c) 2026 Apple Inc. and the Swift project authors
# Licensed under Apache License v2.0 with Runtime Library Exception
#
# See https://swift.org/LICENSE.txt for license information
# See https://swift.org/CONTRIBUTORS.txt for Swift project authors
#

set -e

if [[ $(uname) == Darwin ]] ; then
    mkdir -p "$RUNNER_TOOL_CACHE"
    if ! command -v cmake >/dev/null 2>&1 ; then
        curl -fsSLO https://github.com/Kitware/CMake/releases/download/v4.1.2/cmake-4.1.2-macos-universal.tar.gz
        echo '3be85f5b999e327b1ac7d804cbc9acd767059e9f603c42ec2765f6ab68fbd367 cmake-4.1.2-macos-universal.tar.gz' > cmake-4.1.2-macos-universal.tar.gz.sha256
        sha256sum -c cmake-4.1.2-macos-universal.tar.gz.sha256
        tar -xf cmake-4.1.2-macos-universal.tar.gz
        ln -s "$PWD/cmake-4.1.2-macos-universal/CMake.app/Contents/bin/cmake" "$RUNNER_TOOL_CACHE/cmake"
    fi
elif command -v apt-get >/dev/null 2>&1 ; then # bookworm, noble, jammy
    export DEBIAN_FRONTEND=noninteractive

    apt-get update -y

    # Build tools
    apt-get install -y cmake make
elif command -v dnf >/dev/null 2>&1 ; then # rhel-ubi9
    dnf update -y

    # Build tools
    dnf install -y cmake make
elif command -v yum >/dev/null 2>&1 ; then # amazonlinux2
    yum update -y

    # Build tools
    yum install -y cmake make
fi
