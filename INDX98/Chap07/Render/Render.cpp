#define NAME "Render"
#define TITLE "DirectX Rendering"

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <windowsx.h>
#include <ddraw.h>

LPDIRECTDRAW            lpDD;           // DirectDraw object
LPDIRECTDRAWSURFACE     lpDDSPrimary;   // DirectDraw primary surface
HBRUSH hBrushGreen;                     
HBRUSH hBrushRed;                   
HENHMETAFILE hMetaFile ;            
RECT rectMeta= {200, 250, 500, 475};

static void ReleaseObjects( void )
{
    DeleteObject( hBrushGreen );
    DeleteObject( hBrushRed );
    DeleteEnhMetaFile( hMetaFile );

    if ( lpDD != NULL )
    {
        if ( lpDDSPrimary != NULL )
        {
            lpDDSPrimary->Release();
            lpDDSPrimary = NULL;
        }
        lpDD->Release();
        lpDD = NULL;
    }
}

BOOL Fail( HWND hwnd, char *szMsg)
{
    ReleaseObjects();
    OutputDebugString( szMsg );
    DestroyWindow( hwnd );
    return FALSE;
}

long FAR PASCAL WindowProc( HWND hWnd, UINT message, 
                            WPARAM wParam, LPARAM lParam )
{
    BOOL bActive;

    switch ( message )
    {
        case WM_ACTIVATEAPP:
            bActive = wParam;
            break;

        case WM_CREATE:
            hBrushGreen = CreateHatchBrush( HS_CROSS, RGB( 0, 255, 0 ) );
            hBrushRed = CreateSolidBrush( RGB( 255, 0, 0 ) );
            hMetaFile = GetEnhMetaFile( (LPSTR)"directx.emf" );
            break;

        case WM_SETCURSOR:
            SetCursor( NULL );    // Turn off the mouse cursor
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

static BOOL doInit( HINSTANCE hInstance, int nCmdShow )
{
    HWND                hwnd;
    WNDCLASS            wc;
    DDSURFACEDESC       ddsd;
    HDC                 hdc;
    BYTE                *dest;
    int x, y;
    char szMsg[] = "Some text drawn using GDI's TextOut: Press 'Esc' to Exit";

    /*
     * Set up and register window class
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
     * Create a window
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

    /*
     * Create the DirectDraw object
     */
    if FAILED( DirectDrawCreate( NULL, &lpDD, NULL ) ) 
	{
        return Fail( hwnd, "Couldn't create DirectDraw object.\n" );
    }

    // Get exclusive mode
    if FAILED( lpDD->SetCooperativeLevel( hwnd,
                                DDSCL_EXCLUSIVE | DDSCL_FULLSCREEN ) )
	{
        return Fail( hwnd, "Couldn't set cooperative level.\n" );
    }

    // Set the display mode to 640x480x8, palettized
    if FAILED( lpDD->SetDisplayMode( 640, 480, 8 ) )
	{
        return Fail( hwnd, "Couldn't set display mode.\n" );
    }

    // Create the primary surface
    ddsd.dwSize = sizeof( ddsd );
    ddsd.dwFlags = DDSD_CAPS;
    ddsd.ddsCaps.dwCaps = DDSCAPS_PRIMARYSURFACE;
    
    if FAILED( lpDD->CreateSurface( &ddsd, &lpDDSPrimary, NULL ) )
	{
        return Fail( hwnd, "Couldn't create primary surface.\n" );
    }

    // First, clear the primary surface to white using Lock and Unlock
    // to directly access surface memory

    // Lock the primary surface
    if FAILED( lpDDSPrimary->Lock( NULL, &ddsd, DDLOCK_WAIT , NULL ) )
	{
        return Fail( hwnd, "Couldn't lock the primary surface.\n" );
    }

    // Clear it to white
    // We are assuming: 1) we are palettized, so each
    // pixel is represented by a "byte" and 2) since we are not setting
    // a new palette, entry 255 should be white.
    dest = (BYTE *)ddsd.lpSurface;
    for ( y = 0; y < ( int )ddsd.dwHeight; y++ ) 
	{
        for ( x = 0; x < (( int )ddsd.dwWidth ); x++ ) 
		{
            dest[x] = 0xFF;
        }
        // lPitch contains the pitch in *bytes*
        dest += ddsd.lPitch;
    }

    // Unlock the primary surface
    if FAILED( lpDDSPrimary->Unlock( NULL ) )
	{
        return Fail( hwnd, "Couldn't unlock the primary surface.\n" );
    }

    // Now, do some GDI Stuff

    // First, get the display context
    if FAILED( lpDDSPrimary->GetDC( &hdc ) )
	{
        return Fail( hwnd, "Couldn't get DC.\n" );
    }
    else
    {
        // Draw some text.
        SetBkColor( hdc, RGB( 0, 0, 255 ) );
        SetTextColor( hdc, RGB( 255, 255, 0 ) );
        SetTextAlign( hdc, TA_CENTER );
        TextOut( hdc, 320, 0, szMsg, lstrlen( szMsg ) );

        // Draw some shapes.
        SelectObject( hdc, hBrushRed );
        Rectangle( hdc, 200, 100, 250, 230 );
        SelectObject( hdc, hBrushGreen );
        Ellipse( hdc, 300, 125, 400, 250 );

        // Draw a metafile.
        if ( hMetaFile != NULL )
        {
            PlayEnhMetaFile( hdc, hMetaFile, &rectMeta );
        }

        // Don't ever forget this.
        lpDDSPrimary->ReleaseDC( hdc );
    }
    return TRUE;

} /* doInit */

/*
 * WinMain - initialization, message loop
 */
int PASCAL WinMain( HINSTANCE hInstance, HINSTANCE hPrevInstance,
                        LPSTR lpCmdLine, int nCmdShow)
{
    MSG         msg;

    lpCmdLine = lpCmdLine;
    hPrevInstance = hPrevInstance;

    if( !doInit( hInstance, nCmdShow ) )
    {
        return FALSE;
    }

    while ( GetMessage( &msg, NULL, 0, 0 ) )
    {
        TranslateMessage( &msg );
        DispatchMessage( &msg );
    }

    return msg.wParam;

} /* WinMain */
