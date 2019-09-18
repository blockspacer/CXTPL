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

cmake -E make_directory _build

# cd _build
pushd _build

cmake ..

make -j $(nproc)

make install # with either sudo or DESTDIR as necessary

# Clean after install:

# cd ..
pushd ..

cmake -E remove_directory _build
