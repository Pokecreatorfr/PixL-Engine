#pragma once

#include <Camera.hpp>
#include <generated/shaders.hpp>
#include <glad/glad.h>


class Renderer
{
    protected:
        Renderer();
        static Renderer* instance_;
    public:
        static Renderer* GetInstance();
        void Render();
    private:
        Camera* camera;
};


