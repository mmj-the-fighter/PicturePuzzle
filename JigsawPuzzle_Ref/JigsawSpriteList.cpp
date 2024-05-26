//Copyright 2005 : Manoj M J
#include "JigsawSpriteList.h"

int JIGSAWSpriteList::m_sCurSelection = 0;

JIGSAWSpriteList::JIGSAWSpriteList()
{
	//constructor
	fValidSelection = NO;
    srand((unsigned)time(NULL)); 
}

JIGSAWSpriteList::~JIGSAWSpriteList()
{
	/*
	 */
}
void JIGSAWSpriteList::CreateJigsawSpriteList()
{
	RandomizeJigsawStartingPos();
    for(int i=0; i<6; i++)
		for(int j=0; j<6; j++)
		{
			int Index = i*6+j;
			m_JigsawSpriteObj[Index].Init();
			m_JigsawSpriteObj[Index].Create(Index,i,j);
			g_BlitingOrder[Index] = Index;
			m_JigsawSpriteObj[Index].AnchorItIfPossible();
		}
}
void JIGSAWSpriteList::RandomizeJigsawStartingPos()
{
	
	for(int i=0; i<36; i++)
	{
		g_JigsawDB[i][DBi_INITPOINTX] = 5 + rand()%750;
		g_JigsawDB[i][DBi_INITPOINTY] = 5 + rand()%550;
	}
}
void JIGSAWSpriteList::DrawJigsawSprites()
{
  for(int i=0;i<36;i++)
  {
	  int layerIndex = g_BlitingOrder[i];
      m_JigsawSpriteObj[layerIndex].Display();
  }
}
void JIGSAWSpriteList::OnMousePress(int MouseX, int MouseY)
{
	int temp;
	//IF There is a valid selection register it; ie. don't allow to select if it is in anchor 
	//if it is not then calculate the mouse offset and store
	
    if(g_fValidSelection == YES) return;
	for(int i=0;i<36;i++)
	{
		if(m_JigsawSpriteObj[i].IsMouseOver(MouseX,MouseY))
		{
			if(m_JigsawSpriteObj[i].RegisterSelection(MouseX,MouseY)!=0)
			{
			
				SetCurSelection(i);
				//swap current selection with the bottom of the layers array
				temp = g_BlitingOrder[35];
				g_BlitingOrder[35] = g_BlitingOrder[i];
				g_BlitingOrder[i]  = temp;
				fValidSelection = YES;
				g_fValidSelection = YES;
				break;
			}
		}
	}
	
}
void JIGSAWSpriteList::OnMouseMove(int MouseX, int MouseY)
{
	//if there is a registered selection
	//Move the current tile to the mouseposition
	///Apply the same MouseDragvector to all of its attached cousins to get a dragging effect
	if(fValidSelection==NO)
		return;
	int s = GetCurSelection();
	m_JigsawSpriteObj[s].MoveTo(MouseX,MouseY);
	MoveAttachedJigsawSet(s);
	
}
void JIGSAWSpriteList::OnMouseRelease(int MouseX, int MouseY)
{
	//Anchor the current tile if anchor point reached
	//Check if the current selected tile has found any cousins
	//if the selected tile is anchored;
	//Anchor all the cousins who have not yet been anchored
	int s = GetCurSelection();
	m_JigsawSpriteObj[s].AnchorItIfPossible();
	AttachAdjacentJigsawSetIfAny(s);
	AnchorAllAttachedCousins();
	m_JigsawSpriteObj[s].UnRegisterSelection(MouseX,MouseY);
	fValidSelection = NO;
	g_fValidSelection = NO;
}
void JIGSAWSpriteList::MoveAttachedJigsawSet(int JigsawTileId)
{
	int v,i;
	m_JigsawSpriteObj[JigsawTileId].fVisited = YES;
	theStack.CleanStack();
	theStack.Push(JigsawTileId);
	while(!theStack.IsEmpty())
	{
		v=GetAdjacentUnVisitedTile(theStack.Peek());
		if(v==-1)
			theStack.Pop();
		else
		{
			m_JigsawSpriteObj[v].fVisited = YES;
			if(m_JigsawSpriteObj[v].fAnchored!=YES)
			{
				m_JigsawSpriteObj[v].m_CurrentPos.x += g_MouseDragVector.x;
				m_JigsawSpriteObj[v].m_CurrentPos.y += g_MouseDragVector.y;
				m_JigsawSpriteObj[v].m_PrevPos = m_JigsawSpriteObj[v].m_CurrentPos;
			}
			theStack.Push(v);
		}
	}

	for(i=0;i<36;i++)
		m_JigsawSpriteObj[i].fVisited = NO;
}

