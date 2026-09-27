configure_file(onnxcc/version.h.in onnxcc/version.h @ONLY)
# CLI sources live in onnxcc/cli/; add each new subcommand's .cpp file here.
add_library(onnxcc_lib STATIC
    onnxcc/version.cpp
    onnxcc/cli/cli.cpp
    onnxcc/cli/dump.cpp
)

add_executable(onnxcc onnxcc/main.cpp)

# Link the onnxcc_lib library to the onnxcc executable
target_link_libraries(onnxcc PRIVATE onnxcc_lib)

target_link_libraries(onnxcc_lib
    PRIVATE
        protobuf::libprotobuf
        onnx_proto
        cxxopts::cxxopts
)

# Include the current source directory for the onnxcc_lib target
target_include_directories(onnxcc_lib
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${CMAKE_CURRENT_BINARY_DIR}
)

target_compile_options(onnxcc_lib PRIVATE ${ONNXCC_WARNINGS})
target_compile_options(onnxcc PRIVATE ${ONNXCC_WARNINGS})