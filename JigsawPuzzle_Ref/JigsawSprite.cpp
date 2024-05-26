//Copyright 2005 : Manoj M J
#include "JigsawSprite.h"


void JIGSAWSprite::Init()
{
	fSelected = NO;
	fAnchorPointReached = NO;
	fAnchored = NO;
	fFoundLeft = NA;
	fFoundRight = NA;
	fFoundTop = NA;
	fFoundBottom = NA;
	fVisited = NO;
	m_MouseOffset.x = 0;
	m_MouseOffset.y = 0;
	
}
void JIGSAWSprite::Create(int id,int row,int col)
{
	m_Id    = id;
	m_Row   = row;
	m_Col   = col;
	m_Width  = GetWidthFromDB(m_Id);
	m_Height = GetHeightFromDB(m_Id);
	m_AnchorPoint = GetAnchorPoint(m_Row,m_Col);
	m_CurrentPos = GetInitPointFromDB(m_Id);
	m_PrevPos = m_CurrentPos;

	fFoundLeft = NOT_YET;
	fFoundRight = NOT_YET;
	fFoundTop = NOT_YET;
	fFoundBottom = NOT_YET;

	//top link is not applicable for 0th row
	if(m_Row==0)
		fFoundTop = NA;
	//bottom link is not applicable for last row
	if(m_Row==5)
		fFoundBottom = NA;
	//left link is not applicable for 0th col
	if(m_Col==0)
		fFoundLeft = NA;
	//right link is not applicable for last col
	if(m_Col == 5)
		fFoundRight = NA;
} 
void JIGSAWSprite::Display()
{
	//Do Clipping if necessary
	//store m_ClipRect and curpos in g_JigSprParas
	
	m_ClipRect.left =0;
	m_ClipRect.right =m_Width;
	m_ClipRect.top =0;
	m_ClipRect.bottom =m_Height;
    
    int PosX = m_CurrentPos.x, PosY = m_CurrentPos.y;

	
	if(PosX < 0)
	{
		m_ClipRect.left -= PosX;
		PosX =0;
	}
	else if(PosX + m_Width > 800)
		m_ClipRect.right -= (PosX + m_Width - 800);

	if(PosY < 0)
	{
		m_ClipRect.top -= PosY;
		PosY = 0;
	}
	else if(PosY + m_Height > 600)
		m_ClipRect.bottom -= (PosY + m_Height-600);

	g_SprDisplayParameters[m_Id][0] = PosX ;
	g_SprDisplayParameters[m_Id][1] = PosY ;
	g_SprDisplayParameters[m_Id][2] = m_ClipRect.left;
	g_SprDisplayParameters[m_Id][3] = m_ClipRect.right;
	g_SprDisplayParameters[m_Id][4] = m_ClipRect.top;
	g_SprDisplayParameters[m_Id][5] = m_ClipRect.bottom;
}
void JIGSAWSprite::MoveTo(int x, int y)
{
	if(fAnchored==YES)
		return;
	m_CurrentPos.x = x + g_MouseOffset.x;
	m_CurrentPos.y = y + g_MouseOffset.y;
	g_MouseDragVector.x = m_CurrentPos.x - m_PrevPos.x;
	g_MouseDragVector.y = m_CurrentPos.y - m_PrevPos.y;
	m_PrevPos = m_CurrentPos;
}
int JIGSAWSprite::AnchorItIfPossible()
{
	if((m_CurrentPos.x+LT_OFFX) > (m_AnchorPoint.x - RELAXATION_X)&&
		(m_CurrentPos.x+LT_OFFX) < (m_AnchorPoint.x + RELAXATION_X)&&
		(m_CurrentPos.y+LT_OFFY) > (m_AnchorPoint.y - RELAXATION_Y)&&
		(m_CurrentPos.y+LT_OFFY) < (m_AnchorPoint.y + RELAXATION_Y) 
		)
	{
		fAnchored = YES;
		m_CurrentPos.x = m_AnchorPoint.x - LT_OFFX;
		m_CurrentPos.y = m_AnchorPoint.y- LT_OFFY;
		return 1;
	}
	else
	{
		fAnchored = NO;
		return 0;
	}
}

////////???????????????
int JIGSAWSprite::IsMouseOver(int MouseX,int MouseY)
{
	if((MouseX > (m_CurrentPos.x+LT_OFFX)) && (MouseX < (m_CurrentPos.x+LT_OFFX+BASE_WIDTH))&& 
		(MouseY > m_CurrentPos.y+LT_OFFY) && (MouseY < (m_CurrentPos.y+BASE_HEIGHT+LT_OFFY)))
	  return 1;
	else
	  return 0;
}

int JIGSAWSprite::RegisterSelection(int MouseX,int MouseY)
{
	if(fAnchored==YES)
		return 0;
	fSelected = YES;

	g_MouseOffset.x = m_CurrentPos.x-MouseX;   
	g_MouseOffset.y = m_CurrentPos.y-MouseY;
	return 1;
}
void JIGSAWSprite::UnRegisterSelection(int MouseX,int MouseY)
{
	fSelected = NO;
	m_MouseOffset.x = 0;
	m_MouseOffset.y = 0;
}

int JIGSAWSprite::RegisterAnchoring()
{
	fAnchored = YES;
	fSelected=NO;
	return 1;
}
  