#!/usr/bin/env bash
# Copyright (c) 2018 Denis Trofimov (den.a.trofimov@yandex.ru)
# Distributed under the MIT License.
# See accompanying file LICENSE.md or copy at http://opensource.org/licenses/MIT

set -ev

# cd build
#pushd build

# note AddressSanitizer is not compatible with ThreadSanitizer or MemorySanitizer
# note ThreadSanitizer is not compatible with MemorySanitizer
# NOTE: change PATHS, such as DWEBRTC_SRC_PATH
cmake -E chdir build cmake -E time cmake .. -DENABLE_CODE_COVERAGE=OFF -DWEBRTC_SRC_PATH:STRING="/home/denis/workspace/webrtc-checkout/src" -DWEBRTC_TARGET_PATH:STRING="out/release" -DCMAKE_C_COMPILER="/usr/bin/clang-6.0" -DCMAKE_CXX_COMPILER="/usr/bin/clang++-6.0" -DBOOST_ROOT:STRING="/usr" -DCMAKE_EXPORT_COMPILE_COMMANDS=ON -DCLANG_PATH="/usr/lib/llvm-6.0/lib/clang/6.0.1/include" -DENABLE_IWYU=OFF -DCMAKE_BUILD_TYPE=Debug -Dgloer_BUILD_TESTS=ON -Dgloer_BUILD_EXAMPLES=ON -DAUTORUN_TESTS=OFF -DSANITIZE_UNDEFINED=OFF -DSANITIZE_MEMORY=OFF -DSANITIZE_THREAD=ON -DSANITIZE_ADDRESS=OFF -DENABLE_VALGRIND_TESTS=OFF -DBUILD_DOXY_DOC=OFF

#cmake --build build --target ctest-cleanup
cmake -E chdir build cmake -E time cmake --build . --config Debug -- -j8
cmake -E chdir build cmake -E time cmake --build . --config Debug --target run_all_tests
