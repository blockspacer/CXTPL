# About CXTPL (C++ template engine)

Template engine with full C++ power (transpiles template to valid C++ code, supports Cling, e.t.c.).

Note: this project is provided as is, without any warranty (see License).

## What is `.cxtpl`
`.cxtpl` is file extention for C++ template engine.

Example (more below):
```
<div> some template string here </div>
<CX=> int valid_cpp_code_block_here = 0; <=CX>
<CX=l> int and_valid_cpp_code_line_here = 0;
<div> another template string here </div>
<div> C++ from template = <CX=s> valid_cpp_code_block_here <=CX> </div>
```

You can pass C++ variables by pointers into cxtpl, that is very usefull if you want to use complex data structures as template parameters.

C++ template engine transpiles template into C++ code, so you will gain VERY good performance and full power of C++.

C++ template engine may run in two modes:
+ compile-mode: compile cxtpl code into C++ file or std::string, then `#include` generated code & compile app as usual. Best performance.
+ cling-mode (C++ JIT executed at runtime): compile cxtpl code into C++ file or std::string, then run generated code in Cling interpreter (no need to recompile app, usefull in dev-mode or for php-style apps).

Again: Think about `.cxtpl` as lambda-function returning std::string. Prefer not to use `#include` from `.cxtpl`, just create `.cxtpl.h` file. Then `#include` both generated `.cxtpl.cpp` and created `.cxtpl.h` in your app code.

Code generated from `.cxtpl` must create variable with name `cxtpl_output`, so structure your code as below:
```
/// \note header is NOT generated, it includes stuff for other generated file
#include "../../resources/cxtpl/typeclass_instance_gen_hpp.cxtpl.h"

// ...

void somefunc() {
  std::string cxtpl_output;

  /// \note this is generated .cpp file, it must not use #include
  #include "../../resources/cxtpl/generated/typeclass_instance_gen_hpp.cxtpl.cpp"

  writeToFile(cxtpl_output, gen_hpp_name);
}
```

You need to `#include` all headers used by template generator in your app code. It is good practice to create separate `.cxtpl.h` file near to your `.cxtpl`. Separation of include files allows to use same includes/logic both in compile-mode (just `#include` your `.cxtpl.h`) and cling-mode (pass contents of your `.cxtpl.h` as function argument). See `enum_gen_hpp.cxtpl.h` and `CXTPL_STD.h` as example.

cxtpl uses approach similar to `How to write a template engine in less than 30 lines of code` from https://bits.theorem.co/how-to-write-a-template-library/

+ `<CX=>` means `start execution of C++ code while parsing template`. Requires `<=CX>` as closing tag.
+ `<=CX>` means `end execution of C++ code while parsing template`
+ `<CX=l>` means `start execution of C++ code while parsing template`. Requires newline (`\n`) as closing tag.
+ `<CX=r>` means `add result of execution of C++ code to output while parsing template`. Result must be string. Requires `<=CX>` as closing tag.
+ `<CX=s>` means `add result of execution of C++ code to output while parsing template`. Result will be converted to string (just wrapped in std::to_string). Requires `<=CX>` as closing tag.

Example before template parsing/transpiling:
```
<CX=> // parameters begin

const std::string generator_path = "somepath";

std::vector<std::string> generator_includes{"someinclude"};

// parameters end
/* no newline, see CX=l */ <=CX><CX=l>
// This is generated file. Do not modify directly.
// Path to the code generator: <CX=r> generator_path <=CX>.

<CX=l> for(const auto& fileName: generator_includes) {
<CX=r> fileName /* CX=r used to append to cxtpl_output */ <=CX>
<CX=l> } // end for
```

Example after template parsing/transpiling:
```
// This is generated file. Do not modify directly.
// Path to the code generator: somepath.

someinclude
```

Usefull links:
+ https://bits.theorem.co/how-to-write-a-template-library/
+ https://lambda.xyz/blog/maud-is-fast/
+ https://dzone.com/articles/modern-type-safe-template-engines
+ http://www.wilxoft.com/
+ https://github.com/djc/askama
+ https://www.reddit.com/r/rust/comments/b06z9m/cuach_a_compiletime_html_template_system/

## How to add `.cxtpl`
Modify `CXTPL_config.cpp` to include list of your `.cxtpl` files. `CXTPL_config.cpp` will be executed at runtime, so you can change code to get files from cmd arguments, parse from `.json`, e.t.c.

It is also possible to use `CXTPL.cpp` without cling, just copy `CXTPL.cpp` to you project with minor fixes. You can also pass typed arguments to template, just copy & modify `CXTPL_AnyDict.cpp` to replace `std::map<std::string, std::any>` with your typed arguments.

## Project status
In development

Currently supports only linux.

Note that you can run linux containers under windows/mac/e.t.c.

### Clone code
```
sudo git submodule sync --recursive
sudo git submodule update --init --recursive --depth 50
# or
sudo git submodule update --force --recursive --init --remote
```

