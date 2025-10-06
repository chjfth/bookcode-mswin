#define NAME "Brouhaha"
#define TITLE "Space Brouhaha"

#define WIN32_LEAN_AND_MEAN

#include <assert.h>
#include <tchar.h>
#include <windows.h>
#include <windowsx.h>
#include <ddraw.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <mmsystem.h>

#include "resource.h"
#include "ddutil.h"
#include "ddtools.h"
#include "utility.h"
#include "sprites.h"
#include "gameutil.h"

// Externals

extern LPDIRECTDRAW				lpDD;
extern LPDIRECTDRAWSURFACE		lpDDSPrimary;  
extern LPDIRECTDRAWSURFACE		lpDDSBack;      
extern LPDIRECTDRAWSURFACE		lpDDSOverlay;
extern LPDIRECTDRAWSURFACE		lpDDSShips;		
extern LPDIRECTDRAWSURFACE		lpDDSShots;
extern LPDIRECTDRAWSURFACE		lpDDSGhost;
extern LPDIRECTDRAWPALETTE		lpDDPalette;
extern LPDIRECTDRAWCLIPPER		lpDDClipper;

extern HINSTANCE				g_hInstance;
extern HWND						g_hwnd;
extern RECT						g_rcWindow;
extern BOOL						g_bFullScreen;
extern BOOL						g_bAllowWindowed;
extern DWORD					g_dwRenderSetup;
extern BOOL						g_bReInitialize;
extern BOOL						g_bTryOverlays;
extern BYTE						g_byInput;
extern BYTE						g_byLastInput;
extern SPRITESET				g_shipsprite;
extern SPRITESET				g_shotsprite;
extern SPRITESET				g_ghostsprite;
extern char						*g_szShipBitmap;
extern char						*g_szShotBitmap;
extern char						*g_szGhostBitmap;

extern PLAYERINFO 				g_Players[4];

// Keyboard commands

#define KEY_THRUST 0x0001 // Thrust with the "5" key
#define KEY_LEFT   0x0004 // Turn left with left arrow
#define KEY_RIGHT  0x0008 // Turn right with right arrow
#define KEY_FIRE   0x0020 // Fire with space bar

#define SYNCKEYS   0x000D

// Rendering setups

#define MODE_OVERLAY	1
#define MODE_FULL		2
#define MODE_WINDOWED	3

// Game constants

#define SCREEN_X    639
#define SCREEN_Y    479
#define SHIP_X      ( SCREEN_X - 32 )
#define SHIP_Y      ( SCREEN_Y - 32 )
#define SHOT_X      ( SCREEN_X - 3 )
#define SHOT_Y      ( SCREEN_Y - 3 )

#define FRAME_RATE		25		// 40 frames per second (ms)
#define SHOTLIFE		2000	// lifetime of shote (ms)
#define SHOTFREQ		500		// time between shots (ms)
#define DISABLEDTIME	4000	// time disabled after hit
#define INPUT_RATE		25		// 40 times per second (ms)

// Prototypes

BOOL DDInit( void );


