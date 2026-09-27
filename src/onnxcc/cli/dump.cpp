#include <filesystem>
#include <ostream>
#include <string>
#include <system_error>

#include <cxxopts.hpp>

#include "onnxcc/cli/subcommands.h"

namespace onnxcc::cli {

int run_dump(int argc, const char* const* argv, std::ostream& out, std::ostream& err) {
    cxxopts::Options options("onnxcc dump", "Inspect an ONNX model");
    auto add_option = options.add_options();
    add_option("m,model", "Path to the ONNX model file", cxxopts::value<std::string>(), "PATH");
    add_option("show-graph", "Print the graph structure");
    add_option("verbose", "Print the extra details");
    add_option("h,help", "Show this help message");

    try {
        const auto result = options.parse(argc, argv);
        if (result.count("help") > 0) {
            out << options.help();
            return kExitOk;
        }
        if (!result.unmatched().empty()) {
            err << "onnxcc dump: unexpected argument '" << result.unmatched().front() << "'\n"
                << "Try 'onnxcc dump --help'.\n";
            return kExitUsage;
        }
        if (result.count("model") == 0) {
            err << "onnxcc dump: missing required option --model\n"
                << "Try 'onnxcc dump --help'.\n";
            return kExitUsage;
        }
        const auto model = result["model"].as<std::string>();
        std::error_code ec;
        if (!std::filesystem::is_regular_file(model, ec)) {
            err << "onnxcc dump: cannot open model file: " << model << "\n";
            return kExitFailure;
        }

        out << "model: " << model << "\n";
        if (result.count("verbose") > 0) {
            out << "verbose: on\n";
        }
        if (result.count("show-graph") > 0) {
            out << "graph: (not implemented yet)\n";
        }
        return kExitOk;
    } catch (const cxxopts::exceptions::exception& e) {
        err << "onnxcc dump: " << e.what() << "\n"
            << "Try 'onnxcc dump --help'.\n";
        return kExitUsage;
    }
}

}  // namespace onnxcc::cli