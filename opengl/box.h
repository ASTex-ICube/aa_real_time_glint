#pragma once

#include <iostream>
#include <vector>

#include "openglogl.h"

class Box
{
public:
	Box(const float& size = 1.f);
	~Box();

    GLuint GetVao() { return vao; };
    GLuint GetElementBuffer() { return buffers[0]; }
    GLuint GetPositionBuffer() { return buffers[1]; }
    GLuint GetNumberOfVertices() { return nb_vertices; }

    void draw();

private:
    GLuint nb_vertices;
    GLuint vao;

    std::vector<GLuint> buffers;

private:
	bool createBuffers(std::vector<GLfloat>* vertices, std::vector<GLuint>* indices);
    void deleteBuffers();
};
