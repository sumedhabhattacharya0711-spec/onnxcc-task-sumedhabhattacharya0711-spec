#pragma once

#include <iosfwd>

namespace onnxcc::cli {

inline constexpr int kExitOk = 0;
inline constexpr int kExitFailure = 1;
inline constexpr int kExitUsage = 2;

int run_dump(int argc, const char* const* argv, std::ostream& out, std::ostream& err);

}  // namespace onnxcc::cli