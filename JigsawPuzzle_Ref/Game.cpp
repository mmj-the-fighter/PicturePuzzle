//Copyright 2005 : Manoj M J

#include "Game.h"

DIRECTX_APP *DirectXApp; 
bool g_bIsDXInited = FALSE;
TCHAR szAppName[] = TEXT ("Jigsaw");


INT WINAPI WinMain( HINSTANCE hInst, HINSTANCE hPrevInstance,
                     LPSTR     lpCmdLine,
                     int       nCmdShow)
{
	
	HWND hWnd;
	ShowCursor(FALSE);
	WNDCLASSEX wc = { sizeof(WNDCLASSEX), CS_CLASSDC|CS_NOCLOSE, WndProc, 0L, 0L, 
		              GetModuleHandle(NULL), LoadIcon(hInst, MAKEINTRESOURCE (IDI_ICON1)), NULL, 
					  HBRUSH(GetStockObject(BLACK_BRUSH)),NULL,szAppName, NULL }; 
	RegisterClassEx( &wc );
	hWnd =  CreateWindow(szAppName, szAppName,WS_POPUP|WS_VISIBLE, 
		                 0, 0, GetSystemMetrics(SM_CXSCREEN),GetSystemMetrics(SM_CYSCREEN),
                         GetDesktopWindow(), NULL, wc.hInstance, NULL );
	CoInitialize(NULL);
	
	DirectXApp = new DIRECTX_APP();
	
    DirectXApp->m_hWnd = hWnd;
    g_bIsDXInited = TRUE;
    ShowWindow( DirectXApp->m_hWnd, SW_SHOWDEFAULT );
    UpdateWindow( DirectXApp->m_hWnd );

	if( SUCCEEDED( DirectXApp->InitApp()))
	{
		MSG msg; 
		while(DirectXApp->m_bRunning)
		{
			if(PeekMessage(&msg, NULL, 0U, 0U, PM_REMOVE))
			{
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}

			if(DirectXApp->m_bIsFocusLost == FALSE)
			{
				DWORD StartTime = GetTickCount(); 
				DirectXApp->GameLoop();		

				while(GetTickCount() - StartTime < 20)// limit to 50 Frames per Sec
					;

				if(GetAsyncKeyState(VK_F9))
					SaveScreenShot(DirectXApp->m_hWnd);
			}
		}

		DirectXApp->DestroyApp();
	}
	UnregisterClass(szAppName, wc.hInstance );
	CoUninitialize();
	return 0;
}

LRESULT CALLBACK WndProc( HWND HWnd, UINT msg, WPARAM wParam, LPARAM lParam )
{
    switch( msg )
    {
		case WM_LBUTTONDOWN:
			    DirectXApp->m_MouseClicked = TRUE; 
				if(DirectXApp->m_dwAppState==GAME_CORE)
				{
					g_CurPoint.x = (int)(LOWORD(lParam));
					g_CurPoint.y = (int)(HIWORD(lParam));
					DirectXApp->JigsawSpriteList.OnMousePress(g_CurPoint.x,g_CurPoint.y);
				}
			    break;
		case WM_MOUSEMOVE:
			if(DirectXApp->m_dwAppState==GAME_CORE && g_fValidSelection == YES)
				{
			        g_CurPoint.x = (int)(LOWORD(lParam));
					g_CurPoint.y = (int)(HIWORD(lParam));
					DirectXApp->JigsawSpriteList.OnMouseMove(g_CurPoint.x,g_CurPoint.y);
				}
				break;
		case WM_LBUTTONUP:
			    if(DirectXApp->m_dwAppState==GAME_CORE && g_fValidSelection == YES)
				{
					g_CurPoint.x = (int)(LOWORD(lParam));
					g_CurPoint.y = (int)(HIWORD(lParam));
					DirectXApp->JigsawSpriteList.OnMouseRelease(g_CurPoint.x,g_CurPoint.y);
				}
				break;
		case WM_KEYDOWN:
			    switch(wParam)
			    {	
				 case VK_ESCAPE:				    
					DirectXApp->m_dwAppState = GAME_MENU;					
					DirectXApp->m_MouseClicked = FALSE; /* Clear all previous MouseClicks */
					break;
			    }
			    break;

		case WM_SETFOCUS:		
			if( g_bIsDXInited )
				if(DirectXApp->m_bIsFocusLost)
				{
					DirectXApp->m_bIsFocusLost = FALSE; 
					SetFocus(DirectXApp->m_hWnd);
					DirectXApp->InitApp(); 
				}
			break;
	    case WM_KILLFOCUS:  
			if( g_bIsDXInited )
			{
				DirectXApp->m_bIsFocusLost = TRUE; 
				DirectXApp->DestroyApp();
			}
			break;
    }
    return DefWindowProc( HWnd, msg, wParam, lParam );
}



