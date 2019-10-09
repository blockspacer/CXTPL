cmake_minimum_required( VERSION 3.10.0 FATAL_ERROR )

set(PROJECT_NAME CXTPL_tool)
set(PROJECT_VERSION 1.0.1)

# Building as separate project.
project(${PROJECT_NAME}
  VERSION ${PROJECT_VERSION}
  LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fno-rtti")

# The project has one binary:
add_executable(CXTPL_tool
  ${CMAKE_CURRENT_SOURCE_DIR}/src/main_native.cpp
)

target_link_libraries(CXTPL_tool PUBLIC
  CXTPL_core
)

target_include_directories(CXTPL_tool PUBLIC
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

# POSITION_INDEPENDENT_CODE for -fPIC
set_property(TARGET CXTPL_tool PROPERTY POSITION_INDEPENDENT_CODE ON)

target_include_directories(CXTPL_tool PUBLIC
  ${PROJECT_BINARY_DIR} # path to version.hpp
)

configure_file(${CMAKE_CURRENT_SOURCE_DIR}/includes/version.hpp.in
  ${PROJECT_BINARY_DIR}/version.hpp)

set_source_files_properties(${PROJECT_BINARY_DIR}/version.hpp
  PROPERTIES GENERATED 1)

get_target_property(CXTPL_tool_RUNTIME_FILES_DIRECTORY CXTPL_tool
                    RUNTIME_OUTPUT_DIRECTORY)

add_custom_target(CXTPL_tool-copy-runtime-files ALL
                  COMMAND cmake
                          -E
                          copy
                          ${CMAKE_CURRENT_SOURCE_DIR}/resources/icu/CXTPL_icudtl.dat
                          ${CXTPL_tool_RUNTIME_FILES_DIRECTORY}/CXTPL_icudtl.dat)

add_dependencies(CXTPL_tool CXTPL_tool-copy-runtime-files)

install(TARGETS CXTPL_tool DESTINATION "${CMAKE_INSTALL_PREFIX}/bin")

install(
  DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/resources/icu/" # source directory
  DESTINATION "${CMAKE_INSTALL_PREFIX}/bin" # target directory
  FILES_MATCHING # install only matched files
  PATTERN "*.dat"
  PATTERN "TODO" EXCLUDE)
