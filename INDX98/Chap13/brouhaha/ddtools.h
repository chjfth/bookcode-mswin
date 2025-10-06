#ifndef DDTOOLS_H
#define DDTOOLS_H

// Prototypes

HRESULT DDStartup( LPDIRECTDRAW* , GUID FAR* , HWND, BOOL );
HRESULT DDFullConfigure( LPDIRECTDRAW, 
                            LPDIRECTDRAWSURFACE*,
                            LPDIRECTDRAWSURFACE* );
HRESULT DDCreateFlipper( LPDIRECTDRAW, LPDIRECTDRAWSURFACE*,
							LPDIRECTDRAWSURFACE*, DWORD );
HRESULT DDCreateOverFlipper( LPDIRECTDRAW,
								LPDIRECTDRAWSURFACE*,
								LPDIRECTDRAWSURFACE*,
								DWORD );
HRESULT DDCreateOverlay( LPDIRECTDRAW lpDD,
                            LPDIRECTDRAWSURFACE*,
							LPDIRECTDRAWSURFACE*,
                            LPDIRECTDRAWSURFACE* );
HRESULT DDCreateFakeFlipper( LPDIRECTDRAW,
                             LPDIRECTDRAWSURFACE*,
                             LPDIRECTDRAWSURFACE* );
HRESULT DDWinConfigure( LPDIRECTDRAW lpDD,
                        LPDIRECTDRAWSURFACE*,
                        LPDIRECTDRAWSURFACE*,
                        LPDIRECTDRAWCLIPPER*,
						LPDIRECTDRAWSURFACE*,
                        HWND );
HRESULT DDFillSurface( LPDIRECTDRAWSURFACE , DWORD );
HRESULT DDTextOut ( LPDIRECTDRAWSURFACE,
							char*, COLORREF, COLORREF, int, int );
HRESULT DDMakeOffscreenSurface( LPDIRECTDRAWSURFACE*,
										DWORD ChromaKey);
DWORD	DDCheckOverlay( GUID* );

#endif // DDTOOLS_H