#version 330 core
out vec4 FragColor;
in vec3 TexCoord;

uniform vec3 sunDir; // This is the Moon's direction vector

void main() {
    vec3 dir = normalize(TexCoord);
    vec3 L = normalize(sunDir); // Ensure moon direction is normalized

    // Deep midnight gradient
    vec3 zenithColor = vec3(0.0, 0.005, 0.01); 
    vec3 horizonColor = vec3(0.01, 0.02, 0.05);
    float gradient = clamp(dir.y, 0.0, 1.0);
    vec3 sky = mix(horizonColor, zenithColor, gradient);

    // The Moon Disk
    float moonDot = max(dot(dir, L), 0.0);
    float moon = pow(moonDot, 1500.0); // Super sharp circle
    
    // Atmospheric Glow
    float glow = pow(moonDot, 10.0) * 0.4;

    vec3 moonColor = vec3(0.9, 0.95, 1.0);
    vec3 finalColor = sky + (moon * moonColor) + (glow * moonColor);

    FragColor = vec4(finalColor, 1.0);
}