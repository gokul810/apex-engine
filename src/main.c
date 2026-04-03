#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <stdio.h>
#include "renderer/shader.h" // Our new shader loader

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
}

int main() {
    // 1. Initialize GLFW
    if (!glfwInit()) {
        fprintf(stderr, "ERROR: Could not start GLFW3\n");
        return 1;
    }

    // 2. Set OpenGL 3.3 Core Profile
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_RESIZABLE, GL_TRUE);

    // 3. Create Window
    GLFWwindow* window = glfwCreateWindow(800, 600, "Apex Engine | Day 2", NULL, NULL);
    if (!window) {
        fprintf(stderr, "ERROR: Could not open window with GLFW3\n");
        glfwTerminate();
        return 1;
    }
    
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    // 4. Initialize GLAD
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        fprintf(stderr, "ERROR: Failed to initialize GLAD\n");
        return 1;
    }

    // --- DAY 2: SHADERS & GEOMETRY ---

    // Build and compile our shader program
    unsigned int shaderProgram = shader_create("assets/shaders/vertex.glsl", "assets/shaders/fragment.glsl");

    // Define vertices for a simple triangle (X, Y, Z)
    float vertices[] = {
        -0.5f, -0.5f, 0.0f, // Left  (Bottom)
         0.5f, -0.5f, 0.0f, // Right (Bottom)
         0.0f,  0.5f, 0.0f  // Top   (Middle)
    };

    unsigned int VBO, VAO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    // 1. Bind Vertex Array Object (VAO)
    glBindVertexArray(VAO);

    // 2. Copy vertices array into a buffer for OpenGL to use (VBO)
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    // 3. Set the vertex attributes pointers (Tell OpenGL how to interpret the data)
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // Unbind for safety
    glBindBuffer(GL_ARRAY_BUFFER, 0); 
    glBindVertexArray(0); 

    // --- END GEOMETRY SETUP ---

    glfwSwapInterval(1);
    printf("Renderer: %s\n", glGetString(GL_RENDERER));

    // 5. The Main Loop
    while (!glfwWindowShouldClose(window)) {
        // Background color
        glClearColor(0.02f, 0.04f, 0.08f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        // Render the triangle
        glUseProgram(shaderProgram);
        glBindVertexArray(VAO);
        glDrawArrays(GL_TRIANGLES, 0, 3);
        
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // Cleanup memory before exiting
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteProgram(shaderProgram);

    glfwTerminate();
    return 0;
}