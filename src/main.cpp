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

	glClearColor(0.0f, 1.0f, 0.0f, 1.0f);

	float vertices[] = {
        // positions          // colors           // texture coords
         0.5f,  0.5f, 0.0f,   1.0f, 0.0f, 0.0f,   1.0f, 0.0f, // top right
         0.5f, -0.5f, 0.0f,   0.0f, 1.0f, 0.0f,   1.0f, 1.0f, // bottom right
        -0.5f, -0.5f, 0.0f,   0.0f, 0.0f, 1.0f,   0.0f, 1.0f, // bottom left
        -0.5f,  0.5f, 0.0f,   1.0f, 1.0f, 0.0f,   0.0f, 0.0f  // top left 
    };
    unsigned int indices[] = {
        0, 1, 3, // first triangle
        1, 2, 3  // second triangle
    };
    unsigned int VBO, VAO, EBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    // position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    // color attribute
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    // texture coord attribute
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);
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
		shader.Use();
		texture.Bind();
		glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);


		SDL_GL_SwapWindow(Camera->GetWindow());
		SDL_Delay(16);
	}


	return 0;
}
