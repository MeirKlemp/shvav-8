# Shvav-8
Shvav-8 is a Cross-Platform Chip-8 emulator(Shvav is Chip in Hebrew)

## About
I developed this project to learn emulation and graphics development - areas that interest me very much.
I started with simple steps, creating a Chip-8 emulator - simple emulation and simple graphics.
Shvav-8 uses CMake as its build system, OpenGL to render its graphics, and SDL2 as a sound library.

## Showcase
https://user-images.githubusercontent.com/50945641/139578249-67ee38bf-4e5f-422b-acc0-07617261576f.mp4

## Build
I use [CMake](https://cmake.org/) and [Ninja](https://ninja-build.org/) to build this project.
``` shell
$ git clone --recursive https://github.com/MeirKlemp/shvav-8/
$ cd ./shvav-8/
$ mkdir build
$ cd ./build/
$ cmake -G Ninja .. -DCMAKE_BUILD_TYPE=Release
$ cmake --build .
```

## Usage
``` shell
$ ./shvav-8
Usage: C:\Users\Meir\Desktop\shvav-8\build\shvav-8.exe <ROM>
```
ROM is the path for the Chip-8 game.

## Controls
| Key      | Action                |
|----------|-----------------------|
| `Space`  | Restart               |
| `Escape` | Quit                  |
| `Left`   | Slow Down             |
| `Right`  | Speed Up              |
| `Down`   | Decrease Beeping Tone |
| `Up`     | Increase Beeping Tone |
