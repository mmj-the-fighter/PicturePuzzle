#ifndef _GAME_H_
#define _GAME_H_

#include <ctime>
#include "../swr/swr_image.h"
#include "JigsawSpriteManager.h"


#define GAME_RESOLUTION_X 800
#define GAME_RESOLUTION_Y 600

typedef enum GameState { SHOW_PIC, SHOW_GRID, SHOW_GAME_WON_MESSAGE};

class Game
{
public:
	Game();
	~Game();
	void Init();
	void Restart();
	void Update();
	void Display();
	void OnLeftMouseButtonDown(int x, int y);
	void OnLeftMouseButtonUp(int x, int y);
	void OnRightMouseButtonDown(int x, int y);
	void OnRightMouseButtonUp(int x, int y);
	void OnMouseMoved(int x, int y);
	void SetGameState(GameState state);
	void StartTimer();
	double GetElapsedTime();
public:
	GameState gameState;
	double showPicDelay;
	swr_image* gameWonImage;
	JigsawSpriteManager *spriteManager;
	clock_t startTime, endTime;
};

#endif
