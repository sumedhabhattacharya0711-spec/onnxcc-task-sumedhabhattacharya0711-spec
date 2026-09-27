#pragma once

#include <iosfwd>
// Internal to src/onnxcc/cli/: shared by the dispatcher (cli.cpp) and the subcommand handlers.
namespace onnxcc::cli {

inline constexpr int kExitOk = 0; //success including help
inline constexpr int kExitFailure = 1; //command valid but the work failed
inline constexpr int kExitUsage = 2;// invalid command
// Subcommand handlers. Each receives argv with its own name as argv[0], writes results to
// `out` and errors to `err`, and returns one of the exit codes above.
//
// To add a subcommand:
//   1. implement run_<name> in src/onnxcc/cli/<name>.cpp
//   2. declare it here
//   3. add an entry to kSubcommands in cli.cpp
//   4. add the .cpp file to onnxcc_lib in src/CMakeLists.txt
int run_dump(int argc, const char* const* argv, std::ostream& out, std::ostream& err);

}  // namespace onnxcc::cli