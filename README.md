# pgr-engine

This repository contains an implementation of a semestral assignment from the PGR (Computer Graphics) course at FEE CTU. 


> [!WARNING] 
> This software is under active development.

> [!WARNING]
> This project uses GL_ARB_bindless_texture extension. This may not be supported on all devices. See [compatibility chart](https://opengl.gpuinfo.org/listreports.php?extension=GL_ARB_bindless_texture).

### Runtime dependencies 
- ```glm``` >= 1.0.0
- ```glfw``` >= 3.3
- ```opengl``` >= 4.6 *(glad loader is provided with the project)*
- ```assimp``` >= 5.4

### Build dependencies
- ```premake5``` >= 5.0.0
- ```doxygen``` >= 1.13 (*optional, for generating docs*)
- C++ compiler with at least **C++17** support 

### Building
This build project uses **premake5** build system. To generate build files, run:
``` 
    premake5 [target]
```
in the project's root directory. A list of targets can be found by running ```premake5 --help```. All the build files will be generated in the **build** directory. To build the project, follow the instructions of the chosen target build system.

## Acknowledgements
This project uses and redistributes [```stb_image.h```](https://github.com/nothings/stb/blob/master/stb_image.h), a part of the [stb libraries](https://github.com/nothings/stb/) <br />
Copyright (c) 2017 Sean Barrett, licensed under [MIT](https://github.com/nothings/stb/blob/master/LICENSE) License

This project uses and redistributes [```json```](https://github.com/nlohmann/json), JSON loader <br/>
Copyright (c) 2013-2025 Niels Lohmann, licensed under [MIT](https://github.com/nlohmann/json/blob/develop/LICENSE.MIT) License

This project uses and redistributes [```glad```](https://github.com/Dav1dde/glad) OpenGL Loader <br />
Copyright (c) 2013-2022 David Herberth, licensed under [MIT](https://github.com/Dav1dde/glad/blob/glad2/LICENSE) License

This project uses and redistributes parts of the Khronos Specifications (as a part of glad) <br />
Copyright (c) 2013-2020 The Khronos Group Inc, licensed under [Apache License v. 2.0](https://github.com/Dav1dde/glad/blob/glad2/LICENSE)
