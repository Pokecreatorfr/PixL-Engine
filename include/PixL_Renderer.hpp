#pragma once

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <iostream>
#include <stdio.h>
#include <vector>

// prototypes
class PixL_Renderer;
class PixL_Drawable;
class PixL_Texture;
class PixL_Tilemap;

// Flags for PixL_Renderer_Init

// Rendering modes
#define PIXL_RENDERER_QUEUE_MODE 0x00000001 // Queue mode is the mode where the renderer will render the textures in the order they were added
#define PIXL_RENDERER_PLAN_MODE 0x00000002  // Plan mode is the mode where the renderer will render the textures in the order of their plan value

// Plan modes clear mode
#define PIXL_RENDERER_PLAN_MODE_AUTO_CLEAR 0x00000004   // Automatically clear the plan mode after rendering
#define PIXL_RENDERER_PLAN_MODE_MANUAL_CLEAR 0x00000008 // Manually clear the plan mode after rendering

// debug flags
#define PIXL_RENDERER_DEBUG_RED_WIRE 0x00000010

// PixL_Renderer main class
class PixL_Renderer
{
protected:
    PixL_Renderer();
    ~PixL_Renderer();
    static PixL_Renderer *_instance;

    // flags
    uint32_t _flags;
    SDL_Window *_window = nullptr;
    SDL_Renderer *_renderer = nullptr;
    std::vector<PixL_Drawable *> _drawables;

    // friend functions

    friend int PixL_Renderer_Init(uint32_t flags);
    friend SDL_Window *CreateWindow(const char *title, int x, int y, int w, int h, Uint32 flags);
    friend SDL_Window *GetWindow();
    friend PixL_Texture *CreateTexture(const char *path);
};

// Drawables objects classes

struct PixL_Draw_Property
{
    // center of the texture
    float x;
    float y;

    // width and height of the texture
    float w;
    float h;

    // angle of the texture
    float rot;

    // color modulation
    uint8_t r = 255;
    uint8_t g = 255;
    uint8_t b = 255;
    uint8_t a = 255;
};

class PixL_Drawable
{
public:
    static uint16_t _id;
    static uint16_t _plan;
    static PixL_Draw_Property _property;
};

class PixL_Texture : public PixL_Drawable
{
public:
    uint16_t _id;
    SDL_Texture *_texture = nullptr;
};

class PixL_Tilemap : public PixL_Drawable
{
public:
    uint16_t _id;
    uint16_t _plan;
    PixL_Draw_Property _property;
    uint16_t _texture_id;
    uint16_t _GetMapHeight();
    uint16_t _GetMapWidth();
    void _SetMapHeight(uint16_t height);
    void _SetMapWidth(uint16_t width);
    std::vector<uint16_t> *_GetTilemap();

private:
    std::vector<uint16_t> _map;
    uint16_t _map_width;
    uint16_t _map_height;
};

// function prototypes

int PixL_Renderer_Init(uint32_t flags);

SDL_Window *CreateWindow(const char *title, int x, int y, int w, int h, Uint32 flags);

SDL_Window *GetWindow();

PixL_Texture *CreateTexture(const char *path);