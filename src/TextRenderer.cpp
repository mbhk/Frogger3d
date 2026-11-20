#include "TextRenderer.h"
#include "Shader.h"
#include <glm/gtc/matrix_transform.hpp>
#include <cctype>

static const char* overlayVertexSource = R"(
#version 330 core
layout (location = 0) in vec2 aPos;
layout (location = 1) in vec4 aColor;

uniform mat4 uProjection;

out vec4 vColor;

void main() {
    gl_Position = uProjection * vec4(aPos, 0.0, 1.0);
    vColor = aColor;
}
)";

static const char* overlayFragmentSource = R"(
#version 330 core
in vec4 vColor;
out vec4 FragColor;

void main() {
    FragColor = vColor;
}
)";

namespace {

const int glyphWidth = 5;
const int glyphHeight = 7;
const int glyphAdvance = 6;

struct Glyph {
    char c;
    const char* rows[glyphHeight];
};

const Glyph glyphs[] = {
    {' ', {".....", ".....", ".....", ".....", ".....", ".....", "....."}},
    {'A', {".###.", "#...#", "#...#", "#####", "#...#", "#...#", "#...#"}},
    {'B', {"####.", "#...#", "#...#", "####.", "#...#", "#...#", "####."}},
    {'C', {".####", "#....", "#....", "#....", "#....", "#....", ".####"}},
    {'D', {"####.", "#...#", "#...#", "#...#", "#...#", "#...#", "####."}},
    {'E', {"#####", "#....", "#....", "####.", "#....", "#....", "#####"}},
    {'F', {"#####", "#....", "#....", "####.", "#....", "#....", "#...."}},
    {'G', {".####", "#....", "#....", "#.###", "#...#", "#...#", ".####"}},
    {'H', {"#...#", "#...#", "#...#", "#####", "#...#", "#...#", "#...#"}},
    {'I', {"#####", "..#..", "..#..", "..#..", "..#..", "..#..", "#####"}},
    {'J', {"..###", "...#.", "...#.", "...#.", "...#.", "#..#.", ".##.."}},
    {'K', {"#...#", "#..#.", "#.#..", "##...", "#.#..", "#..#.", "#...#"}},
    {'L', {"#....", "#....", "#....", "#....", "#....", "#....", "#####"}},
    {'M', {"#...#", "##.##", "#.#.#", "#.#.#", "#...#", "#...#", "#...#"}},
    {'N', {"#...#", "##..#", "#.#.#", "#..##", "#...#", "#...#", "#...#"}},
    {'O', {".###.", "#...#", "#...#", "#...#", "#...#", "#...#", ".###."}},
    {'P', {"####.", "#...#", "#...#", "####.", "#....", "#....", "#...."}},
    {'Q', {".###.", "#...#", "#...#", "#...#", "#.#.#", "#..#.", ".##.#"}},
    {'R', {"####.", "#...#", "#...#", "####.", "#.#..", "#..#.", "#...#"}},
    {'S', {".####", "#....", "#....", ".###.", "....#", "....#", "####."}},
    {'T', {"#####", "..#..", "..#..", "..#..", "..#..", "..#..", "..#.."}},
    {'U', {"#...#", "#...#", "#...#", "#...#", "#...#", "#...#", ".###."}},
    {'V', {"#...#", "#...#", "#...#", "#...#", "#...#", ".#.#.", "..#.."}},
    {'W', {"#...#", "#...#", "#...#", "#.#.#", "#.#.#", "#.#.#", ".#.#."}},
    {'X', {"#...#", "#...#", ".#.#.", "..#..", ".#.#.", "#...#", "#...#"}},
    {'Y', {"#...#", "#...#", ".#.#.", "..#..", "..#..", "..#..", "..#.."}},
    {'Z', {"#####", "....#", "...#.", "..#..", ".#...", "#....", "#####"}},
    {'0', {".###.", "#...#", "#..##", "#.#.#", "##..#", "#...#", ".###."}},
    {'1', {"..#..", ".##..", "..#..", "..#..", "..#..", "..#..", ".###."}},
    {'2', {".###.", "#...#", "....#", "...#.", "..#..", ".#...", "#####"}},
    {'3', {"#####", "...#.", "..#..", "...#.", "....#", "#...#", ".###."}},
    {'4', {"...#.", "..##.", ".#.#.", "#..#.", "#####", "...#.", "...#."}},
    {'5', {"#####", "#....", "####.", "....#", "....#", "#...#", ".###."}},
    {'6', {"..##.", ".#...", "#....", "####.", "#...#", "#...#", ".###."}},
    {'7', {"#####", "....#", "...#.", "..#..", ".#...", ".#...", ".#..."}},
    {'8', {".###.", "#...#", "#...#", ".###.", "#...#", "#...#", ".###."}},
    {'9', {".###.", "#...#", "#...#", ".####", "....#", "...#.", ".##.."}},
    {'?', {".###.", "#...#", "....#", "...#.", "..#..", ".....", "..#.."}},
    {'!', {"..#..", "..#..", "..#..", "..#..", "..#..", ".....", "..#.."}},
    {':', {".....", "..#..", "..#..", ".....", "..#..", "..#..", "....."}},
    {'.', {".....", ".....", ".....", ".....", ".....", ".##..", ".##.."}},
    {'-', {".....", ".....", ".....", "#####", ".....", ".....", "....."}},
    {'+', {".....", "..#..", "..#..", "#####", "..#..", "..#..", "....."}},
    {'/', {".....", "....#", "...#.", "..#..", ".#...", "#....", "....."}},
    {'(', {"..#..", ".#...", "#....", "#....", "#....", ".#...", "..#.."}},
    {')', {"..#..", "...#.", "....#", "....#", "....#", "...#.", "..#.."}},
};

const Glyph& findGlyph(char c) {
    for (const auto& glyph : glyphs) {
        if (glyph.c == c) {
            return glyph;
        }
    }
    return glyphs[0];
}

}

