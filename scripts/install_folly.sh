#!/usr/bin/env bash
# Copyright (c) 2018 Denis Trofimov (den.a.trofimov@yandex.ru)
# Distributed under the MIT License.
# See accompanying file LICENSE.md or copy at http://opensource.org/licenses/MIT

set -ev

#ls submodules/folly

cmake -E make_directory submodules/folly

# cd submodules/build-g3log
pushd submodules/folly

git submodule update --init --recursive

# store rev before patches
prev_rev_parse=$(git rev-parse HEAD)

# fix for https://github.com/facebook/folly/issues/976
cat ../../patches/folly/0001-clang-cling-support.patch | git am

cmake -E make_directory _build

# cd _build
#pushd _build

ls -artl

ls -artl CMake

# CMAKE_POSITION_INDEPENDENT_CODE for -fPIC
cmake -E chdir _build cmake -DBUILD_STATIC_LIBS=ON -DBUILD_SHARED_LIBS=OFF -DCMAKE_POSITION_INDEPENDENT_CODE=ON ..

cmake -E chdir _build make -j $(nproc)

cmake -E chdir _build make install # with either sudo or DESTDIR as necessary

# Revert patches after install:
# NOTE: git reset --hard '@{u}' deletes all your local changes on
# the current branch, including commits.
# git reset --hard '@{u}'
# NOTE: Deletes the most recent commit:
# git reset --hard HEAD~1
# NOTE: reset �hard to set the current branch HEAD to the commit you want.
git reset --hard $prev_rev_parse
# NOTE: git clean -f -d to remove all the untracked
# files in your working directory
git clean -f -d

# Clean after install:

# cd ..
#pushd ..

cmake -E remove_directory _build
