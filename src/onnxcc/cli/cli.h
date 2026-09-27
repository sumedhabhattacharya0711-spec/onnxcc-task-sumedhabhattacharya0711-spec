#pragma once

#include <iosfwd>

namespace onnxcc::cli {

[[nodiscard]] int run(int argc, const char* const* argv, std::ostream& out, std::ostream& err);

}  // namespace onnxcc::cli