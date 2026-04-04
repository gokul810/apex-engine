#version 330 core
layout (location = 0) in vec3 aPos;
out vec3 TexCoord;

uniform mat4 view;
uniform mat4 projection;

void main() {
    TexCoord = aPos;
    // Remove the mat3 conversion here, we pass the correct matrix from C
    vec4 pos = projection * view * vec4(aPos, 1.0);
    
    // The xyww trick: ensures depth is always 1.0 (maximum distance)
    gl_Position = pos.xyww; 
}