DIRECTX_APP::DIRECTX_APP()
{
	

	m_bIsFocusLost = FALSE; 
	m_dwAppState   = GAME_MENU;
	m_hWnd         = NULL;
    m_lpDD         = NULL;
    m_lpDDSPrimary = NULL;
    m_lpDDSBack    = NULL;
		
	//Game
    m_lpTitle_Surf = NULL;
	m_lpBackGnd_Surf  = NULL;
	m_lpEiefel_Surf = NULL;
	m_lpGameOver_Surf = NULL;
	m_lpSuccess_Surf = NULL;
	m_lpTime_Surf = NULL;
	anchor_surf = NULL;
	//InitJigsawSurfaceArray();

	//UI
    m_lpStart_Surf = NULL;
    m_lpHelp_Surf  = NULL;
    m_lpMusic_Surf = NULL;
    m_lpSound_Surf  = NULL;
    m_lpExit_Surf = NULL;
    m_lpHelpPage_Surf  = NULL;
    m_lpBack_Surf      = NULL;
    m_lpMousePtr_Surf   = NULL;
    m_lpSwitch_Surf     = NULL;
    m_lpDigit_Surf    = NULL;
	
	
  
	m_bRunning     = TRUE;  
	m_MouseClicked = FALSE;
	m_MusicMode = m_SoundMode = ON;
	m_pLoader      = NULL;
    m_pPerformance = NULL;

	Init_3DMusic(&m_BkGndMusic);
	Init_3DMusic(&m_ButtonMusic);	
	Init_3DMusic(&m_MouseOverMusic);
	Init_3DMusic(&m_WonMusic);
}


HRESULT DIRECTX_APP::InitDirectDraw()
{
	DDSURFACEDESC2 ddsd;
    DDSCAPS2 ddscaps;
	HRESULT		hRet;
 
    //Create the main DirectDraw object
    LPDIRECTDRAW pDD;

    hRet = DirectDrawCreate(NULL,&pDD, NULL);
    ATTEMPT((hRet == DD_OK),"DirectDraw create failed","InitDirectDraw failed")

	hRet = pDD->QueryInterface(IID_IDirectDraw7, (LPVOID *) & m_lpDD);
    ATTEMPT((hRet == DD_OK),"Querry interface failed","InitDirectDraw failed")
    
	//Set cooperative level
	hRet = m_lpDD->SetCooperativeLevel(m_hWnd,DDSCL_EXCLUSIVE | DDSCL_FULLSCREEN |DDSCL_ALLOWREBOOT| DDSCL_ALLOWMODEX);
	ATTEMPT((hRet == DD_OK),"Set cooperative level failed","InitDirectDraw failed")
	 
	hRet = m_lpDD->SetDisplayMode( 800,600,16,0,0);
	ATTEMPT((hRet == DD_OK),"Can't set suitable display mode","InitDirectDraw failed")

	ZeroMemory(&ddsd, sizeof(ddsd));
	ddsd.dwSize = sizeof(ddsd);
	ddsd.dwFlags = DDSD_CAPS | DDSD_BACKBUFFERCOUNT;
	ddsd.ddsCaps.dwCaps = DDSCAPS_PRIMARYSURFACE | DDSCAPS_FLIP | DDSCAPS_COMPLEX;
    ddsd.dwBackBufferCount = 1;

	hRet = m_lpDD->CreateSurface(&ddsd, &m_lpDDSPrimary, NULL);
 	ATTEMPT((hRet == DD_OK),"Create Front Surface failed","InitDirectDraw failed")

    ZeroMemory( &ddscaps, sizeof( ddscaps ) );
	ddscaps.dwCaps=DDSCAPS_BACKBUFFER;
	hRet = m_lpDDSPrimary->GetAttachedSurface(&ddscaps,&m_lpDDSBack);
	ATTEMPT((hRet == DD_OK),"Create Back Surface failed","InitDirectDraw failed")
	
	pDD->Release();
    return true;
}

VOID DIRECTX_APP::InitDirectMusic()
{
  CoCreateInstance( CLSID_DirectMusicLoader, NULL, CLSCTX_INPROC,IID_IDirectMusicLoader8, (void**)&m_pLoader );
  CoCreateInstance( CLSID_DirectMusicPerformance, NULL, CLSCTX_INPROC, IID_IDirectMusicPerformance8, (void**)&m_pPerformance );
  m_pPerformance->InitAudio( NULL, NULL, NULL,DMUS_APATH_SHARED_STEREOPLUSREVERB, 64,DMUS_AUDIOF_ALL, NULL );
}

VOID DIRECTX_APP::Init_3DMusic(MUSIC3DSTRUCT *_3DMusic )
{
	_3DMusic->m_pMusicSeg   = NULL;
	_3DMusic->m_pAudioPath  = NULL;
	_3DMusic->m_pDSB        = NULL;
}

DWORD DIRECTX_APP::ConvertGDIColor( COLORREF dwGDIColor )
{
	if( m_lpDDSPrimary == NULL )
	    return 0x00000000;

    COLORREF       rgbT;
    HDC            hdc;
    DWORD          dw = CLR_INVALID;
    DDSURFACEDESC2 ddsd;
    HRESULT        hr;

    //  Use GDI SetPixel to color match for us
    if( dwGDIColor != CLR_INVALID && m_lpDDSPrimary->GetDC(&hdc) == DD_OK)
    {
        rgbT = GetPixel(hdc, 0, 0);     // Save current pixel value
        SetPixel(hdc, 0, 0, dwGDIColor);       // Set our value
        m_lpDDSPrimary->ReleaseDC(hdc);
    }

    // Now lock the surface so we can read back the converted color
    ddsd.dwSize = sizeof(ddsd);
    hr = m_lpDDSPrimary->Lock( NULL, &ddsd, DDLOCK_WAIT, NULL );
    if( hr == DD_OK)
    {
        dw = *(DWORD *) ddsd.lpSurface; 
        if( ddsd.ddpfPixelFormat.dwRGBBitCount < 32 ) // Mask it to bpp
            dw &= ( 1 << ddsd.ddpfPixelFormat.dwRGBBitCount ) - 1;  
        m_lpDDSPrimary->Unlock(NULL);
    }

    //  Now put the color that was there back.
    if( dwGDIColor != CLR_INVALID && m_lpDDSPrimary->GetDC(&hdc) == DD_OK )
    {
        SetPixel( hdc, 0, 0, rgbT );
        m_lpDDSPrimary->ReleaseDC(hdc);
    }
    
    return dw;    
}

