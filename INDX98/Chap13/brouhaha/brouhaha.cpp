#include "brouhaha.h"

LPDIRECTDRAW            lpDD;          
LPDIRECTDRAWSURFACE     lpDDSPrimary;  
LPDIRECTDRAWSURFACE     lpDDSBack;      
LPDIRECTDRAWSURFACE		lpDDSOverlay;
LPDIRECTDRAWPALETTE		lpDDPalette;
LPDIRECTDRAWCLIPPER		lpDDClipper;

LPDIRECTDRAWSURFACE		lpDDSShips;
LPDIRECTDRAWSURFACE		lpDDSShots;
LPDIRECTDRAWSURFACE		lpDDSGhost;

HINSTANCE               g_hInstance;
HWND                	g_hwnd;
PLAYERINFO 				g_Players[4];
BOOL                    g_bActive;       
BOOL					g_bFullScreen = FALSE;
BOOL					g_bAllowWindowed = TRUE;
BOOL                    g_bReInitialize = FALSE;
DWORD					g_dwRenderSetup;
BYTE					g_byInput;
BYTE					g_byLastInput;
RECT					g_rcWindow;

SPRITESET				g_shipsprite;
SPRITESET				g_shotsprite;
SPRITESET				g_ghostsprite;

char*	g_szShipBitmap = "SHIPS";
char*	g_szShotBitmap = "SHOTS";
char*	g_szGhostBitmap = "GHOST";

LRESULT WINAPI WindowProc( HWND hWnd, UINT message, 
                            WPARAM wParam, LPARAM lParam )
{
    switch( message )
    {
    case WM_ACTIVATEAPP:
		// If we wanted to pause the application when it
        // became inactive, we could do so here. In this case,
		// we decided to make it active at all times. When it is
		// minimized to the task bar, it just updates position.
		OutputDebugString( "Active App!\n ");
        break;

    case WM_CREATE:
        break;

	case WM_SIZE:
		// Our window size is fixed, so this could
        // only be a minimize or maximize
        if ( wParam == SIZE_MINIMIZED ) {
            // We've been minimized, no need to
            // redraw the screen.
            InvalidateRect( hWnd, NULL, TRUE );
            g_bActive = FALSE;
        }
        else
        {
            g_bActive = TRUE;
        }
        return 0;
        break;

    case WM_MOVE:
		// get the client rectangle
        if ( g_bFullScreen )
        {
            SetRect( &g_rcWindow, 0, 0, GetSystemMetrics( SM_CXSCREEN ), 
                        GetSystemMetrics( SM_CYSCREEN ) );
        }
        else
        {
            GetClientRect( hWnd, &g_rcWindow );
            ClientToScreen( hWnd, (LPPOINT)&g_rcWindow );
            ClientToScreen( hWnd, (LPPOINT)&g_rcWindow+1 );
        }
		break;

	case WM_PALETTECHANGED:
		// First check to see if we caused this message
		if ( (HWND)wParam != hWnd ) {
			// We didn't cause it, so we have lost the palette.
			OutputDebugString( "Palette lost.\n" );

			// Realize our palette
			lpDDSPrimary->SetPalette( lpDDPalette );
			
			// convert the sprite images to the new palette
			RestoreSprite( &g_shipsprite, g_szShipBitmap );
			RestoreSprite( &g_shotsprite, g_szShotBitmap );
			RestoreSprite( &g_ghostsprite, g_szGhostBitmap );
		}
		break;

	case WM_QUERYNEWPALETTE:
		// Ignore this message if we're transitioning -- we may
		// not yet have created the the surface and palette.
		if ( !lpDDSPrimary || !lpDDPalette )
		{
			OutputDebugString( "Ignoring palette message.\n" );
			return TRUE;
		}
		// We have control of the palette.
		OutputDebugString( "We have the palette.\n" );
		lpDDSPrimary->SetPalette( lpDDPalette );

		// convert the sprite images to the new palette
		RestoreSprite( &g_shipsprite, g_szShipBitmap );
		RestoreSprite( &g_shotsprite, g_szShotBitmap );
		RestoreSprite( &g_ghostsprite, g_szGhostBitmap );
		break;

    case WM_SETCURSOR:
        if ( g_bFullScreen ) SetCursor( NULL );
        return FALSE;
 
	// For the sake of simplicity, we're not using DirectInput but are
	// reading the keyboard through the message queue. You will see the
	// detrimental effect this has on the frame rate. DirectInput is a
	// project for the reader.
    case WM_KEYDOWN:
        switch( wParam )
        {
            case VK_LEFT:
            case VK_NUMPAD4:
                g_byInput |= KEY_LEFT;
                break;
            case VK_RIGHT:
            case VK_NUMPAD6:
                g_byInput |= KEY_RIGHT;
                break;
		    case VK_UP:
		    case VK_NUMPAD5:
			    g_byInput |= KEY_THRUST;
			    break;
            case VK_ESCAPE:
                PostMessage( hWnd, WM_CLOSE, 0, 0 );
                return 0;
		    case VK_SPACE:
                g_byInput |= KEY_FIRE;
                break;
        }
        break;
 
    case WM_KEYUP:
        switch( wParam )
        {
            case VK_LEFT:
            case VK_NUMPAD4:
                g_byInput &= ~KEY_LEFT;
                break;
            case VK_RIGHT:
            case VK_NUMPAD6:
                g_byInput &= ~KEY_RIGHT;
                break;
            case VK_UP:
            case VK_NUMPAD5:
		        g_byInput &= ~KEY_THRUST;
		        break;
	        case VK_SPACE:
                g_byInput &= ~KEY_FIRE;
                break;
        }
        break;

    case WM_PAINT:
        // We are redrawing every frame so we don't need
        // to process this message. If we were using a
        // pause screen, we could paint it here.
        break;

    case WM_SYSKEYUP:
        switch( wParam )
        {
            // handle ALT+ENTER ( fullscreen/windowed switch )
            case VK_RETURN:
                OutputDebugString( "Alt enter...\n" );

				// If windowed configs aren't allowed, get out
				if ( !g_bAllowWindowed ) break;

                g_bReInitialize = TRUE;
                g_bFullScreen = !g_bFullScreen;
                
                // Destroy the window and DirectDraw objects
                CleanUp();
                
                // Create a new DirectDraw configuration
                if FAILED( DDInit() )
                {
                    OutputDebugString( "Reinitialization failed.\n" );
                }

                ShowWindow( g_hwnd, SW_SHOW );
                g_bReInitialize = FALSE;
                break;
        }
        break;

    case WM_DESTROY:
        if ( !g_bReInitialize )
        {
            PostQuitMessage( 0 );
        }
        return 0;
    }

    return DefWindowProc(hWnd, message, wParam, lParam);
}

