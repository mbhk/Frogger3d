#include "Renderer.h"
#include "Window.h"
#include "Shader.h"
#include <iostream>

static const char* vertexShaderSource = R"(
#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aColor;

uniform mat4 uModel;
uniform mat4 uView;
uniform mat4 uProjection;

out vec3 vColor;
out vec3 vPosition;

void main() {
    gl_Position = uProjection * uView * uModel * vec4(aPos, 1.0);
    vColor = aColor;
    vPosition = vec3(uModel * vec4(aPos, 1.0));
}
)";

static const char* fragmentShaderSource = R"(
#version 330 core
in vec3 vColor;
in vec3 vPosition;
out vec4 FragColor;

uniform vec3 uViewPos;
uniform vec3 uLightPos[32];
uniform vec3 uLightColor[32];
uniform int uLightCount;
uniform bool uEmissive;
uniform float uSpecularStrength;
uniform float uShininess;
uniform vec3 uAmbientColor;
uniform vec3 uSunDirection;
uniform vec3 uSunColor;

void main() {
    if (uEmissive) {
        FragColor = vec4(vColor, 1.0);
        return;
    }

    // Faces are flat-shaded: the normal comes from the triangle's screen-space derivatives
    vec3 norm = normalize(cross(dFdx(vPosition), dFdy(vPosition)));
    if (dot(norm, normalize(uViewPos - vPosition)) < 0.0) {
        norm = -norm;
    }

    vec3 viewDir = normalize(uViewPos - vPosition);
    vec3 lighting = uAmbientColor + max(dot(norm, uSunDirection), 0.0) * uSunColor;
    vec3 specular = vec3(0.0);
    for (int i = 0; i < uLightCount; ++i) {
        vec3 toLight = uLightPos[i] - vPosition;
        float dist = length(toLight);
        vec3 lightDir = toLight / dist;
        float attenuation = 1.0 / (1.0 + 0.2 * dist + 0.1 * dist * dist);
        float diff = max(dot(norm, lightDir), 0.0);
        lighting += diff * attenuation * uLightColor[i] * 2.5;

        vec3 halfway = normalize(lightDir + viewDir);
        float spec = pow(max(dot(norm, halfway), 0.0), uShininess);
        specular += spec * attenuation * uLightColor[i] * uSpecularStrength * 2.5;
    }

    FragColor = vec4(lighting * vColor + specular, 1.0);
}
)";

Renderer::Renderer(SDL_Window* window) : window(window) {
    // Set up OpenGL context attributes
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
    
    // Create OpenGL context
    context = SDL_GL_CreateContext(window);
    if (!context) {
        std::cerr << "OpenGL context could not be created! SDL_Error: " << SDL_GetError() << std::endl;
        return;
    }

    // Get window dimensions
    int width, height;
    SDL_GetWindowSize(window, &width, &height);
    this->width = width;
    this->height = height;

    // Initialize GLEW
    glewExperimental = GL_TRUE;
    GLenum err = glewInit();
    if (err != GLEW_OK) {
        std::cerr << "GLEW could not initialize! Error: " << glewGetErrorString(err) << std::endl;
    }

    // Set viewport
    setViewport(width, height);
    
    // Enable depth testing
    glEnable(GL_DEPTH_TEST);

    setClearColor(0.1f, 0.1f, 0.2f, 1.0f);

    shader = std::make_unique<Shader>(vertexShaderSource, fragmentShaderSource);
}

Renderer::~Renderer() {
    // Release GL objects while the context is still alive
    shader.reset();
    if (context) {
        SDL_GL_DeleteContext(context);
    }
}

void Renderer::clear() const {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void Renderer::swap() const {
    SDL_GL_SwapWindow(window);
}

void Renderer::setViewport(int width, int height) const {
    glViewport(0, 0, width, height);
}

void Renderer::setClearColor(float r, float g, float b, float a) const {
    glClearColor(r, g, b, a);
}

void Renderer::setCamera(const glm::mat4& view, const glm::mat4& projection) const {
    shader->use();
    shader->setMat4("uView", view);
    shader->setMat4("uProjection", projection);
}

const Shader& Renderer::getShader() const {
    return *shader;
}
