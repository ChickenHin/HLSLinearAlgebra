# Linear Algebra Library

A unified linear algebra library supporting both **CPU** and **FPGA** deployment with conditional HLS optimization, plus optional HLS-specific numerical formats for high-performance computing.

## Architecture

The library uses a **single implementation** with conditional compilation to eliminate code duplication:

### 🔄 **Unified Linear Algebra** (`linalg` namespace)
- **Target**: CPU + FPGA with same codebase
- **Types**: Standard C++ types (double, float, int)
- **Features**: Matrix/vector operations, Lie group operations (SO3), numerical algorithms
- **CPU Mode**: Uses `std::` math functions, no HLS pragmas
- **FPGA Mode**: Uses `hls::` math functions, includes HLS pragmas for optimization
- **Include**: `#include "linalg/linalg.h"` (works for both targets)

### 🔢 **HLS Numerical Formats** (`hls_numerics`)
- **Target**: FPGA-only specialized arithmetic
- **Types**: FloatX (custom floating-point), Posit arithmetic
- **Features**: Bit-level optimized numerical representations
- **Include**: `#include "hls_numerics/FloatX.h"` or `#include "hls_numerics/Posit.h"`

## Project Structure

```
HLSLinearAlgebra/
├── include/
│   ├── linalg/                  # Unified CPU+FPGA linear algebra
│   │   ├── linalg.h             # Single implementation with conditional HLS pragmas
│   │   ├── common.h             # Utility functions
│   │   └── shift_registers.h    # Shift register utilities
│   └── hls_numerics/            # FPGA-only numerical formats
│       ├── FloatX.h             # Custom floating-point
│       ├── Posit.h              # Posit arithmetic
│       ├── common.h             # HLS utility functions
│       └── hls_compat*.h        # CPU compatibility layers
├── tests/
│   ├── cpu/                     # CPU tests
│   │   ├── basic_cpu_test.cpp   # Basic functionality
│   │   └── linalg_cpu_test.cpp  # Linear algebra tests
│   └── hls/                     # HLS/FPGA tests (same linalg code)
│       ├── hls_linalg_test.cpp  # HLS linear algebra tests
│       ├── FloatX_*.cpp         # FloatX tests
│       ├── Posit_test.cpp       # Posit tests
│       └── rmse_test.cpp        # Benchmark tests
└── CMakeLists.txt               # Build configuration
```

## Quick Start

### Prerequisites
- CMake 3.14+
- C++17 compatible compiler
- For FPGA synthesis: Xilinx Vitis/Vivado toolchain

### CPU Development & Testing

```bash
# Configure for CPU testing only
cmake -S . -B build -DBUILD_TESTS=ON -DBUILD_HLS_TESTS=OFF

# Build and test
cmake --build build
cd build && ctest --output-on-failure

# Run individual test suites
./build/tests/basic_cpu_test      # Basic functionality
./build/tests/linalg_cpu_test     # Linear algebra operations
```

### HLS/FPGA Development & Testing

```bash
# Configure for HLS testing (requires Vitis toolchain)
cmake -S . -B build_hls -DBUILD_TESTS=OFF -DBUILD_HLS_TESTS=ON

# Build HLS components
cmake --build build_hls
cd build_hls && ctest --output-on-failure

# Run HLS-specific tests
./build_hls/tests/hls_linalg_test  # HLS linear algebra
```

### Build Options
- `BUILD_TESTS=ON/OFF`: Enable/disable CPU tests (default: OFF)
- `BUILD_HLS_TESTS=ON/OFF`: Enable/disable HLS-specific tests (default: OFF)
- `BUILD_BENCHMARKS=ON/OFF`: Enable/disable benchmark tests (default: OFF)

## Usage Examples

### Unified Linear Algebra (CPU & FPGA)
```cpp
#include "linalg/linalg.h"

// Same code works for both CPU and FPGA!
linalg::Mat<double, 3, 3> matrix = linalg::Mat<double, 3, 3>::Identity();
matrix(0, 1) = 2.5;  // Set element at row 0, column 1

// Vector operations
linalg::Mat<double, 4, 1> vector;
vector(0, 0) = 1.0;
vector(1, 0) = 2.0;

// CPU build: Uses std::sin, std::cos, no HLS pragmas
// FPGA build: Uses hls::sin, hls::cos, includes HLS pragmas automatically
```

### HLS Numerical Formats (FPGA-only)
```cpp
#include "hls_numerics/FloatX.h"
#include "hls_numerics/Posit.h"

// Custom floating-point arithmetic (FPGA synthesis only)
FloatX<32, 8> custom_float = 3.14159;
Posit<32, 3> posit_num = 2.718;

// Can be used with unified linalg:
linalg::Mat<FloatX<16, 5>, 3, 3> optimized_matrix;
```

## Features

### ✅ **CPU Components** (Working)
- **Core Linear Algebra**: Matrix/vector operations, Lie groups (SO3)
- **Testing Framework**: GoogleTest integration with 16 passing tests  
- **Cross-platform**: Works on any C++17 compiler
- **Header-only**: Easy integration

### ⚡ **Unified Components** (CPU + FPGA)
- **Single Implementation**: No code duplication between CPU and FPGA
- **Conditional Compilation**: HLS pragmas and math functions added automatically
- **Numerical Formats**: FloatX and Posit for specialized FPGA arithmetic
- **Vitis Integration**: Ready for FPGA synthesis

## Development Strategy

### Unified Architecture Benefits
1. **Single Codebase**: Write once, deploy on CPU or FPGA
2. **No Duplication**: Eliminates maintenance overhead of separate implementations
3. **Automatic Optimization**: HLS pragmas added conditionally during synthesis
4. **Seamless Migration**: Same code works on both targets

### Testing Strategy
- **CPU Tests**: Algorithm validation using standard C++ types (16 tests passing)
- **HLS Tests**: Same tests run with HLS pragmas enabled
- **Unified Testing**: Both test suites use identical `linalg` namespace

### Development Workflow
```cpp
// Single implementation works everywhere:
#include "linalg/linalg.h"

// CPU development (fast iteration):
linalg::Mat<double, 3, 3> matrix;

// FPGA synthesis (same code, HLS optimized):
// - Automatically uses hls::sin, hls::cos
// - Automatically includes HLS pragmas
// - No code changes needed!

// Custom types for FPGA optimization:
#include "hls_numerics/FloatX.h"
linalg::Mat<FloatX<16, 5>, 3, 3> optimized_matrix;
```

## Contributing

1. **Linear Algebra**: Use unified `linalg` namespace for both CPU and FPGA
2. **Numerical Formats**: Use `hls_numerics` for FPGA-only custom types
3. **Testing**: Add CPU tests to `cpu/` directory, HLS tests to `hls/` directory
4. **Verification**: Ensure `cd build && ctest` passes for CPU tests
5. **HLS Testing**: Use Vitis HLS toolchain for FPGA synthesis validation

## License

[Add your license information here]