BOOL DDInit( void )
{
    // Use globals to initialize DD setup

	if ( GetSystemMetrics( SM_CXSCREEN ) == 640 )
	{
		// The desktop is already in 640x480. We can't use
		// windowed mode because there's not enough room for
		// our 640x480 playing field and the window.
		g_bAllowWindowed = FALSE;
		g_bFullScreen = TRUE;
	}

	if ( g_bFullScreen )
	{
		g_hwnd = CreateFullScreenWindow( g_hInstance, WindowProc );
	}
	else
	{
		g_hwnd = CreateDesktopWindow( g_hInstance, WindowProc,
										640, 480 );
	}

	if ( g_hwnd == NULL ) 
		return FALSE;

	if FAILED( DDStartup( &lpDD, NULL, g_hwnd, g_bFullScreen ) )
	{
        OutputDebugString( "DDStartup failed.\n" );
        return FALSE;
    }

    if ( g_bFullScreen )
    {
	    if FAILED( DDFullConfigure( lpDD, &lpDDSPrimary, &lpDDSBack ) )
	    {
            return FALSE;
        }
		else
		{
			g_dwRenderSetup = MODE_FULL;
		}
    }
    else
    {
        if FAILED( DDWinConfigure( lpDD, &lpDDSPrimary, &lpDDSBack,
                                        &lpDDClipper, &lpDDSOverlay,
										g_hwnd ) )
        {
            return FALSE;
        }
		else
		{
			if ( lpDDSOverlay )
			{
				g_dwRenderSetup = MODE_OVERLAY;
			}
			else
			{
				g_dwRenderSetup = MODE_WINDOWED;
			}
		}
    }
    return TRUE;
}

static BOOL doInit( HINSTANCE hInstance, int nCmdShow )
{
    g_hInstance = hInstance;

	// Attempt to initialize DirectDraw
    if FAILED( DDInit() ) 
	{
        MessageBox( NULL, "DirectDraw Init FAILED", "ERROR", MB_OK );
    	CleanUp();
		return FALSE;
	}

 	if( !GameInit() ) 
		return FALSE;

	ShowWindow( g_hwnd, SW_SHOW );
          
    return TRUE;
}

int WINAPI WinMain( HINSTANCE hInstance, HINSTANCE hPrevInstance,
                        LPSTR lpCmdLine, int nCmdShow)
{
    MSG         msg;

    // We'll use CoCreateInstance in this application

    CoInitialize( NULL );

    if( !doInit( hInstance, nCmdShow ) )
    {
       	return FALSE;
    }

	g_dwFrameTime = timeGetTime();

	while( 1 )
	{
    	if( PeekMessage( &msg, NULL, 0, 0, PM_NOREMOVE ) )
    	{
        	if( !GetMessage( &msg, NULL, 0, 0 ) )
        	{
            	return (int)msg.wParam;
        	}
        	TranslateMessage(&msg);
        	DispatchMessage(&msg);
    	}
		else
		{	
			// We're active all the time, even partially active when
			// the window is minimized. This is wasteful of CPU but
			// makes it easier to demonstrate clipping and handle
			// multiple players.
			if FAILED( UpdateFrame( g_bActive ) )
			{
				// Inside UpdateFrame, we clear the backbuffer, draw
				// our sprites, then perform a flip. Any one of these
				// operations could encounter lost surfaces. You can
				// test your application's lost surface recovery using
				// the DDTest SDK sample.
				AttemptRestore();
			}
		}
	}
	CoUninitialize();
	CloseLinkedList();
}
