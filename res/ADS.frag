#version 400

in vec3 FragPos;
in vec3 Normal;
in vec2 tC;
in vec4 viewSpace;

uniform vec3 lightPos;
uniform vec3 viewPos;
uniform vec3 lightColor;
uniform vec3 objectColor;
uniform vec3 rimColor;
uniform float rimPower;
uniform bool useRim;

uniform sampler2D diffuse;
uniform bool useTexture;

// Fog parametreleri
uniform vec3 fogColor;
uniform float fogMinDist;
uniform float fogMaxDist;
uniform bool useFog;

out vec4 FragColor;

void main()
{
    // === ADS LIGHTING ===

    // AMBIENT
    float ambientStrength = 0.25;
    vec3 ambient = ambientStrength * lightColor;

    // DIFFUSE
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(lightPos - FragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuseLight = diff * lightColor;

    // SPECULAR
    float specularStrength = 0.8;
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);
    vec3 specular = specularStrength * spec * lightColor;

    // ATTENUATION
    float distance = length(lightPos - FragPos);
    float attenuation = 1.0 / (1.0 + 0.09 * distance + 0.032 * distance * distance);

    ambient *= attenuation;
    diffuseLight *= attenuation;
    specular *= attenuation;

    // TEMEL RENK
    vec3 baseColor;
    if (useTexture)
        baseColor = texture2D(diffuse, tC).rgb;
    else
        baseColor = objectColor;

    vec3 result = (ambient + diffuseLight + specular) * baseColor;

    // === RIM LIGHTING ===
    if (useRim)
    {
        float rimDot = 1.0 - dot(viewDir, norm);
        float rimIntensity = pow(rimDot, rimPower);
        vec3 rim = rimColor * rimIntensity;
        result += rim;
    }

    // === FOG ===
    if (useFog)
    {
        // View space mesafesi
        float dist = length(viewSpace.xyz);

        // Fog faktoru hesapla (linear fog)
        // dist <= minDist: tamamen gorunur (fog=0)
        // dist >= maxDist: tamamen sisli (fog=1)
        float fogFactor = (dist - fogMinDist) / (fogMaxDist - fogMinDist);
        fogFactor = clamp(fogFactor, 0.0, 1.0);

        // Sonuc rengini fog rengi ile karistir
        result = mix(result, fogColor, fogFactor);
    }

    FragColor = vec4(result, 1.0);
}
