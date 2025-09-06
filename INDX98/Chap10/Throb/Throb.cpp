#define NAME "Throb"
#define TITLE "Palette Animation"

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <windowsx.h>
#include <ddraw.h>

#define TIMER_ID        1
#define TIMER_RATE      25

LPDIRECTDRAW            lpDD;           // DirectDraw object.
LPDIRECTDRAWSURFACE     lpDDSPrimary;   // DirectDraw primary surface.
LPDIRECTDRAWPALETTE     lpDDPal;        // The primary surface palette.
BOOL                    bActive;        // Is application active?

static void ReleaseObjects( void )
{
    if ( lpDD != NULL )
    {
        if ( lpDDSPrimary != NULL )
        {
            lpDDSPrimary->Release();
            lpDDSPrimary = NULL;
        }
        if ( lpDDPal != NULL )
        {
            lpDDPal->Release();
            lpDDPal = NULL;
        }
        lpDD->Release();
        lpDD = NULL;
    }
}

BOOL Fail( HWND hwnd,  char *szMsg)
{
    ReleaseObjects();
    OutputDebugString( szMsg );
    DestroyWindow( hwnd );
    return FALSE;
}

LRESULT WINAPI WindowProc( HWND hWnd, UINT message, 
                            WPARAM wParam, LPARAM lParam )
{
    PALETTEENTRY        ape;

    switch ( message )
    {
        case WM_ACTIVATEAPP:
            bActive = (BOOL)wParam;
            break;

        case WM_CREATE:
            break;

        case WM_SETCURSOR:
            SetCursor(NULL);    // Turn off the mouse cursor.
            return TRUE;

        case WM_TIMER:
            if ( bActive )
            {
                // Get palette entry number 1.
                lpDDPal->GetEntries( 0, 1, 1, &ape );

                // Twiddle the red value, incrementing it
                // in steps of 4, repeatedly.
                ape.peRed = ( BYTE )( ( ape.peRed + 4 ) % 256 );

                // One way to prevent tearing, sparkling, flashing,
                // and other nasty side effects of changing the palette
                // indiscriminately.  Uncomment this line to see it work.

                // lpDD->WaitForVerticalBlank( DDWAITVB_BLOCKBEGIN, NULL );

                // Put the new values back into the palette.
                lpDDPal->SetEntries( 0, 1, 1, &ape );
            }
           
            break;
 
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
}

/*
 * doInit - Create the window, initialize data, etc.
 */
static BOOL doInit( HINSTANCE hInstance, int nCmdShow )
{
    HWND                hwnd;
    WNDCLASS            wc;
    DDSURFACEDESC       ddsd;
    PALETTEENTRY        ape[256];
    int                 i;
	DDBLTFX     ddbltfx = {sizeof(DDBLTFX)};
	HRESULT hret = 0;

    // set up and register window class
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
    
    // create a window
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

    // create the DirectDraw object
	hret = DirectDrawCreate( NULL, &lpDD, NULL );
    if ( FAILED(hret) )
	{
        return Fail( hwnd, "Couldn't create DirectDraw object.\n" );
    }

    // Get exclusive mode.
	hret = lpDD->SetCooperativeLevel( hwnd,	
		DDSCL_EXCLUSIVE | DDSCL_FULLSCREEN 
		);
    if ( FAILED(hret) )
	{
        return Fail( hwnd, "Couldn't set cooperative level.\n" );
    }


    // Set display mode to 640x480x8 palettized.
	hret = lpDD->SetDisplayMode( 640, 480, 8 );
    if ( FAILED(hret) )
	{
        return Fail( hwnd, "Couldn't set display mode.\n" );
    }

    // Create the primary surface.
    ddsd.dwSize = sizeof( ddsd );
    ddsd.dwFlags = DDSD_CAPS;
    ddsd.ddsCaps.dwCaps = DDSCAPS_PRIMARYSURFACE;
    
	hret = lpDD->CreateSurface( &ddsd, &lpDDSPrimary, NULL );
    if ( FAILED(hret) )
	{
        return Fail( hwnd, "Couldn't create primary surface.\n" );
    }

    // Setup our palette.
    // We'll leave entries 0 and 256 alone, and set
    // all the others to zero.
    for ( i = 1; i < 255; i++ )
    {
        ape[i].peRed   = ( BYTE )0;
        ape[i].peGreen = ( BYTE )0;
        ape[i].peBlue  = ( BYTE )0;
        ape[i].peFlags = ( BYTE )0;
    }

	hret = lpDD->CreatePalette( DDPCAPS_8BIT, 
		ape, &lpDDPal, NULL );
    if ( FAILED(hret) )
	{
        return Fail( hwnd, "Couldn't create palette.\n" );
    }

	hret = lpDDSPrimary->SetPalette( lpDDPal );
    if ( FAILED(hret) )
	{
        return Fail( hwnd, "Couldn't set palette.\n" );
    }

    // This is the only "painting" we will do -- from now on,
    // the color of the display will be changed by manipulating
    // palette entries, not moving bits.

    // Fill the surface with color.
    ddbltfx.dwSize = sizeof( ddbltfx );
    // Since we are palettized, we set the fill color
    // to a palette index rather than an RGB value.
    ddbltfx.dwFillColor = 1;

	hret = lpDDSPrimary->Blt(
		NULL,   // dest rect
		NULL,   // src surface
		NULL,   // src rect
		DDBLT_COLORFILL | DDBLT_WAIT,
		&ddbltfx );
    if( FAILED(hret) )
	{
        return Fail( hwnd, "Couldn't fill surface.\n" );
    }

    // Create a timer to drive animation.
    SetTimer( hwnd, TIMER_ID, TIMER_RATE, NULL );

    return TRUE;
} /* doInit */

/*
 * WinMain - initialization, message loop
 */
int WINAPI WinMain( HINSTANCE hInstance, HINSTANCE hPrevInstance,
                    LPSTR lpCmdLine, int nCmdShow )
{
    MSG         msg;

    lpCmdLine = lpCmdLine;
    hPrevInstance = hPrevInstance;

    if ( !doInit( hInstance, nCmdShow ) )
    {
        return FALSE;
    }

    while ( GetMessage( &msg, NULL, 0, 0 ) )
    {
        TranslateMessage( &msg );
        DispatchMessage( &msg );
    }

    return (int)msg.wParam;
} /* WinMain */
