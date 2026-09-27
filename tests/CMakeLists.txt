add_executable(unit_tests
    unit/test_sanity.cpp
    unit/test_cli.cpp
)

# link the onnxcc_lib library to the unit_tests executable.
# link the gtest_main library to the unit_tests executable so that the main function for the tests is provided by gtest_main.
target_link_libraries(unit_tests
    PRIVATE
        onnxcc_lib
        GTest::gtest_main
)
target_compile_options(unit_tests PRIVATE ${ONNXCC_WARNINGS})

# Make the directory path available to the executable as a preprocessor macro because ctest runs from build/ while relative paths are relative to source dir
# Build tests with the same warning flags as the library, so problems in test code are caught too.
target_compile_definitions(unit_tests
    PRIVATE
        ONNXCC_FIXTURE_DIR="${CMAKE_CURRENT_SOURCE_DIR}/fixtures"
) 

include(GoogleTest)
# run the exe in background and discover all the tests in the unit_tests executable and register them with CTest. 
gtest_discover_tests(unit_tests)
function(onnxcc_add_e2e_test name args expected_code expected_stream)
    add_test(
        NAME e2e.${name}
        COMMAND ${CMAKE_COMMAND}
            -DEXE=$<TARGET_FILE:onnxcc>
            -DARGS=${args}
            -DEXPECT_CODE=${expected_code}
            -DEXPECT_STREAM=${expected_stream}
            -P ${CMAKE_CURRENT_SOURCE_DIR}/e2e/check_cli.cmake
    )
endfunction()

set(ONNXCC_E2E_MODEL "${CMAKE_CURRENT_BINARY_DIR}/e2e_model.onnx")
# Empty model file in the build tree, so these tests don't depend on generated fixtures.
file(TOUCH "${ONNXCC_E2E_MODEL}")

# End-to-end tests run the real binary, covering what unit tests cannot reach:
# main.cpp wiring the real stdout/stderr and returning the exit code.
onnxcc_add_e2e_test(top_level_help "--help" 0 stdout)
onnxcc_add_e2e_test(no_arguments "" 2 stderr)
onnxcc_add_e2e_test(dump_valid_model "dump|--model|${ONNXCC_E2E_MODEL}" 0 stdout)
onnxcc_add_e2e_test(unknown_subcommand "explode" 2 stderr)
onnxcc_add_e2e_test(missing_model_file "dump|--model|does_not_exist.onnx" 1 stderr)