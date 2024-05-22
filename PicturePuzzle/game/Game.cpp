#include "../swr/swr_utils.h"
#include "Game.h"

Game::Game() :gameOver(false), lmbDown(false), rmbDown(false)
{
	//initialize random number
	swr_init_random();
	spriteManager = new JigsawSpriteManager();
}

Game::~Game()
{
//deallocate if anything
	delete spriteManager;
}

void Game::Init()
{
	gameOver = false;
	//create tile set
	//randomize tile locations
	spriteManager->SetGameWorldLimits(0, 0, GAME_RESOLUTION_X, GAME_RESOLUTION_Y);
	spriteManager->Load("gameres/roadgrid.png", 4, 4);
}

void Game::Restart()
{
	//TODO:
	//gameOver = false;
	//randomize tile locations
	//spriteManager->RandomizePositions();

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


void Game::Display()
{
	spriteManager->Display();
}