## DEPENDENCIES
```
# Boost
sudo add-apt-repository ppa:boost-latest/ppa
sudo apt-get update && sudo apt-get upgrade
aptitude search boost
sudo apt-get install libboost-dev

# MPI
sudo apt-get install openmpi-bin openmpi-common libopenmpi-dev

# CMake
bash scripts/install_cmake.sh

# Folly deps from https://github.com/facebook/folly#ubuntu-1604-lts
sudo apt-get install \
    libevent-dev \
    libdouble-conversion-dev \
    libgoogle-glog-dev \
    libgflags-dev \
    libiberty-dev \
    liblz4-dev \
    liblzma-dev \
    libsnappy-dev \
    zlib1g-dev \
    binutils-dev \
    libjemalloc-dev \
    libssl-dev \
    pkg-config

# g3log
bash scripts/install_g3log.sh

# gtest
bash scripts/install_gtest.sh

# folly
bash scripts/install_folly.sh
```

## How to build
```
# tested with gcc
export CC=gcc
export CXX=g++
```

```
# create build dir
cmake -E remove_directory build
cmake -E make_directory build
```

```
# configure
cmake -E chdir build cmake -E time cmake -DENABLE_CLING=FALSE -DCMAKE_BUILD_TYPE=Debug ..
# build
cmake -E chdir build cmake -E time cmake --build . -- -j6
```

```
# install lib and CXTPL_tool
cmake -E chdir build make install
./CXTPL_tool --help
```

```
# run CXTPL_tool
cmake -E time cmake -E chdir build/tool ./CXTPL_tool --help
```

```
# example input
echo "file1.cxtpl" >> build/file1.cxtpl
echo "file2.cxtpl" >> build/file2.cxtpl
echo "file3.cxtpl" >> build/file3.cxtpl
echo "file4.cxtpl" >> build/file4.cxtpl
cmake -E time ./build/tool/CXTPL_tool --threads 6 --input_files build/file1.cxtpl build/file2.cxtpl --output_files build/file1.cxtpl.generated.cpp build/file2.cxtpl.generated.cpp
```

## How to use CXTPL_tool
`--help` for list of available commandline options

`--input_files` for input files.

`--output_files` for output files. If not set, than output file names will generated based on input file names.

Number of input files must be equal to the number of output files. File order is important.

```
./build/tool/CXTPL_tool --threads 6 --input_files build/file1.cxtpl build/file2.cxtpl build/file3.cxtpl build/file4.cxtpl --output_files build/file1.cxtpl.generated.cpp build/file2.cxtpl.generated.cpp build/file3.cxtpl.generated.cpp build/file4.cxtpl.generated.cpp -L ".=DBG9"
```

`-L .=DBG9` is log configuration in format https://github.com/facebook/folly/blob/master/folly/logging/docs/Config.md

Example of log configuration which writes both into the file and console stream:
```
./build/tool/CXTPL_tool --threads 6 --input_files build/file1.cxtpl build/file2.cxtpl --output_files build/file1.cxtpl.generated.cpp build/file2.cxtpl.generated.cpp j -L ".:=INFO:default:console; default=file:path=y.log,async=true,sync_level=DBG9;console=stream:stream=stderr"
```

`--srcdir` to change current filesystem path for input files.

`--resdir` to change current filesystem path for output files.

`--global_timeout_ms` to limit execution time for input files (performs check after task completion to stop other tasks from running).

`--single_task_timeout_ms` to limit execution time for singe input file (performs check after task completion to stop other tasks from running).

## Projects that use CXTPL
+ CXXCTP (https://github.com/blockspacer/CXXCTP) is a transpiler that extends C++ for new introspection, reflection and compile-time execution.

## About cling
You can use cling to execute C++ at compile-time/run-time, for hot code reload / REPL / Fast C++ prototyping / Scripting engine / JIT / e.t.c.

Usefull links:
 + (how to add Cling into CMake project) https://github.com/derofim/cling-cmake
 + https://github.com/root-project/cling/tree/master/www/docs/talks
 + https://github.com/caiorss/C-Cpp-Notes/blob/master/Root-cern-repl.org

## RTTI
RTTI enabled only in command line tool (CXTPL_tool), RTTI required by boost.po.

CXTPL library disabled RTTI and uses BOOST_NO_RTTI/BOOST_NO_TYPEID (as private cmake config).

## Similar projects
+ (compile-time) https://github.com/burner/sweet.hpp/tree/master/amber
+ (compile-time) https://github.com/evgeny-panasyuk/ctte
+ (compile-time) https://github.com/rep-movsd/see-phit
+ (run-time) https://github.com/no1msd/mstch
+ (run-time) https://github.com/henryr/cpp-mustache
+ (run-time) https://github.com/pantor/inja
+ (run-time) https://github.com/jinja2cpp/Jinja2Cpp
+ (Rust) Type-safe, compiled Jinja-like templates for Rust https://github.com/djc/askama
+ (article) https://dzone.com/articles/modern-type-safe-template-engines
