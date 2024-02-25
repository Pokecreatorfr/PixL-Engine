#pragma one
#include <Graphics.hpp>
#include <FontsRenderer.hpp>

struct gui_param
{
    int* int_param1 = nullptr ; // if not used, set to nullptr
    int* int_param2 = nullptr; // if not used, set to nullptr
    std::string* string_param1 = nullptr; // if not used, set to nullptr
    std::string* string_param2 = nullptr; // if not used, set to nullptr
    bool* bool_param1 = nullptr; // if not used, set to nullptr
    bool* bool_param2 = nullptr; // if not used, set to nullptr
    float* float_param1 = nullptr; // if not used, set to nullptr
    float* float_param2 = nullptr; // if not used, set to nullptr
    SDL_Color* color_param1 = nullptr; // if not used, set to nullptr
    SDL_Color* color_param2 = nullptr; // if not used, set to nullptr
    std::vector<int>* int_vector_param1 = nullptr; // if not used, set to nullptr
    bool visible = true;
};

struct gui_element
{
    void (*PtrGui)(camera*, int*  , int*  , int*  , int*  , std::vector<SDL_Texture*>*, FontsRenderer*, gui_param );
    uint16_t priority; // lower is drawn first
    int* w;
    int* h;
    int* x;
    int* y;
    std::vector<SDL_Texture*> textures;
    FontsRenderer* font_renderer;
    gui_param param;
    int uid; // unique id set by the renderer
};

class GuiRenderer
{
    public:
        GuiRenderer(camera* cam);
        ~GuiRenderer();
        int add_gui_element(gui_element* element);
        void remove_gui_element(int uid);
        void draw_gui();


    private:
        std::vector<gui_element*> elements;
        camera* Camera;
};