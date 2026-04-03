#include <glad/glad.h>  // Use GLAD instead of GLEW
#include <GLFW/glfw3.h>
#include <stdio.h>

// Callback to handle window resizing
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
    
    // Hint for Wayland/KDE compatibility
    glfwWindowHint(GLFW_RESIZABLE, GL_TRUE);

    // 3. Create Window
    GLFWwindow* window = glfwCreateWindow(800, 600, "Apex Engine | Dev", NULL, NULL);
    if (!window) {
        fprintf(stderr, "ERROR: Could not open window with GLFW3\n");
        glfwTerminate();
        return 1;
    }
    
    // Create the context BEFORE initializing GLAD
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    // 4. Initialize GLAD
    // glfwGetProcAddress is what makes this work natively on Wayland
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        fprintf(stderr, "ERROR: Failed to initialize GLAD\n");
        return 1;
    }

    // Sync with monitor refresh rate (prevents screen tearing on Plasma)
    glfwSwapInterval(1);

    // Print GPU info to terminal
    printf("Renderer: %s\n", glGetString(GL_RENDERER));
    printf("OpenGL version: %s\n", glGetString(GL_VERSION));

    // 5. The Main Loop
    while (!glfwWindowShouldClose(window)) {
        // Clear to that "NFS Carbon" dark blue-grey
        glClearColor(0.02f, 0.04f, 0.08f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        // This is where your future racing game rendering will live!
        
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}