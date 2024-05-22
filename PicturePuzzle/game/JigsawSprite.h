#ifndef _JIGSAW_SPRITE_H_
#define _JIGSAW_SPRITE_H_

#include "../swr/swr_image.h"


class JigsawSprite
{
public:
	int id;
	int row;
	int col;
	int layer;
	int x;
	int y;
	int width;
	int height;
	bool isSelected;
	swr_image* image;
	int leftNeighbourId;
	int rightNeighbourId;
	int upNeighbourId;
	int downNeighbourId;
	bool hasFoundLeftNeighbour;
	bool hasFoundRightNeighbour;
	bool hasFoundUpNeighbour;
	bool hasFoundDownNeighbour;
	bool isVisited;
public:
	JigsawSprite();
	void Set(int aid, int arow, int acol, int alayer, int ax, int ay, int awidth, int aheight, swr_image* aimage);
	bool IsMouseOver(int mx, int my);
	void RegisterSelection(int mx, int my);
	void UnRegisterSelection();
	void ApplyDrag(int dx, int dy);
	void Display();
};

#endif