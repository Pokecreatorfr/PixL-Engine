#pragma once

#include <PixL_Renderer.hpp>
#include <imgui.h>
#include <imgui_impl_sdl3.h>
#include <modules/PixL-Particle.hpp>

static bool show_debug_menu = true;
static bool ctrl_d_down = false;

inline void Imgui_debug_menu(void *user_data)
{
    bool ctrl = ImGui::IsKeyDown(ImGuiKey_LeftCtrl) || ImGui::IsKeyDown(ImGuiKey_RightCtrl);
    bool d_pressed = ImGui::IsKeyPressed(ImGuiKey_D);

    if (ctrl && d_pressed && !ctrl_d_down)
    {
        show_debug_menu = !show_debug_menu;
        ctrl_d_down = true;
    }

    if (!ImGui::IsKeyDown(ImGuiKey_D))
    {
        ctrl_d_down = false;
    }

    if (show_debug_menu)
    {
        ImGui::Begin("Debug Menu");

        // Display the current FPS
        ImGui::Text("FPS: %.1f", ImGui::GetIO().Framerate);

        // Display the number of draw calls
        ImGui::Text("Draw Calls: %d", PixL_GetDrawCalls());

        // Display the window size
        glm::vec2 windowSize = PixL_GetWindowSize();
        ImGui::Text("Window Size: %.0f x %.0f", windowSize.x, windowSize.y);

        // Display the number of particles
        ImGui::Text("Particles: %d", PixL_Particle::GetParticlesNumber());

        ImGui::End();
    }
}