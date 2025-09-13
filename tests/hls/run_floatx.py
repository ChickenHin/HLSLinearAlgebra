import os

from run_vitis import run_vitis

# --- Configuration ---
PART = 'xc7z020clg400-1'
CLOCK_PERIOD_NS = "10"
COMPONENT_NAME = "floatX_hls"
TOP_FUNCTION_NAME = "floatx_top"
SYNTHESIS_FILE = "floatX_top.cpp"
TESTBENCH_FILE = "floatX_test.cpp"

def main():
    """Main function to run the Vitis HLS flow."""
    # --- Paths ---
    # The script is expected to be run from the 'tests/hls' directory
    cwd = os.getcwd()
    project_root = os.path.abspath(os.path.join(cwd, '..', '..'))
    include_path = os.path.join(project_root, 'include')
    workspace_path = os.path.join(cwd, "vitis_workspace")

    run_vitis(workspace_path, include_path, PART, CLOCK_PERIOD_NS, COMPONENT_NAME, TOP_FUNCTION_NAME, SYNTHESIS_FILE, TESTBENCH_FILE, 700)

if __name__ == "__main__":
    main()