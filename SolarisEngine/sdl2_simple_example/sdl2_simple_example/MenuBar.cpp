#include "MenuBar.h"
#include <SDL2/SDL.h>
#include <GL/glew.h>
#include "App.h"
#include "imgui.h"
#include <vector>
#include <iostream>

#if defined(_WIN32) || defined(_WIN64)
#include <windows.h>  // Incluye Windows.h para acceder a GlobalMemoryStatusEx
#elif defined(__linux__)
#include <unistd.h>
#include <fstream>
#elif defined(__APPLE__)
#include <mach/mach.h>
#endif

MenuBar::MenuBar()
    : showDemo(false), fpsHistory{}, currentFPS(0.0f), frameCounter(0) {}

size_t MenuBar::GetMemoryUsage() {
#if defined(_WIN32) || defined(_WIN64)
    MEMORYSTATUSEX memInfo;
    memInfo.dwLength = sizeof(MEMORYSTATUSEX);
    if (GlobalMemoryStatusEx(&memInfo)) {
        return (memInfo.ullTotalPhys - memInfo.ullAvailPhys) / (1024 * 1024);  // Devuelve en MB
    }
    return 0;

#elif defined(__linux__)
    std::ifstream statm("/proc/self/statm");
    size_t memory = 0;
    if (statm.is_open()) {
        statm >> memory;
        memory *= sysconf(_SC_PAGESIZE); // Convertir a bytes
    }
    return memory / 1024 / 1024;

#elif defined(__APPLE__)
    struct mach_task_basic_info info;
    mach_msg_type_number_t infoCount = MACH_TASK_BASIC_INFO_COUNT;
    if (task_info(mach_task_self(), MACH_TASK_BASIC_INFO, (task_info_t)&info, &infoCount) == KERN_SUCCESS) {
        return info.resident_size / 1024 / 1024;
    }
    return 0;
#else
    return 0;
#endif
}

void MenuBar::Render() {
    // Incrementar el contador de frames
    frameCounter++;

    // Calcular FPS y actualizar historial cada 10 frames
    if (frameCounter >= 10) {
        float fps = 1.0f / ImGui::GetIO().DeltaTime;
        fpsHistory.push_back(fps);
        if (fpsHistory.size() > 100) {
            fpsHistory.erase(fpsHistory.begin());  // Limitar historial de FPS
        }
        currentFPS = fps;
        frameCounter = 0;  // Reiniciar contador
    }

    if (ImGui::BeginMainMenuBar()) {
        if (ImGui::BeginMenu("Menu")) {
            if (ImGui::MenuItem("Exit")) {
                SDL_Event quit_event;
                quit_event.type = SDL_QUIT;
                SDL_PushEvent(&quit_event);
            }
            if (ImGui::MenuItem("ShowDemo")) {
                showDemo = !showDemo;
            }
            ImGui::EndMenu();
        }

        if (ImGui::BeginMenu("Config")) {
            ImGui::Text("FPS: %.1f", currentFPS);
            ImGui::PlotLines("FPS History", fpsHistory.data(), fpsHistory.size(), 0, nullptr, 0.0f, 100.0f, ImVec2(0, 80));

            size_t memUsedMB = GetMemoryUsage();
            ImGui::Text("Memory Usage: %zu MB", memUsedMB);

            ImGui::Separator();
            ImGui::Text("Hardware & Software Info:");
            ImGui::Text("SDL Version: %d.%d.%d", SDL_MAJOR_VERSION, SDL_MINOR_VERSION, SDL_PATCHLEVEL);
            ImGui::Text("OpenGL Version: %s", glGetString(GL_VERSION));

            ImGui::EndMenu();
        }

        if (ImGui::BeginMenu("Primitive")) {
            ImGui::EndMenu();
        }

        ImGui::EndMainMenuBar();
    }

    if (showDemo) {
        ImGui::ShowDemoWindow();
    }
}
