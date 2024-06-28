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

Demos (click on images to go to videos) : 

[![VEHICLE PHYSICS](https://img.youtube.com/vi/z52WxTNrIpg/sddefault.jpg)](https://www.youtube.com/watch?v=z52WxTNrIpg)
[![VEHICLE PHYSICS](https://img.youtube.com/vi/abnopf9OntY/sddefault.jpg)](https://www.youtube.com/watch?v=abnopf9OntY)

[![SKELETAL ANIMATION](https://img.youtube.com/vi/NbSgJ3GrD_0/sddefault.jpg)](https://www.youtube.com/watch?v=NbSgJ3GrD_0)
[![SKELETAL ANIMATION](https://img.youtube.com/vi/RszWEUHOxoE/sddefault.jpg)](https://www.youtube.com/watch?v=RszWEUHOxoE)

[![FRUSTUM CULLING](https://img.youtube.com/vi/H0ntyFogFvU/sddefault.jpg)](https://www.youtube.com/watch?v=H0ntyFogFvU)

To-do : 

- Shader compilation at runtime
- Improve box stack stability in physics engine
- Write CMake file to generate visual studio project 

#Compilation

You will need a MacOS to run demos. I setup this project on XCode unfortunately. But I have plans to write CMake to generate projects for windows & MacOS both. 
