#version 400

layout (location = 0) in vec3 VertexPosition;
layout (location = 1) in vec2 TexCoords;
layout (location = 2) in vec3 VertexNormal;

uniform mat4 transform;
uniform float time;

out vec2 tC;

void main()
{
    tC = TexCoords;

    // Alevin ustune dogru hafif dalgalanma
    vec3 pos = VertexPosition;
    pos.x += sin(time * 5.0 + pos.y * 3.0) * 0.05 * pos.y;

    gl_Position = transform * vec4(pos, 1.0);
}
