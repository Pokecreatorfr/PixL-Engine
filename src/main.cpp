#include <GameLogic/GameLogicThread.hpp>
#include <Render/GameRenderer.hpp>
#include <SystemEvent.hpp>
#include <config.hpp>
#include <iostream>
#include <stdio.h>
#include <thread>

int WinMain(int argv, char **args)
{
	DataLoader loader(LDTK_FILE_PATH);
	quicktype::LdtkJson ldtkjs = loader.get_ldtk_json();

	RendererWrapper *renderer = RendererWrapper::get_instance(RendererType::SDL2);
	SDL_Window *window = renderer->create_window("Test", 800, 600);
	renderer->init();
	SystemEvent *event = SystemEvent::getInstance(new SDL_Event(), window);

	mutex mutex;
	for (int i = 0; i < SDL_GetNumRenderDrivers(); ++i)
	{
		SDL_RendererInfo info;
		SDL_GetRenderDriverInfo(i, &info);
		std::cout << "Renderer " << i << " name: " << info.name << std::endl;
		std::cout << "Renderer " << i << " flags: " << info.flags << std::endl;
		std::cout << "Renderer " << i << " num_texture_formats: " << info.num_texture_formats << std::endl;
		std::cout << "Renderer " << i << " max_texture_width: " << info.max_texture_width << std::endl;
		std::cout << "Renderer " << i << " max_texture_height: " << info.max_texture_height << std::endl;
		for (int j = 0; j < info.num_texture_formats; ++j)
		{
			std::cout << "Renderer " << i << " texture_formats[" << j << "]: " << info.texture_formats[j] << std::endl;
		}
		std::cout << std::endl;
	}

	GameRenderer *gameRenderer = new GameRenderer(renderer);

	std::thread logicThread(GameLogicThread, event, &mutex);

	while (!event->isQuit())
	{
		SDL_Delay(16);
		mutex.lock();
		gameRenderer->update();
		gameRenderer->render();
		event->update();
		mutex.unlock();
	}

	logicThread.join();

	delete gameRenderer;
	delete event;
	delete renderer;

	return 0;
}