TextRenderer::TextRenderer()
    : shader(std::make_unique<Shader>(overlayVertexSource, overlayFragmentSource)) {
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(2 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glBindVertexArray(0);
}

TextRenderer::~TextRenderer() {
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
}

void TextRenderer::begin(int width, int height) {
    vertices.clear();

    glDisable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    shader->use();
    shader->setMat4("uProjection", glm::ortho(0.0f, static_cast<float>(width), static_cast<float>(height), 0.0f, -1.0f, 1.0f));
}

void TextRenderer::end() {
    if (!vertices.empty()) {
        glBindVertexArray(VAO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_DYNAMIC_DRAW);
        glDrawArrays(GL_TRIANGLES, 0, static_cast<GLsizei>(vertices.size() / 6));
        glBindVertexArray(0);
    }

    glDisable(GL_BLEND);
    glEnable(GL_DEPTH_TEST);
}

float TextRenderer::textWidth(const std::string& text, float scale) const {
    if (text.empty()) {
        return 0.0f;
    }
    return static_cast<float>(text.size() * glyphAdvance) * scale - scale;
}

void TextRenderer::drawText(const std::string& text, float x, float y, float scale, const glm::vec4& color) {
    float cursor = x;
    for (char ch : text) {
        const Glyph& glyph = findGlyph(static_cast<char>(std::toupper(static_cast<unsigned char>(ch))));
        for (int row = 0; row < glyphHeight; ++row) {
            for (int col = 0; col < glyphWidth; ++col) {
                if (glyph.rows[row][col] == '#') {
                    addQuad(cursor + col * scale, y + row * scale, scale, scale, color);
                }
            }
        }
        cursor += glyphAdvance * scale;
    }
}

void TextRenderer::drawRect(float x, float y, float width, float height, const glm::vec4& color) {
    addQuad(x, y, width, height, color);
}

void TextRenderer::addQuad(float x, float y, float width, float height, const glm::vec4& color) {
    auto push = [&](float px, float py) {
        vertices.insert(vertices.end(), {px, py, color.r, color.g, color.b, color.a});
    };
    push(x, y);
    push(x + width, y);
    push(x + width, y + height);
    push(x, y);
    push(x + width, y + height);
    push(x, y + height);
}
