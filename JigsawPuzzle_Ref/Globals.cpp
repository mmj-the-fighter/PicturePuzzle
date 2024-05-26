//Copyright 2005 : Manoj M J
#include "Globals.h"

//For debugging 
FILE   *g_fplog;
int  g_ErrorNo;
char g_ErrStr[256];
unsigned long  g_OldTextColor,g_OldBGColor;
///////

int g_fValidSelection=0;
struct _XYPOINT g_tempXYPoint,g_CurPoint,g_MouseDragVector,g_MouseOffset;
int g_SprDisplayParameters[36][6];
int g_BlitingOrder[36];

int g_JigsawDB[36][4]=
{
	{99,82,0,0},
	{83,76,0,0},
	{84,77,0,0},
	{83,65,0,0},
	{82,77,0,0},
	{81,76,0,0},

	{99,77,0,0},
	{83,77,0,0},
	{98,77,0,0},
	{99,77,0,0},
	{82,77,0,0},
	{81,65,0,0},

	{99,77,0,0},
	{83,77,0,0},
	{83,77,0,0},
	{83,77,0,0},
	{98,77,0,0},
	{81,77,0,0},

	{83,77,0,0},
	{83,65,0,0},
	{99,82,0,0},
	{83,77,0,0},
	{98,65,0,0},
	{81,77,0,0},

	{83,65,0,0},
	{83,65,0,0},
	{99,65,0,0},
	{83,65,0,0},
	{82,77,0,0},
	{81,77,0,0},

	{99,63,0,0},
	{83,64,0,0},
	{83,64,0,0},
	{83,64,0,0},
	{82,64,0,0},
	{81,64,0,0}
};