void JIGSAWSpriteList::AttachAdjacentJigsawSetIfAny(int JigsawTileId)
{
	int i,v;
    LinkAdjacentCousins(JigsawTileId);
    m_JigsawSpriteObj[JigsawTileId].fVisited = YES;
	theStack.CleanStack();
    theStack.Push(JigsawTileId);

	while(!theStack.IsEmpty())
	{
		v=GetAdjacentUnVisitedTile(theStack.Peek());
		if(v==-1)
			theStack.Pop();
		else
		{
			m_JigsawSpriteObj[v].fVisited = YES;
			LinkAdjacentCousins(v);
			theStack.Push(v);
		}
	}

	for(i=0;i<36;i++)
		m_JigsawSpriteObj[i].fVisited = NO;
}
int JIGSAWSpriteList::GetAdjacentUnVisitedTile(int Index)
{
	int row,col,Id;

	row = m_JigsawSpriteObj[Index].m_Row;
	col = m_JigsawSpriteObj[Index].m_Col;

	   
	//Check Left 
	if((col-1)>=0 && m_JigsawSpriteObj[Index].fFoundLeft == YES)
	{
		col=col-1;
		Id = row*6+col;
		if(m_JigsawSpriteObj[Id].fVisited == NO)
		{
			return Id;
		}
	}

	row = m_JigsawSpriteObj[Index].m_Row;
	col = m_JigsawSpriteObj[Index].m_Col;

	//Check Right 
	if((col+1)<=5 && m_JigsawSpriteObj[Index].fFoundRight == YES)
	{
		col=col+1;
		Id = row*6+col;
		if(m_JigsawSpriteObj[Id].fVisited == NO)
		{
			return Id;
		}
	}

	row = m_JigsawSpriteObj[Index].m_Row;
	col = m_JigsawSpriteObj[Index].m_Col;

	//Check Top
	if((row-1)>=0 && m_JigsawSpriteObj[Index].fFoundTop == YES)
	{
		row=row-1;
		Id = row*6+col;
		if(m_JigsawSpriteObj[Id].fVisited == NO)
		{
			return Id;
		}
	}

	row = m_JigsawSpriteObj[Index].m_Row;
	col = m_JigsawSpriteObj[Index].m_Col;

	//Check Bottom
	if((row+1)<=5 && m_JigsawSpriteObj[Index].fFoundBottom == YES)
	{
		row=row+1;
		Id = row*6+col;
		if(m_JigsawSpriteObj[Id].fVisited == NO)
		{
			return Id;
		}
	}
	
	 return -1;
}

