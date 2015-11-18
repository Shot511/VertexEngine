// ImGui SDL2 binding with OpenGL 3 and greater
// If you use this binding you'll need to call 4 functions: ImGui_ImplXXXX_Init(), ImGui_ImplXXXX_NewFrame(), ImGui::Render() and ImGui_ImplXXXX_Shutdown().
#pragma once
#include <imgui\imgui.h>

struct SDL_Window;
typedef union SDL_Event SDL_Event;

IMGUI_API bool        ImGui_ImplSdl_Init(SDL_Window * window);
IMGUI_API void        ImGui_ImplSdl_Shutdown();
IMGUI_API void        ImGui_ImplSdl_NewFrame(SDL_Window * window);
IMGUI_API bool        ImGui_ImplSdl_ProcessEvent(SDL_Event * event);

// Use if you want to reset your rendering device without losing ImGui state.
IMGUI_API void        ImGui_ImplSdl_InvalidateDeviceObjects();
IMGUI_API bool        ImGui_ImplSdl_CreateDeviceObjects();
