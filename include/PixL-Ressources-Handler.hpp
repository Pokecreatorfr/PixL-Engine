#pragma once

#include <PixL-Rendering-Engine/include/PixL_Renderer.hpp>

class PixL_RessourcesHandler
{
protected:
    PixL_RessourcesHandler() = default;
    ~PixL_RessourcesHandler() = default;
    static PixL_RessourcesHandler *_instance;

public:
    static PixL_RessourcesHandler *getInstance()
    {
        if (!_instance)
        {
            _instance = new PixL_RessourcesHandler();
        }
        return _instance;
    }
};