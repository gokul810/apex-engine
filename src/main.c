#define STB_IMAGE_IMPLEMENTATION
#include "vendor/stb_image.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <cglm/cglm.h>   
#include <cglm/struct.h> 
#include "renderer/shader.h"

extern void apex_ui_init(GLFWwindow* window);
extern void apex_ui_new_frame();
extern void apex_ui_draw_debug_panel(float* roadWidth);
extern void apex_ui_render();
extern void apex_ui_shutdown();

typedef struct {
    float bloom_threshold;
    float bloom_intensity;
    float vignette_strength;
    float saturation;
    float contrast;
    float chromatic_aberration;
    float film_grain;
    float color_warmth;
} PostProcessingSettings;

extern PostProcessingSettings* apex_ui_get_post_settings();

#ifndef GL_EXT_texture_filter_anisotropic
#define GL_TEXTURE_MAX_ANISOTROPY_EXT 0x84FE
#define GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT 0x84FF
#endif

// Camera & Timing
vec3s cameraPos   = {0.0f, 2.0f, 10.0f};  
vec3s cameraFront = {0.0f, 0.0f, -1.0f}; 
vec3s cameraUp    = {0.0f, 1.0f, 0.0f};
float deltaTime = 0.0f, lastFrame = 0.0f;
float lastX = 640, lastY = 360, yaw = -90.0f, pitch = 0.0f;
bool firstMouse = true;
bool mouseLookActive = false;

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods) {
    if (button == GLFW_MOUSE_BUTTON_RIGHT) {
        if (action == GLFW_PRESS) {
            mouseLookActive = true;
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
        } else if (action == GLFW_RELEASE) {
            mouseLookActive = false;
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
            firstMouse = true;
        }
    }
}

unsigned int loadTexture(char const * path) {
    unsigned int textureID;
    glGenTextures(1, &textureID);
    int width, height, nrComponents;
    unsigned char *data = stbi_load(path, &width, &height, &nrComponents, 0);
    if (data) {
        GLenum format;
        if (nrComponents == 1) format = GL_RED;
        else if (nrComponents == 3) format = GL_RGB;
        else if (nrComponents == 4) format = GL_RGBA;

        glBindTexture(GL_TEXTURE_2D, textureID);
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);

        if (glfwExtensionSupported("GL_EXT_texture_filter_anisotropic")) {
            float maxAniso; glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &maxAniso);
            glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, maxAniso);
        }

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        stbi_image_free(data);
    } else {
        printf("Texture failed to load at path: %s\n", path);
        stbi_image_free(data);
    }
    return textureID;
}

vec3s get_bezier_point(vec3s p0, vec3s p1, vec3s p2, float t) {
    float invT = 1.0f - t;
    vec3s res = glms_vec3_scale(p0, invT * invT);
    res = glms_vec3_add(res, glms_vec3_scale(p1, 2.0f * invT * t));
    res = glms_vec3_add(res, glms_vec3_scale(p2, t * t));
    return res;
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height) { glViewport(0, 0, width, height); }

void mouse_callback(GLFWwindow* window, double xpos, double ypos) {
    if (!mouseLookActive) return;
    if (firstMouse) { lastX = (float)xpos; lastY = (float)ypos; firstMouse = false; return; }
    float xoffset = ((float)xpos - lastX) * 0.1f;
    float yoffset = (lastY - (float)ypos) * 0.1f;
    lastX = (float)xpos; lastY = (float)ypos;
    yaw += xoffset; pitch += yoffset;
    if (pitch > 89.0f) pitch = 89.0f; if (pitch < -89.0f) pitch = -89.0f;
    vec3s front = { cos(glm_rad(yaw)) * cos(glm_rad(pitch)), sin(glm_rad(pitch)), sin(glm_rad(yaw)) * cos(glm_rad(pitch)) };
    cameraFront = glms_vec3_normalize(front);
}

void processInput(GLFWwindow *window) {
    float speed = 15.0f * deltaTime;
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) cameraPos = glms_vec3_add(cameraPos, glms_vec3_scale(cameraFront, speed));
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) cameraPos = glms_vec3_sub(cameraPos, glms_vec3_scale(cameraFront, speed));
    vec3s strafeDir = glms_vec3_normalize(glms_vec3_cross(cameraFront, cameraUp));
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) cameraPos = glms_vec3_sub(cameraPos, glms_vec3_scale(strafeDir, speed));
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) cameraPos = glms_vec3_add(cameraPos, glms_vec3_scale(strafeDir, speed));
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) glfwSetWindowShouldClose(window, true);
}

