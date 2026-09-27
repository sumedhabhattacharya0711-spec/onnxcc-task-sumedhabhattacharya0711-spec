#pragma once

#include <iosfwd>

namespace onnxcc::cli {
//argc/argv: the command line received by the main(), argv[0] is the program name
// out: normal output(std::cout in the real program)
// err: error messages(std::cerr in the real program) 
// [[nodiscard]] the return value is the exit code so ignoring that is almost certainly a bug
[[nodiscard]] int run(int argc, const char* const* argv, std::ostream& out, std::ostream& err);

}  // namespace onnxcc::cli