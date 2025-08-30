#define NAME "Flip"
#define TITLE "Page Flipping yes"

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <windowsx.h>
#include <stdio.h>
#include <ddraw.h>
#include <mmsystem.h>

#pragma comment(lib, "winmm.lib") // for timeGetTime()

LPDIRECTDRAW            lpDD;           // DirectDraw object
LPDIRECTDRAWSURFACE     lpDDSPrimary;   // DirectDraw primary surface
LPDIRECTDRAWSURFACE     lpDDSBack;      // DirectDraw back buffer surface
BOOL                    bActive;        // Is application active

DWORD                   dwFrameTime = 0;    // Time of the last frame.
DWORD                   dwFrames = 0;       // The frame rate (frames/second).
DWORD                   dwFrameCount = 0;   // Frames displayed.

LONG                    position = 0;   // Current position of "sprite".

// BUFFERS sets the number of back buffers and should be at least 1 and not
// greater than can be accommodated by your display card's memory.
#define BUFFERS     1
// Simulate a more complex rendering cycle by increasing the value of SPRITES.
#define SPRITES     100

static void ReleaseObjects( void )
{
    if ( lpDD != NULL )
    {
        if ( lpDDSPrimary != NULL )
        {
            // Just need to release the primary surface, back buffers
            // will be released automatically.
            lpDDSPrimary->Release();
            lpDDSPrimary = NULL;
        }
        lpDD->Release();
        lpDD = NULL;
    }
}

BOOL Fail( HWND hwnd,  char *szMsg )
{
    ReleaseObjects();
    OutputDebugString( szMsg );
    DestroyWindow( hwnd );
    return FALSE;
}

LRESULT WINAPI WindowProc( HWND hWnd, UINT message, 
                            WPARAM wParam, LPARAM lParam )
{
    switch ( message )
    {
        case WM_ACTIVATEAPP:
            bActive = (BOOL)wParam;
            break;

        case WM_SETCURSOR:
            SetCursor( NULL );    // Turn off the mouse cursor.
            return TRUE;
 
        case WM_KEYDOWN:
            switch ( wParam )
            {
                case VK_ESCAPE:
                case VK_F12:
                    PostMessage( hWnd, WM_CLOSE, 0, 0 );
                    break;
            }
            break;

        case WM_DESTROY:
            ReleaseObjects();
            PostQuitMessage( 0 );
            break;
        }

    return DefWindowProc( hWnd, message, wParam, lParam );
} /* WindowProc */

/*
 * doInit - Create the window, initialize data, etc.
 */
static BOOL doInit( HINSTANCE hInstance, int nCmdShow )
{
    HWND                hwnd;
    WNDCLASS            wc;
    DDSURFACEDESC       ddsd;       // Surface description structure.
    DDSCAPS             ddscaps;    // Surface capabilities structure.

    /*
     * set up and register window class
     */
    wc.style = CS_HREDRAW | CS_VREDRAW;
    wc.lpfnWndProc = WindowProc;
    wc.cbClsExtra = 0;
    wc.cbWndExtra = 0;
    wc.hInstance = hInstance;
    wc.hIcon = LoadIcon( hInstance, IDI_APPLICATION );
    wc.hCursor = LoadCursor( NULL, IDC_ARROW );
    wc.hbrBackground = NULL;
    wc.lpszMenuName = NAME;
    wc.lpszClassName = NAME;
    RegisterClass( &wc );
    
    /*
     * create a window
     */
    hwnd = CreateWindowEx(
        WS_EX_TOPMOST,
        NAME,
        TITLE,
        WS_POPUP,
        0, 0,
        GetSystemMetrics( SM_CXSCREEN ),
        GetSystemMetrics( SM_CYSCREEN ),
        NULL,
        NULL,
        hInstance,
        NULL );

    if ( !hwnd )
    {
        return FALSE;
    }

    ShowWindow( hwnd, nCmdShow );
    UpdateWindow( hwnd );

    // Create the DirectDraw object.
    if ( FAILED( DirectDrawCreate( NULL, &lpDD, NULL ) ) )
	{
        return Fail( hwnd, "Couldn't create DirectDraw object." );
    }

    // Get exclusive mode.
    if ( FAILED( lpDD->SetCooperativeLevel( hwnd,
                    DDSCL_EXCLUSIVE | DDSCL_FULLSCREEN ) ) )
	{
        return Fail( hwnd, "Couldn't set cooperative level." );
    }

    // Set the display mode to 640 by 480 palettized.
    if ( FAILED( lpDD->SetDisplayMode( 640, 480, 8 ) ) )
	{
        return Fail( hwnd, "Couldn't set display mode." );
    }

    // Create the primary surface with back buffer(s).

    ddsd.dwSize = sizeof( ddsd );
    ddsd.dwFlags = DDSD_CAPS | DDSD_BACKBUFFERCOUNT;
    ddsd.ddsCaps.dwCaps = DDSCAPS_PRIMARYSURFACE | 
                            DDSCAPS_FLIP | 
                            DDSCAPS_COMPLEX |
                            DDSCAPS_VIDEOMEMORY ;
    ddsd.dwBackBufferCount = BUFFERS;
  
    if ( FAILED( lpDD->CreateSurface( &ddsd, &lpDDSPrimary, NULL ) ) )
	{
        return Fail( hwnd, "Couldn't create primary surface." );
    }

    // Get a pointer to the back buffer.
    ddscaps.dwCaps = DDSCAPS_BACKBUFFER;
    if ( FAILED( lpDDSPrimary->GetAttachedSurface( &ddscaps, 
		                                           &lpDDSBack ) ) )
	{
        return Fail( hwnd, "Couldn't find the back buffer.\n" );
    }
    
    // Start the frame timer.
    dwFrameTime = timeGetTime();

    return TRUE;
} /* doInit */

