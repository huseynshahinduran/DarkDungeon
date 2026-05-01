#version 400

in vec3 FragPos;
in vec3 Normal;
in vec2 tC;
in vec4 viewSpace;

uniform vec3 lightPos;
uniform vec3 viewPos;
uniform vec3 lightColor;
uniform vec3 objectColor;

uniform sampler2D diffuse;
uniform bool useTexture;

// Fog
uniform vec3 fogColor;
uniform float fogMinDist;
uniform float fogMaxDist;
uniform bool useFog;

out vec4 FragColor;

void main()
{
    // Temel renk
    vec3 baseColor;
    if (useTexture)
        baseColor = texture2D(diffuse, tC).rgb;
    else
        baseColor = objectColor;

    // Normal ve isik yonu
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(lightPos - FragPos);

    // Isik yogunlugu (diffuse)
    float intensity = max(dot(norm, lightDir), 0.0);

    // === TOON SHADING: yogunluga gore renk bantlari ===
    vec3 color;

    if (intensity > 0.75)
    {
        // En parlak bant - tam isik
        color = baseColor * 1.0;
    }
    else if (intensity > 0.5)
    {
        // Orta parlak
        color = baseColor * 0.7;
    }
    else if (intensity > 0.25)
    {
        // Koyu
        color = baseColor * 0.4;
    }
    else
    {
        // En karanlik bant
        color = baseColor * 0.15;
    }

    // Mesafe azalmasi (attenuation)
    float distance = length(lightPos - FragPos);
    float attenuation = 1.0 / (1.0 + 0.09 * distance + 0.032 * distance * distance);
    color *= attenuation;

    // Kenar cizgisi efekti (edge detection)
    vec3 viewDir = normalize(viewPos - FragPos);
    float edge = dot(viewDir, norm);
    if (edge < 0.2)
    {
        // Kenarlar siyah cizgi (outline efekti)
        color = vec3(0.0, 0.0, 0.0);
    }

    // FOG
    if (useFog)
    {
        float dist = length(viewSpace.xyz);
        float fogFactor = (dist - fogMinDist) / (fogMaxDist - fogMinDist);
        fogFactor = clamp(fogFactor, 0.0, 1.0);
        color = mix(color, fogColor, fogFactor);
    }

    FragColor = vec4(color, 1.0);
}
