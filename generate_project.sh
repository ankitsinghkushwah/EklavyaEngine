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

# Pick a CMake that supports the newest installed Visual Studio, and the matching generator.
# Sets CMAKE_BIN and VS_GENERATOR.
find_vs_cmake() {
    local vswhere="/c/Program Files (x86)/Microsoft Visual Studio/Installer/vswhere.exe"
    local vs_path="" vs_major=""

    if [ -x "$vswhere" ]; then
        vs_path=$("$vswhere" -latest -prerelease -products '*' -property installationPath | tr -d '\r')
        vs_major=$("$vswhere" -latest -prerelease -products '*' -property installationVersion | tr -d '\r' | cut -d. -f1)
    fi

    # Map VS major version -> CMake generator name
    case "$vs_major" in
        18) VS_GENERATOR="Visual Studio 18 2026" ;;
        17) VS_GENERATOR="Visual Studio 17 2022" ;;
        16) VS_GENERATOR="Visual Studio 16 2019" ;;
        *)    VS_GENERATOR="" ;;
    esac

    # Candidate CMakes: the one on PATH first, then the one bundled with Visual Studio
    local candidates=()
    command -v cmake >/dev/null 2>&1 && candidates+=("$(command -v cmake)")
    if [ -n "$vs_path" ]; then
        local unix_vs_path
        unix_vs_path=$(cygpath -u "$vs_path" 2>/dev/null || echo "$vs_path")
        candidates+=("$unix_vs_path/Common7/IDE/CommonExtensions/Microsoft/CMake/CMake/bin/cmake.exe")
    fi

    CMAKE_BIN=""
    local c
    for c in "${candidates[@]}"; do
        [ -x "$c" ] || continue
        if [ -z "$VS_GENERATOR" ] || "$c" --help 2>/dev/null | grep -q "$VS_GENERATOR"; then
            CMAKE_BIN="$c"
            break
        fi
    done

    if [ -z "$CMAKE_BIN" ]; then
        echo "Error: no CMake found that supports '$VS_GENERATOR'. Update CMake or install the VS CMake component."
        exit 1
    fi

    # Fall back to CMake's default VS generator if VS could not be detected
    if [ -z "$VS_GENERATOR" ]; then
        VS_GENERATOR=$("$CMAKE_BIN" --help | sed -n 's/^\* \(Visual Studio [0-9]* [0-9]*\).*/\1/p' | head -n1)
    fi
}

# Function to generate Visual Studio project for Windows (supports all configurations)
generate_vs_project() {
    local build_dir="vs_proj"
    find_vs_cmake
    echo "Using CMake: $CMAKE_BIN ($("$CMAKE_BIN" --version | head -n1))"
    echo "Generating Visual Studio project ($VS_GENERATOR) for Windows in $build_dir..."

    # A cache from a different generator makes CMake refuse to configure; clear it
    if [ -f "$build_dir/CMakeCache.txt" ] && \
       ! grep -q "^CMAKE_GENERATOR:INTERNAL=$VS_GENERATOR\$" "$build_dir/CMakeCache.txt"; then
        echo "Existing $build_dir was generated with a different generator; clearing CMake cache..."
        rm -rf "$build_dir/CMakeCache.txt" "$build_dir/CMakeFiles"
    fi

    mkdir -p "$build_dir"
    "$CMAKE_BIN" -G "$VS_GENERATOR" -A Win32 -B "$build_dir" "$@" || { echo "CMake generation failed."; exit 1; }
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