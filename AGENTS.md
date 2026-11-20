# Frogger 3D Game Development Guide

## Project Overview
This is a portable 3D Frogger game implemented in C++ using SDL2 for cross-platform window management and OpenGL for 3D rendering.

## Key Technologies
- C++17
- SDL2 for window management and input handling
- OpenGL 3.3 Core Profile for 3D rendering
- GLEW for OpenGL extension loading
- GLM for mathematical operations
- CMake for cross-platform builds

## Build System
- Build system: CMake
- Required libraries: SDL2, OpenGL, GLEW, GLM
- Build command: `mkdir build && cd build && cmake .. && make`

## Code Structure
```
src/
├── main.cpp          # Entry point
├── Game.cpp          # Main game loop and state management
├── Player.cpp        # Player character implementation
├── Enemy.cpp         # Enemy implementation
├── Renderer.cpp      # OpenGL rendering setup and utilities
└── Window.cpp        # SDL window management

include/
├── Game.h
├── Player.h
├── Enemy.h
├── Renderer.h
└── Window.h
```

## Critical Implementation Details

### 1. OpenGL Context Management
- OpenGL context must be created using SDL_GL_CreateContext with proper attributes
- SDL_GLContext is stored directly, not as a unique_ptr
- Context must be properly initialized before GLEW initialization

### 2. Modern OpenGL Usage
- Uses Vertex Array Objects (VAO), Vertex Buffer Objects (VBO), and Element Buffer Objects (EBO)
- No deprecated immediate mode OpenGL functions
- Proper shader pipeline setup required for rendering

### 3. Window Initialization
- Window must be created with SDL_WINDOW_OPENGL flag
- SDL_Init must be called before window creation
- Error checking for all SDL operations

### 4. Rendering Flow
1. Create window with OpenGL support
2. Create OpenGL context with proper version/profile
3. Initialize GLEW
4. Set up viewport and enable depth testing
5. Create vertex buffers and shaders
6. In render loop: clear, render objects, swap buffers

## Common Issues & Solutions

### Black Screen Issues
- Ensure OpenGL context is properly created
- Check GLEW initialization returns GLEW_OK
- Verify viewport is set correctly
- Confirm depth testing is enabled

### Build Failures
- Ensure all dependencies (SDL2, OpenGL, GLEW, GLM) are installed
- CMakeLists.txt may need adjustments for your system's library paths

### Immediate Exit Issues
- Add error checking in main function and constructor
- Verify all initialization steps succeed
- Check for exceptions being thrown

## Running the Game
1. Build: `mkdir build && cd build && cmake .. && make`
2. Run: `./Frogger3D`

## Debugging Commands
- `gdb ./Frogger3D` - For debugging crashes
- `valgrind ./Frogger3D` - For memory issues

## Testing
- Test on multiple platforms (Linux, Windows, macOS)
- Verify rendering works with different window sizes
- Check that game loop runs properly without immediate exit