LPDIRECTDRAWSURFACE7 DIRECTX_APP::CreateBmpSurface(LPCTSTR file_name)
{
	HDC hdc;
	HBITMAP bit;
	LPDIRECTDRAWSURFACE7 surf;

	// load the interface bitmap
	bit=(HBITMAP) LoadImage(NULL,file_name,IMAGE_BITMAP,0,0,
								LR_DEFAULTSIZE|LR_LOADFROMFILE);
	if (!bit) 
		// failed to load, return failure to calle*r
		return NULL;

	// get bitmap dimensions
	BITMAP bitmap;
    GetObject( bit, sizeof(BITMAP), &bitmap );
	int surf_width=bitmap.bmWidth;
	int surf_height=bitmap.bmHeight;

	// create surface
	HRESULT ddrval;
	DDSURFACEDESC2 ddsd;
	ZeroMemory(&ddsd,sizeof(ddsd));
	ddsd.dwSize = sizeof(DDSURFACEDESC2);
	ddsd.dwFlags = DDSD_CAPS | DDSD_WIDTH | DDSD_HEIGHT ;
	ddsd.ddsCaps.dwCaps = DDSCAPS_OFFSCREENPLAIN;
	ddsd.dwWidth = surf_width;
	ddsd.dwHeight = surf_height;

	// attempt to create surface
	ddrval = m_lpDD->CreateSurface(&ddsd,&surf,NULL);

	// created ok?
	if (ddrval!=DD_OK) {

		// no, release the bitmap and return failure to caller

		DeleteObject(bit);
		return NULL;

	} else {

		// yes, get a DC for the surface

		surf->GetDC(&hdc);

		// generate a compatible DC

		HDC bit_dc=CreateCompatibleDC(hdc);

		// blit the interface to the surface

		SelectObject(bit_dc,bit);
		BitBlt(hdc,0,0,surf_width,surf_height,bit_dc,0,0,SRCCOPY);

		// release the DCs

		surf->ReleaseDC(hdc);
		DeleteDC(bit_dc);
	}

	// clear bitmap 
	DeleteObject(bit);

	// return pointer to caller
	ATTEMPT( surf ,	"Can't Load %s! Game is Exiting!!", "DirectDraw" )
    return surf;
}

HRESULT DIRECTX_APP::InitApp()
{
	HRESULT hr;  
	if( FAILED( hr = InitDirectDraw() ) )
		return hr;

	m_lpTitle_Surf    = CreateBmpSurface("Tex\\UI\\Title.Tex");	 

	anchor_surf    = CreateBmpSurface("Tex\\Game\\Anchor.Tex");	
	m_lpBackGnd_Surf    = CreateBmpSurface("Tex\\Game\\BackGnd.Tex");	
	m_lpGameOver_Surf  = CreateBmpSurface("Tex\\Game\\GameOver.Tex");
	m_lpSuccess_Surf   = CreateBmpSurface("Tex\\Game\\Success.Tex");
	m_lpEiefel_Surf	   = CreateBmpSurface("Tex\\Game\\Eiefel.Tex");
	m_lpTime_Surf	   = CreateBmpSurface("Tex\\Game\\Time.Tex");

	m_lpStart_Surf   =  CreateBmpSurface("Tex\\UI\\Start.Tex");
    m_lpHelp_Surf    =  CreateBmpSurface("Tex\\UI\\Help.Tex");
    m_lpMusic_Surf   =  CreateBmpSurface("Tex\\UI\\Music.Tex");
    m_lpSound_Surf   =  CreateBmpSurface("Tex\\UI\\Sound.Tex");
    m_lpExit_Surf   =  CreateBmpSurface("Tex\\UI\\Exit.Tex");
    m_lpHelpPage_Surf   =  CreateBmpSurface("Tex\\UI\\HelpPage.Tex");
    m_lpBack_Surf   =  CreateBmpSurface("Tex\\UI\\Back.Tex");	  
    m_lpMousePtr_Surf   =  CreateBmpSurface("Tex\\UI\\MousePtr.Tex");	  
    m_lpSwitch_Surf   =  CreateBmpSurface("Tex\\UI\\Switch.Tex");	  
    m_lpDigit_Surf   =  CreateBmpSurface("Tex\\UI\\Digits.Tex");	  

	//Set the color key to green
  	//set color key
	ddck.dwColorSpaceLowValue  = ConvertGDIColor(RGB(0,255,0));
	ddck.dwColorSpaceHighValue = ConvertGDIColor(RGB(0,255,0));
	//Game
	
	anchor_surf ->SetColorKey( DDCKEY_SRCBLT, &ddck); 
	m_lpTitle_Surf->SetColorKey( DDCKEY_SRCBLT, &ddck); 
	m_lpGameOver_Surf->SetColorKey( DDCKEY_SRCBLT, &ddck); 
	m_lpSuccess_Surf->SetColorKey( DDCKEY_SRCBLT, &ddck ); 
	m_lpTime_Surf->SetColorKey( DDCKEY_SRCBLT, &ddck ); 
	//UI
    m_lpStart_Surf->SetColorKey( DDCKEY_SRCBLT, &ddck ); 
    m_lpHelp_Surf->SetColorKey( DDCKEY_SRCBLT, &ddck ); 
    m_lpMusic_Surf->SetColorKey( DDCKEY_SRCBLT, &ddck ); 
    m_lpSound_Surf->SetColorKey( DDCKEY_SRCBLT, &ddck ); 
    m_lpExit_Surf->SetColorKey( DDCKEY_SRCBLT, &ddck );  
	m_lpBack_Surf->SetColorKey( DDCKEY_SRCBLT, &ddck ); 
    m_lpHelpPage_Surf->SetColorKey( DDCKEY_SRCBLT, &ddck ); 
    m_lpMousePtr_Surf->SetColorKey( DDCKEY_SRCBLT, &ddck );     
    m_lpDigit_Surf->SetColorKey( DDCKEY_SRCBLT, &ddck ); 
	m_lpSwitch_Surf->SetColorKey( DDCKEY_SRCBLT, &ddck ); 

    //Jigsaw bmps
	LoadJigsawSurfaceArray();
	JigsawSpriteList.CreateJigsawSpriteList();
	JigsawSpriteList.RandomizeJigsawStartingPos();

		

	/* Create a Sprite from Bitmap */
	SetSprite(&m_MousePtrSpr,MOUSEPTR,1/*nFrames*/, 3/* MaxFrameDelay*/,13/*Width*/ ,22/*Height*/);		 

  
	InitDirectMusic(); /* Initialize the DirectMusic part */
	if( FAILED( hr = LoadBkGnd3DMusic(L"Aud\\BkGnd.mus",&m_BkGndMusic) ) )
		return hr;
       
	
	PlayBkGnd3DMusic(&m_BkGndMusic);
	LoadForeGnd3DMusic(L"Aud\\Button.Aud",&m_ButtonMusic);		
	LoadForeGnd3DMusic(L"Aud\\Complete.Aud",&m_WonMusic);
	LoadForeGnd3DMusic(L"Aud\\MouseOver.Aud",&m_MouseOverMusic);		
	return S_OK;
}

