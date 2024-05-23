#include <cstdio>
#include <algorithm>
#include <stack>
#include <cstdlib>
#include <ctime>
#include "../swr/swr_utils.h"
#include "JigsawSpriteManager.h"

bool compareByLayer(const JigsawSprite* s1, const JigsawSprite* s2)
{
	return s1->layer < s2->layer;
}

JigsawSpriteManager::JigsawSpriteManager()
{
	sourceImage = NULL;
	imageSlicerResult.tiles = NULL;
	imageSlicerResult.tile_count = 0;
	gameWorldStartX = 0;
	gameWorldStartY = 0;
	gameWorldEndX = 100;
	gameWorldEndY = 100;
}

JigsawSpriteManager::~JigsawSpriteManager()
{ 
	UnLoad(); 
}

void JigsawSpriteManager::SetGameWorldLimits(int startX, int startY, int endX, int endY)
{
	gameWorldStartX = startX;
	gameWorldStartY = startY;
	gameWorldEndX = endX;
	gameWorldEndY = endY;
}

void JigsawSpriteManager::Load(const char* filename, int rows, int cols)
{
	UnLoad();
	sourceImage = swr_imag_read_png_using_stb(filename);
	desiredRows = rows;
	desiredCols = cols;
	swr_image_slice_image(sourceImage, &imageSlicerResult, rows, cols);
	for (int i = 0; i < imageSlicerResult.tile_count; i++)
	{
		JigsawSprite *js = new JigsawSprite();
		int c = imageSlicerResult.tiles[i].column;
		int r = imageSlicerResult.tiles[i].row;
		swr_image* image = imageSlicerResult.tiles[i].tile;
		int w = image->width;
		int h = image->height;
		int id = (r * desiredCols + c);
		int x =  c*w;
		int y =  r*h;
		int layer = i;
		js->Set(id, r, c, layer, x, y, w, h, image);
		int nr, nc;
		//left
		nr = r;
		nc = c - 1;
		js->leftNeighbourId = (nc >= 0) ? nr * desiredCols + nc : -1;

		//right
		nr = r;
		nc = c + 1;
		js->rightNeighbourId = (nc < desiredCols) ? nr * desiredCols + nc : -1;

		//up
		nr = r - 1;
		nc = c;
		js->upNeighbourId = (nr >= 0) ? nr * desiredCols + nc : -1;

		//down
		nr = r + 1;
		nc = c;
		js->downNeighbourId = (nr < desiredRows) ? nr * desiredCols + nc : -1;

		spriteRenderList.push_back(js);
		spriteList.push_back(js);
		//printf("%d %d %d %d\n", id, c, r, desiredCols);
	}
	RandomizePositions();
	RandomizeLayers();
	spriteCount = spriteRenderList.size();
}

void JigsawSpriteManager::BreakConnections()
{
	for (std::vector<JigsawSprite*>::iterator it = spriteList.begin();
		it != spriteList.end();
		++it) {
		JigsawSprite* spr = *it;
		spr->DisconnectWithNeighbours();
	}
}

void JigsawSpriteManager::UnLoad()
{
	if (sourceImage != NULL) {
		swr_destroy_image(sourceImage);
	}

	if (imageSlicerResult.tiles != NULL)
	{
		swr_destroy_image_tile_array(
			imageSlicerResult.tiles, 
			imageSlicerResult.tile_count
			);
	}
	int n = spriteRenderList.size();
	if (n > 0){
		for (std::vector<JigsawSprite*>::iterator it = spriteRenderList.begin(); 
			it != spriteRenderList.end(); 
			++it) {
			JigsawSprite* spr = *it;
			delete spr;
		}
		spriteRenderList.clear();
		spriteList.clear();
	}
}

void JigsawSpriteManager::Display()
{
	for (std::vector<JigsawSprite*>::iterator it = spriteRenderList.begin();
		it != spriteRenderList.end();
		++it) {
		JigsawSprite* spr = *it;
		spr->Display();
	}
}

void JigsawSpriteManager::dbg_PrintLayers()
{
	//Caution: Debugging with intially randomized layers might be difficult
	printf("Layers begin\n");
	for (std::vector<JigsawSprite*>::iterator it = spriteList.begin();
		it != spriteList.end();
		++it) {
		JigsawSprite* spr = *it;
		printf("%d - %d \n", spr->id, spr->layer);
	}
	printf("Layers end\n");

}

