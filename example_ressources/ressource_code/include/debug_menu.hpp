#pragma once
#include <SDL3/SDL.h>
#include <imgui.h>
#include <iostream>
#include <retro_renderer/retro_renderer.hpp>

class DebugMenu
{
public:
    static void Init()
    {
        current_mode = static_cast<int>(RetroRenderer::DebugViewMode::Final);
        depth_near = 1.0f;
        depth_far = 5000.0f;
        show_window = true;
        shadows_enabled = RetroRenderer::GetShadowsEnabled();
        last_frame_time = SDL_GetTicks();
        frame_count = 0;
        fps = 0.0f;
    }

    static void Render()
    {
        
        Uint64 current_time = SDL_GetTicks();
        frame_count++;
        if (current_time - last_frame_time >= 1000)
        {
            fps = frame_count * 1000.0f / (current_time - last_frame_time);
            frame_count = 0;
            last_frame_time = current_time;
        }

        if (!show_window)
            return;

        ImGui::SetNextWindowPos(ImVec2(10, 10), ImGuiCond_FirstUseEver);
        ImGui::SetNextWindowSize(ImVec2(300, 320), ImGuiCond_FirstUseEver);

        if (ImGui::Begin("Debug View", &show_window))
        {
            
            ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), "FPS: %.1f (%.2f ms)", fps, fps > 0 ? 1000.0f / fps : 0.0f);
            ImGui::Separator();

            
            if (ImGui::Checkbox("Shadows Enabled", &shadows_enabled))
            {
                RetroRenderer::SetShadowsEnabled(shadows_enabled);
            }
            ImGui::Separator();
            const char *mode_names[] = {
                "Final Render",
                "Position (World)",
                "Normal (World)",
                "Albedo",
                "Material (M/R/AO)",
                "Emissive",
                "Depth (Linearized)"};

            int mode_count = static_cast<int>(RetroRenderer::DebugViewMode::COUNT);

            if (ImGui::Combo("View Mode", &current_mode, mode_names, mode_count))
            {
                RetroRenderer::Debug_SetViewMode(static_cast<RetroRenderer::DebugViewMode>(current_mode));
            }

            ImGui::Spacing();
            ImGui::TextColored(ImVec4(0.5f, 0.8f, 1.0f, 1.0f), "Current: %s", mode_names[current_mode]);

            if (current_mode == static_cast<int>(RetroRenderer::DebugViewMode::Depth))
            {
                ImGui::Spacing();
                ImGui::Separator();
                ImGui::Text("Depth Range Settings");

                bool changed = false;
                changed |= ImGui::DragFloat("Near Plane", &depth_near, 0.1f, 0.01f, depth_far - 0.01f, "%.2f");
                changed |= ImGui::DragFloat("Far Plane", &depth_far, 10.0f, depth_near + 0.01f, 100000.0f, "%.1f");

                if (changed)
                {
                    RetroRenderer::Debug_SetDepthRange(depth_near, depth_far);
                }

                ImGui::TextColored(ImVec4(0.7f, 0.7f, 0.7f, 1.0f), "White = near, Black = far");
            }

            ImGui::Spacing();
            ImGui::Separator();
            ImGui::TextColored(ImVec4(0.6f, 0.6f, 0.6f, 1.0f), "Shortcuts:");
            ImGui::BulletText("F1: Final");
            ImGui::BulletText("F2: Position");
            ImGui::BulletText("F3: Normal");
            ImGui::BulletText("F4: Albedo");
            ImGui::BulletText("F5: Material");
            ImGui::BulletText("F6: Emissive");
            ImGui::BulletText("F7: Depth");
        }
        ImGui::End();
    }

    static void HandleKeyboardShortcut(SDL_Keycode key)
    {
        RetroRenderer::DebugViewMode new_mode = RetroRenderer::Debug_GetViewMode();

        switch (key)
        {
        case SDLK_F1:
            new_mode = RetroRenderer::DebugViewMode::Final;
            break;
        case SDLK_F2:
            new_mode = RetroRenderer::DebugViewMode::GBufferPosition;
            break;
        case SDLK_F3:
            new_mode = RetroRenderer::DebugViewMode::GBufferNormal;
            break;
        case SDLK_F4:
            new_mode = RetroRenderer::DebugViewMode::GBufferAlbedo;
            break;
        case SDLK_F5:
            new_mode = RetroRenderer::DebugViewMode::GBufferMaterial;
            break;
        case SDLK_F6:
            new_mode = RetroRenderer::DebugViewMode::GBufferEmissive;
            break;
        case SDLK_F7:
            new_mode = RetroRenderer::DebugViewMode::Depth;
            break;
        case SDLK_F8:
            show_window = !show_window;
            break;
        default:
            return; 
        }

        RetroRenderer::Debug_SetViewMode(new_mode);
        current_mode = static_cast<int>(new_mode);
    }

    static void Toggle()
    {
        show_window = !show_window;
    }

    static bool IsVisible()
    {
        return show_window;
    }

private:
    static inline int current_mode = 0;
    static inline float depth_near = 1.0f;
    static inline float depth_far = 5000.0f;
    static inline bool show_window = true;
    static inline bool shadows_enabled = true;
    static inline Uint64 last_frame_time = 0;
    static inline int frame_count = 0;
    static inline float fps = 0.0f;
};
