#pragma once

#include <PixL_Renderer.hpp>
#include <imgui.h>
#include <imgui_impl_sdl3.h>
#include <imgui_impl_sdlgpu3.h>
#include <map>
#include <modules/Modules-config.hpp>
#include <string>

typedef void (*Imgui_Draw_Function)(void *user_data);

struct Imgui_Callback
{
    Imgui_Draw_Function func;
    void *user_data;
};

class PixL_Imgui
{
protected:
    PixL_Imgui();
    ~PixL_Imgui();
    static PixL_Imgui *_instance;

public:
    static PixL_Imgui &getInstance()
    {
        if (!_instance)
        {
            _instance = new PixL_Imgui();
        }
        return *_instance;
    }

    uint16_t addCallback(Imgui_Draw_Function func, void *user_data)
    {
        uint16_t id = 0;
        Imgui_Callback callback = {func, user_data};

        bool found = false;
        while (!found)
        {
            if (callbacks.find(id) == callbacks.end())
            {
                found = true;
            }
            else
            {
                id++;
            }
        }

        callbacks[id] = callback;
        return id;
    }

    void removeCallback(uint16_t id)
    {
        auto it = callbacks.find(id);
        if (it != callbacks.end())
        {
            callbacks.erase(it);
        }
    }

private:
    SDL_GPUDevice *gpu_device;
    SDL_Window *window;

    std::map<uint16_t, Imgui_Callback> callbacks;

    friend void PixL_Imgui_Update_Callbacks();
};

void PixL_Imgui_Update_Callbacks();
#define PixL_Imgui_ProcessEvents(e_ptr) (ImGui_ImplSDL3_ProcessEvent((e_ptr)))
