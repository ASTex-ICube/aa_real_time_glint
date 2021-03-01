#version 330

uniform ivec2 Resolution;

uniform sampler2D Tex;
layout(location = 0) out vec4 FragColor;

void main()
{
    vec2 uv = gl_FragCoord.xy / vec2(Resolution);

    FragColor = vec4(textureLod(Tex,uv,0.).xyz,1.);
}