VOID DIRECTX_APP::InitJigsawSurfaceArray()
{
 for(int i=0;i<36;i++)
   m_lpJigsawSurf[i] = NULL;
}

VOID DIRECTX_APP::ReleaseJigsawSurfaceArray()
{
 for(int i=0;i<36;i++)
   SAFE_RELEASE(m_lpJigsawSurf[i]);
}

VOID DIRECTX_APP::LoadJigsawSurfaceArray()
{
 char fname[64];
 int i,j,arrIndex;
		 
 for(i=0;i<6;i++)
 {
	for(j=0;j<6;j++)
	 {
		 arrIndex = i*6+j;
		 sprintf(fname,"Tex\\Game\\Level\\E%d%d.Tex",(i+1),(j+1));
		 m_lpJigsawSurf[arrIndex] =  CreateBmpSurface(fname);
		 m_lpJigsawSurf[arrIndex]->SetColorKey(DDCKEY_SRCBLT,&ddck); 
	 }
 }

}

HRESULT DIRECTX_APP::LoadBkGnd3DMusic(WCHAR* Filename,MUSIC3DSTRUCT *_3DMusic)
{
    if( FAILED( m_pLoader->LoadObjectFromFile( CLSID_DirectMusicSegment,
                                               IID_IDirectMusicSegment8, Filename,
											   (LPVOID*) &_3DMusic->m_pMusicSeg ) ) )
    {
        MessageBox( NULL, "Media not found, Program will now quit","Game",MB_OK );
		m_pPerformance->CloseDown();
        m_pLoader->Release(); 
        m_pPerformance->Release();
        return 0;
    }

	/* Create an 3D audiopath with a 3d buffer. We can then play all segments into this buffer 
	and directly control its 3D parameters. */    
    m_pPerformance->CreateStandardAudioPath( DMUS_APATH_DYNAMIC_3D, 
		64, TRUE, &_3DMusic->m_pAudioPath );      

    // Get the IDirectSound3DBuffer8 from the 3D audiopath    
	_3DMusic->m_pAudioPath->GetObjectInPath( DMUS_PCHANNEL_ALL, DMUS_PATH_BUFFER, 0, 
                                 GUID_NULL, 0, IID_IDirectSound3DBuffer, 
								 (LPVOID*) &_3DMusic->m_pDSB );		

	return S_OK;
}

VOID DIRECTX_APP::PlayBkGnd3DMusic(MUSIC3DSTRUCT *_3DMusic)
{

    
	/* Download the segment's instruments to the synthesizer */
	_3DMusic->m_pMusicSeg->Download( m_pPerformance );

	// Tell DirectMusic to repeat this segment forever
	_3DMusic->m_pMusicSeg->SetRepeats( DMUS_SEG_REPEAT_INFINITE );

    /* Play segment on the Default audio path */
    m_pPerformance->PlaySegmentEx( _3DMusic->m_pMusicSeg, NULL, NULL, 0, 
                                   0, NULL, NULL, _3DMusic->m_pAudioPath );
	return;
}

