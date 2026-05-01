#version 400

// Fragment Shader - texture rengini piksel olarak ciktilar

in vec2 tC;  // vertex shader'dan gelen texture koordinatlari

uniform sampler2D diffuse;  // texture verisi

out vec4 FragColor;

void main()
{
    // texture'dan renk oku ve ciktila
    FragColor = texture2D(diffuse, tC);
}
