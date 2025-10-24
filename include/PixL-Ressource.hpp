#pragma once

#include <PixL_Pak.hpp>
#include <PixL_Renderer.hpp>
#include <const/blank_texture.h>

class PixL_Ressource
{
protected:
    PixL_Ressource();
    ~PixL_Ressource();

    static PixL_Ressource *_instance;

public:
    static PixL_Ressource *getInstance()
    {
        if (!_instance)
        {
            _instance = new PixL_Ressource();
        }
        return _instance;
    }

    static void destroyInstance();

    int8_t loadPak(std::string pakFileName);
    bool unloadPak(int8_t pakID);

    bool CreatePipeline(std::string name, Shader_Struct *vertexShader, Shader_Struct *fragmentShader, bool depthTest, SDL_GPUCompareOp compareOp, bool enable_depth_test, bool enable_depth_write);
    bool CreateTexture(std::string name, std::string imagePath);

private:
    std::vector<std::pair<int8_t, PixL_Pak::Context *>> pakContexts;
};