VOID DIRECTX_APP::PlayForeGnd3DMusic( MUSIC3DSTRUCT *_3DMusic  )
{     
	if(m_SoundMode == OFF)  
		  return;
	_3DMusic->m_pMusicSeg->Download( m_pPerformance );  

	GetCursorPos(&m_CursorPoint);
	/* Set the position of sound according to MouseCursor */
    _3DMusic->m_pDSB->SetPosition( (m_CursorPoint.x-400)/800.0f, (m_CursorPoint.y-300)/600.0f, 0.0f, DS3D_IMMEDIATE );

	// Play segment on the default audio path	
	m_pPerformance->PlaySegmentEx( _3DMusic->m_pMusicSeg , NULL, NULL,DMUS_SEGF_QUEUE |DMUS_SEGF_SECONDARY, 
                                   0, NULL, NULL, _3DMusic->m_pAudioPath );
	return;	
}

HRESULT DIRECTX_APP::LoadForeGnd3DMusic(WCHAR* Filename,MUSIC3DSTRUCT *_3DMusic)
{
	/* Turn off every Music */
	if( m_SoundMode == OFF )
		 return FALSE;

	if( FAILED( m_pLoader->LoadObjectFromFile( CLSID_DirectMusicSegment,
                                               IID_IDirectMusicSegment8,
                                               Filename,
	                                               (LPVOID*) &_3DMusic->m_pMusicSeg   ) ) 
												   )
    {
        MessageBox( NULL, "Media not found, sample will now quit", 
                          "Game", MB_OK );
        m_pPerformance->CloseDown();
        m_pLoader->Release(); 
        m_pPerformance->Release();
        return false;
    }    
  	
	m_pPerformance->CreateStandardAudioPath( DMUS_APATH_DYNAMIC_3D, 
                                             64, TRUE, &_3DMusic->m_pAudioPath );
  
	// Get the IDirectSound3DBuffer8 from the 3D audiopath    
    _3DMusic->m_pAudioPath->GetObjectInPath( DMUS_PCHANNEL_ALL, DMUS_PATH_BUFFER, 0, 
                                 GUID_NULL, 0, IID_IDirectSound3DBuffer, 
                                 (LPVOID*) &_3DMusic->m_pDSB );	

	return true;	
}


//UI  and Input
RECT* DIRECTX_APP::pRectangle(int left,int top,int right,int bottom)
{
	m_rDest.left=left;
	m_rDest.top=top;
	m_rDest.right=right;
	m_rDest.bottom=bottom;
	return &m_rDest;
}

VOID DIRECTX_APP::DrawDigit(long Value, int StartX, int StartY,int Pad)
{
  RECT Rect;
  Rect.top    = 0;
  Rect.bottom = 26;    

   /* Limit the value within limits */
  long MaxValue = (long)pow(10,Pad)-1;
  if( Value >= MaxValue )
	  Value = MaxValue;
  
  long Divisor = (long)pow(10,Pad-1);
  
  for(long Digit; Divisor != 0;  )  
   {
		Digit = (Value/Divisor);
		Rect.left   = Digit*16;
		Rect.right  = Rect.left+16; 
		m_lpDDSBack->BltFast(StartX,StartY,m_lpDigit_Surf, &Rect, DDBLTFAST_SRCCOLORKEY | DDBLTFAST_WAIT  ); 
		StartX += 18;  
		Value %= Divisor;
		Divisor/= 10;		
   }	
}


int DIRECTX_APP::IsKeyDown(int key)
{
	if(GetAsyncKeyState(key)) 
	   {
		   GetAsyncKeyState(key); 
		   return TRUE;
	   }
	else
		return FALSE;
}

//Animated sprites 
VOID DIRECTX_APP::SetSprite(SPRITE * Sprite,int Id, int nFrames,int MaxFrameDelay,int Width, int Height )
{
	Sprite->Id = Id;
	Sprite->nFrames = nFrames;
	Sprite->iCurFrame = 0;
	Sprite->MaxFrameDelay = MaxFrameDelay;
	Sprite->CurFrameDelay = 0;
	Sprite->Width = Width;
	Sprite->Height= Height;     
}

VOID DIRECTX_APP::DrawSprite( SPRITE *Sprite,int PosX,int PosY,int FrameNo)
{
  RECT Rect;
  gbFrameComplete=FALSE;
  
  if( (++Sprite->CurFrameDelay) > (Sprite->MaxFrameDelay))
  {
     Sprite->CurFrameDelay = 0;
	 Sprite->iCurFrame = ++Sprite->iCurFrame;
	 if( Sprite->iCurFrame >= Sprite->nFrames)
	 {
       Sprite->iCurFrame=0; 
	   gbFrameComplete=TRUE;
  	 }
  }
  if(FrameNo == INVALID)
	  Rect.left   = Sprite->iCurFrame * Sprite->Width;
  else 
	  Rect.left   = FrameNo * Sprite->Width;

  Rect.right  = Rect.left + Sprite->Width;
  Rect.top    = 0;
  Rect.bottom = Sprite->Height;

  if( PosX < 0 )
  {
	  Rect.left -= PosX;
      PosX = 0;
  }
  else if(PosX + Sprite->Width > 800)
	  Rect.right -= (PosX+Sprite->Width-800);

  if( PosY < 0 )
  {
	  Rect.top -= PosY;
      PosY = 0;
  }
  else if(PosY + Sprite->Height > 600)
	  Rect.bottom -= (PosY+Sprite->Height-600);


  switch(Sprite->Id)
  {
	case MOUSEPTR :m_lpDDSBack->BltFast(PosX,PosY,m_lpMousePtr_Surf,&Rect,DDBLTFAST_SRCCOLORKEY | DDBLTFAST_WAIT  ); 
					break;  	

	case SWITCH   :m_lpDDSBack->BltFast(PosX,PosY,m_lpSwitch_Surf,&Rect,DDBLTFAST_SRCCOLORKEY | DDBLTFAST_WAIT  );   
		           break;  
 }
}

