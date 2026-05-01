#version 400

layout (location = 0) in vec3 VertexPosition;
layout (location = 1) in vec2 TexCoords;
layout (location = 2) in vec3 aNormal;

uniform mat4 transform;
uniform mat4 model;
uniform mat4 view;

out vec3 FragPos;
out vec3 Normal;
out vec2 tC;
out vec4 viewSpace;

void main()
{
    FragPos = vec3(model * vec4(VertexPosition, 1.0));
    Normal = mat3(transpose(inverse(model))) * aNormal;
    tC = TexCoords;
    viewSpace = view * model * vec4(VertexPosition, 1.0);
    gl_Position = transform * vec4(VertexPosition, 1.0);
}
