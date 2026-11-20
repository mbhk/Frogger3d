#include "Shader.h"
#include <glm/gtc/type_ptr.hpp>
#include <iostream>

static GLuint compileStage(GLenum type, const char* source) {
    GLuint shader = glCreateShader(type);
    glShaderSource(shader, 1, &source, nullptr);
    glCompileShader(shader);

    GLint success;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        char log[512];
        glGetShaderInfoLog(shader, sizeof(log), nullptr, log);
        std::cerr << "Shader compilation failed: " << log << std::endl;
        glDeleteShader(shader);
        return 0;
    }
    return shader;
}

Shader::Shader(const char* vertexSource, const char* fragmentSource) : program(0) {
    GLuint vertex = compileStage(GL_VERTEX_SHADER, vertexSource);
    GLuint fragment = compileStage(GL_FRAGMENT_SHADER, fragmentSource);
    if (!vertex || !fragment) {
        glDeleteShader(vertex);
        glDeleteShader(fragment);
        return;
    }

    program = glCreateProgram();
    glAttachShader(program, vertex);
    glAttachShader(program, fragment);
    glLinkProgram(program);

    GLint success;
    glGetProgramiv(program, GL_LINK_STATUS, &success);
    if (!success) {
        char log[512];
        glGetProgramInfoLog(program, sizeof(log), nullptr, log);
        std::cerr << "Shader program linking failed: " << log << std::endl;
        glDeleteProgram(program);
        program = 0;
    }

    glDeleteShader(vertex);
    glDeleteShader(fragment);
}

Shader::~Shader() {
    glDeleteProgram(program);
}

void Shader::use() const {
    glUseProgram(program);
}

void Shader::setMat4(const std::string& name, const glm::mat4& value) const {
    glUniformMatrix4fv(glGetUniformLocation(program, name.c_str()), 1, GL_FALSE, glm::value_ptr(value));
}

void Shader::setVec3(const std::string& name, const glm::vec3& value) const {
    glUniform3fv(glGetUniformLocation(program, name.c_str()), 1, glm::value_ptr(value));
}

void Shader::setInt(const std::string& name, int value) const {
    glUniform1i(glGetUniformLocation(program, name.c_str()), value);
}

void Shader::setFloat(const std::string& name, float value) const {
    glUniform1f(glGetUniformLocation(program, name.c_str()), value);
}
