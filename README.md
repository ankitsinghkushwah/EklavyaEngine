# EklavyaEngine

Game Engine written in C++ and OpenGL 4.1

Features : 

- Scene Graph with Frustum culling
- Renderer : Renderpasses, shadows and post processing
- Skeletal animation with smooth transition
- Custom Physics :
     - SAT for collision
     - Euler Integration
     - Uses impulse based method for resolution
     - Support for sphere & OBB
     - ray cast & sphere cast

- Asset Loader : Texture,Model & Animation loading using assimp, Shaders etc

ToDO : 

- Shader compilation at runtime
- Improve box stack stability in physics engine
- Write CMake file to generate visual studio project 

#Compilation

You will need a MacOS to run demos. I setup this project on XCode unfortunately. But I have plans to write CMake to generate projects for windows & MacOS both. 