void JigsawSpriteManager::OnLeftMouseButtonDown(int x, int y)
{
	curSelectionId = -1;
	int i = spriteRenderList.size() - 1;
	for (std::vector<JigsawSprite*>::reverse_iterator it = spriteRenderList.rbegin();
		it != spriteRenderList.rend();
		++it) 
	{
		JigsawSprite* spr = *it;
		if (spr->IsMouseOver(x, y)) 
		{
			spr->RegisterSelection(x, y);
			curSelectionId = spr->id;
			curClickX = x;
			curClickY = y;
			prevClickX = curClickX;
			prevClickY = curClickY;
			break;
		}
		--i;
	}


	if (curSelectionId >= 0){
		int n = spriteRenderList.size();
		int selectedLayer = spriteList[curSelectionId]->layer;
		int topLayer = spriteRenderList[0]->layer;
		for (int i = 1; i < n; i++) {
			int ly = spriteRenderList[i]->layer;
			if (ly > topLayer){
				topLayer = ly;
			}
		}
		printf("top %d \n", topLayer);
		if (selectedLayer != topLayer)
		{
			for (int i = 0; i < n; i++) {
				int layer = spriteRenderList[i]->layer;
				if (layer == selectedLayer){
					spriteRenderList[i]->layer = topLayer;
				}
				else if (layer > selectedLayer){
					spriteRenderList[i]->layer -= 1;
				}
			}
			std::sort(spriteRenderList.begin(), spriteRenderList.end(), compareByLayer);
		}
	}
	
}

void JigsawSpriteManager::OnLeftMouseButtonUp(int x, int y)
{
	if (curSelectionId >= 0)
	{
		AttachAdjacentJigsawSetIfAny(curSelectionId);
		spriteList[curSelectionId]->UnRegisterSelection();
		int n = spriteRenderList.size();
		int selectedLayer = spriteList[curSelectionId]->layer;
		int topLayer = spriteRenderList[0]->layer;
		for (int i = 1; i < n; i++) {
			int ly = spriteRenderList[i]->layer;
			if (ly > topLayer){
				topLayer = ly;
			}
		}
		//printf("top %d \n", topLayer);
		if (selectedLayer != topLayer)
		{
			for (int i = 0; i < n; i++) {
				int layer = spriteRenderList[i]->layer;
				if (layer == selectedLayer){
					spriteRenderList[i]->layer = topLayer;
				}
				else if (layer > selectedLayer){
					spriteRenderList[i]->layer -= 1;
				}
			}
			std::sort(spriteRenderList.begin(), spriteRenderList.end(), compareByLayer);
		}
	}
	curSelectionId = -1;
	//printf("mouse up");
}

void JigsawSpriteManager::OnMouseMoved(int x, int y)
{

	if (curSelectionId >= 0)
	{
		curClickX = x;
		curClickY = y;
		curDragX = curClickX - prevClickX;
		curDragY = curClickY - prevClickY;
		prevClickX = curClickX;
		prevClickY = curClickY;
		spriteList[curSelectionId]->ApplyDrag(curDragX, curDragY);
		MoveAttachedJigsawSetIfAny(curSelectionId, curDragX, curDragY);
	}
	

}

void JigsawSpriteManager::RandomizePositions()
{
	for (std::vector<JigsawSprite*>::iterator it = spriteRenderList.begin();
		it != spriteRenderList.end();
		++it) {
		JigsawSprite* spr = *it;
		spr->x = swr_random_in_range(gameWorldStartX, gameWorldEndX-spr->width);
		spr->y = swr_random_in_range(gameWorldStartY, gameWorldEndY-spr->height);
	}
}

void JigsawSpriteManager::RandomizeLayers()
{
	std::random_shuffle(spriteRenderList.begin(), spriteRenderList.end());
	int i = 0;
	for (std::vector<JigsawSprite*>::iterator it = spriteRenderList.begin();
		it != spriteRenderList.end();
		++it) {
		JigsawSprite* spr = *it;
		spr->layer = i;
		++i;
	}
	std::sort(spriteRenderList.begin(), spriteRenderList.end(), compareByLayer);
}


void JigsawSpriteManager::AttachAdjacentJigsawSetIfAny(int spriteId)
{
	std::stack<int> traversalStack;
	InitGraphTraversal();
	LinkAdjacentSpritesForASingleSprite(spriteId);
	spriteList[spriteId]->isVisited = true;
	traversalStack.push(spriteId);
	int v;
	int top;
	int minLayer = spriteList[spriteId]->layer;
	while (!traversalStack.empty()){
		top = traversalStack.top();
		v = GetAdjacentUnVisitedSpriteId(top);
		if (v < 0) {
			traversalStack.pop();
		}
		else{
			if (spriteList[v]->layer < minLayer){
				minLayer = spriteList[v]->layer;
			}
			spriteList[v]->isVisited = true;
			LinkAdjacentSpritesForASingleSprite(v);
			traversalStack.push(v);
		}
	}

	InitGraphTraversal();
	spriteList[spriteId]->isVisited = true;
	traversalStack.push(spriteId);
	spriteList[spriteId]->layer = minLayer;

	while (!traversalStack.empty()){
		top = traversalStack.top();
		v = GetAdjacentUnVisitedSpriteId(top);
		if (v < 0) {
			traversalStack.pop();
		}
		else{
			spriteList[v]->layer = minLayer;
			spriteList[v]->isVisited = true;
			traversalStack.push(v);
		}
	}

}

