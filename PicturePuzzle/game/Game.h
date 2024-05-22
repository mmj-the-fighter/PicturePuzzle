#ifndef _GAME_H_
#define _GAME_H_

#include "JigsawSpriteManager.h"

#define GAME_RESOLUTION_X 800
#define GAME_RESOLUTION_Y 600

class Game
{
public:
	Game();
	~Game();
	void Init();
	void Restart();
	void Display();
	void OnLeftMouseButtonDown(int x, int y);
	void OnLeftMouseButtonUp(int x, int y);
	void OnRightMouseButtonDown(int x, int y);
	void OnRightMouseButtonUp(int x, int y);
	void OnMouseMoved(int x, int y);
	inline bool IsGameOver(){ return gameOver; }
public:
	bool gameOver;
	bool lmbDown;
	bool rmbDown;
	JigsawSpriteManager *spriteManager;
};

#endif
