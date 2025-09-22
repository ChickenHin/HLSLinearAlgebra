import vitis
import os
import sys

sys.path.append("../..")

from run_vitis import run_vitis

# --- Configuration ---
PART = 'xc7z020clg400-1'
CLOCK_PERIOD_NS = "10"
COMPONENT_NAME = "linalg_so3exp_hls"
TOP_FUNCTION_NAME = "linalg_so3exp_top"
SYNTHESIS_FILE = "linalg_so3exp_top.cpp"
TESTBENCH_FILE = "linalg_so3exp_test.cpp"


def main():
    """Run the Vitis HLS flow for SO3 exponential map."""
    cwd = os.getcwd()
    project_root = os.path.abspath(os.path.join(cwd, '..', '..', '..', '..'))
    include_path = os.path.join(project_root, 'include')
    workspace_path = os.path.join(project_root, "build/vitis_workspace_linalg_so3exp")

    run_vitis(workspace_path, include_path, PART, CLOCK_PERIOD_NS, COMPONENT_NAME,
              TOP_FUNCTION_NAME, SYNTHESIS_FILE, TESTBENCH_FILE, latency_threshold=500)

if __name__ == "__main__":
    main()
