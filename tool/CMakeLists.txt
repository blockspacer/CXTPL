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
  # Custom libs
  stdc++fs # C++17 std::filesystem
  CACHE INTERNAL "USED_SYSTEM_LIBS" )

target_link_libraries(CXTPL_tool
  # system libs
  ${USED_SYSTEM_LIBS}
  # boost libs
  ${USED_BOOST_LIBS}
  # CXTPL
  CXTPL_core
)

#target_compile_options(CXTPL_tool PRIVATE
#  -fno-rtti)

set_target_properties(CXTPL_tool
  PROPERTIES ENABLE_EXPORTS 1)
