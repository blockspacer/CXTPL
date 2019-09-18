# Copyright (c) 2019 Denis Trofimov (den.a.trofimov@yandex.ru)
# Distributed under the MIT License.
# See accompanying file LICENSE.md or copy at http://opensource.org/licenses/MIT

#
# If you have built boost statically you will need to set the Boost_USE_STATIC_LIBS CMake variable to ON
# also don`t forget to set DBOOST_LOG_DYN_LINK https://stackoverflow.com/a/17868918/10904212
# set( Boost_USE_STATIC_LIBS FALSE )
# set( Boost_USE_STATIC_RUNTIME FALSE )
# set( Boost_USE_MULTITHREADED TRUE )
set( BOOST_ROOT CACHE STRING /usr )
set( Boost_ADDITIONAL_VERSIONS "1.62 1.63 1.64 1.65 1.66 1.67 1.68 1.69" )
set( BOOST_LIBS CACHE STRING ${BOOST_ROOT}/lib )

find_package( Boost
  COMPONENTS program_options filesystem regex date_time system thread graph log
  EXACT REQUIRED )

add_library( boost_outcome INTERFACE )
target_include_directories( boost_outcome SYSTEM INTERFACE "submodules/boost.outcome/include" )
get_target_property (BOOST_OUTCOME_IMPORTED_LOCATION boost_outcome INTERFACE_INCLUDE_DIRECTORIES)
message( "boost_outcome=${BOOST_OUTCOME_IMPORTED_LOCATION}" )

add_library(microsoft_gsl INTERFACE)
target_include_directories(microsoft_gsl SYSTEM INTERFACE "submodules/GSL/include")
get_target_property (microsoft_gsl_IMPORTED_LOCATION microsoft_gsl INTERFACE_INCLUDE_DIRECTORIES)
message( "microsoft_gsl=${microsoft_gsl_IMPORTED_LOCATION}" )

find_package( Threads REQUIRED )
message(STATUS "CMAKE_THREAD_LIBS_INIT = ${CMAKE_THREAD_LIBS_INIT}")

option(USE_FOLLY "Use facebook/folly library (Apache License 2.0)" ON)
if(USE_FOLLY)
  findPackageCrossPlatform( Folly REQUIRED )
endif()

find_package( X11 REQUIRED )
message(STATUS "X11_LIBRARIES = ${X11_LIBRARIES}")

find_package( EXPAT REQUIRED )
message(STATUS "EXPAT_LIBRARIES = ${EXPAT_LIBRARIES}")

find_package( ZLIB REQUIRED )
message(STATUS "ZLIB_LIBRARIES = ${ZLIB_LIBRARIES}")

message(STATUS "CMAKE_DL_LIBS = ${CMAKE_DL_LIBS}")
