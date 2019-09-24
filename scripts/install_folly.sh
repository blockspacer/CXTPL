#!/usr/bin/env bash
# Copyright (c) 2018 Denis Trofimov (den.a.trofimov@yandex.ru)
# Distributed under the MIT License.
# See accompanying file LICENSE.md or copy at http://opensource.org/licenses/MIT

set -ev

ls submodules/folly

cmake -E make_directory submodules/folly

# cd submodules/build-g3log
pushd submodules/folly

git submodule update --init --recursive

# fix for https://github.com/facebook/folly/issues/976
cat ../../patches/folly/0001-clang-cling-support.patch | git am

cmake -E make_directory _build

# cd _build
pushd _build

cmake -DBUILD_STATIC_LIBS=ON -DBUILD_SHARED_LIBS=OFF ..

make -j $(nproc)

make install # with either sudo or DESTDIR as necessary

# Clean after install:

# cd ..
pushd ..

cmake -E remove_directory _build
