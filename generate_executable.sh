#!/bin/bash

# Default build type is debug
BUILD_TYPE="debug"

# Check if the build type is provided
if [ -n "$1" ]; then
    BUILD_TYPE=$(echo "$1" | tr '[:upper:]' '[:lower:]')
fi

# Validate the build type
if [ "$BUILD_TYPE" != "debug" ] && [ "$BUILD_TYPE" != "release" ]; then
    echo "Invalid build type. Use 'debug' or 'release'."
    read -p "Press Enter to exit..."
    exit 1
fi

# Determine the platform
PLATFORM=$(uname -s)

# Set the build directory based on the platform and build type
if [[ "$PLATFORM" == "MINGW"* || "$PLATFORM" == "MSYS"* || "$PLATFORM" == "CYGWIN"* ]]; then
    # Windows platform
    BUILD_DIR="vs_proj/${BUILD_TYPE}_build"
    GENERATOR="Visual Studio 17 2022" # Use Visual Studio 17 2022 on Windows
    CMAKE_EXTRA_ARGS="-A Win32" # Specify Win32 architecture
elif [[ "$PLATFORM" == "Darwin"* ]]; then
    # macOS platform
    BUILD_DIR="xcode_proj/${BUILD_TYPE}_build"
    GENERATOR="Unix Makefiles" # Use Unix Makefiles on macOS
    CMAKE_EXTRA_ARGS=""
else
    echo "Unsupported platform: $PLATFORM"
    read -p "Press Enter to exit..."
    exit 1
fi

# Create the build directory if it doesn't exist
mkdir -p "$BUILD_DIR"

# Navigate to the build directory
cd "$BUILD_DIR" || exit

# Run CMake to configure the build
cmake -G "$GENERATOR" -DCMAKE_BUILD_TYPE=$BUILD_TYPE $CMAKE_EXTRA_ARGS ../..

# Build the project
cmake --build . --config $BUILD_TYPE

# Clean up Visual Studio project files (Windows only)
if [[ "$PLATFORM" == "MINGW"* || "$PLATFORM" == "MSYS"* || "$PLATFORM" == "CYGWIN"* ]]; then
    echo "Cleaning up Visual Studio project files..."
    rm -rf CMakeFiles CMakeCache.txt *.sln *.vcxproj *.vcxproj.filters
fi

# Return to the original directory
cd - || exit

echo "Build completed successfully in $BUILD_DIR"

# Keep the terminal open
read -p "Press Enter to exit..."