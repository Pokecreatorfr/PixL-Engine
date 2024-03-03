#pragma one
#include <Graphics.hpp>
#include <FontsRenderer.hpp>

struct gui_param
{
    // values parameters
    std::vector<int> int_params;  
    std::vector<float> float_params; 
    std::vector<std::string> string_params; 
    std::vector<u16string> u16string_params; 
    std::vector<bool> bool_params; 
    std::vector<SDL_Color> color_params; 
    
    // pointer parameters
    std::vector<int*> int_ptr_params; 
    std::vector<float*> float_ptr_params; 
    std::vector<std::string*> string_ptr_params; 
    std::vector<u16string*> u16string_ptr_params; 
    std::vector<bool*> bool_ptr_params; 
    std::vector<SDL_Color*> color_ptr_params; 
};

struct gui_element
{
    void (*PtrGui)(Camera*, int*  , int*  , int*  , int*  , std::vector<SDL_Texture*>*, FontsRenderer*, gui_param );
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
    protected:
        GuiRenderer();
        static GuiRenderer* instance_;

    public:
        static GuiRenderer* GetInstance();
        ~GuiRenderer();
        int add_gui_element(gui_element* element);
        void remove_gui_element(int uid);
        void draw_gui();


    private:
        std::vector<gui_element*> elements;
        Camera* Camera;
};