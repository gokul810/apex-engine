#ifndef SHADER_H
#define SHADER_H

#include <glad/glad.h>

unsigned int shader_create(const char* vertexPath, const char* fragmentPath);
void shader_use(unsigned int ID);

#endif