int main() {
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3); glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    
    glfwWindowHint(GLFW_SAMPLES, 4);

    GLFWwindow* window = glfwCreateWindow(1280, 640, "Apex Engine | Anti-Aliased Long Road", NULL, NULL);
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetMouseButtonCallback(window, mouse_button_callback);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
    gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);

    apex_ui_init(window);
    glEnable(GL_MULTISAMPLE); 

    unsigned int shaderProgram = shader_create("assets/shaders/vertex.glsl", "assets/shaders/fragment.glsl");
    unsigned int skyShader = shader_create("assets/shaders/sky_vertex.glsl", "assets/shaders/sky_fragment.glsl");
    unsigned int postShader = shader_create("assets/shaders/post_vertex.glsl", "assets/shaders/post_fragment.glsl");

    // --- LONGER, GENTLER ROAD ---
    vec3s p0 = {0.0f, -2.0f, 0.0f}; 
    vec3s p1 = {15.0f, -2.0f, -150.0f}; 
    vec3s p2 = {0.0f, -2.0f, -300.0f}; 
    
    int segments = 400; 
    float roadWidth = 10.0f; 
    float textureRepeatDist = 12.0f; 
    float uvScale = 1.0f / textureRepeatDist;

    float* vertices = malloc(segments * 6 * 11 * sizeof(float));
    float distL = 0.0f, distR = 0.0f;

    for (int i = 0; i < segments; i++) {
        float t1 = (float)i / segments;
        float t2 = (float)(i + 1.002f) / segments; // Tiny overlap 

        vec3s pos1 = get_bezier_point(p0, p1, p2, t1), pos2 = get_bezier_point(p0, p1, p2, t2);
        vec3s T = glms_vec3_normalize(glms_vec3_sub(pos2, pos1));
        vec3s side = glms_vec3_normalize(glms_vec3_cross(T, (vec3s){0.0f, 1.0f, 0.0f}));
        vec3s N = glms_vec3_normalize(glms_vec3_cross(side, T));

        vec3s l1 = glms_vec3_add(pos1, glms_vec3_scale(side, -roadWidth));
        vec3s r1 = glms_vec3_add(pos1, glms_vec3_scale(side, roadWidth));
        vec3s l2 = glms_vec3_add(pos2, glms_vec3_scale(side, -roadWidth));
        vec3s r2 = glms_vec3_add(pos2, glms_vec3_scale(side, roadWidth));
        
        float stepL = glms_vec3_distance(l1, l2);
        float stepR = glms_vec3_distance(r1, r2);

        float vL1 = distL * uvScale, vL2 = (distL + stepL) * uvScale;
        float vR1 = distR * uvScale, vR2 = (distR + stepR) * uvScale;
        float uMax = (roadWidth * 2.0f) * uvScale;
        
        float vData[] = {
            l1.x, l1.y, l1.z,   0.0f, vL1,      N.x, N.y, N.z, T.x, T.y, T.z,
            r1.x, r1.y, r1.z,   uMax, vR1,      N.x, N.y, N.z, T.x, T.y, T.z,
            r2.x, r2.y, r2.z,   uMax, vR2,      N.x, N.y, N.z, T.x, T.y, T.z,

            l1.x, l1.y, l1.z,   0.0f, vL1,      N.x, N.y, N.z, T.x, T.y, T.z,
            r2.x, r2.y, r2.z,   uMax, vR2,      N.x, N.y, N.z, T.x, T.y, T.z,
            l2.x, l2.y, l2.z,   0.0f, vL2,      N.x, N.y, N.z, T.x, T.y, T.z
        };
        memcpy(&vertices[i * 6 * 11], vData, sizeof(vData));
        distL += stepL; distR += stepR;
    }

    unsigned int VAO, VBO; glGenVertexArrays(1, &VAO); glGenBuffers(1, &VBO);
    glBindVertexArray(VAO); glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, segments * 6 * 11 * sizeof(float), vertices, GL_STATIC_DRAW);
    
    int stride = 11 * sizeof(float);
    glEnableVertexAttribArray(0); glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, (void*)0);
    glEnableVertexAttribArray(1); glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, stride, (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(2); glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, stride, (void*)(5 * sizeof(float)));
    glEnableVertexAttribArray(3); glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, stride, (void*)(8 * sizeof(float)));

    // Skybox & Quad
    float skyboxVertices[] = { -1,1,-1, -1,-1,-1, 1,-1,-1, 1,-1,-1, 1,1,-1, -1,1,-1, -1,-1,1, -1,-1,-1, -1,1,-1, -1,1,-1, -1,1,1, -1,-1,1, 1,-1,-1, 1,-1,1, 1,1,1, 1,1,1, 1,1,-1, 1,-1,-1, -1,-1,1, -1,1,1, 1,1,1, 1,1,1, 1,-1,1, -1,-1,1, -1,1,-1, 1,1,-1, 1,1,1, 1,1,1, -1,1,1, -1,1,-1, -1,-1,-1, -1,-1,1, 1,-1,-1, 1,-1,-1, -1,-1,1, 1,-1,1 };
    unsigned int skyVAO, skyVBO; glGenVertexArrays(1, &skyVAO); glGenBuffers(1, &skyVBO);
    glBindVertexArray(skyVAO); glBindBuffer(GL_ARRAY_BUFFER, skyVBO); glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), skyboxVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0); glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3*4, (void*)0);

    float quadVertices[] = { -1,1,0,1, -1,-1,0,0, 1,-1,1,0, -1,1,0,1, 1,-1,1,0, 1,1,1,1 };
    unsigned int quadVAO, quadVBO; glGenVertexArrays(1, &quadVAO); glGenBuffers(1, &quadVBO);
    glBindVertexArray(quadVAO); glBindBuffer(GL_ARRAY_BUFFER, quadVBO); glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), quadVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0); glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4*4, (void*)0);
    glEnableVertexAttribArray(1); glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4*4, (void*)(2*4));

    // MSAA FBO Setup
    unsigned int msaaFBO, msaaTex, msaaRBO;
    glGenFramebuffers(1, &msaaFBO); glBindFramebuffer(GL_FRAMEBUFFER, msaaFBO);
    glGenTextures(1, &msaaTex); glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, msaaTex);
    glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, 4, GL_RGB, 1280, 640, GL_TRUE);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D_MULTISAMPLE, msaaTex, 0);
    glGenRenderbuffers(1, &msaaRBO); glBindRenderbuffer(GL_RENDERBUFFER, msaaRBO);
    glRenderbufferStorageMultisample(GL_RENDERBUFFER, 4, GL_DEPTH24_STENCIL8, 1280, 640);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, msaaRBO);

    // Resolve FBO Setup
    unsigned int screenFBO, screenTex;
    glGenFramebuffers(1, &screenFBO); glBindFramebuffer(GL_FRAMEBUFFER, screenFBO);
    glGenTextures(1, &screenTex); glBindTexture(GL_TEXTURE_2D, screenTex);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 1280, 640, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, screenTex, 0);

    unsigned int roadDiff = loadTexture("assets/textures/asphalt_02_diff_4k.jpg");
    unsigned int roadNor = loadTexture("assets/textures/asphalt_02_nor_gl_4k.png");
    unsigned int roadRough = loadTexture("assets/textures/asphalt_02_rough_4k.jpg");
    vec3s sunDir = {0.2f, 0.5f, -1.0f};

    while (!glfwWindowShouldClose(window)) {
        float time = (float)glfwGetTime(); deltaTime = time - lastFrame; lastFrame = time;
        processInput(window);
        apex_ui_new_frame();
        apex_ui_draw_debug_panel(&roadWidth);

        glBindFramebuffer(GL_FRAMEBUFFER, msaaFBO);
        glEnable(GL_DEPTH_TEST); glClearColor(0.01f, 0.02f, 0.04f, 1.0f); glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        mat4s projection = glms_perspective(glm_rad(45.0f), 1280.0f/640.0f, 0.1f, 1000.0f);
        mat4s view = glms_lookat(cameraPos, glms_vec3_add(cameraPos, cameraFront), cameraUp);

        // Sky
        mat4s skyView = glms_mat4_identity(); skyView = glms_mat4_ins3(glms_mat4_pick3(view), skyView);
        glDepthMask(GL_FALSE); glDepthFunc(GL_LEQUAL); glUseProgram(skyShader);
        glUniformMatrix4fv(glGetUniformLocation(skyShader, "view"), 1, GL_FALSE, (float*)&skyView);
        glUniformMatrix4fv(glGetUniformLocation(skyShader, "projection"), 1, GL_FALSE, (float*)&projection);
        glUniform3f(glGetUniformLocation(skyShader, "sunDir"), sunDir.x, sunDir.y, sunDir.z);
        glBindVertexArray(skyVAO); glDrawArrays(GL_TRIANGLES, 0, 36);
        glDepthMask(GL_TRUE); glDepthFunc(GL_LESS);

        // Road
        mat4s modelId = glms_mat4_identity(); 
        glUseProgram(shaderProgram);
        glActiveTexture(GL_TEXTURE0); glBindTexture(GL_TEXTURE_2D, roadDiff);
        glActiveTexture(GL_TEXTURE1); glBindTexture(GL_TEXTURE_2D, roadNor);
        glActiveTexture(GL_TEXTURE2); glBindTexture(GL_TEXTURE_2D, roadRough);
        glUniform1i(glGetUniformLocation(shaderProgram, "diffuseMap"), 0);
        glUniform1i(glGetUniformLocation(shaderProgram, "normalMap"), 1);
        glUniform1i(glGetUniformLocation(shaderProgram, "roughMap"), 2);
        glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "view"), 1, GL_FALSE, (float*)&view);
        glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "projection"), 1, GL_FALSE, (float*)&projection);
        glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "model"), 1, GL_FALSE, (float*)&modelId);
        vec3s lightPosition = glms_vec3_scale(sunDir, 100.0f); 
        glUniform3f(glGetUniformLocation(shaderProgram, "lightPos"), lightPosition.x, lightPosition.y, lightPosition.z);
        glUniform3f(glGetUniformLocation(shaderProgram, "viewPos"), cameraPos.x, cameraPos.y, cameraPos.z);
        glBindVertexArray(VAO); glDrawArrays(GL_TRIANGLES, 0, segments * 6);

        // Resolve MSAA
        glBindFramebuffer(GL_READ_FRAMEBUFFER, msaaFBO);
        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, screenFBO);
        glBlitFramebuffer(0, 0, 1280, 640, 0, 0, 1280, 640, GL_COLOR_BUFFER_BIT, GL_NEAREST);

        // Post-processing
        glBindFramebuffer(GL_FRAMEBUFFER, 0); glDisable(GL_DEPTH_TEST); glClear(GL_COLOR_BUFFER_BIT);
        glUseProgram(postShader); glBindVertexArray(quadVAO);
        glActiveTexture(GL_TEXTURE0); glBindTexture(GL_TEXTURE_2D, screenTex);
        
        PostProcessingSettings* pp = (PostProcessingSettings*)apex_ui_get_post_settings();
        glUniform1i(glGetUniformLocation(postShader, "screenTexture"), 0);
        glUniform2f(glGetUniformLocation(postShader, "u_resolution"), 1280.0f, 640.0f);
        glUniform1f(glGetUniformLocation(postShader, "u_bloom_threshold"), pp->bloom_threshold);
        glUniform1f(glGetUniformLocation(postShader, "u_bloom_intensity"), pp->bloom_intensity);
        glUniform1f(glGetUniformLocation(postShader, "u_vignette_strength"), pp->vignette_strength);
        glUniform1f(glGetUniformLocation(postShader, "u_saturation"), pp->saturation);
        glUniform1f(glGetUniformLocation(postShader, "u_contrast"), pp->contrast);
        glUniform1f(glGetUniformLocation(postShader, "u_chromatic_aberration"), pp->chromatic_aberration);
        glUniform1f(glGetUniformLocation(postShader, "u_film_grain"), pp->film_grain);
        glUniform1f(glGetUniformLocation(postShader, "u_color_warmth"), pp->color_warmth);
        
        glDrawArrays(GL_TRIANGLES, 0, 6);

        glDisable(GL_DEPTH_TEST);
        apex_ui_render();

        glfwSwapBuffers(window); glfwPollEvents();
    }
    free(vertices);
    apex_ui_shutdown();
    glfwTerminate(); return 0;
}