#include <gtest/gtest.h>

#include <filesystem>
#include <sstream>
#include <string>
#include <vector>

#include "onnxcc/cli/cli.h"

namespace {

struct CliResult {
    int code;
    std::string out;
    std::string err;
};

CliResult run_cli(std::vector<const char*> args) {
    args.insert(args.begin(), "onnxcc");
    std::ostringstream out;
    std::ostringstream err;
    const int code = onnxcc::cli::run(static_cast<int>(args.size()), args.data(), out, err);
    return {code, out.str(), err.str()};
}

bool contains(const std::string& text, const std::string& part) {
    return text.find(part) != std::string::npos;
}

const std::string kFixtureDir = ONNXCC_FIXTURE_DIR;
const std::string kModel = kFixtureDir + "/mlp.onnx";
const std::string kInput = kFixtureDir + "/mlp_input.bin";

}  // namespace



TEST(CliTopLevel, NoArgumentsIsUsageError) {
    const auto r = run_cli({});
    EXPECT_EQ(r.code, 2);
    EXPECT_TRUE(r.out.empty());
    EXPECT_TRUE(contains(r.err, "Usage:")) << "stderr: " << r.err;
}

TEST(CliTopLevel, HelpGoesToStdout) {
    for (const char* flag : {"-h", "--help"}) {
        const auto r = run_cli({flag});
        EXPECT_EQ(r.code, 0) << flag;
        EXPECT_TRUE(contains(r.out, "Usage:")) << flag;
        EXPECT_TRUE(r.err.empty()) << flag;
    }
}

TEST(CliTopLevel, UnknownSubcommandIsUsageError) {
    const auto r = run_cli({"explode"});
    EXPECT_EQ(r.code, 2);
    EXPECT_TRUE(r.out.empty());
    EXPECT_TRUE(contains(r.err, "unknown subcommand 'explode'")) << "stderr: " << r.err;
}

TEST(CliTopLevel, OptionBeforeSubcommandIsUsageError) {
    const auto r = run_cli({"--model", kModel.c_str()});
    EXPECT_EQ(r.code, 2);
    EXPECT_TRUE(r.out.empty());
    EXPECT_FALSE(r.err.empty());
}



TEST(CliDump, HelpGoesToStdout) {
    const auto r = run_cli({"dump", "--help"});
    EXPECT_EQ(r.code, 0);
    EXPECT_TRUE(contains(r.out, "--model")) << "stdout: " << r.out;
    EXPECT_TRUE(r.err.empty());
}

TEST(CliDump, ValidModelSucceeds) {
    ASSERT_TRUE(std::filesystem::exists(kModel)) << "missing fixture: " << kModel;
    const auto r = run_cli({"dump", "--model", kModel.c_str()});
    EXPECT_EQ(r.code, 0);
    EXPECT_TRUE(contains(r.out, "model: " + kModel)) << "stdout: " << r.out;
    EXPECT_TRUE(r.err.empty()) << "stderr: " << r.err;
}

TEST(CliDump, EqualsSyntaxIsAccepted) {
    const std::string arg = "--model=" + kModel;
    const auto r = run_cli({"dump", arg.c_str()});
    EXPECT_EQ(r.code, 0);
    EXPECT_TRUE(r.err.empty());
}

TEST(CliDump, OptionalFlagsInAnyOrder) {
    const auto r = run_cli({"dump", "--verbose", "--show-graph", "--model", kModel.c_str()});
    EXPECT_EQ(r.code, 0);
    EXPECT_TRUE(contains(r.out, "verbose")) << "stdout: " << r.out;
    EXPECT_TRUE(contains(r.out, "graph")) << "stdout: " << r.out;
    EXPECT_TRUE(r.err.empty());
}



TEST(CliDump, MissingModelIsUsageError) {
    const auto r = run_cli({"dump"});
    EXPECT_EQ(r.code, 2);
    EXPECT_TRUE(r.out.empty());
    EXPECT_TRUE(contains(r.err, "--model")) << "stderr: " << r.err;
}

TEST(CliDump, ModelWithoutValueIsUsageError) {
    const auto r = run_cli({"dump", "--model"});
    EXPECT_EQ(r.code, 2);
    EXPECT_TRUE(r.out.empty());
    EXPECT_FALSE(r.err.empty());
}

TEST(CliDump, UnknownFlagIsUsageError) {
    const auto r = run_cli({"dump", "--model", kModel.c_str(), "--banana"});
    EXPECT_EQ(r.code, 2);
    EXPECT_TRUE(r.out.empty());
    EXPECT_TRUE(contains(r.err, "banana")) << "stderr: " << r.err;
}

TEST(CliDump, StrayArgumentIsUsageError) {
    const auto r = run_cli({"dump", "--model", kModel.c_str(), "extra"});
    EXPECT_EQ(r.code, 2);
    EXPECT_TRUE(r.out.empty());
    EXPECT_TRUE(contains(r.err, "extra")) << "stderr: " << r.err;
}

TEST(CliDump, NonexistentModelIsRuntimeError) {
    const auto r = run_cli({"dump", "--model", "does_not_exist.onnx"});
    EXPECT_EQ(r.code, 1);
    EXPECT_TRUE(r.out.empty());
    EXPECT_TRUE(contains(r.err, "does_not_exist.onnx")) << "stderr: " << r.err;
}



TEST(Fixture, InputIsSixteenBytes) {
    ASSERT_TRUE(std::filesystem::exists(kInput)) << "missing fixture: " << kInput;
    EXPECT_EQ(std::filesystem::file_size(kInput), 16u);
}