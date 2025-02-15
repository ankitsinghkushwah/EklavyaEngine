#!/bin/bash

# Check if the build type is provided
if [ -z "$1" ]; then
  echo "Usage: $0 <debug|release>"
  exit 1
fi

# Set build type based on the argument
BUILD_TYPE=$(echo "$1" | tr '[:upper:]' '[:lower:]')

if [ "$BUILD_TYPE" != "debug" ] && [ "$BUILD_TYPE" != "release" ]; then
  echo "Invalid build type. Use 'debug' or 'release'."
  exit 1
fi

# Determine the platform
PLATFORM=$(uname -s)

# Set build directory based on platform and build type
if [[ "$PLATFORM" == "MINGW"* || "$PLATFORM" == "MSYS"* || "$PLATFORM" == "CYGWIN"* ]]; then
  # Windows
  BUILD_DIR="vs_proj/${BUILD_TYPE}_build"
  GENERATOR="Ninja" # Use Ninja for faster builds on Windows
elif [[ "$PLATFORM" == "Darwin" ]]; then
  # macOS
  BUILD_DIR="xcode_proj/${BUILD_TYPE}_build"
  GENERATOR="Unix Makefiles" # Use Makefiles on macOS
else
  echo "Unsupported platform: $PLATFORM"
  exit 1
fi

# Create build directory if it doesn't exist
mkdir -p "$BUILD_DIR"
cd "$BUILD_DIR" || exit 1

# Run CMake to configure the build
echo "Configuring $BUILD_TYPE build..."
cmake -G "$GENERATOR" -DCMAKE_BUILD_TYPE=$BUILD_TYPE ../..

# Build the project
echo "Building the project..."
cmake --build .

# Locate the executable
if [[ "$PLATFORM" == "MINGW"* || "$PLATFORM" == "MSYS"* || "$PLATFORM" == "CYGWIN"* ]]; then
  # Windows: Look for the executable in the build directory
  EXECUTABLE=$(find . -name "*.exe")
elif [[ "$PLATFORM" == "Darwin" ]]; then
  # macOS: Look for the executable in the build directory
  EXECUTABLE=$(find . -type f -perm +111 -print | grep -v ".dSYM")
else
  echo "Unsupported platform: $PLATFORM"
  exit 1
fi

# Check if the executable was found
if [ -z "$EXECUTABLE" ]; then
  echo "Error: Executable not found!"
  exit 1
else
  echo "Executable generated at: $EXECUTABLE"
fi