//Clean up
void DIRECTX_APP::DestroyDirectDraw()
{
    SAFE_RELEASE(m_lpTitle_Surf );
	SAFE_RELEASE(m_lpGameOver_Surf);
	SAFE_RELEASE(m_lpSuccess_Surf);
	SAFE_RELEASE(m_lpEiefel_Surf);
	SAFE_RELEASE(m_lpTime_Surf);
	SAFE_RELEASE(m_lpStart_Surf);
    SAFE_RELEASE(m_lpHelp_Surf );
	SAFE_RELEASE(m_lpMusic_Surf);
    SAFE_RELEASE(m_lpSound_Surf);
	SAFE_RELEASE(m_lpExit_Surf);
	SAFE_RELEASE(m_lpHelpPage_Surf);
    SAFE_RELEASE(m_lpBack_Surf);
	SAFE_RELEASE(m_lpMousePtr_Surf);
    SAFE_RELEASE(m_lpSwitch_Surf);
	SAFE_RELEASE(m_lpDigit_Surf);
	SAFE_RELEASE(m_lpBackGnd_Surf);
	SAFE_RELEASE(anchor_surf);

	    


	SAFE_RELEASE(m_lpDDSBack);
	SAFE_RELEASE(m_lpDDSPrimary);
	SAFE_RELEASE(m_lpDD);
}	  

VOID DIRECTX_APP::Destroy_3DMusic(MUSIC3DSTRUCT *_3DMusic)
{
	if(_3DMusic->m_pDSB)_3DMusic->m_pDSB->Release();
    if(_3DMusic->m_pAudioPath)_3DMusic->m_pAudioPath->Release();
	if(_3DMusic->m_pMusicSeg)_3DMusic->m_pMusicSeg->Release();   
}
    
VOID DIRECTX_APP::DestroyDirectMusic()
{
  	/* Stop the music, and close down  */
	if(m_pPerformance)m_pPerformance->Stop( NULL, NULL, 0, 0 );

	Destroy_3DMusic(&m_BkGndMusic);
	Destroy_3DMusic(&m_ButtonMusic);
	Destroy_3DMusic(&m_MouseOverMusic);
	Destroy_3DMusic(&m_WonMusic);

    // Close down DirectMusic after releasing the DirectSound buffers
    if(m_pPerformance)m_pPerformance->CloseDown();
    if(m_pPerformance)m_pPerformance->Release();
	if(m_pLoader)m_pLoader->Release(); 
}

VOID DIRECTX_APP::DestroyApp()
{
	DestroyDirectMusic();
	DestroyDirectDraw();
}


VOID DIRECTX_APP::HandleIfButtonPressed(LPDIRECTDRAWSURFACE7 lpButton_Surf, int SurfId, int Left, int Top, int Right, int Bottom )
{
	RECT Rect;  
	static PrevSurfId = NIL;  
	bool  MouseOverButt = FALSE; 
	if((m_CursorPoint.x > Left && (m_CursorPoint.x  < Right||((SurfId == SOUND || SurfId == MUSIC)
		&& (m_CursorPoint.x  < Right+40)) )
		&& m_CursorPoint.y > Top && m_CursorPoint.y < Bottom ) 	   
		)
	{
		MouseOverButt = TRUE;
		if(PrevSurfId != SurfId) 
		{
			PlayForeGnd3DMusic(&m_MouseOverMusic);		
			PrevSurfId = SurfId;
		}

		if( m_MouseClicked ) 
		{
		  PlayForeGnd3DMusic(&m_ButtonMusic);		
		  
		  //Menu Button Pressed 
		  switch( SurfId )
		  {
			  case  START:
				  LevelInit();
				 m_dwAppState = SHOW_JIGSAW_BITMAP;
				  //m_bRunning = FALSE;
				  break;
			  case  SOUND :
				   m_SoundMode = (m_SoundMode  == ON) ? OFF: ON;									
				   break;
			  case  MUSIC:
				  if( m_MusicMode == ON )
				  {
					  m_MusicMode = OFF ;
					  // Stop the music, and close RIGHT   
					  m_pPerformance->Stop( NULL, NULL, 0, 0 ); 
				  }
         	      else 
				  {
				   m_MusicMode = ON ;
				   PlayBkGnd3DMusic(&m_BkGndMusic);
				  }
				  break;

			  case  EXIT:       
				   m_bRunning = FALSE;
 				   break;

			  case HELP:
				   m_dwAppState = HELP; 
			       break;

			  case BACK:       
				   m_dwAppState = GAME_MENU;
				   break;  
		  }		  
	  }
  }
  else if(PrevSurfId == SurfId)
      PrevSurfId = NIL;

  Rect.top = 0;
  Rect.bottom = Bottom - Top ;
  Rect.left = ( MouseOverButt )? (Right-Left):0;
  Rect.right = Rect.left + (Right-Left);    

  m_lpDDSBack->BltFast( Left,Top,lpButton_Surf,&Rect, DDBLTFAST_SRCCOLORKEY | DDBLTFAST_WAIT );     
 
  if( SurfId == MUSIC || SurfId == SOUND )
  {
	Rect.top = 0;
	Rect.bottom = 24;

	if( SurfId == MUSIC )
		Rect.left  = (m_MusicMode == OFF)?  0 : 24; 
	else
		Rect.left  = (m_SoundMode == OFF)?  0 : 24; 
	Rect.right = Rect.left + 24;
	m_lpDDSBack->BltFast(SWITCHX,Top+10,m_lpSwitch_Surf,&Rect,DDBLTFAST_SRCCOLORKEY | DDBLTFAST_WAIT );     
  }
}

