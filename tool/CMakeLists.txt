cmake_minimum_required( VERSION 3.10.0 FATAL_ERROR )

# Building as separate project.
project(CXTPL_tool)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fno-rtti")

set( BOOST_ROOT CACHE STRING /usr )
set( Boost_ADDITIONAL_VERSIONS "1.62 1.63 1.64 1.65 1.66 1.67 1.68 1.69" )
set( BOOST_LIBS CACHE STRING ${BOOST_ROOT}/lib )
find_package( Boost
  COMPONENTS program_options filesystem regex date_time system thread graph log
  EXACT REQUIRED )

find_package( Threads REQUIRED )
message(STATUS "CMAKE_THREAD_LIBS_INIT = ${CMAKE_THREAD_LIBS_INIT}")

find_package( X11 REQUIRED )
message(STATUS "X11_LIBRARIES = ${X11_LIBRARIES}")

find_package( EXPAT REQUIRED )
message(STATUS "EXPAT_LIBRARIES = ${EXPAT_LIBRARIES}")

find_package( ZLIB REQUIRED )
message(STATUS "ZLIB_LIBRARIES = ${ZLIB_LIBRARIES}")

message(STATUS "CMAKE_DL_LIBS = ${CMAKE_DL_LIBS}")

# The project has one binary:
add_executable(CXTPL_tool
  ${CMAKE_CURRENT_SOURCE_DIR}/src/main_native.cpp
)

set( USED_SYSTEM_LIBS
  Threads::Threads # pthread, https://cmake.org/cmake/help/v3.13/module/FindThreads.html
  ${X11_LIBRARIES} # https://cmake.org/cmake/help/v3.13/module/FindX11.html
  ${CMAKE_DL_LIBS} # https://cmake.org/cmake/help/v3.13/variable/CMAKE_DL_LIBS.html
  EXPAT::EXPAT # https://cmake.org/cmake/help/v3.13/module/FindEXPAT.html
  ZLIB::ZLIB # https://cmake.org/cmake/help/v3.13/module/FindZLIB.html
  # Custom libs
  stdc++fs # C++17 std::filesystem
  CACHE INTERNAL "USED_SYSTEM_LIBS" )

# TODO: Catch FakeIt

#message(STATUS G3LOG_LIBRARIES2=${G3LOG_LIBRARIES})

target_link_libraries(CXTPL_tool PUBLIC
  # system libs
  ${USED_SYSTEM_LIBS}
  # boost libs
  ${USED_BOOST_LIBS}
  boost_outcome
  microsoft_gsl
  double-conversion
  # CXTPL
  CXTPL_core
  ${FOLLY_LIBRARIES}
  # @note: Order matters https://stackoverflow.com/a/10269201/10904212
  ${LIBIBERTY_LIBRARY} # used by folly
  ${LIBEVENT_LIB} # used by folly
  ${LZ4_LIBRARY} # used by folly
  ${LIBGFLAGS_LIBRARY} # used by folly
  ${G3LOG_LIBRARIES} # used by folly
  glog
  gflags
)

#get_target_property (glog_IMPORTED_LOCATION glog INTERFACE_INCLUDE_DIRECTORIES)
#message( "glog_IMPORTED_LOCATION=${glog_IMPORTED_LOCATION}" )
#
#get_target_property (glog_IMPORTED_LIB glog INTERFACE_LINK_LIBRARIES)
#message( "glog_IMPORTED_LIB=${glog_IMPORTED_LIB}" )

target_include_directories(CXTPL_core PUBLIC
  ${LIBIBERTY_INCLUDE_DIR} # used by folly
  ${G3LOG_INCLUDE_DIR} # used by folly
  ${LIBEVENT_INCLUDE_DIR} # used by folly
  ${LZ4_INCLUDE_DIR} # used by folly
  ${LIBGFLAGS_INCLUDE_DIR} # used by folly
  ${FOLLY_INCLUDE_DIR}
)

target_compile_options(CXTPL_tool PRIVATE
  -frtti)

set_target_properties(CXTPL_tool
  PROPERTIES ENABLE_EXPORTS 1)

install(TARGETS CXTPL_tool DESTINATION bin)
