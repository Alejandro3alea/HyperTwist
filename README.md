# HyperTwist

> _A love letter for Rhythm games_


[![Version]( https://img.shields.io/badge/version-0.1.0-blue)](https://github.com/alejandro3alea/HyperTwist/)  [![Build Status](https://github.com/alejandro3alea/HyperTwist/actions/workflows/c-cpp.yml/badge.svg)](https://github.com/alejandro3alea/HyperTwist/) 

HyperTwist is a Rhythm engine, editor and game.


## Game
Nowadays there are several 4-pad foot rythm games. However, I personally feel that none of them delve inough into their mechanics, leaving them for more ideas that can be exploited. This game aims to expand on these innovative features and improved gameplay that promise to engage players and enhance their experience.

![Song Select Menu](https://github.com/Alejandro3alea/HyperTwist/blob/main/Game/data/engine/texture/SongSelect/Reference.png?raw=true)


#### Features
- **Song Processing (.sm, .ssc)**
    - Song resources and properties loader
    - Chart loader (notes, difficulty...)
- **Song Conversion to (.smd, .scd)**
    - *.smd* (*Step Main Data*) Saves the main info about a Song, to avoid a load overhead with charts.
    - *.scd* (*Step Chart Data*) Saves the chart data in a more compressed way than .sm and .ssc files, reducing memory space.
- **Game Loop**
    - Basic game pipeline added:
    - *Dev Window:*
        - View and edit game config.
    - *Main Menu:*
    - *Account Screen:*
    - *Mode Select:*
    - *Song Select:*
    - *Gameplay:*
    - *Song Results:*
    - *Game Results:*
    - *Game end:*
- **UI/UX**
    - Basic UI implemented
    - Some textures are temporarily placeholders.


## Editor
![Song Select Menu](https://github.com/Alejandro3alea/HyperTwist/blob/main/docs/King.png?raw=true)
![Song Select Menu](https://github.com/Alejandro3alea/HyperTwist/blob/main/docs/Houkago-Stride.png?raw=true)


#### Features
- **Song Processing (.sm, .scc)**
    - Song resources and properties loader
    - Chart loader (notes, difficulty...)
- **DDR/ITG/SM**
    - Basic UI implemented
    - Note creation/deletion
    - Snap to measure (1/4, 1/8, 1/6...)
    - Hold and Roll notes
    - Assist tick + note ticks
    - Chart properties
- **Texture atlases**
    - Instanced rendering for note rendering
- **Song serialization**
- **Pauses & Gimmicks**


## Libraries

HyperTwist works thanks to the following libraries:
| Plugin | Purpose | README |
| ------ | ------ | ------ |
| SDL2 | Window creation & management | [SDL2/README.txt](https://github.com/davidsiaw/SDL2/blob/master/README.txt) |
| GLEW | Graphics programming with OpenGL | [glew/README.md](https://github.com/nigels-com/glew/blob/master/README.md) |
| SFML | Multimedia API | [SFML/README.md](https://github.com/SFML/SFML/blob/master/readme.md) |
| FreeType | Font rendering | [freetype/README](https://github.com/freetype/freetype/blob/master/README) |
| ImGui | GUI library for C++ | [imgui/docs/README.md](https://github.com/ocornut/imgui/blob/master/docs/README.md) |
| OpenAL ***(Old)*** | Audio programming | [openal.org](https://www.openal.org/) |



## Installation

Previously, the project worked just with a Visual Studio Solution (.sln). However, with the new updates to the project, now it works a CMake project.

To configure CMake, select a build configuration, and after a successful CMake generation, compile the C++ code to execute the program.

_Note: User might have to install some dependencies from the [libraries](#libraries) mentioned above._


## Development

Want to contribute? Great! Everyone is welcome to request, create a discussion/pull request, and improve the project!


## CMake

The project is build using CMake, with each configuration structured as the following:
> [**PLATFORM**] **Project** - **Build_Config**

For the future the project might support Mac, FreeBSD or other configurations.


## License

The MIT License (MIT)

Copyright (c) 2022-2026 Alejandro Balea

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
