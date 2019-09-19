#!/usr/bin/env bash
# Copyright (c) 2018 Denis Trofimov (den.a.trofimov@yandex.ru)
# Distributed under the MIT License.
# See accompanying file LICENSE.md or copy at http://opensource.org/licenses/MIT

set -ev

cmake -E make_directory ~/Downloads

pushd ~/Downloads

wget https://github.com/gflags/gflags/archive/v2.2.2.tar.gz && \
tar zxf v2.2.2.tar.gz && \
rm -f v2.2.2.tar.gz && \
cd gflags-2.2.2 && \
cmake -DGFLAGS_BUILD_SHARED_LIBS=OFF -DGFLAGS_BUILD_STATIC_LIBS=ON && \
make && \
make install
