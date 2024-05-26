#include <chrono>
#include <const/Config.hpp>
#include <Light.hpp>
#include <Log.hpp>
#include <OverworldRenderer.hpp>
#include <generated/map2cpp.hpp>
#include <generated/font2cpp.hpp>
#include <FontsRenderer.hpp>
#include <Cinematics.hpp>
#include <GuiElements.hpp>
#include <CoordCalculator.hpp>
#include <SpriteRenderer.hpp>
#include <GameLogicMainClass.hpp>
#include <Particle.hpp>
#include <Renderer.hpp>

using namespace std;
using namespace std::chrono;




int main(int argc, char* argv[])
{
	// init SDL
	if (SDL_Init(SDL_INIT_VIDEO) != 0)
	{
		return 1;
	}
	// create window
	Camera* Camera = Camera::GetInstance();
	Renderer* Renderer = Renderer::GetInstance();
	Logger* logger = Logger::GetInstance();

	std::cout << "OpenGL Version: " << glGetString(GL_VERSION) << std::endl;
    std::cout << "GLSL Version: " << glGetString(GL_SHADING_LANGUAGE_VERSION) << std::endl;

	
	bool quit = false;

	OpenGLTexture texture = OpenGLTexture(tileset_tileset1_ressource);
	OpenGLShader shader = OpenGLShader(Shader_basic_shader);


	// main loop
	while (!quit)
	{
		SDL_Event event;
		while (SDL_PollEvent(&event))
		{
			// quit
			if (event.type == SDL_QUIT)
			{
				quit = true;
			}

			// if window resized
			if (event.type == SDL_WINDOWEVENT)
			{
				if (event.window.event == SDL_WINDOWEVENT_RESIZED)
				{
					SDL_GetWindowSize(Camera->GetWindow(), &Camera->GetSize()->x, &Camera->GetSize()->y);
					int width = event.window.data1;
        			int height = event.window.data2;
        			glViewport(0, 0, width, height);
				}
			}

			// if key pressed
			if (event.type == SDL_KEYDOWN)
			{
				std::vector<int> keys;
				// get keys pressed (ne pas tenir compte des touches relachées)
				const Uint8* state = SDL_GetKeyboardState(NULL);
				for (int i = 0; i < SDL_NUM_SCANCODES; i++)
				{
					if (state[i])
					{
						keys.push_back(i);
					}
				}
				switch (event.key.keysym.sym)
				{
					
				}
			}
			
			
		}

		glClear(GL_COLOR_BUFFER_BIT);
		//shader.Use();
		texture.Bind();
		glEnable(GL_TEXTURE_2D);
    	glBegin(GL_QUADS);
		// draw texture
		

		
		glTexCoord2f(0.0f, 0.0f); glVertex2f(-1.0f, 1.0f);
		glTexCoord2f(1.0f, 0.0f); glVertex2f(1.0f, 1.0f);
		glTexCoord2f(1.0f, 1.0f); glVertex2f(1.0f, -1.0f);
		glTexCoord2f(0.0f, 1.0f); glVertex2f(-1.0f, -1.0f);


    	glEnd();
		glDisable(GL_TEXTURE_2D);

		SDL_GL_SwapWindow(Camera->GetWindow());
		SDL_Delay(16);
	}


	return 0;
}