void JIGSAWSpriteList::LinkAdjacentCousins(int Index)
{
	XYPOINT TilePt,adjTilePt,Delta,Base,Rel;
	int row,col,adjRow,adjCol,adjId;
	int fAnchorIt;
	
   
	//if i am in anchor i will bring others stick to me
	fAnchorIt = (m_JigsawSpriteObj[Index].fAnchored == YES) ? 1: 0;

	Base.x = BASE_WIDTH;
	Base.y = BASE_HEIGHT;
	Rel.x = RELAXATION_X;
	Rel.y = RELAXATION_Y;

    row = m_JigsawSpriteObj[Index].m_Row;
	col = m_JigsawSpriteObj[Index].m_Col;
	TilePt = m_JigsawSpriteObj[Index].m_CurrentPos;

	//check LEFT
	adjRow = row;
	adjCol = col-1;

	if(adjCol>=0)
	{
		adjId = adjRow*6 + adjCol;
		adjTilePt = m_JigsawSpriteObj[adjId].m_CurrentPos;
		Delta.x  =  TilePt.x-adjTilePt.x;
		Delta.y  =  TilePt.y-adjTilePt.y;
		
		if(Delta.x>0&&
		    abs(Delta.x) >= (Base.x-Rel.x) && abs(Delta.x) <= (Base.x+Rel.x)&&
			Delta.y  >= -Rel.y && Delta.y <= Rel.y
			)
		{
			m_JigsawSpriteObj[Index].fFoundLeft = YES;
			m_JigsawSpriteObj[adjId].fFoundRight = YES;
			if(fAnchorIt) m_JigsawSpriteObj[adjId].fAnchored = YES;
			m_JigsawSpriteObj[Index].m_CurrentPos.x = m_JigsawSpriteObj[adjId].m_CurrentPos.x + BASE_WIDTH;
		} 
	}
		

	//check RIGHT
	adjRow = row;
	adjCol = col+1;

	if(adjCol<=5)
	{
		adjId = adjRow*6 + adjCol;
		adjTilePt = m_JigsawSpriteObj[adjId].m_CurrentPos;
		Delta.x  =  TilePt.x-adjTilePt.x;
		Delta.y  =  TilePt.y-adjTilePt.y;
		
		if(Delta.x<0&&
		    abs(Delta.x) >= (Base.x-Rel.x) && abs(Delta.x) <= (Base.x+Rel.x)&&
			Delta.y  >= -Rel.y && Delta.y <= Rel.y
			)
		{
			m_JigsawSpriteObj[Index].fFoundRight = YES;
			m_JigsawSpriteObj[adjId].fFoundLeft = YES;
			if(fAnchorIt) m_JigsawSpriteObj[adjId].fAnchored = YES;
			m_JigsawSpriteObj[Index].m_CurrentPos.x = m_JigsawSpriteObj[adjId].m_CurrentPos.x - BASE_WIDTH;
		}
	}
		

	//check TOP
	adjRow = row-1;
	adjCol = col;
	if(adjRow >=0)
	{
		adjId = adjRow*6 + adjCol;
		adjTilePt = m_JigsawSpriteObj[adjId].m_CurrentPos;
		Delta.x  =  TilePt.x-adjTilePt.x;
		Delta.y  =  TilePt.y-adjTilePt.y;
		
		if(Delta.y>0&&
		    abs(Delta.y) >= (Base.y-Rel.y) && abs(Delta.y) <= (Base.y+Rel.y)&&
			Delta.x  >= -Rel.x && Delta.x <= Rel.x
			)
		{
			m_JigsawSpriteObj[Index].fFoundTop = YES;
			m_JigsawSpriteObj[adjId].fFoundBottom = YES;
			if(fAnchorIt) m_JigsawSpriteObj[adjId].fAnchored = YES;
			m_JigsawSpriteObj[Index].m_CurrentPos.y = m_JigsawSpriteObj[adjId].m_CurrentPos.y + BASE_HEIGHT;
		}
	}
		

	//check BOTTOM
	adjRow = row+1;
	adjCol = col;
	if(adjRow <= 5)
	{
		adjId = adjRow*6 + adjCol;
		adjTilePt = m_JigsawSpriteObj[adjId].m_CurrentPos;
		Delta.x  =  TilePt.x-adjTilePt.x;
		Delta.y  =  TilePt.y-adjTilePt.y;
		
		if(Delta.y<0&&
		    abs(Delta.y) >= (Base.y-Rel.y) && abs(Delta.y) <= (Base.y+Rel.y)&&
			Delta.x  >= -Rel.x && Delta.x <= Rel.x
			)
		{
			m_JigsawSpriteObj[Index].fFoundBottom = YES;
			m_JigsawSpriteObj[adjId].fFoundTop = YES;
			if(fAnchorIt) m_JigsawSpriteObj[adjId].fAnchored = YES;
			m_JigsawSpriteObj[Index].m_CurrentPos.y = m_JigsawSpriteObj[adjId].m_CurrentPos.y - BASE_HEIGHT;
		}
	}
}
int JIGSAWSpriteList::CheckIfAllTilesAreInPlace()
{
	for(int i=0;i<36;i++)
	{
		if(m_JigsawSpriteObj[i].fAnchored == NO)
            return 0;
    }
	return 1;
}

void JIGSAWSpriteList::AnchorAllAttachedCousins()
{
   int anchored_list[36],id=0,k=0,i=0,row=0,col=0;
	for(i=0;i<36;i++)
	{
		if(m_JigsawSpriteObj[i].fAnchored == YES)
		  anchored_list[k++] =  i;
	}

	for(i=0;i<k;i++)
	{
		id = anchored_list[i];
		row = id/6;
		col = id%6;
		
		m_JigsawSpriteObj[id].m_CurrentPos.x = m_JigsawSpriteObj[id].m_AnchorPoint.x -  LT_OFFX;
		m_JigsawSpriteObj[id].m_CurrentPos.y = m_JigsawSpriteObj[id].m_AnchorPoint.y -  LT_OFFY;
	}
}


