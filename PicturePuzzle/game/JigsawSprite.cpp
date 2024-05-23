#include <cstdio>
#include "JigsawSprite.h"
#include "../swr/swr_rasterizer.h"

JigsawSprite::JigsawSprite()
{
	leftNeighbourId = -1;
	rightNeighbourId = -1;
	upNeighbourId = -1;
	downNeighbourId = -1;
	hasFoundLeftNeighbour = false;
	hasFoundRightNeighbour = false;
	hasFoundUpNeighbour = false;
	hasFoundDownNeighbour = false;

}

void JigsawSprite::DisconnectWithNeighbours()
{
	hasFoundLeftNeighbour = false;
	hasFoundRightNeighbour = false;
	hasFoundUpNeighbour = false;
	hasFoundDownNeighbour = false;
}

void JigsawSprite::Set(int aid, int arow, int acol, int alayer, int ax, int ay, int awidth, int aheight, swr_image *aimage)
{
	id = aid;
	row = arow;
	col = acol;
	layer = alayer;
	x = ax;
	y = ay;
	width = awidth;
	height = aheight;
	image = aimage;

}

bool JigsawSprite::IsMouseOver(int mx, int my)
{
	return (
		(mx > x) && mx < (x + width) &&
		(my > y) && my < (y + height)
		);
}

void JigsawSprite::RegisterSelection(int mx, int my)
{
	isSelected = true;
}


void JigsawSprite::UnRegisterSelection()
{
	isSelected = false;
}


void JigsawSprite::ApplyDrag(int dx, int dy)
{
	if (isSelected)
	{
		x += dx;
		y += dy;
	}
	//else{
	//	printf("not selected\n");
	//}
}

void JigsawSprite::Display()
{
	rasterizer_copy_pixels(x, y, image->width, image->height, image->pixels);
}
