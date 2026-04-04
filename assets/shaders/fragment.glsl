#version 330 core
out vec4 FragColor;

in vec2 TexCoord;
in vec3 TangentLightPos;
in vec3 TangentViewPos;
in vec3 TangentFragPos;

uniform sampler2D diffuseMap;
uniform sampler2D normalMap;
uniform sampler2D roughMap;

void main() {
    vec3 normal = texture(normalMap, TexCoord).rgb;
    normal = normalize(normal * 2.0 - 1.0);  
    
    vec3 color = texture(diffuseMap, TexCoord).rgb;
    float rawRoughness = texture(roughMap, TexCoord).r;
    
    // --- WET LOOK FIX ---
    // Multiply by a small factor (0.1) to keep the surface very smooth/wet
    float adjustedRoughness = clamp(rawRoughness * 0.1, 0.0, 1.0); 
    // ----------------------

    vec3 ambient = 0.05 * color;

    vec3 lightDir = normalize(TangentLightPos - TangentFragPos);
    float diff = max(dot(lightDir, normal), 0.0);
    vec3 diffuse = diff * color;
    
    vec3 viewDir = normalize(TangentViewPos - TangentFragPos);
    vec3 halfwayDir = normalize(lightDir + viewDir); 
    
    // Sharpen the highlight (256.0) and make it much stronger for the wet effect
    float spec = pow(max(dot(normal, halfwayDir), 0.0), 256.0);
    vec3 specular = vec3(1.0) * spec * (1.0 - adjustedRoughness);

    FragColor = vec4(ambient + diffuse + specular, 1.0);
}