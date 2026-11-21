cmake_minimum_required(VERSION 3.10)
project(Frogger3D)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Prefer GLVND OpenGL libraries (CMP0072 is available from CMake 3.11)
if(POLICY CMP0072)
    cmake_policy(SET CMP0072 NEW)
endif()
set(OpenGL_GL_PREFERENCE GLVND)

# Find SDL2
find_package(SDL2 REQUIRED)
find_package(OpenGL REQUIRED)

# Find GLEW
find_package(PkgConfig REQUIRED)
pkg_check_modules(GLEW REQUIRED glew)

# Find glm
find_path(GLM_INCLUDE_DIR glm/glm.hpp
    PATHS /usr/include
)

# Add executable
add_executable(Frogger3D
    src/main.cpp
    src/Game.cpp
    src/Player.cpp
    src/Enemy.cpp
    src/Pickup.cpp
    src/Renderer.cpp
    src/Window.cpp
    src/Shader.cpp
    src/TextRenderer.cpp
    src/Ground.cpp
    src/Lamp.cpp
)

# Include directories
target_include_directories(Frogger3D PRIVATE include ${GLM_INCLUDE_DIR} ${GLEW_INCLUDE_DIRS})

# Link libraries
target_link_libraries(Frogger3D SDL2::SDL2 OpenGL::OpenGL ${GLEW_LIBRARIES})