#version 400

layout (location = 0) in vec3 VertexPosition;
layout (location = 1) in vec2 TexCoords;
layout (location = 2) in vec3 aNormal;

uniform mat4 transform;  // MVP matrisi
uniform mat4 model;      // Model matrisi
uniform mat4 view;       // View matrisi (fog mesafesi icin)

out vec3 FragPos;     // world space pozisyon
out vec3 Normal;      // world space normal
out vec2 tC;          // texture koordinatlari
out vec4 viewSpace;   // view space pozisyon (fog icin)

void main()
{
    FragPos = vec3(model * vec4(VertexPosition, 1.0));
    Normal = mat3(transpose(inverse(model))) * aNormal;
    tC = TexCoords;

    // Fog icin view space pozisyon
    viewSpace = view * model * vec4(VertexPosition, 1.0);

    gl_Position = transform * vec4(VertexPosition, 1.0);
}
