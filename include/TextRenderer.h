#ifndef TEXT_RENDERER_H
#define TEXT_RENDERER_H

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <memory>
#include <string>
#include <vector>

class Shader;

class TextRenderer {
public:
    TextRenderer();
    ~TextRenderer();

    TextRenderer(const TextRenderer&) = delete;
    TextRenderer& operator=(const TextRenderer&) = delete;

    void begin(int width, int height);
    void end();

    float textWidth(const std::string& text, float scale) const;
    void drawText(const std::string& text, float x, float y, float scale, const glm::vec4& color);
    void drawRect(float x, float y, float width, float height, const glm::vec4& color);

private:
    void addQuad(float x, float y, float width, float height, const glm::vec4& color);

    std::unique_ptr<Shader> shader;
    unsigned int VAO, VBO;
    std::vector<float> vertices;
};

#endif // TEXT_RENDERER_H
