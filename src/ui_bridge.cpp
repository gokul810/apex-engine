#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include <stdio.h>
#include <string.h>

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

static PostProcessingSettings g_settings = {
    .bloom_threshold = 0.6f,
    .bloom_intensity = 0.4f,
    .vignette_strength = 0.5f,
    .saturation = 1.1f,
    .contrast = 1.15f,
    .chromatic_aberration = 0.003f,
    .film_grain = 0.03f,
    .color_warmth = 0.1f
};

static PostProcessingSettings g_defaults = {
    .bloom_threshold = 0.6f,
    .bloom_intensity = 0.4f,
    .vignette_strength = 0.5f,
    .saturation = 1.1f,
    .contrast = 1.15f,
    .chromatic_aberration = 0.003f,
    .film_grain = 0.03f,
    .color_warmth = 0.1f
};

extern "C" {
    PostProcessingSettings* apex_ui_get_post_settings();

    void apex_ui_init(GLFWwindow* window) {
        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        ImGuiIO* io = &ImGui::GetIO();
        io->IniSavingRate = 0.0f;
        ImGui_ImplGlfw_InitForOpenGL(window, true);
        ImGui_ImplOpenGL3_Init("#version 330");
        ImGui::StyleColorsDark();
        ImGuiStyle* style = &ImGui::GetStyle();
        style->WindowRounding = 4.0f;
        style->FrameRounding = 4.0f;
        style->GrabRounding = 4.0f;
        style->Colors[ImGuiCol_WindowBg].w = 0.92f;
        style->Colors[ImGuiCol_Header] = ImVec4(0.15f, 0.15f, 0.15f, 0.9f);
        style->Colors[ImGuiCol_HeaderHovered] = ImVec4(0.2f, 0.2f, 0.2f, 0.9f);
        style->Colors[ImGuiCol_HeaderActive] = ImVec4(0.25f, 0.25f, 0.25f, 0.9f);
        
        FILE* f = fopen("config/post_processing.cfg", "r");
        if (f) {
            fscanf(f, "# Post Processing Settings\n");
            fscanf(f, "bloom_threshold=%f\n", &g_settings.bloom_threshold);
            fscanf(f, "bloom_intensity=%f\n", &g_settings.bloom_intensity);
            fscanf(f, "vignette_strength=%f\n", &g_settings.vignette_strength);
            fscanf(f, "saturation=%f\n", &g_settings.saturation);
            fscanf(f, "contrast=%f\n", &g_settings.contrast);
            fscanf(f, "chromatic_aberration=%f\n", &g_settings.chromatic_aberration);
            fscanf(f, "film_grain=%f\n", &g_settings.film_grain);
            fscanf(f, "color_warmth=%f\n", &g_settings.color_warmth);
            fclose(f);
        }
    }

    void apex_ui_new_frame() {
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
    }

    void apex_ui_save_settings() {
        FILE* f = fopen("config/post_processing.cfg", "w");
        if (f) {
            fprintf(f, "# Post Processing Settings\n");
            fprintf(f, "bloom_threshold=%f\n", g_settings.bloom_threshold);
            fprintf(f, "bloom_intensity=%f\n", g_settings.bloom_intensity);
            fprintf(f, "vignette_strength=%f\n", g_settings.vignette_strength);
            fprintf(f, "saturation=%f\n", g_settings.saturation);
            fprintf(f, "contrast=%f\n", g_settings.contrast);
            fprintf(f, "chromatic_aberration=%f\n", g_settings.chromatic_aberration);
            fprintf(f, "film_grain=%f\n", g_settings.film_grain);
            fprintf(f, "color_warmth=%f\n", g_settings.color_warmth);
            fclose(f);
        }
    }

    void apex_ui_reset_defaults() {
        g_settings = g_defaults;
    }

    void apex_ui_draw_debug_panel(float* roadWidth) {
        ImGuiIO* io = &ImGui::GetIO();
        int display_w = (int)io->DisplaySize.x;
        
        float panelWidth = 240.0f;
        float margin = 10.0f;
        float xPos = (float)display_w - panelWidth - margin;
        
        static bool first_frame = true;
        if (first_frame) {
            ImGui::SetNextWindowPos(ImVec2(xPos, margin), ImGuiCond_Always);
            ImGui::SetNextWindowSize(ImVec2(panelWidth, 0), ImGuiCond_Always);
            first_frame = false;
        }
        
        ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoResize | 
                                       ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_AlwaysAutoResize;
        
        ImGui::Begin("Apex Engine Controls", NULL, window_flags);
        
        ImGui::PushStyleColor(ImGuiCol_Header, ImVec4(0.3f, 0.5f, 0.8f, 0.8f));
        ImGui::PushStyleColor(ImGuiCol_HeaderHovered, ImVec4(0.4f, 0.6f, 0.9f, 0.8f));
        
        if (ImGui::CollapsingHeader("##Road", ImGuiTreeNodeFlags_DefaultOpen)) {
            ImGui::Indent();
            ImGui::Text("Road Settings");
            ImGui::SliderFloat("Width", roadWidth, 1.0f, 50.0f, "%.1f");
            ImGui::Unindent();
        }
        
        if (ImGui::CollapsingHeader("##Bloom", ImGuiTreeNodeFlags_DefaultOpen)) {
            ImGui::Indent();
            ImGui::Text("Bloom Effect");
            ImGui::SliderFloat("Threshold", &g_settings.bloom_threshold, 0.0f, 1.0f, "%.2f");
            ImGui::SliderFloat("Intensity", &g_settings.bloom_intensity, 0.0f, 2.0f, "%.2f");
            ImGui::Unindent();
        }
        
        if (ImGui::CollapsingHeader("##Color", ImGuiTreeNodeFlags_DefaultOpen)) {
            ImGui::Indent();
            ImGui::Text("Color Grading");
            ImGui::SliderFloat("Saturation", &g_settings.saturation, 0.0f, 2.0f, "%.2f");
            ImGui::SliderFloat("Contrast", &g_settings.contrast, 0.5f, 2.0f, "%.2f");
            ImGui::SliderFloat("Warmth", &g_settings.color_warmth, -0.3f, 0.3f, "%.2f");
            ImGui::Unindent();
        }
        
        if (ImGui::CollapsingHeader("##Effects", ImGuiTreeNodeFlags_DefaultOpen)) {
            ImGui::Indent();
            ImGui::Text("Screen Effects");
            ImGui::SliderFloat("Vignette", &g_settings.vignette_strength, 0.0f, 1.5f, "%.2f");
            ImGui::SliderFloat("Chromatic", &g_settings.chromatic_aberration, 0.0f, 0.02f, "%.3f");
            ImGui::SliderFloat("Grain", &g_settings.film_grain, 0.0f, 0.1f, "%.3f");
            ImGui::Unindent();
        }
        
        ImGui::PopStyleColor(2);
        
        ImGui::Spacing();
        ImGui::Separator();
        ImGui::Spacing();
        
        ImGui::TextColored(ImVec4(0.6f, 0.8f, 1.0f, 1.0f), "FPS: %.1f", io->Framerate);
        
        ImGui::Spacing();
        ImGui::Separator();
        ImGui::Spacing();
        
        if (ImGui::Button("Save Settings", ImVec2(120, 0))) {
            apex_ui_save_settings();
        }
        ImGui::SameLine();
        if (ImGui::Button("Reset", ImVec2(60, 0))) {
            apex_ui_reset_defaults();
        }
        
        ImGui::Spacing();
        ImGui::Text("Hold Right Click to look");
        ImGui::Text("WASD - Move");
        ImGui::Text("ESC - Exit");
        
        ImGui::End();
    }

    void apex_ui_render() {
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    }

    void apex_ui_shutdown() {
        apex_ui_save_settings();
        ImGui_ImplOpenGL3_Shutdown();
        ImGui_ImplGlfw_Shutdown();
        ImGui::DestroyContext();
    }

    PostProcessingSettings* apex_ui_get_post_settings() {
        return &g_settings;
    }
}
