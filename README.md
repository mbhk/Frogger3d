# 3D Frogger Game

A portable 3D Frogger game implemented in C++ using SDL2 and OpenGL with cross-platform compatibility.

## Features

- 3D graphics using OpenGL
- Cross-platform compatibility (Windows, macOS, Linux)
- Modular C++ implementation
- SDL2 for window management and input handling
- OpenGL rendering with shaders

## Requirements

- C++11 or later
- CMake 3.10 or later
- SDL2
- OpenGL
- GLEW (OpenGL Extension Wrangler Library)
- GLM (OpenGL Mathematics library)

## Building

### Linux (Arch Linux)
```bash
# Install dependencies
sudo pacman -S sdl2 glew glm

# Build the project
mkdir build
cd build
cmake ..
make
```

### Linux (Ubuntu/Debian)
```bash
# Install dependencies
sudo apt-get install libsdl2-dev libglew-dev libglm-dev

# Build the project
mkdir build
cd build
cmake ..
make
```

### macOS
```bash
# Install dependencies with Homebrew
brew install sdl2 glew glm

# Build the project
mkdir build
cd build
cmake ..
make
```

### Windows
```bash
# Install dependencies with vcpkg
vcpkg install sdl2 glew glm

# Build the project
mkdir build
cd build
cmake .. -DCMAKE_TOOLCHAIN_FILE=[vcpkg root]/scripts/buildsystems/vcpkg.cmake
cmake --build .
```

## Project Structure

```
.
├── CMakeLists.txt
├── README.md
├── include/
│   ├── Window.h
│   ├── Renderer.h
│   ├── Game.h
│   ├── Player.h
│   └── Enemy.h
├── src/
│   ├── Window.cpp
│   ├── Renderer.cpp
│   ├── Game.cpp
│   ├── Player.cpp
│   └── Enemy.cpp
└── src/
    └── main.cpp
```

## Controls

- Arrow Keys: Move the player
- ESC: Quit the game

## Game Components

- **Window**: Manages SDL2 window creation and event handling
- **Renderer**: Handles OpenGL rendering setup and drawing operations
- **Game**: Main game loop and state management
- **Player**: Player character with movement and rendering
- **Enemy**: Enemy entities with simple AI and rendering

## Implementation Details

The game uses a modular approach with separate classes for different responsibilities:
- Window handles the display and input events
- Renderer manages OpenGL context and drawing operations
- Game controls the main loop and updates game state
- Player and Enemy classes implement specific game logic and rendering

The 3D rendering is implemented using basic OpenGL primitives with colored cubes representing game objects.

## License

This project is licensed under the MIT License - see the LICENSE file for details.

## Author

Frogger 3D Game Implementation