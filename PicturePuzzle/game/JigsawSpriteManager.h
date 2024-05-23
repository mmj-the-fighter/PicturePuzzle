#ifndef _JIGSAW_SPRITE_GRID_H_
#define _JIGSAW_SPRITE_GRID_H_

#include <vector>
#include "JigsawSprite.h"
#include "../swr/swr_image.h"

class JigsawSpriteManager
{
private:
	int curSelectionId;
	std::vector<JigsawSprite*> spriteRenderList;
	std::vector<JigsawSprite*> spriteList;
	swr_slice_image_result imageSlicerResult;
	swr_image* sourceImage;
	int desiredCols;
	int desiredRows;
	int spriteCount;
	int prevClickX;
	int prevClickY;
	int curClickX;
	int curClickY;
	int curDragX;
	int curDragY;

	int gameWorldStartX;
	int gameWorldEndX;
	int gameWorldStartY;
	int gameWorldEndY;

public:
	JigsawSpriteManager();
	~JigsawSpriteManager();
	void SetGameWorldLimits(int startX, int startY, int endX, int endY);
	void Load(const char* filename, int rows, int cols);
	void RandomizePositions();
	void RandomizeLayers();
	void BreakConnections();
	void Display();
	void DisplaySourceImage();
	void OnLeftMouseButtonDown(int x, int y);
	void OnLeftMouseButtonUp(int x, int y);
	void OnMouseMoved(int x, int y);
	void UnLoad();
	void InitGraphTraversal();
	int GetAdjacentUnVisitedSpriteId(int spriteId);
	void LinkAdjacentSpritesForASingleSprite(int spriteId);
	void AttachAdjacentJigsawSetIfAny(int spriteId);
	void MoveAttachedJigsawSetIfAny(int spriteId, int dragX, int dragY);
	bool IsAllSpritesAreInSameLayer();
	void dbg_PrintLayers();
};



bool compareByLayer(const JigsawSprite* s1, const JigsawSprite* s2); 




#endif