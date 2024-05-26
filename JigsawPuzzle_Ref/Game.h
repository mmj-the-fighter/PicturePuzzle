//Copyright 2005 : Manoj M J
//All rights reserved
#pragma once
#ifndef  _GAME_HEADER_
#define  _GAME_HEADER_

#include <ddraw.h>
#include <dmusici.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <stdio.h>
#include <process.h>			
#include <mmsystem.h> 
#include "resource.h"
#include "CapScrn.h"
#include "Globals.h"
#include "JigsawSpriteList.h"


#pragma comment(lib,"ddraw.lib") 
#pragma comment(lib,"dxguid.lib")
#pragma comment(lib,"winmm.lib")

#define SWITCHX 481
#define SWITCH_TOPY 302
#define SWITCH_INCRY 64

typedef enum _UIShade{DARK, BRIGHT }UIShade;
typedef enum _Switch{ OFF, ON }Switch;
typedef enum _GameState{ GAME_MENU, GAME_CORE, START, HELP,MUSIC,SOUND, EXIT, BACK,RESET,GAMEOVER, SHOW_JIGSAW_BITMAP,
                         SUCCESS,NIL=0xff}GameState;
typedef enum _SpriteElement{MOUSEPTR=989,SWITCH=987,INVALID = -333 }SpriteElement;
typedef enum _GameResult{LEVELUP=987,WON,LOST,NO_RESULT=-1}GameResult;


typedef struct SPRITE
{  
  int Id;
  int nFrames;
  int iCurFrame;
  int MaxFrameDelay;
  int CurFrameDelay;
  int Width, Height;
}SPRITE;



typedef struct _3DMUSIC
{
	IDirectMusicSegment8*     m_pMusicSeg ;	
	IDirectMusicAudioPath8*   m_pAudioPath ;	
	IDirectSound3DBuffer8*    m_pDSB ;
}MUSIC3DSTRUCT;



LRESULT CALLBACK WndProc (HWND, UINT, WPARAM, LPARAM) ;


class DIRECTX_APP
{
 public:

	BOOL   m_bRunning,m_MusicMode, m_SoundMode,
		   m_MouseClicked,m_bIsFocusLost,gbFrameComplete;
	INT    m_dwAppState,m_KeyboardPollTimer;
	POINT  m_CursorPoint, m_StartPoint, m_EndPoint, m_CurrentPt;
    RECT   m_rDest;
    GameResult m_GameResult;
	SPRITE m_MousePtrSpr;
	JIGSAWSpriteList   JigsawSpriteList;  
	
	//Graphics objects
   	HWND                    m_hWnd;
	LPDIRECTDRAW7           m_lpDD;
	LPDIRECTDRAWSURFACE7    m_lpDDSPrimary,m_lpDDSBack;
	LPDIRECTDRAWSURFACE7    m_lpTitle_Surf,
		                    m_lpStart_Surf, m_lpHelp_Surf, m_lpMusic_Surf,
							m_lpSound_Surf,m_lpSwitch_Surf,m_lpExit_Surf, 
		                    m_lpHelpPage_Surf,m_lpBack_Surf, m_lpMousePtr_Surf,
							m_lpDigit_Surf,m_lpGameOver_Surf, m_lpSuccess_Surf,m_lpTime_Surf,
							m_lpBackGnd_Surf, m_lpEiefel_Surf;

	LPDIRECTDRAWSURFACE7 m_lpJigsawSurf[36],anchor_surf;	
	DDCOLORKEY ddck;

	//Sound Objects 
	IDirectMusicLoader8*      m_pLoader;
	IDirectMusicPerformance8* m_pPerformance;
    MUSIC3DSTRUCT m_BkGndMusic, m_ButtonMusic,m_WonMusic,m_MouseOverMusic;
		          
	   
	//Constructor and destructor
	DIRECTX_APP();
	~DIRECTX_APP(){DestroyApp();};
    
	//Graphics init; sound init
	HRESULT InitDirectDraw();	
	VOID InitDirectMusic();	
	VOID Init_3DMusic(MUSIC3DSTRUCT *_3DMusic);
	DWORD ConvertGDIColor( COLORREF dwGDIColor );			
	LPDIRECTDRAWSURFACE7 CreateBmpSurface(LPCTSTR file_name);
	HRESULT InitApp();

	////Game specific
	VOID InitJigsawSurfaceArray();
	VOID LoadJigsawSurfaceArray();
	VOID ReleaseJigsawSurfaceArray();
		
	//Sound and music
	HRESULT LoadBkGnd3DMusic(WCHAR* Filename,MUSIC3DSTRUCT *_3DMusic );
	HRESULT LoadForeGnd3DMusic(WCHAR* Filename,MUSIC3DSTRUCT *_3DMusic );
	VOID    PlayBkGnd3DMusic  (MUSIC3DSTRUCT *_3DMusic );
	VOID    PlayForeGnd3DMusic(MUSIC3DSTRUCT *_3DMusic );
	
	//UI and Input
	RECT* pRectangle(int left,int top,int right,int bottom);
	VOID DrawDigit(long Value, int StartX, int StartY,int Pad);
    INT  IsKeyDown(int Key);    
	
	//Animated sprites 
	VOID SetSprite(SPRITE * Sprite,int Id, int nFrames,int MaxFrameDelay,int Width, int Height );	
	VOID DrawSprite( SPRITE *Sprite,int PosX,int PosY,int FrameNo);
    
	//Clean up	
	VOID Destroy_3DMusic(_3DMUSIC *_3DMusic);
	VOID DestroyDirectMusic();
	VOID DestroyDirectDraw();	
	VOID DestroyApp();	
	
	////UI and Common
	VOID HandleIfButtonPressed(LPDIRECTDRAWSURFACE7 lpButton_Surf, int SurfId, int Left, int Top, int Right, int Bottom );
	VOID HelpPage();	
	VOID DrawMenu();
	VOID HandleInput();
	VOID GameLoop();
		
	////Gamespecific
	VOID LevelInit();
	VOID LoadLevel(int inex);
	VOID DrawLevel();
	VOID DrawWorld();
	GameResult CheckIfGameOver();

	//For debugging
	HRESULT hret;
	HDC dc;
	int  m_ErrorNo;
	char m_ErrStr[256];
    VOID BltToDC(int xpos, int ypos);
};

#endif
