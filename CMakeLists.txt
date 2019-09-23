cmake_minimum_required(VERSION 3.1.3)

# Building as separate project.
set(PROJECT_NAME CXTPL)
set(PROJECT_VERSION 1.0.1)

project(
  ${PROJECT_NAME}
  VERSION ${PROJECT_VERSION}
  LANGUAGES CXX)

list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake)
list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake/modules)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fno-rtti")

include( Dependencies.cmake )

option(USE_G3LOG "Use g3log logger" ON)

option(USE_FOLLY "Use facebook/folly library (Apache License 2.0)" ON)

option(USE_RANG "Use RANG for coloring terminal" ON)

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
  Boost::atomic
  Boost::chrono
  Boost::date_time
  Boost::filesystem
  Boost::graph
  Boost::log
  Boost::program_options
  Boost::regex
  Boost::system
  Boost::thread
  CACHE INTERNAL "USED_BOOST_LIBS" )

add_library(CXTPL_core
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
  ${CMAKE_CURRENT_SOURCE_DIR}/include/interations/outcome/error_utils.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/interations/outcome/error_macros.hpp
)

#message(STATUS G3LOG_LIBRARIES1=${G3LOG_LIBRARIES})

target_link_libraries(CXTPL_core PUBLIC
  # system libs
  ${USED_SYSTEM_LIBS}
  # boost libs
  ${USED_BOOST_LIBS}
  #${FOLLY_LIBRARIES}
  boost_outcome
  microsoft_gsl
  double-conversion
  # @note: Order matters https://stackoverflow.com/a/10269201/10904212
  #${LIBIBERTY_LIBRARY} # used by folly
  #${LIBEVENT_LIB} # used by folly
  #${G3LOG_LIBRARIES} # used by folly
  #glog
  #gflags
)

target_include_directories(CXTPL_core PUBLIC
  ${CMAKE_CURRENT_SOURCE_DIR}/src
  ${CMAKE_CURRENT_SOURCE_DIR}/include
)

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

install(TARGETS CXTPL_core DESTINATION lib)

add_subdirectory(tool)

add_subdirectory(examples)