void JigsawSpriteManager::MoveAttachedJigsawSetIfAny(int spriteId, int dragX, int dragY)
{
	std::stack<int> traversalStack;
	InitGraphTraversal();
	spriteList[spriteId]->isVisited = true;
	traversalStack.push(spriteId);
	int v;
	while (!traversalStack.empty()){
		v = GetAdjacentUnVisitedSpriteId(traversalStack.top());
		if (v < 0) {
			traversalStack.pop();
		}
		else{
			spriteList[v]->isVisited = true;
			spriteList[v]->x += dragX;
			spriteList[v]->y += dragY;
			traversalStack.push(v);
			//printf("moved %d \n", v);
		}
	}
}

void JigsawSpriteManager::InitGraphTraversal()
{
	for (std::vector<JigsawSprite*>::iterator it = spriteList.begin();
		it != spriteList.end();
		++it) {
		JigsawSprite* spr = *it;
		spr->isVisited = false;
	}
}


void JigsawSpriteManager::LinkAdjacentSpritesForASingleSprite(int spriteId)
{
	JigsawSprite *spr = spriteList[spriteId];
	int left = spr->leftNeighbourId;
	int right = spr->rightNeighbourId;
	int up = spr->upNeighbourId;
	int down = spr->downNeighbourId;

	JigsawSprite *leftSpr = (spr->leftNeighbourId >= 0) ? spriteList[spr->leftNeighbourId] : NULL;
	JigsawSprite *rightSpr = (spr->rightNeighbourId >= 0) ? spriteList[spr->rightNeighbourId] : NULL;
	JigsawSprite *upSpr = (spr->upNeighbourId >= 0) ? spriteList[spr->upNeighbourId] : NULL;
	JigsawSprite *downSpr = (spr->downNeighbourId >= 0) ? spriteList[spr->downNeighbourId] : NULL;

	int dx, dy;
	int allowedError = 20;//pixels
	int adx, ady;
	int w = spr->width;
	int h = spr->height;
	int w1 = w - allowedError;
	int w2 = w + allowedError;
	int h1 = h - allowedError;
	int h2 = h + allowedError;

	if (leftSpr != NULL)
	{
		dx = leftSpr->x - spr->x;
		dy = leftSpr->y - spr->y;
		adx = abs(dx);
		ady = abs(dy);
		if ((dx < 0 && adx >= w1 && adx <= w2) && ady <= allowedError){
			leftSpr->hasFoundRightNeighbour = true;
			spr->hasFoundLeftNeighbour = true;
			leftSpr->x = spr->x - w;
			leftSpr->y = spr->y;
		}
	}

	if (rightSpr != NULL)
	{
		dx = rightSpr->x - spr->x;
		dy = rightSpr->y - spr->y;
		adx = abs(dx);
		ady = abs(dy);
		if ((dx > 0 && adx >= w1 && adx <= w2) && ady <= allowedError){
			rightSpr->hasFoundLeftNeighbour= true;
			spr->hasFoundRightNeighbour = true;
			rightSpr->x = spr->x + w;
			rightSpr->y = spr->y;
		}
	}

	if (upSpr != NULL)
	{
		dx = upSpr->x - spr->x;
		dy = upSpr->y - spr->y;
		adx = abs(dx);
		ady = abs(dy);
		if ((dy < 0 && ady >= h1 && ady <= h2) && adx <= allowedError){
			upSpr->hasFoundDownNeighbour = true;
			spr->hasFoundUpNeighbour = true;
			upSpr->y = spr->y - h;
			upSpr->x = spr->x;
		}
	}

	if (downSpr != NULL)
	{
		dx = downSpr->x - spr->x;
		dy = downSpr->y - spr->y;
		adx = abs(dx);
		ady = abs(dy);
		if ((dy > 0 && ady >= h1 && ady <= h2) && adx <= allowedError){
			downSpr->hasFoundUpNeighbour = true;
			spr->hasFoundDownNeighbour = true;
			downSpr->y = spr->y + h;
			downSpr->x = spr->x;
		}
	}

}


int JigsawSpriteManager::GetAdjacentUnVisitedSpriteId(int spriteId)
{
	JigsawSprite *spr = spriteList[spriteId];

	if (spr->leftNeighbourId >= 0 && 
		spr->hasFoundLeftNeighbour && 
		spriteList[spr->leftNeighbourId]->isVisited == false){
		return spr->leftNeighbourId;
	}

	if (spr->rightNeighbourId >= 0 && 
		spr->hasFoundRightNeighbour &&
		spriteList[spr->rightNeighbourId]->isVisited == false){
		return spr->rightNeighbourId;
	}

	if (spr->upNeighbourId >= 0 && 
		spr->hasFoundUpNeighbour &&
		spriteList[spr->upNeighbourId]->isVisited == false){
		return spr->upNeighbourId;
	}

	if (spr->downNeighbourId >= 0 && 
		spr->hasFoundDownNeighbour &&
		spriteList[spr->downNeighbourId]->isVisited == false){
		return spr->downNeighbourId;
	}

	return -1;
}

