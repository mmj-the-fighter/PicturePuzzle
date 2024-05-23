#include "../swr/swr_utils.h"
#include "../swr/swr_rasterizer.h"
#include "Game.h"

Game::Game()
{
	swr_init_random();
	spriteManager = new JigsawSpriteManager();
}

Game::~Game()
{
	delete spriteManager;
}

void Game::Init()
{
	SetGameState(SHOW_PIC);
	spriteManager->SetGameWorldLimits(0, 0, GAME_RESOLUTION_X, GAME_RESOLUTION_Y);
	//spriteManager->Load("gameres/roadgrid.png", 4, 4);
	spriteManager->Load("gameres/road.png", 3, 4);
}

void Game::Restart()
{
	//TODO:
	spriteManager->BreakConnections();
	spriteManager->RandomizePositions();
	spriteManager->RandomizeLayers();
	SetGameState(SHOW_PIC);
}

void Game::OnLeftMouseButtonDown(int x, int y)
{
	//printf("\nLMB DOWN %d %d", x, y);
	spriteManager->OnLeftMouseButtonDown(x, y);
}

void Game::OnLeftMouseButtonUp(int x, int y)
{
	//printf("\nLMB UP %d %d", x, y);
	spriteManager->OnLeftMouseButtonUp(x, y);
}
void Game::OnRightMouseButtonDown(int x, int y)
{
	//printf("\nRMB DOWN %d %d", x, y);
	spriteManager->dbg_PrintLayers();
}

void Game::OnRightMouseButtonUp(int x, int y)
{
	//printf("\nRMB UP %d %d", x, y);
}

void Game::OnMouseMoved(int x, int y)
{
	//printf("\nMOUSE MOTION %d %d", x, y);
	spriteManager->OnMouseMoved(x, y);
}

void Game::SetGameState(GameState state)
{
	if (state == SHOW_PIC){
		StartTimer();
	}
	gameState = state;
}


void Game::StartTimer()
{
	startTime = clock();
}

double Game::GetElapsedTime()
{
	endTime = clock();
	return static_cast<double>(endTime - startTime) / CLOCKS_PER_SEC;
}

void Game::Update()
{
	switch (gameState)
	{
	case SHOW_PIC:
		if (GetElapsedTime() > 6.0){
			SetGameState(SHOW_GRID);
		}
		break;
	case SHOW_GRID:
		if (spriteManager->IsAllSpritesAreInSameLayer()){
			SetGameState(SHOW_GAME_WON_MESSAGE);
		}
		break;
	case SHOW_GAME_WON_MESSAGE:
		break;
	}

}


void Game::Display()
{
	switch (gameState)
	{
	case SHOW_PIC:
		spriteManager->DisplaySourceImage();
		break;
	case SHOW_GRID:
		spriteManager->Display();
		break;
	case SHOW_GAME_WON_MESSAGE:
		rasterizer_draw_text(GAME_RESOLUTION_X / 2 - 100, GAME_RESOLUTION_Y/2-100, "You solved the puzzle! [Press Space]");
		break;
	}
	
}