#ifndef GAMEUTIL_H
#define GAMEUTIL_H

typedef struct
{  
    DWORD           dwStatus;
	LPNODE			lpNode;
}PLAYERINFO;
typedef PLAYERINFO* LPPLAYERINFO;

// Prototypes

void	DDRelease( void );
void	CleanUp( void );
BOOL	GameInit( void );
BOOL	CreateSprite(  LPSPRITESET, LPDIRECTDRAW lpDD,
						LPDIRECTDRAWSURFACE, int, int, int, LPCSTR,
						LPDDCOLORKEY, DWORD, LPDDPIXELFORMAT );
HRESULT RestoreSprite( LPSPRITESET, LPCSTR );
HRESULT RestoreSurfaces( void );
void	AttemptRestore( void );
BOOL	CreatePalette( LPDIRECTDRAW, LPDIRECTDRAWPALETTE,
								LPDIRECTDRAWSURFACE, LPCSTR );
BOOL	LoadGameArt( DWORD, LPDDPIXELFORMAT );
HRESULT UpdateFrame( BOOL );
void	UpdateShot( LPNODE );
LPNODE	CreateShip( double, double, double, double, int, int );
LPNODE 	CreateShot( double, double, double, double, int );
void	UpdateShip( LPNODE );
HRESULT FlipSurfaces( DWORD );
HWND	CreateDesktopWindow( HANDLE, WNDPROC, DWORD, DWORD );
HWND	CreateFullScreenWindow( HANDLE, WNDPROC );

// Frame rate variables
extern DWORD					g_dwFrameTime;
extern DWORD					g_dwFrames;
extern DWORD					g_dwFrameCount;

#endif // GAMEUTIL_H
