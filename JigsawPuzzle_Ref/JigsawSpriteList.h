//Copyright 2005 : Manoj M J
#ifndef __JIGSAWSPRITELIST_H__
#define __JIGSAWSPRITELIST_H__

#include<stdio.h>
#include<stdlib.h>
#include <time.h>
#include "Stack.h"
#include "Globals.h"
#include "JigsawSprite.h"

class JIGSAWSpriteList
{
public:
	JIGSAWSprite m_JigsawSpriteObj[36];
	StackX theStack;
	int fValidSelection;
	static int  m_sCurSelection;
public:
	JIGSAWSpriteList();
	~JIGSAWSpriteList();
	void CreateJigsawSpriteList();
	void RandomizeJigsawStartingPos();
	void DrawJigsawSprites();
	void OnMousePress(int MouseX,int MouseY);
	void OnMouseMove(int MouseX, int MouseY);
	void OnMouseRelease(int MouseX,int MouseY);
	void LinkAdjacentCousins(int JigsawTileId);
	int  GetAdjacentUnVisitedTile(int Index);
	void MoveAttachedJigsawSet(int JigsawTileId);
	void AttachAdjacentJigsawSetIfAny(int JigsawTileId);
	void AnchorAllAttachedCousins();
	int  CheckIfAllTilesAreInPlace();
	static int GetCurSelection()
	{
		return JIGSAWSpriteList::m_sCurSelection;
	}
	static void SetCurSelection(int id)
	{
		m_sCurSelection = id;
	}
};

#endif