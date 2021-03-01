#version 330

uniform samplerCube SkyBoxTex;
in vec3 Vpos;
layout(location = 0) out vec4 FragColor;

void main() {
    vec3 texColor = texture(SkyBoxTex, normalize(Vpos)).rgb;
    FragColor = vec4(texColor,1);
}