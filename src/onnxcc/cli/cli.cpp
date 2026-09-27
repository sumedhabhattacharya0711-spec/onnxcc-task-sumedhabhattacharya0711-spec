#include "onnxcc/cli/cli.h"

#include <algorithm>
#include <array>
#include <cstddef>
#include <ostream>
#include <string>
#include <string_view>

#include "onnxcc/cli/subcommands.h"

namespace onnxcc::cli {
namespace {

using SubcommandHandler = int (*)(int, const char* const*, std::ostream&, std::ostream&);

struct Subcommand {
    std::string_view name;
    std::string_view description;
    SubcommandHandler handler;
};

constexpr std::array kSubcommands{
    Subcommand{"dump", "Inspect an ONNX model", &run_dump},
};

const Subcommand* find_subcommand(std::string_view name) {
    for (const auto& cmd : kSubcommands) {
        if (cmd.name == name) {
            return &cmd;
        }
    }
    return nullptr;
}

void print_usage(std::ostream& os) {
    std::size_t width = 0;
    for (const auto& cmd : kSubcommands) {
        width = std::max(width, cmd.name.size());
    }

    os << "Usage: onnxcc <subcommand> [options]\n"
          "\n"
          "Subcommands:\n";
    for (const auto& cmd : kSubcommands) {
        os << "  " << cmd.name << std::string(width - cmd.name.size() + 4, ' ') << cmd.description
           << "\n";
    }
    os << "\n"
          "Run 'onnxcc <subcommand> --help' for more information on a subcommand.\n";
}

}  // namespace

int run(int argc, const char* const* argv, std::ostream& out, std::ostream& err) {
    if (argc < 2) {
        print_usage(err);
        return kExitUsage;
    }
    const std::string_view sub = argv[1];
    if (sub == "-h" || sub == "--help") {
        print_usage(out);
        return kExitOk;
    }
    if (const Subcommand* cmd = find_subcommand(sub)) {
        return cmd->handler(argc - 1, argv + 1, out, err);
    }
    if (sub.starts_with('-')) {
        err << "onnxcc: expected a subcommand before options, got '" << sub << "'\n";
    } else {
        err << "onnxcc: unknown subcommand '" << sub << "'\n";
    }
    print_usage(err);
    return kExitUsage;
}

}  // namespace onnxcc::cli