BOOL UpdateFrame( HWND hwnd )
{
    HDC                 hdc;
    char                str[255];
    DDBLTFX             ddbltfx;
    RECT                dest;
    DWORD               dwTime;
    int                 i;

    // When a second or more has elapsed (dwTime > 1000) since
    // the last update, update the frame rate estimate.

    dwFrameCount++;
    dwTime = timeGetTime() - dwFrameTime;
    if ( dwTime > 1000 )
    {
        dwFrames = ( dwFrameCount*1000 )/dwTime;
        dwFrameTime = timeGetTime();
        dwFrameCount = 0;
    }

    // Clear the back buffer to black using the blitter.

    ddbltfx.dwSize = sizeof( ddbltfx );
    ddbltfx.dwFillColor = 0;

    if ( FAILED( lpDDSBack->Blt( NULL, NULL, NULL,
                    DDBLT_COLORFILL | DDBLT_WAIT, &ddbltfx ) ) )
	{
        return Fail( hwnd, "Couldn't clear buffer." );
    }

    // Use the blitter to draw a white square on the surface, moving it
    // slowly from left to right.
    // This operation is performed multiple times in a loop to simulate the
    // drawing of multiple sprites.

    position += 1;
    if ( position > 540 ) 
		position = 0;

    ddbltfx.dwFillColor = 255;
    dest.left = position;
    dest.top = 190;
    dest.right = dest.left + 100;
    dest.bottom = 290;

    for ( i = 0; i < SPRITES; i++ ) 
	{
        if ( FAILED( lpDDSBack->Blt( &dest, NULL, NULL,
                        DDBLT_COLORFILL | DDBLT_WAIT, &ddbltfx ) ) )
		{
            return Fail( hwnd, "Couldn't Blt." );
        }
    }

    // Use GDI to write the current frame rate.
    if ( FAILED( lpDDSBack->GetDC( &hdc ) ) )
	{
        return Fail( hwnd, "Couldn't get DC." );
    }
    else
    {
        _snprintf( str, _TRUNCATE, "Flip Framerate: %d", dwFrames );
        SetBkColor( hdc, RGB( 0, 0, 0 ) );
        SetTextColor( hdc, RGB( 255, 255, 255 ) );
        SetTextAlign( hdc, TA_CENTER );
        TextOut( hdc, 320, 0, str, lstrlen( str ) );

        // Don't ever forget this.
        lpDDSBack->ReleaseDC( hdc );
    }

    // Finally, perform the flip.
    if ( FAILED( lpDDSPrimary->Flip( NULL, DDFLIP_WAIT ) ) )
	{
        return Fail( hwnd, "Couldn't perform flip." );
    }

    return TRUE;
}

/*
 * WinMain - initialization, message loop
 */
int PASCAL WinMain( HINSTANCE hInstance, HINSTANCE hPrevInstance,
                    LPSTR lpCmdLine, int nCmdShow)
{
    MSG         msg;

    if ( !doInit( hInstance, nCmdShow ) )
    {
        return FALSE;
    }

    while ( 1 )
    {
        if ( PeekMessage( &msg, NULL, 0, 0, PM_NOREMOVE ) )
        {
            if ( !GetMessage( &msg, NULL, 0, 0 ) )
            {
                return (int)msg.wParam;
            }
            TranslateMessage( &msg );
            DispatchMessage( &msg );
        }
        else if ( bActive )
        {   
            UpdateFrame( NULL );
        }
        else
        {
            WaitMessage();
        }
    }
} /* WinMain */
