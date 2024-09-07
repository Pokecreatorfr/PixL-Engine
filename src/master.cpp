#include <master.hpp>

Master::Master(Uint32 initFlags , const char *title, int x, int y, int w, int h, Uint32 windowFlags, int rendererIndex, Uint32 rendererFlags)
{
    Logger* logger = Logger::GetInstance();
    if( SDL_Init( SDL_INIT_VIDEO ) == -1 )
    {
        logger->log("SDL could not initialize! SDL_Error: " + std::string(SDL_GetError()));
        return;
    }

    this->window = SDL_CreateWindow( title, x, y, w, h, windowFlags );

    if( this->window == NULL )
    {
        logger->log("Window could not be created! SDL_Error: " + std::string(SDL_GetError()));
        return;
    }

    this->renderer = SDL_CreateRenderer(this->window, rendererIndex, rendererFlags);

    if( this->renderer == NULL )
    {
        logger->log("Renderer could not be created! SDL_Error: " + std::string(SDL_GetError()));
        return;
    }
}

Master::~Master()
{
    SDL_DestroyRenderer(this->renderer);
    SDL_DestroyWindow(this->window);
    SDL_Quit();
}


Master* Master::instance_ = nullptr;

Master *Master::getInstance(Uint32 initFlags, const char *title, int x, int y, int w, int h, Uint32 windowFlags, int rendererIndex, Uint32 rendererFlags)
{
    if (instance_ == nullptr)
    {
        instance_ = new Master(initFlags, title, x, y, w, h, windowFlags, rendererIndex, rendererFlags);
    }
    return instance_;
}

Master *Master::getInstance()
{
    if (instance_ == nullptr)
    {
        Logger::GetInstance()->log("Master instance is null");
    }
    return instance_;
}

SDL_Window *Master::getWindow()
{
    return this->window;
}

SDL_Renderer *Master::getRenderer()
{
    return this->renderer;
}

void Master::updateEvent()
{
    SDL_PollEvent(&this->e);
    if (this->e.type == SDL_QUIT)
    {
        SDL_Quit();
    }
}

SDL_Event *Master::getEvent()
{
    return &this->e;
}
