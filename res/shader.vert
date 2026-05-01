#version 400

// Vertex Shader - pozisyonlari donusturup fragment shader'a texture koordinatlarini gonderir

layout (location = 0) in vec3 VertexPosition;  // pozisyon verisi (VAO'dan)
layout (location = 1) in vec2 TexCoords;        // texture koordinatlari (VAO'dan)
layout (location = 2) in vec3 VertexNormal;     // normal verisi (VAO'dan)

uniform mat4 transform;  // MVP matrisi (Model * View * Projection)

out vec2 tC;  // texture koordinatlarini fragment shader'a gonder

void main()
{
    // texture koordinatlarini aktar
    tC = TexCoords;

    // pozisyonu MVP matrisi ile donustur (3D dunya -> 2D ekran)
    gl_Position = transform * vec4(VertexPosition, 1.0);
}
