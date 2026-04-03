#version 330 core
layout (location = 0) in vec3 aPos; // The position variable has attribute position 0

void main() {
    // We just pass the position straight through for now
    gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);
}