cmake_minimum_required( VERSION 3.10.0 FATAL_ERROR )

set(PROJECT_NAME CXTPL)
set(PROJECT_VERSION 1.0.1)

# Building as separate project.
project(
  ${PROJECT_NAME}
  VERSION ${PROJECT_VERSION}
  LANGUAGES CXX)

list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake)
list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake/modules)
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/build/") # for conan

# Setup conan and include everything.
#include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
#conan_basic_setup()
include(conanbuildinfo)
conan_basic_setup(TARGETS)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fno-rtti")

include( Dependencies.cmake )

option(USE_G3LOG "Use g3log logger" ON)

option(USE_FOLLY "Use facebook/folly library (Apache License 2.0)" ON)

option(USE_RANG "Use RANG for coloring terminal" OFF)

option(BUILD_EXAMPLES "Use built-in examples" OFF)

option(ENABLE_TESTS "Use unit tests" ON)

set( USED_SYSTEM_LIBS
  Threads::Threads # pthread, https://cmake.org/cmake/help/v3.13/module/FindThreads.html
  ${X11_LIBRARIES} # https://cmake.org/cmake/help/v3.13/module/FindX11.html
  ${CMAKE_DL_LIBS} # https://cmake.org/cmake/help/v3.13/variable/CMAKE_DL_LIBS.html
  EXPAT::EXPAT # https://cmake.org/cmake/help/v3.13/module/FindEXPAT.html
  ZLIB::ZLIB # https://cmake.org/cmake/help/v3.13/module/FindZLIB.html
  # Custom libs
  stdc++fs # C++17 std::filesystem
  CACHE INTERNAL "USED_SYSTEM_LIBS" )

set( USED_BOOST_LIBS
  Boost::boost
  Boost::context
  Boost::filesystem
  Boost::program_options
  Boost::regex
  Boost::system
  CACHE INTERNAL "USED_BOOST_LIBS" )

add_library(CXTPL_core STATIC
  # sources
  ${CMAKE_CURRENT_SOURCE_DIR}/src/core/CXTPL.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/core/defaults/defaults.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/codegen/cpp/cpp_codegen.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/core/errors/errors_GeneratorError.cpp
  # includes
  ${CMAKE_CURRENT_SOURCE_DIR}/include/core/CXTPL.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/core/defaults/defaults.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/core/tags.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/core/errors/errors.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/codegen/cpp/cpp_codegen.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/integrations/outcome/error_utils.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/integrations/outcome/error_macros.hpp
)

#message(STATUS G3LOG_LIBRARIES1=${G3LOG_LIBRARIES})

target_link_libraries(CXTPL_core PUBLIC
  ${FOLLY_LIBRARIES}
  # system libs
  ${USED_SYSTEM_LIBS}
  # @note: Order matters https://stackoverflow.com/a/10269201/10904212
  ${LIBIBERTY_LIBRARY} # used by folly
  ${DOUBLE_CONVERSION_LIBRARY} # used by folly
  ${LIBEVENT_LIB} # used by folly
  ${LZ4_LIBRARY} # used by folly
  ${LIBUNWIND_LIBRARIES} # used by folly
  ${LIBLZMA_LIBRARIES} # used by folly
  ${LIBGFLAGS_LIBRARY} # used by folly
  ${G3LOG_LIBRARIES} # used by folly
  glog
  gflags
  # boost libs
  ${USED_BOOST_LIBS}
  boost_outcome
  microsoft_gsl
  #${CONAN_LIBS}
  #CONAN_PKG::catch2
)

target_include_directories(CXTPL_core PUBLIC
  ${CMAKE_CURRENT_SOURCE_DIR}/submodules/boost.outcome/include/outcome/quickcpplib/include/ # TODO
  ${LIBIBERTY_INCLUDE_DIR} # used by folly
  ${G3LOG_INCLUDE_DIR} # used by folly
  ${LIBEVENT_INCLUDE_DIR} # used by folly
  ${LZ4_INCLUDE_DIR} # used by folly
  ${LIBGFLAGS_INCLUDE_DIR} # used by folly
  ${FOLLY_INCLUDE_DIR}
)

target_include_directories(CXTPL_core PRIVATE
  ${CMAKE_CURRENT_SOURCE_DIR}
  ${CMAKE_CURRENT_SOURCE_DIR}/src
)

target_include_directories(CXTPL_core PUBLIC
  ${CMAKE_CURRENT_SOURCE_DIR}/include
)

# POSITION_INDEPENDENT_CODE for -fPIC
set_property(TARGET CXTPL_core PROPERTY POSITION_INDEPENDENT_CODE ON)

#target_compile_options(CXTPL_core PRIVATE
#  -fno-rtti)

#target_compile_definitions( CXTPL_core PRIVATE
#  # https://stackoverflow.com/q/14557313/12073498
#  BOOST_NO_RTTI
#  BOOST_NO_TYPEID
#)

target_compile_definitions( CXTPL_core PUBLIC
  GOOGLE_GLOG_DLL_DECL=
  # https://stackoverflow.com/a/30877725
  #BOOST_SYSTEM_NO_DEPRECATED
  #BOOST_ERROR_CODE_HEADER_ONLY
)

set_target_properties(CXTPL_core
  PROPERTIES ENABLE_EXPORTS 1)

install(TARGETS CXTPL_core DESTINATION "${CMAKE_INSTALL_PREFIX}/lib")

add_subdirectory(tool)

if(ENABLE_TESTS)
  # Now enable our tests.
  enable_testing()
  add_subdirectory(tests)
endif(ENABLE_TESTS)

if(BUILD_EXAMPLES)
  add_subdirectory(examples)
endif(BUILD_EXAMPLES)
