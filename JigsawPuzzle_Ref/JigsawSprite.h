//Copyright 2005 : Manoj M J
//All rights reserved
#ifndef __JIGSAWSPRITE_H__
#define __JIGSAWSPRITE_H__

#include "Globals.h"
#include <stdio.h>

class JIGSAWSprite
{
public:  
	int m_Id,m_Row,m_Col,m_Width,m_Height;
	CLIPRECT m_ClipRect;
	XYPOINT m_AnchorPoint, m_CurrentPos, m_PrevPos, m_MouseOffset;
	int    fSelected, fAnchorPointReached, fAnchored, fVisited,
		   fFoundLeft, fFoundRight,fFoundTop,fFoundBottom;
public: 
	JIGSAWSprite(){ };
	~JIGSAWSprite(){ };
	void  Init();
	void  Create(int ID,int Row,int Col);
	void  Display(); 
	void  MoveTo(int xpos,int ypos);
	int   IsMouseOver(int MouseX,int MouseY);
	int   RegisterSelection(int xpos,int ypos);
	void  UnRegisterSelection(int xpos,int ypos);
	int   AnchorItIfPossible();
	int   RegisterAnchoring();

	static int GetWidthFromDB(int Id)
	{
		return g_JigsawDB[Id][DBi_WIDTH];
	}
	static int GetHeightFromDB(int Id)
	{
		return g_JigsawDB[Id][DBi_HEIGHT];
	}
	static XYPOINT GetInitPointFromDB(int Id)
	{
		XYPOINT InitPt;
		InitPt.x = g_JigsawDB[Id][DBi_INITPOINTX];
		InitPt.y = g_JigsawDB[Id][DBi_INITPOINTY];
		return InitPt;
	}
	static XYPOINT GetAnchorPoint(int row,int col)
	{
		XYPOINT AnchorPt;
		AnchorPt.x = JIGSAW_STARTX + col*BASE_WIDTH;
		AnchorPt.y = JIGSAW_STARTY + row*BASE_HEIGHT;
		return AnchorPt;
	}
	static int GetJIGSAWId(int r, int c)
	{
		return((r*6+c));
	}
};
#endif
