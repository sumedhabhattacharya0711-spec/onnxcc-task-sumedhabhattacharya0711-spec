# onnxcc

A C++20 inference engine for ONNX models, built from scratch. This repository currently contains the
selection-task work: the CLI skeleton, the test-fixture generator, and the tests.

## Build and test

Requirements: CMake ≥ 3.26, a C++20 compiler. Dependencies (protobuf, ONNX, GoogleTest) are fetched
at configure time; cxxopts v3.3.1 is vendored in `third_party/cxxopts/`.

```bash
cmake -S . -B build
cmake --build build -j4
ctest --test-dir build --output-on-failure
```

Presets are also available: `cmake --workflow --preset debug` (configure, build, test).

Tested on Ubuntu (WSL2) with GCC 15.2 and CMake 4.2. The first build compiles protobuf and ONNX
from source and takes a while.

## CLI

```bash
./build/onnxcc dump --model path/to/model.onnx [--show-graph] [--verbose]
./build/onnxcc --help
./build/onnxcc dump --help
```

| Code | Meaning |
|---|---|
| 0 | Success, including help that was asked for (stdout) |
| 1 | Command was valid but failed, e.g. model file not found (stderr) |
| 2 | Usage error: missing `--model`, unknown option/subcommand, stray argument (stderr) |

Nothing is written to stdout when a command fails. `dump` is currently a skeleton: it validates
the arguments and that the model file exists, then echoes the path and flags. It does not parse the
model yet.

## Test fixtures

```bash
python3 -m venv .venv && source .venv/bin/activate
pip install -r scripts/requirements.txt
python scripts/generate_test_models.py
```

Writes `tests/fixtures/mlp.onnx` (4 → 8 → 2 MLP, Relu after each layer, opset 13, 6 nodes,
4 initializers) and `tests/fixtures/mlp_input.bin` (one 1×4 float32 input, 16 bytes). Output is
deterministic (seed 42). The generated files are not committed; the tests do not need them.

## Layout

```
src/onnxcc/main.cpp        thin entry point, calls cli::run
src/onnxcc/cli/            argument parsing and subcommands
  cli.h / cli.cpp          public entry point, subcommand table, usage text
  subcommands.h            exit codes and handler declarations (internal)
  dump.cpp                 the dump subcommand
tests/unit/                GoogleTest unit tests (call cli::run with in-memory streams)
tests/e2e/                 end-to-end tests that run the real onnxcc binary
scripts/                   fixture generator
third_party/cxxopts/       vendored cxxopts header and license
```

**Adding a subcommand:** write a handler in `src/onnxcc/cli/<name>.cpp`, declare it in
`subcommands.h`, add one entry to `kSubcommands` in `cli.cpp`, and list the file in
`src/CMakeLists.txt`. Dispatch and the usage text update automatically.

## Tests

- 13 GoogleTest cases, one per row of the CLI contract above: exit code, and which stream got the
  message (and that the other stream stayed empty).
- 5 end-to-end CTest tests run the built binary, covering what unit tests cannot: `main.cpp`
  wiring the real stdout/stderr and returning the exit code.

## Decisions and notes

- Stray positional arguments (`dump --model m.onnx extra`) are rejected; cxxopts ignores them by default.
- If `--help` is combined with an invalid option, the parse error wins (exit 2).
- Test warning flags: `unit_tests` now builds with the same `-Wall -Wextra -Wpedantic` as the library.
- Once `dump` parses models, tests will need a real model. Options: generate fixtures during the
  build, commit an approved fixture, or construct the model in C++ inside the test.
- Possible improvement: `EXCLUDE_FROM_ALL` on the FetchContent dependencies would skip building
  unused targets, but needs CMake 3.28 (the project requires 3.26).