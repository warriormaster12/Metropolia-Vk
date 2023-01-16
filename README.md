# Metropolia-Vk
## Download 
- select ```starting-point``` branch
- ```git clone ``` or download the zip
## Installation 
### prerequisites
- [vulkan-sdk](https://vulkan.lunarg.com/)
- [cmake](https://cmake.org/download/)
  - Most linux distros either include or have a way to download cmake through package manager
-Visual Studio on Windows
-XCode on MacOS
-Makefile on Linux (usually built-in)

### compiling on Linux and MacOs
```
mkdir build
cd build
cmake ..
make -j5 (number stands for number of cores on your cpu)
```
### compiling on Window
-create "build" folder
-[Generate compiler instructions with cmake-gui and build](https://youtu.be/LxHV-KNEG3k?t=155)
