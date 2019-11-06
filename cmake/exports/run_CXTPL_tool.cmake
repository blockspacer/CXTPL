string(REPLACE ";" " " INPUTS "${INPUTS}")
string(REPLACE ";" " " OUTPUTS "${OUTPUTS}")

message(STATUS "CXTPL_tool_PROGRAM = ${CXTPL_tool_PROGRAM}")
message(STATUS "input_files
  (for ${CXTPL_tool_PROGRAM}) = ${INPUTS}")
message(STATUS "output_files
  (for ${CXTPL_tool_PROGRAM}) = ${OUTPUTS}")
message(STATUS "INPUTS_DIR
  (for ${CXTPL_tool_PROGRAM}) = ${INPUTS_DIR}")
message(STATUS "OUTPUTS_DIR
  (for ${CXTPL_tool_PROGRAM}) = ${OUTPUTS_DIR}")
message(STATUS "GENERATOR_PATH
  (for ${CXTPL_tool_PROGRAM}) = ${GENERATOR_PATH}")
message(STATUS "CXTPL_EXTRA_ARGS
  (for ${CXTPL_tool_PROGRAM}) = ${CXTPL_EXTRA_ARGS}")

message(
  STATUS "CXTPL_tool_LOG_CONFIG (for ${CXTPL_tool_PROGRAM}) = ${CXTPL_tool_LOG_CONFIG}")

if(NOT ${GENERATOR_PATH} STREQUAL "")
  set(CXTPL_GENERATOR_ARG --generator_path)
  set(CXTPL_GENERATOR_PATH ${GENERATOR_PATH})
endif()

string(REPLACE " " ";" CXTPL_EXTRA_ARGS_as_list "${CXTPL_EXTRA_ARGS}")

message(STATUS "running CXTPL_tool command:
${CXTPL_tool_PROGRAM} \
                        -L \"${CXTPL_tool_LOG_CONFIG}\" \
                        --threads ${THREADS} \
                        --input_files ${INPUTS} \
                        --output_files ${OUTPUTS} \
                        --srcdir ${INPUTS_DIR} \
                        --resdir ${OUTPUTS_DIR} \
                        ${CXTPL_GENERATOR_ARG} ${CXTPL_GENERATOR_PATH} \
                        ${CXTPL_EXTRA_ARGS_as_list} \
")

execute_process(COMMAND ${CXTPL_tool_PROGRAM}
                        -L
                        "${CXTPL_tool_LOG_CONFIG}"
                        --threads
                        ${THREADS}
                        --input_files
                        ${INPUTS}
                        --output_files
                        ${OUTPUTS}
                        --srcdir
                        ${INPUTS_DIR}
                        --resdir
                        ${OUTPUTS_DIR}
                        ${CXTPL_GENERATOR_ARG}
                        ${CXTPL_GENERATOR_PATH}
                        ${CXTPL_EXTRA_ARGS_as_list}
                TIMEOUT 7200 # sec
                RESULT_VARIABLE retcode
                ERROR_VARIABLE _ERROR_VARIABLE)
if(NOT "${retcode}" STREQUAL "0")
  message(FATAL_ERROR "Bad exit status ${retcode} ${_ERROR_VARIABLE}")
endif()
