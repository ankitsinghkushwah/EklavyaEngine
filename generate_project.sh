#!/bin/bash

# Function to generate Xcode project for macOS (supports all configurations)
generate_xcode_project() {
    local build_dir="xcode_proj"
    echo "Generating Xcode project for macOS in $build_dir..."
    mkdir -p "$build_dir"
    cmake -G "Xcode" -B "$build_dir" "$@"
    echo "Xcode project generated in $build_dir."
    echo "Note: Xcode is a multi-config generator. You can select Debug/Release in Xcode."
}

# Function to generate Visual Studio project for Windows (supports all configurations)
generate_vs_project() {
    local build_dir="vs_proj"
    echo "Generating Visual Studio project for Windows in $build_dir..."
    mkdir -p "$build_dir"
    cmake -G "Visual Studio 17 2022" -B "$build_dir" "$@"
    echo "Visual Studio project generated in $build_dir."
    echo "Note: Visual Studio is a multi-config generator. You can select Debug/Release in Visual Studio."
}

# Determine the operating system
OS_NAME=$(uname -s)

# Parse command-line arguments
ENABLE_ASAN="OFF" # Default value
while [[ $# -gt 0 ]]; do
    case "$1" in
        --enable-asan)
            ENABLE_ASAN="ON"
            shift
            ;;
        *)
            echo "Unknown argument: $1"
            exit 1
            ;;
    esac
done

# Generate the appropriate project based on the OS
if [ "$OS_NAME" == "Darwin" ]; then
    echo "Detected macOS. Generating Xcode project..."
    generate_xcode_project -DENABLE_ASAN="$ENABLE_ASAN"
elif [[ "$OS_NAME" == *"MINGW"* ]] || [[ "$OS_NAME" == *"MSYS"* ]] || [[ "$OS_NAME" == *"CYGWIN"* ]]; then
    echo "Detected Windows. Generating Visual Studio project..."
    generate_vs_project -DENABLE_ASAN="$ENABLE_ASAN"
else
    echo "Unsupported operating system: $OS_NAME"
    exit 1
fi

echo "Project generation complete!"