#version 330

void main()
{
    gl_Position = vec4(2.*vec2(gl_VertexID%2,gl_VertexID/2)-1.,0,1);
}
