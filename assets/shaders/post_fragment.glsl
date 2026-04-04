#version 330 core
out vec4 FragColor;
in vec2 TexCoords;

uniform sampler2D screenTexture;
uniform vec2 u_resolution;

uniform float u_bloom_threshold;
uniform float u_bloom_intensity;
uniform float u_vignette_strength;
uniform float u_saturation;
uniform float u_contrast;
uniform float u_chromatic_aberration;
uniform float u_film_grain;
uniform float u_color_warmth;

float random(vec2 st) {
    return fract(sin(dot(st.xy, vec2(12.9898, 78.233))) * 43758.5453);
}

vec2 hash22(vec2 p) {
    p = vec2(dot(p, vec2(127.1, 311.7)), dot(p, vec2(269.5, 183.3)));
    return fract(sin(p) * 43758.5453123);
}

vec3 sampleBloom(vec2 uv, float radius) {
    vec3 bloom = vec3(0.0);
    float total = 0.0;
    int samples = 8;
    float r = radius / u_resolution.x;
    
    for (int i = 0; i < samples; i++) {
        float angle = float(i) * 6.28318 / float(samples);
        vec2 offset = vec2(cos(angle), sin(angle)) * r;
        vec3 samp = texture(screenTexture, uv + offset).rgb;
        float brightness = dot(samp, vec3(0.2126, 0.7152, 0.0722));
        if (brightness > u_bloom_threshold) {
            bloom += samp;
            total += 1.0;
        }
    }
    return total > 0.0 ? bloom / total : vec3(0.0);
}

void main() {
    vec2 uv = TexCoords;
    vec2 center = vec2(0.5, 0.5);
    vec2 toCenter = uv - center;
    float dist = length(toCenter);
    
    float chromaR = u_chromatic_aberration * dist;
    float chromaB = -u_chromatic_aberration * dist;
    
    float r = texture(screenTexture, uv + vec2(chromaR, 0.0)).r;
    float g = texture(screenTexture, uv).g;
    float b = texture(screenTexture, uv + vec2(0.0, chromaB)).b;
    vec3 color = vec3(r, g, b);
    
    float bloomRadius = 12.0;
    vec3 bloom = sampleBloom(uv, bloomRadius);
    color += bloom * u_bloom_intensity;
    
    float luma = dot(color, vec3(0.2126, 0.7152, 0.0722));
    vec3 grayscale = vec3(luma);
    color = mix(grayscale, color, u_saturation);
    
    color = (color - 0.5) * u_contrast + 0.5;
    
    color.r += u_color_warmth * 0.3;
    color.g += u_color_warmth * 0.1;
    color.b -= u_color_warmth * 0.2;
    
    float vignette = 1.0 - dist * u_vignette_strength * 1.5;
    vignette = clamp(vignette, 0.0, 1.0);
    vignette = pow(vignette, 0.8);
    color *= vignette;
    
    float grain = (random(uv * u_resolution + vec2(0.5, 0.5)) - 0.5) * u_film_grain;
    color += grain;
    
    color = clamp(color, 0.0, 1.0);
    
    FragColor = vec4(color, 1.0);
}
