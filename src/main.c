#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <stdio.h>
#include <cglm/cglm.h>   
#include <cglm/struct.h> 
#include "renderer/shader.h"

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
}

int main() {
    if (!glfwInit()) return 1;

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(800, 600, "Apex Engine", NULL, NULL);
    if (!window) { 
        glfwTerminate(); 
        return 1; 
    }
    
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) return 1;

    // Enable Depth Testing so triangles don't overlap incorrectly in 3D
    glEnable(GL_DEPTH_TEST); 

    unsigned int shaderProgram = shader_create("assets/shaders/vertex.glsl", "assets/shaders/fragment.glsl");

    float vertices[] = {
        -0.5f, -0.5f, 0.0f,
         0.5f, -0.5f, 0.0f,
         0.0f,  0.5f, 0.0f 
    };

    unsigned int VBO, VAO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // Get uniform locations once before the loop to save CPU cycles
    unsigned int modelLoc = glGetUniformLocation(shaderProgram, "model");
    unsigned int viewLoc  = glGetUniformLocation(shaderProgram, "view");
    unsigned int projLoc  = glGetUniformLocation(shaderProgram, "projection");

    glfwSwapInterval(1);

    while (!glfwWindowShouldClose(window)) {
        // Clear both the color and the depth buffer
        glClearColor(0.02f, 0.04f, 0.08f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glUseProgram(shaderProgram);

        // --- MATH ENGINE ---
        
        // 1. Model: Rotate based on time. Note: glm_rad (no 's') is used for float return
        mat4s model = glms_mat4_identity();
        model = glms_rotate(model, (float)glfwGetTime() * glm_rad(50.0f), (vec3s){0.5f, 1.0f, 0.0f});

        // 2. View: Move the camera back 3 units along Z
        mat4s view = glms_mat4_identity();
        view = glms_translate(view, (vec3s){0.0f, 0.0f, -3.0f});

        // 3. Projection: Perspective with 45 degree FoV
        int width, height;
        glfwGetFramebufferSize(window, &width, &height);
        float aspect = (float)width / (float)height;
        mat4s projection = glms_perspective(glm_rad(45.0f), aspect, 0.1f, 100.0f);

        // Upload matrices to the GPU
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, (float*)&model);
        glUniformMatrix4fv(viewLoc,  1, GL_FALSE, (float*)&view);
        glUniformMatrix4fv(projLoc,  1, GL_FALSE, (float*)&projection);

        glBindVertexArray(VAO);
        glDrawArrays(GL_TRIANGLES, 0, 3);
        
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteProgram(shaderProgram);
    glfwTerminate();
    return 0;
}