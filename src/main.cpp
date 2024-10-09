#include <LoadDatas.hpp>
#include <Render/RendererWrapper.hpp>


int main(int argv, char** args) {
	DataLoader loader("data/Exemple.ldtk");
	quicktype::LdtkJson ldtkjs = loader.get_ldtk_json();

	RendererWrapper renderer(RendererType::SDL2);

	renderer.init();

	SDL_Window *window = renderer.create_window("Test", 800, 600);

	SDL_Delay(3000);

	renderer.destroy_window();

	return 0;
}