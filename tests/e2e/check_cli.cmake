# End-to-end check: runs the onnxcc binary and verifies its exit code and output streams.
# Invoked by CTest as:
#   cmake -DEXE=<binary> -DARGS=<a|b|c> -DEXPECT_CODE=<n> -DEXPECT_STREAM=stdout|stderr -P check_cli.cmake
# ARGS uses '|' as a separator because ';' would be split by add_test before reaching this script.


string(REPLACE "|" ";" cli_args "${ARGS}")

execute_process(
    COMMAND "${EXE}" ${cli_args}
    RESULT_VARIABLE code
    OUTPUT_VARIABLE out
    ERROR_VARIABLE err
)

set(report "command: ${EXE} ${cli_args}\nexit code: ${code}\nstdout:\n${out}\nstderr:\n${err}")

if(NOT code EQUAL EXPECT_CODE)
    message(FATAL_ERROR "expected exit code ${EXPECT_CODE}\n${report}")
endif()

if(EXPECT_STREAM STREQUAL "stdout")
    set(expected "${out}")
    set(other "${err}")
elseif(EXPECT_STREAM STREQUAL "stderr")
    set(expected "${err}")
    set(other "${out}")
else()
    message(FATAL_ERROR "EXPECT_STREAM must be stdout or stderr, got '${EXPECT_STREAM}'")
endif()

if(expected STREQUAL "")
    message(FATAL_ERROR "expected output on ${EXPECT_STREAM}\n${report}")
endif()
if(NOT other STREQUAL "")
    message(FATAL_ERROR "expected nothing on the other stream\n${report}")
endif()