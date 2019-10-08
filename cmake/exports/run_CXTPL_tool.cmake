string(REPLACE ";" " " INPUTS "${INPUTS}")
string(REPLACE ";" " " OUTPUTS "${OUTPUTS}")

message(STATUS "CXTPL_tool_PROGRAM = ${CXTPL_tool_PROGRAM}")
message(STATUS "input_files (for ${CXTPL_tool_PROGRAM}) = ${INPUTS}")
message(STATUS "output_files (for ${CXTPL_tool_PROGRAM}) = ${OUTPUTS}")
message(STATUS "INPUTS_DIR (for ${CXTPL_tool_PROGRAM}) = ${INPUTS_DIR}")
message(STATUS "OUTPUTS_DIR (for ${CXTPL_tool_PROGRAM}) = ${OUTPUTS_DIR}")

message(
  STATUS "CXTPL_tool_LOG_CONFIG (for ${CXTPL_tool_PROGRAM}) = ${CXTPL_tool_LOG_CONFIG}")

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
                TIMEOUT 7200 # sec
                RESULT_VARIABLE retcode
                ERROR_VARIABLE _ERROR_VARIABLE)
if(NOT "${retcode}" STREQUAL "0")
  message(FATAL_ERROR "Bad exit status ${retcode} ${_ERROR_VARIABLE}")
endif()
