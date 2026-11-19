cmake_minimum_required(VERSION 3.10)
project(Frogger3D)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find SDL2
find_package(SDL2 REQUIRED)
find_package(OpenGL REQUIRED)

# Try to find GLEW
find_package(PkgConfig REQUIRED)
pkg_check_modules(GLEW REQUIRED glew)

# Add executable
add_executable(Frogger3D
    src/main.cpp
    src/Game.cpp
    src/Player.cpp
    src/Enemy.cpp
    src/Renderer.cpp
    src/Window.cpp
)

# Include directories
target_include_directories(Frogger3D PRIVATE include)

# Link libraries
target_link_libraries(Frogger3D SDL2::SDL2 OpenGL::OpenGL ${GLEW_LIBRARIES})
target_include_directories(Frogger3D PRIVATE ${GLEW_INCLUDE_DIRS})