VOID DIRECTX_APP::HelpPage()
{
	m_lpDDSBack->BltFast(0,0,m_lpBackGnd_Surf,NULL, DDBLTFAST_WAIT); 
	m_lpDDSBack->BltFast((800-390)/2,(600-276)/2,m_lpHelpPage_Surf,NULL,DDBLTFAST_SRCCOLORKEY|DDBLTFAST_WAIT); 
	GetCursorPos(&m_CursorPoint);
	HandleIfButtonPressed(m_lpBack_Surf,BACK, 353, 527, 353+94,  527+39);	
	m_MouseClicked = FALSE; /* Clear all previous MouseClicks */	
	DrawSprite(&m_MousePtrSpr,m_CursorPoint.x,m_CursorPoint.y,INVALID);     
	m_lpDDSPrimary->Flip(0,DDFLIP_WAIT);
}

void DIRECTX_APP::DrawMenu()
{      
 /* Draw the BackGnd Image */
  m_lpDDSBack->BltFast(0,0,m_lpBackGnd_Surf,NULL,DDBLTFAST_WAIT);
  m_lpDDSBack->BltFast((800-330)/2,80,m_lpTitle_Surf,NULL,DDBLTFAST_SRCCOLORKEY | DDBLTFAST_WAIT  ); 
  /* Draws the Animated Cursor Sprite */
  GetCursorPos(&m_CursorPoint); 	

   /* Handle the menu button press */
   HandleIfButtonPressed(m_lpStart_Surf,START,          314,  160, 314+158,  160+37);
   HandleIfButtonPressed(m_lpHelp_Surf,HELP,            344,  221, 344+102,  221+41);
   HandleIfButtonPressed(m_lpMusic_Surf,MUSIC,          328,  293, 328+118,  293+39);
   HandleIfButtonPressed(m_lpSound_Surf,SOUND,          326,  360, 326+122,  360+39);
   HandleIfButtonPressed(m_lpExit_Surf,EXIT,            299,  430, 299+166,  430+39);
  	  	
   m_MouseClicked = FALSE; /* Clear all previous MouseClicks */
   DrawSprite(&m_MousePtrSpr,m_CursorPoint.x,m_CursorPoint.y,INVALID);    
   m_lpDDSPrimary->Flip(0,DDFLIP_WAIT); 	  
}

void DIRECTX_APP::HandleInput()
{
	 if(m_KeyboardPollTimer > 0)
	 {
		 --m_KeyboardPollTimer;
		 return;
	 }
	 if( IsKeyDown(VK_F2) )
     {
		 m_KeyboardPollTimer = KEYBOARD_POLLTICKS;
		 JigsawSpriteList.CreateJigsawSpriteList();
	 }
}

VOID DIRECTX_APP::GameLoop()
{
	static int count=0;
	switch(m_dwAppState)
	{
	case GAME_MENU:
		DrawMenu();
		break;

	case SHOW_JIGSAW_BITMAP:
		m_lpDDSBack->BltFast(0,0,m_lpBackGnd_Surf,NULL,DDBLTFAST_WAIT  ); 
		m_lpDDSBack->BltFast(JIGSAW_STARTX,JIGSAW_STARTY,m_lpEiefel_Surf,NULL,DDBLTFAST_WAIT  ); 
		m_lpDDSPrimary->Flip(0,DDFLIP_WAIT);
		
		Sleep(2000);
		m_MouseClicked = FALSE; 
		m_dwAppState=GAME_CORE;
		break;


    case GAME_CORE:
		DrawWorld();
		HandleInput();
		m_GameResult=CheckIfGameOver();
		

		if(m_GameResult == WON)
			m_dwAppState=SUCCESS;
		break;

   case SUCCESS:
		m_lpDDSBack->BltFast(0,0,m_lpBackGnd_Surf,NULL,DDBLTFAST_WAIT  ); 
		m_lpDDSBack->BltFast(JIGSAW_STARTX,JIGSAW_STARTY,m_lpEiefel_Surf,NULL,DDBLTFAST_WAIT  ); 
		m_lpDDSBack->BltFast((800-567)/2,(600-67)/2,m_lpSuccess_Surf,NULL,DDBLTFAST_SRCCOLORKEY | DDBLTFAST_WAIT  ); 
		m_lpDDSPrimary->Flip(0,DDFLIP_WAIT);
		
		Sleep(5000);
		m_MouseClicked = FALSE; 
		JigsawSpriteList.CreateJigsawSpriteList();
		m_dwAppState=GAME_MENU;

		break;

   case GAMEOVER:
		m_dwAppState=GAME_MENU;
		m_MouseClicked = FALSE; 
		break;

   case HELP:
		HelpPage();
		break;
	}
}
 
