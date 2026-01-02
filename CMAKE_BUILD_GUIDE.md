# Building CLox with CMake

This project now supports CMake as the primary build system, providing cross-platform compatibility and modern C++ tooling integration.

## Prerequisites

- CMake 3.15 or higher
- A C++20 compatible compiler:
  - **Windows**: Visual Studio 2019 or later (with MSVC)
  - **Linux/macOS**: GCC 10+ or Clang 10+
- **Optional**: Ninja build system for faster builds

## Building on Windows with Visual Studio

### Method 1: Using Visual Studio 2022 (Recommended)

1. Open Visual Studio 2022
2. Select **"Open a local folder"** and navigate to the CLox directory
3. Visual Studio will automatically detect `CMakeLists.txt` and configure the project
4. Select a configuration from the dropdown (e.g., "x64-Debug" or "x64-Release")
5. Build using **Build > Build All** or press `Ctrl+Shift+B`

### Method 2: Using CMake from Command Line

```bash
# Configure (Debug)
cmake -B build -S . -DCMAKE_BUILD_TYPE=Debug

# Build
cmake --build build

# Run
.\build\Debug\CLox.exe
```

```bash
# Configure (Release)
cmake -B build -S . -DCMAKE_BUILD_TYPE=Release

# Build
cmake --build build --config Release

# Run
.\build\Release\CLox.exe
```

### Method 3: Using CMake Presets

```bash
# List available presets
cmake --list-presets

# Configure using a preset
cmake --preset x64-debug

# Build using a preset
cmake --build --preset x64-debug

# Run
.\build\x64-debug\CLox.exe
```

## Building on Linux/macOS

```bash
# Configure
cmake -B build -DCMAKE_BUILD_TYPE=Release

# Build
cmake --build build -j$(nproc)

# Run
./build/CLox
```

## Build Configurations

- **Debug**: Includes debug symbols, no optimizations
- **Release**: Optimized build for production use

## CMake Options

The `CMakeLists.txt` automatically configures:
- C++20 standard
- Platform-specific compiler flags
- Proper warning levels
- Security checks (SDL on MSVC)

## IDE Integration

### Visual Studio Code
Install the CMake Tools extension and use the CMake sidebar to configure and build.

### CLion
Simply open the folder containing `CMakeLists.txt`, and CLion will handle the rest.

## Troubleshooting

### "CMake not found"
Ensure CMake is installed and added to your PATH. Download from https://cmake.org/download/

### Compiler not detected
Make sure you have a C++20 compatible compiler installed and accessible from the command line.

### Build fails with C++20 errors
Verify your compiler version supports C++20 features.
