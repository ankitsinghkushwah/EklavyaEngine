# EklavyaEngine

A Game Engine written in C++ and OpenGL 4.1.

## Table of Contents

1. [Project Generation](#project-generation)
2. [Build Generation](#build-generation)
3. [Features](#features)
4. [Demos](#demos)
5. [To-Do](#to-do)

## Project Generation

To generate project files for Visual Studio or Xcode, you need to install **CMake**.
Download it from [here](https://cmake.org/download/). Once installed, you can run shell file to generate project automatically for your platform (only supported macos and windows currently)

```sh
cd EklavyaEngine #change directory to root project directory 
chmod +x generate_project.sh # Only for MacOS terminal to make script executable
./generate_project.sh  #additionaly pass in --enable-asan to enable Address Sanitizer
```

For MacOS, it will generate xcode project in "xcode_proj".
For Windows, it will generate visual studio project in "vs_proj".

Note : Shell file will only run on windows if you have git bash installed.
Install it from [here](https://git-scm.com/downloads/). 

Alternatively, you can use the CMake GUI if yo don't want to install Git.

---

## Build Generation

You can also spit out DEBUG or RELEASE build for both macOS and windows by running a shell command :

```sh
cd EklavyaEngine #change directory to root project directory 
chmod +x generate_executable.sh # Only for MacOS terminal to make script executable
./generate_executable.sh  #additionaly pass debug or release flag for specific build type. By default it generates debug
```

For MacOS, builds will be generated in xcode_proj/debug_build for *debug* and xcode_proj/release_build for *release* .
For Windows, builds will be generated in vs_proj/debug_build for *debug* and vs_proj/release_build for *release* .

---

## Features

### Scene Management
- Scene Graph with Frustum Culling

### Rendering
- Support for multiple Render Passes
- Shadows and Post-Processing

### Animation
- Skeletal Animation with Smooth Transitions

### Physics Engine
- Custom Physics Engine
  - **Collision Detection** using Separating Axis Theorem (SAT)
  - **Motion Integration** using Euler Integration
  - **Collision Resolution** using Impulse-based Method
  - Support for Sphere & Oriented Bounding Box (OBB) Collision
  - Ray Casting & Sphere Casting

### Asset Management
- Asset Loader supports:
  - Textures
  - Models & Animations (via Assimp)
  - Shader Loading

---

## Demos

Click on the images below to watch demo videos:

### Vehicle Physics
[![VEHICLE PHYSICS](https://img.youtube.com/vi/z52WxTNrIpg/sddefault.jpg)](https://www.youtube.com/watch?v=z52WxTNrIpg)
[![VEHICLE PHYSICS](https://img.youtube.com/vi/abnopf9OntY/sddefault.jpg)](https://www.youtube.com/watch?v=abnopf9OntY)

### Skeletal Animation
[![SKELETAL ANIMATION](https://img.youtube.com/vi/NbSgJ3GrD_0/sddefault.jpg)](https://www.youtube.com/watch?v=NbSgJ3GrD_0)
[![SKELETAL ANIMATION](https://img.youtube.com/vi/RszWEUHOxoE/sddefault.jpg)](https://www.youtube.com/watch?v=RszWEUHOxoE)

### Frustum Culling
[![FRUSTUM CULLING](https://img.youtube.com/vi/H0ntyFogFvU/sddefault.jpg)](https://www.youtube.com/watch?v=H0ntyFogFvU)

---

## To-Do
- Implement Shader Compilation at Runtime
