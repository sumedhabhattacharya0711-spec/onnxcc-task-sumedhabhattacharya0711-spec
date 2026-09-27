# Generates the onnxcc test fixtures: a 4 -> 8 -> 2 MLP with Relu after each layer.
# Graph: 6 nodes (MatMul, Add, Relu, MatMul, Add, Relu), 4 initializers (W1, b1, W2, b2).
# Outputs tests/fixtures/mlp.onnx and tests/fixtures/mlp_input.bin (1x4 float32, 16 raw bytes).

from pathlib import Path

import numpy as np
import onnx
from onnx import TensorProto, helper, numpy_helper
# Pinned so the output doesn't depend on the installed onnx version's defaults.
# IR version 7 is the file-format version that matches opset 13.
SEED = 42
OPSET = 13
IR_VERSION = 7

REPO_ROOT = Path(__file__).resolve().parent.parent
OUT_DIR = REPO_ROOT / "tests" / "fixtures"
MODEL_PATH = OUT_DIR / "mlp.onnx"
INPUT_PATH = OUT_DIR / "mlp_input.bin"

# MatMul computes x @ W, so weights are stored as [in, out]
# (the transpose of PyTorch's nn.Linear layout).
def make_weights(rng: np.random.Generator) -> dict[str, np.ndarray]:
    return {
        "W1": rng.standard_normal((4, 8)).astype(np.float32),
        "b1": rng.standard_normal((8,)).astype(np.float32),
        "W2": rng.standard_normal((8, 2)).astype(np.float32),
        "b2": rng.standard_normal((2,)).astype(np.float32),
    }


def build_model(weights: dict[str, np.ndarray]) -> onnx.ModelProto:
    # Nodes are connected by tensor names: one node's output name is the next node's input name.
    nodes = [
        helper.make_node("MatMul", ["x", "W1"], ["mm1"], name="matmul1"),
        helper.make_node("Add", ["mm1", "b1"], ["h1"], name="add1"),
        helper.make_node("Relu", ["h1"], ["a1"], name="relu1"),
        helper.make_node("MatMul", ["a1", "W2"], ["mm2"], name="matmul2"),
        helper.make_node("Add", ["mm2", "b2"], ["h2"], name="add2"),
        helper.make_node("Relu", ["h2"], ["y"], name="relu2"),
    ]
    initializers = [numpy_helper.from_array(arr, name) for name, arr in weights.items()]
    graph = helper.make_graph(
        nodes,
        "mlp_4_8_2",
        inputs=[helper.make_tensor_value_info("x", TensorProto.FLOAT, [1, 4])],
        outputs=[helper.make_tensor_value_info("y", TensorProto.FLOAT, [1, 2])],
        initializer=initializers,
    )
    model = helper.make_model(
        graph,
        opset_imports=[helper.make_operatorsetid("", OPSET)],
        producer_name="onnxcc-fixtures",
    )
    model.ir_version = IR_VERSION
    # full_check also runs shape inference, so shape mistakes fail here, before anything is saved.
    onnx.checker.check_model(model, full_check=True)
    return model

# The same computation in plain numpy, printed as the expected output
# so the model can be cross-checked against ONNX Runtime.
def reference_forward(x: np.ndarray, weights: dict[str, np.ndarray]) -> np.ndarray:
    a1 = np.maximum(x @ weights["W1"] + weights["b1"], 0)
    return np.maximum(a1 @ weights["W2"] + weights["b2"], 0)


def main() -> None:
    # One seeded generator for the weights and then the input, in a fixed order,
    # makes every run produce identical files.
    rng = np.random.default_rng(SEED)
    weights = make_weights(rng)
    x = rng.standard_normal((1, 4)).astype(np.float32)

    model = build_model(weights)
    OUT_DIR.mkdir(parents=True, exist_ok=True)
    onnx.save(model, MODEL_PATH)
    # '<f4' = little-endian float32 with no header: the same 16 bytes on every machine.
    INPUT_PATH.write_bytes(x.astype("<f4").tobytes())

    assert INPUT_PATH.stat().st_size == 16, "input must be exactly 16 bytes"

    print(f"wrote {MODEL_PATH.relative_to(REPO_ROOT)}: {len(model.graph.node)} nodes, "
          f"{len(model.graph.initializer)} initializers, opset {OPSET}")
    print("op types:", [node.op_type for node in model.graph.node])
    print(f"wrote {INPUT_PATH.relative_to(REPO_ROOT)}: {INPUT_PATH.stat().st_size} bytes")
    print("input:          ", x)
    print("expected output:", reference_forward(x, weights))


if __name__ == "__main__":
    main()