#include <iostream>

#include "onnxcc/cli/cli.h"

int main(int argc, char** argv) {
    return onnxcc::cli::run(argc, argv, std::cout, std::cerr);
}