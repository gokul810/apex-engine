#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoord;
layout (location = 2) in vec3 aNormal;
layout (location = 3) in vec3 aTangent; // The new 11th float

out vec2 TexCoord;
out vec3 TangentLightPos;
out vec3 TangentViewPos;
out vec3 TangentFragPos;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

uniform vec3 lightPos; // Moon position
uniform vec3 viewPos;  // Camera position

void main() {
    gl_Position = projection * view * model * vec4(aPos, 1.0);
    TexCoord = aTexCoord;
    
    // 1. Create the TBN Matrix
    // We multiply by the Normal Matrix to handle scaling/rotation correctly
    mat3 normalMatrix = transpose(inverse(mat3(model)));
    vec3 T = normalize(normalMatrix * aTangent);
    vec3 N = normalize(normalMatrix * aNormal);
    
    // Re-orthogonalize T with respect to N (Gram-Schmidt process)
    T = normalize(T - dot(T, N) * N);
    vec3 B = cross(N, T); // Calculate Bitangent
    
    // 2. Create the matrix to go from World -> Tangent Space
    mat3 TBN = transpose(mat3(T, B, N));
    
    // 3. Transform positions to Tangent Space for the Fragment Shader
    TangentLightPos = TBN * lightPos;
    TangentViewPos  = TBN * viewPos;
    TangentFragPos  = TBN * vec3(model * vec4(aPos, 1.0));
}