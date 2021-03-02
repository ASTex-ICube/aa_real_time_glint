#version 330

layout (location = 0) in vec3 VertexPosition;

uniform mat4 VP;

out vec3 Vpos;

void main() {
    Vpos = VertexPosition;
    gl_Position = VP * vec4(VertexPosition,1.0);
}