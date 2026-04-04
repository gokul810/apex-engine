#version 330 core
out vec4 FragColor;
in vec2 TexCoords;
uniform sampler2D screenTexture;

void main() {
    vec3 col = texture(screenTexture, TexCoords).rgb;
    // Classic 2006 Bloom: Everything bright gets 'blown out'
    vec3 bloom = max(col - vec3(0.4), vec3(0.0)) * 2.0;
    col += bloom;
    // Saturation & Contrast
    col = pow(col, vec3(1.1)); 
    FragColor = vec4(col, 1.0);
}