#version 400

in vec2 tC;

uniform float time;

out vec4 FragColor;

// Basit noise fonksiyonu
float noise(vec2 p)
{
    return fract(sin(dot(p, vec2(12.9898, 78.233))) * 43758.5453);
}

// Yumusak noise
float smoothNoise(vec2 p)
{
    vec2 i = floor(p);
    vec2 f = fract(p);
    f = f * f * (3.0 - 2.0 * f);

    float a = noise(i);
    float b = noise(i + vec2(1.0, 0.0));
    float c = noise(i + vec2(0.0, 1.0));
    float d = noise(i + vec2(1.0, 1.0));

    return mix(mix(a, b, f.x), mix(c, d, f.x), f.y);
}

void main()
{
    // UV koordinatlarini ortala (-0.5 to 0.5)
    vec2 uv = tC - vec2(0.5, 0.0);

    // Zaman bazli animasyon
    float t = time * 2.0;

    // Noise ile alev sekli olustur
    float n1 = smoothNoise(vec2(uv.x * 4.0, uv.y * 4.0 - t * 3.0));
    float n2 = smoothNoise(vec2(uv.x * 8.0, uv.y * 8.0 - t * 4.0));
    float n3 = smoothNoise(vec2(uv.x * 2.0, uv.y * 2.0 - t * 2.0));
    float noiseVal = n1 * 0.5 + n2 * 0.3 + n3 * 0.2;

    // Alev sekli: alta dogru genis, uste dogru dar
    float flameWidth = 0.4 - tC.y * 0.35;
    float dist = abs(uv.x);

    // Alev yogunlugu
    float flame = 1.0 - smoothstep(0.0, flameWidth, dist + noiseVal * 0.2);

    // Yukari dogru azalsin
    flame *= 1.0 - smoothstep(0.0, 1.0, tC.y);

    // Renk gradyani: beyaz -> sari -> turuncu -> kirmizi
    vec3 color;
    if (flame > 0.7)
        color = mix(vec3(1.0, 0.9, 0.5), vec3(1.0, 1.0, 0.8), (flame - 0.7) / 0.3);  // sari-beyaz
    else if (flame > 0.4)
        color = mix(vec3(1.0, 0.5, 0.0), vec3(1.0, 0.9, 0.5), (flame - 0.4) / 0.3);   // turuncu-sari
    else if (flame > 0.1)
        color = mix(vec3(0.8, 0.1, 0.0), vec3(1.0, 0.5, 0.0), (flame - 0.1) / 0.3);   // kirmizi-turuncu
    else
        color = vec3(0.3, 0.05, 0.0);  // koyu kirmizi

    // Alpha (saydamlik)
    float alpha = flame * 1.5;
    alpha = clamp(alpha, 0.0, 1.0);

    // Cok kucuk degerler icin tamamen saydam yap
    if (alpha < 0.05)
        discard;

    FragColor = vec4(color * flame * 2.0, alpha);
}
