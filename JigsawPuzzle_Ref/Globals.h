//Copyright 2005 : Manoj M J
//All rights reserved
#ifndef  _GLOBALS_HEADER_
#define  _GLOBALS_HEADER_
#include <stdio.h>
#include<memory.h>
/////////////////////////////////////////
#define NA      -1
#define NOT_YET  0
#define YES      1
#define NO      0
#define DBi_WIDTH  0
#define DBi_HEIGHT 1
#define DBi_INITPOINTX 2
#define DBi_INITPOINTY 3
#define RELAXATION_X 3 
#define RELAXATION_Y 3
#define LT_OFFX 15 
#define LT_OFFY 14 
#define BASE_WIDTH  68
#define BASE_HEIGHT 52
#define JIGSAW_STARTX 200
#define JIGSAW_STARTY 150


#define KEYBOARD_POLLTICKS 15
#define ATTEMPT(Check,Text,Caption)  \
           { \
			   if(!Check) \
			   {\
				   MessageBox(NULL,Text,Caption,MB_OK); \
				   DestroyApp();\
				   _exit(-1);\
			   } \
		   }
#define SAFE_RELEASE(x)\
		{ \
		 if(x) \
		 { \
		  x->Release(); \
		  x = NULL; \
		 } \
		}
#define WRITETOLOG(x)\
	    {\
         g_fplog=fopen("Errlog.txt","a+");\
         fprintf(g_fplog,"\n Reached Execution Point %d",x);\
         fclose(g_fplog);\
		}

#define WRITETEXTVALUELOG(Text,Value)\
		{\
         g_fplog=fopen("Errlog.txt","a+");\
         fprintf(g_fplog,"\n %s %d",Text,Value);\
         fclose(g_fplog);\
}

#define WRITETEXTLOG(Text)\
		{\
         g_fplog=fopen("Errlog.txt","a+");\
         fprintf(g_fplog,"\n %s",Text);\
         fclose(g_fplog);\
}
//For debugging 
extern FILE   *g_fplog;
extern  int  g_ErrorNo;
extern  char g_ErrStr[256];
extern  unsigned long g_OldTextColor,g_OldBGColor;
/////
 /////////////////////////////////////////

typedef struct _XYPOINT
{
	int x,y;
}XYPOINT;


typedef struct _CLIPRECT
{
	long top;
	long bottom;
	long right;
	long left;
}CLIPRECT;

extern int g_fValidSelection;
extern int g_JigsawDB[36][4];
extern struct _XYPOINT g_tempXYPoint,g_CurPoint,g_MouseDragVector,g_MouseOffset;
extern int g_SprDisplayParameters[36][6];
extern int g_BlitingOrder[36];

//////////////////////////////////////
#endif
