#include <cstdio>
#include "../swr/swr_sdl_window.h"
#include "../swr/swr_rasterizer.h"
#include "../swr/swr_image.h"
#include "../swr/swr_rfont.h"
#include "../swr/swr_screenshot.h"
#include "Game.h"

swr_sdl_context* g_ctx = NULL;
Game *game;


void display(swr_sdl_context* ctx)
{
	game->Update();
	rasterizer_clear();
	game->Display();
	//Frame stats
	/*char buffer[256];
	sprintf(buffer, "%0.2f ms", ctx->extra);
	rasterizer_draw_text(100, 300, buffer);*/

}


int input(SDL_Event* e)
{
	int button_index;
	int x, y;
	if (e->type == SDL_KEYDOWN) {
		switch (e->key.keysym.scancode)
		{
		case SDL_SCANCODE_ESCAPE:
			return 1;
		case SDL_SCANCODE_F12:
			swr_save_screenshot(g_ctx);
			break;
		case SDL_SCANCODE_SPACE:
		case SDL_SCANCODE_F5:
			game->Restart();
			break;
		}
	}
	else {
		button_index = e->button.button;
		x = e->button.x;
		y = e->button.y;
		switch (e->type)
		{
		case SDL_MOUSEBUTTONDOWN:
			if (button_index == 1){
				game->OnLeftMouseButtonDown(x, y);
			}
			else if (button_index == 3){
				game->OnRightMouseButtonDown(x, y);
			}
			break;
		case SDL_MOUSEBUTTONUP:
			if (button_index == 1){
				game->OnLeftMouseButtonUp(x, y);
			}
			else if (button_index == 3){
				game->OnRightMouseButtonUp(x, y);
			}
			break;
		case SDL_MOUSEMOTION:
			game->OnMouseMoved(x, y);
			break;
		}
	}
	return 0;
}


void init() {
	swr_sdl_set_frames_per_second(35);
	swr_sdl_create_context("Picture Puzzle using Graphics Lab Framework", 800, 600, "swrres/fonts/glyphs.ppm", "swrres/fonts/glyphs.csv");
	swr_sdl_set_input_handler(input);
	swr_sdl_set_display_handler(display);
	g_ctx = swr_sdl_get_context();
	rasterizer_set_swr_sdl_context(g_ctx);
	rasterizer_set_cur_color(255, 255, 255, g_ctx->opaque_alpha_value);
	rasterizer_set_clear_color(0, 0, 128, g_ctx->transparent_alpha_value);
	game = new Game();
	game->Init();
}

void cleanup() 
{
	delete game;
}

int main(int argc, char** argv)
{	
	init();
	swr_sdl_main_loop();
	cleanup();
	return 0;
}
