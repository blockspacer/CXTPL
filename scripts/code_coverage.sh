#!/usr/bin/env bash
# Copyright (c) 2018 Denis Trofimov (den.a.trofimov@yandex.ru)
# Distributed under the MIT License.
# See accompanying file LICENSE.md or copy at http://opensource.org/licenses/MIT

set -ev

# NOTE: code coverage requires g++/gcc

# rm -rf build
cmake -E remove_directory build
#cmake -E remove_directory bin

# mkdir build
cmake -E make_directory build
#cmake -E make_directory bin

# NOTE: coverage requires DCMAKE_CXX_COMPILER="g++" !

# NOTE: change PATHS, such as DWEBRTC_SRC_PATH
cmake -E chdir build cmake -E time cmake .. -DWEBRTC_SRC_PATH:STRING="/home/denis/workspace/webrtc-checkout/src" -DWEBRTC_TARGET_PATH:STRING="out/release" -DCMAKE_C_COMPILER="gcc" -DCMAKE_CXX_COMPILER="g++" -DBOOST_ROOT:STRING="/usr" -DCMAKE_EXPORT_COMPILE_COMMANDS=ON -DCLANG_PATH="/usr/lib/llvm-6.0/lib/clang/6.0.1/include" -DENABLE_IWYU=OFF -DCMAKE_BUILD_TYPE=Debug -Dgloer_BUILD_TESTS=ON -Dgloer_BUILD_EXAMPLES=OFF -DAUTORUN_TESTS=OFF -DBUILD_DOXY_DOC=OFF -DENABLE_CODE_COVERAGE=ON

#cmake --build build --target ctest-cleanup
cmake -E chdir build cmake -E time cmake --build . --config Debug -- -j8
cmake -E chdir build cmake -E time cmake --build . --config Debug --target unit_tests_coverage