void DIRECTX_APP::DrawWorld()
{
	int xpos, ypos, layerIndex, i, j,k; 
	RECT rct;
	//draw 
	//////////////////////////////////////////////////////////////////////////////
	m_lpDDSBack->BltFast(0,0,m_lpBackGnd_Surf,NULL,DDBLTFAST_WAIT);

	JigsawSpriteList.DrawJigsawSprites();
	for(i=0;i<6;i++)
		for(j=0;j<6;j++)
		{
			k=i*6+j;
			g_tempXYPoint = JigsawSpriteList.m_JigsawSpriteObj[k].m_AnchorPoint;//.GetAnchorPoint(i,j);
			m_lpDDSBack->BltFast(g_tempXYPoint.x-4, g_tempXYPoint.y-4, anchor_surf,NULL,DDBLTFAST_WAIT | DDBLTFAST_SRCCOLORKEY ); 
		}

	
	for(i=0;i<36;i++)
	{
		xpos = g_SprDisplayParameters[i][0];
		ypos = g_SprDisplayParameters[i][1];
		rct.left   =  g_SprDisplayParameters[i][2];
		rct.right  =  g_SprDisplayParameters[i][3];
		rct.top    =  g_SprDisplayParameters[i][4];
		rct.bottom =  g_SprDisplayParameters[i][5];
		//layerIndex = g_BlitingOrder[i];
        m_lpDDSBack->BltFast(xpos,ypos,m_lpJigsawSurf[i],&rct,DDBLTFAST_SRCCOLORKEY | DDBLTFAST_WAIT  ); 
	}

	//i=JIGSAWSpriteList::GetCurSelection();
	//xpos = g_SprDisplayParameters[i][0];
	//ypos = g_SprDisplayParameters[i][1];
	//rct.left   =  g_SprDisplayParameters[i][2];
	//rct.right  =  g_SprDisplayParameters[i][3];
	//rct.top    =  g_SprDisplayParameters[i][4];
	//rct.bottom =  g_SprDisplayParameters[i][5];
	//m_lpDDSBack->BltFast(xpos,ypos,m_lpJigsawSurf[i],&rct,DDBLTFAST_SRCCOLORKEY | DDBLTFAST_WAIT  ); 

	/////////////////////////////////////////////////////////////////////////////
	//sprintf(m_ErrStr,"ON:%d",JIGSAWSpriteList::m_sCurSelection);
	//BltToDC(400,460);

	//sprintf(m_ErrStr,"curx(node1):%d",JigsawSpriteList.m_JigsawSpriteObj[0].m_CurrentPos.x);
	//BltToDC(400,480);

	//sprintf(m_ErrStr,"cury(node1):%d",JigsawSpriteList.m_JigsawSpriteObj[0].m_CurrentPos.y);
	//BltToDC(400,500);

	//sprintf(m_ErrStr,"curx(node2):%d",JigsawSpriteList.m_JigsawSpriteObj[1].m_CurrentPos.x);
	//BltToDC(400,520);

	//sprintf(m_ErrStr,"cury(node2):%d",JigsawSpriteList.m_JigsawSpriteObj[1].m_CurrentPos.y);
	//BltToDC(400,540);

	//JigsawSpriteList.check_links();

	///*sprintf(m_ErrStr,"DragX:%d",g_MouseDragVector.x);
	//BltToDC(400,400);*/

	/*GetCursorPos(&m_CursorPoint);
	g_tempXYPoint.x = (int) m_CursorPoint.x;
	g_tempXYPoint.y = (int) m_CursorPoint.y;

	sprintf(m_ErrStr,"mouseX:%d",g_tempXYPoint.x);
	BltToDC(50,420);
	sprintf(m_ErrStr,"mouseY:%d",g_tempXYPoint.y);
	BltToDC(50,440);*/

	////////////////////


	//////////////////
	
	GetCursorPos(&m_CursorPoint); 	

    DrawSprite(&m_MousePtrSpr,m_CursorPoint.x,m_CursorPoint.y,INVALID);  
	m_lpDDSPrimary->Flip(0,DDFLIP_WAIT);
}


GameResult DIRECTX_APP::CheckIfGameOver()
{
	m_GameResult= NO_RESULT;
	if(JigsawSpriteList.CheckIfAllTilesAreInPlace())
		m_GameResult= WON;
	return m_GameResult;
}

VOID DIRECTX_APP::LoadLevel(int index)
{
///dd
}

VOID DIRECTX_APP::LevelInit()
{

//dd
}

VOID DIRECTX_APP::BltToDC(int xpos, int ypos)
{
  m_lpDDSBack->GetDC(&dc);
  g_OldTextColor = SetTextColor(dc,RGB(0,0,255));
  g_OldBGColor   = SetTextColor(dc,RGB(255,255,255));
  SetBkMode(dc,TRANSPARENT);
  TextOut(dc,xpos,ypos,m_ErrStr,(int)strlen(m_ErrStr));
  SetBkMode(dc,OPAQUE);
  m_lpDDSBack->ReleaseDC(dc); 
}
