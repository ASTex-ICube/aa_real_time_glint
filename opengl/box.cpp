#include "box.h"


Box::Box(const float& size)
{

    float side2 = size * 0.5f;
    std::vector<GLfloat> vertices = {
        // Front
        -side2, -side2, side2,
         side2, -side2, side2,
         side2,  side2, side2,
        -side2,  side2, side2,
        // Right
         side2, -side2, side2,
         side2, -side2, -side2,
         side2,  side2, -side2,
         side2,  side2, side2,
        // Back
        -side2, -side2, -side2,
        -side2,  side2, -side2,
         side2,  side2, -side2,
         side2, -side2, -side2,
        // Left
        -side2, -side2, side2,
        -side2,  side2, side2,
        -side2,  side2, -side2,
        -side2, -side2, -side2,
        // Bottom
        -side2, -side2, side2,
        -side2, -side2, -side2,
         side2, -side2, -side2,
         side2, -side2, side2,
        // Top
        -side2,  side2, side2,
         side2,  side2, side2,
         side2,  side2, -side2,
        -side2,  side2, -side2
    };

    std::vector<GLuint> indices = {
        0,2,1,0,3,2,
        4,6,5,4,7,6,
        8,10,9,8,11,10,
        12,14,13,12,15,14,
        16,18,17,16,19,18,
        20,22,21,20,23,22
    };

    createBuffers(&vertices, &indices);
}

void Box::draw() {
    if (vao == 0) return;

    glBindVertexArray(vao);
    glDrawElements(GL_TRIANGLES, nb_vertices, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}

bool Box::createBuffers(std::vector<GLfloat>* vertices, std::vector<GLuint>* indices)
{
    if (!buffers.empty()) deleteBuffers();

    // Must have data for indices, points, and normals
    if (indices == nullptr || vertices == nullptr)
        return false;

    nb_vertices = (GLuint)indices->size();

    GLuint indexBuf = 0, posBuf;
    glGenBuffers(1, &indexBuf);
    buffers.push_back(indexBuf);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuf);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices->size() * sizeof(GLuint), indices->data(), GL_STATIC_DRAW);

    glGenBuffers(1, &posBuf);
    buffers.push_back(posBuf);
    glBindBuffer(GL_ARRAY_BUFFER, posBuf);
    glBufferData(GL_ARRAY_BUFFER, vertices->size() * sizeof(GLfloat), vertices->data(), GL_STATIC_DRAW);

    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuf);

    // Position
    glBindBuffer(GL_ARRAY_BUFFER, posBuf);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(0);  // Vertex position

    glBindVertexArray(0);
    return true;
}

void Box::deleteBuffers() {
    if (buffers.size() > 0) {
        glDeleteBuffers((GLsizei)buffers.size(), buffers.data());
        buffers.clear();
    }

    if (vao != 0) {
        glDeleteVertexArrays(1, &vao);
        vao = 0;
    }
}

Box::~Box()
{
    deleteBuffers();
}