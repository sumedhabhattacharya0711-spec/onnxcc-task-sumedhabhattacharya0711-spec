#include <iostream>

#include "onnxcc/cli/cli.h"
// Kept deliberately thin: all logic lives in cli::run, so it can be unit tested.
int main(int argc, char** argv) {
    return onnxcc::cli::run(argc, argv, std::cout, std::cerr);
}