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

extern "C" {
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libavutil/avutil.h>
#include <libavfilter/avfilter.h>
}
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
	Logger* logger = Logger::GetInstance();
	Tileset* tileset = new Tileset(&Tileset_tileset1);
	OverworldRenderer* overworld = new OverworldRenderer();
	FontsRenderer* font = new FontsRenderer(&font_ressource_m5x7);
	GuiRenderer* gui = GuiRenderer::GetInstance();

	gui_element* lifebar_element = new gui_element();
	lifebar_element->PtrGui = lifebar;
	lifebar_element->priority = 0;
	lifebar_element->w = new int(104);
	lifebar_element->h = nullptr;
	lifebar_element->x = new int(100);
	lifebar_element->y = new int(0);
	lifebar_element->font_renderer = font;

	int pv = 100;
	int max_pv = 100;
	gui_param lifebar_param;
	lifebar_param.int_ptr_params.push_back(&pv);
	lifebar_param.int_ptr_params.push_back(&max_pv);
	lifebar_param.bool_ptr_params.push_back(new bool(true));
	lifebar_element->param = lifebar_param;

	gui_element* caminfo_element = new gui_element();
	caminfo_element->PtrGui = caminfo;
	caminfo_element->priority = 0;
	caminfo_element->w = new int(100);
	caminfo_element->h = nullptr;
	caminfo_element->x = new int(0);
	caminfo_element->y = new int(0);
	caminfo_element->font_renderer = font;

	gui_param caminfo_param;
	caminfo_element->param = caminfo_param;
	bool* caminfo_visibility = new bool(false);
	caminfo_element->param.bool_ptr_params.push_back(caminfo_visibility);


	gui->add_gui_element(lifebar_element);
	gui->add_gui_element(caminfo_element);



	CoordCalculator* coord_calculator = CoordCalculator::GetInstance();
	coord_calculator->add_coord_to_adjust(lifebar_element->x, WIDTH, 0.8);
	coord_calculator->add_coord_to_adjust(lifebar_element->w, WIDTH, 0.2);
	coord_calculator->add_coord_to_adjust(caminfo_element->w, WIDTH, 0.3);
	coord_calculator->adjust_coords();

	SDL_Texture* sprite_texture = Load_Texture( ow_sprite_ressource,  Camera->GetRenderer());
	SpriteRenderer* spriter = new SpriteRenderer(sprite_texture, 32, 32);

	bool quit = false;
	
	sprite* sprite1 = new sprite();
	sprite1->texture = &ow_sprite_ressource;
	sprite1->walk_speed = 45;
	sprite1->run_speed = 15;
	sprite1->animation_speed = 15;
	sprite1->width = 32;
	sprite1->height = 64;
	sprite1->hitbox = {32, 64};
	sprite1->face_down = {0};
	sprite1->face_up = {1};
	sprite1->face_left = {2};
	sprite1->face_right = {3};
	sprite1->walk_down = {4, 5};
	sprite1->walk_up = {6, 7};
	sprite1->walk_left = {8, 9};
	sprite1->walk_right = {10, 11};

	GameLogicMainClass* game_logic = GameLogicMainClass::GetInstance();
	ParticleEmitter* emitter = new ParticleEmitter();
	OverworldParticleEmitter* overworld_emitter = new OverworldParticleEmitter(0);
	int mx, my;
	bool tp = false;

	// open BigBuckBunny.mp4 and display its metadata
	AVFormatContext* pFormatCtx = nullptr;
	avformat_open_input(&pFormatCtx, "BigBuckBunny.mp4", nullptr, nullptr);

	

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
					coord_calculator->adjust_coords();
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
				GameLogicData::GetInstance()->SetPressedKeys(keys);
				switch (event.key.keysym.sym)
				{
					case SDLK_ESCAPE:
						quit = true;
						break;
					case SDLK_UP:
						Camera->GetPosition()->y -= 32;
						break;
					case SDLK_DOWN:
						Camera->GetPosition()->y += 32;
						break;
					case SDLK_LEFT:
						Camera->GetPosition()->x -= 32;
						break;
					case SDLK_RIGHT:
						Camera->GetPosition()->x += 32;
						break;
					case SDLK_KP_PLUS:
						*Camera->GetZoom() += 0.1f;
						tp = !tp;
						break;
					case SDLK_KP_MINUS:
						if (*Camera->GetZoom() > 0.1f)*Camera->GetZoom() -= 0.1f;
						break;
					case SDLK_m :
						if (pv > 0)
						{
							pv -= 1;
							logger->log("pv : " + to_string(pv));
						}
						break;
					case SDLK_p :
						if (pv < max_pv)
						{
							pv += 1;
							logger->log("pv : " + to_string(pv));
						}
						break;
					case SDLK_o :
						*caminfo_visibility = !*caminfo_visibility;
						break;
					
				}
			}
			
			
		}

		if (Camera->GetFrame() % 2 == 0)
		{
			SDL_GetMouseState(&mx, &my);
			//if (tp) emitter->add_particle(FIRE, {mx, my}, 5 , {10, 10});
			//else emitter->add_particle(SMOKE, {mx, my} ,4 , {10, 10});
			overworld_emitter->add_particle(FIRE, {500, 500}, 5 , {10, 10});
		}
		
		// clear screen
		SDL_SetRenderDrawColor(Camera->GetRenderer(), 0, 0, 0, 255);
		SDL_RenderClear(Camera->GetRenderer());

		// draw map
		overworld->check_maps_visibility();
		overworld->draw();
		spriter->Draw_World_coord({18 * TILE_SIZE, 9 * TILE_SIZE}, Camera->GetFrame()/100 % 4);
		
		uint8_t tr = static_cast<int>(255 * sin(0.01 * Camera->GetFrame()));
		uint8_t tg = static_cast<int>(255 * sin(0.01 * Camera->GetFrame() + 2));
		uint8_t tb = static_cast<int>(255 * sin(0.01 * Camera->GetFrame() + 4));
		// draw text
		font->render_text(100, 100, 64, 64, 32, u'R', {tr, tg, tb});
		font->render_text(132, 100, 64, 64, 32, u'G', {tr, tg, tb});
		font->render_text(164, 100, 64, 64, 32, u'B', {tr, tg, tb});

		// draw particles
		//emitter->update();
		//emitter->render();

		overworld_emitter->update();
		overworld_emitter->render();

		// draw gui
		gui->draw_gui();

		// update screen
		SDL_RenderPresent(Camera->GetRenderer());
		Camera->addFrame();
		SDL_Delay(16);
	}


	return 0;
}