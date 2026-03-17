//-----------------------------------------------------------------------------
// File: RoadRage.cpp
//
// Desc: The main RoadRage code.  The CMyD3DApplication class handles
//		 most of the RoadRage functionality.
//
// Copyright (c) 1999 William Chin and Peter Kovach. All rights reserved.
//-----------------------------------------------------------------------------
#define STRICT
#define D3D_OVERLOADS
#include "resource.h"
#include <assert.h>
#include <tchar.h>
#include <math.h>
#include <time.h>
#include <stdio.h>
#include "D3DMath.h"
#include "D3DApp.h"
#include "D3DUtil.h"
#include "D3DEnum.h"
#include "RoadRage.hpp"
#include <RECTxy.h>
#include "CxxParamDialog.h"
#include "chjutils.h"
#include "verstr.h"

#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")

#define SAFE_DELETE(p)       { if(p) { delete (p);     (p)=NULL; } }
#define SAFE_DELETE_ARRAY(p) { if(p) { delete[] (p);   (p)=NULL; } }
#define SAFE_RELEASE(p)      { if(p) { (p)->Release(); (p)=NULL; } }

//-----------------------------------------------------------------------------
// Defines, constants, and global variables
//-----------------------------------------------------------------------------



#define EXITCODE_FORWARD     1  // Dialog success, so go forward
#define EXITCODE_BACKUP      2  // Dialog canceled, show previous dialog
#define EXITCODE_QUIT        3  // Dialog quit, close app
#define EXITCODE_ERROR       4  // Dialog error, terminate app


extern INT_PTR CALLBACK AppAbout(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
INT_PTR CALLBACK AboutProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM );

FLOAT        g_fCurrentTime;
extern GUID  g_AppGUID; 
extern TCHAR g_strAppName[256];

HDC hdc;

int PrintMsgX = 10;
int PrintMsgY = 10;

int total_allocated_memory_count = 0;
float angle_deg = 0.0f; // debug
float temp_f	= 0.0f; // debug

// Chap 5 >>>

BOOL logfile_start_flag = TRUE;
BOOL RR_exit_debug_flag = FALSE;
SETUPINFO setupinfo;

int num_op_guns = 0;
int num_cars;

int num_light_sources;

// Chap5_chj <<<

CMyD3DApplication* pCMyApp;

void PrintMemAllocated(int mem, char *message);
VOID DisplayError( CHAR* strMessage );


//-----------------------------------------------------------------------------
// Name: AboutProc()
// Desc: Minimal message proc function for the about box
//-----------------------------------------------------------------------------
INT_PTR CALLBACK AboutProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM )
{
    if( WM_COMMAND == uMsg )
	{
		if( IDOK == LOWORD(wParam) || IDCANCEL == LOWORD(wParam) )
			EndDialog (hWnd, TRUE);
	}

    return ( WM_INITDIALOG == uMsg ) ? TRUE : FALSE;
}


//-----------------------------------------------------------------------------
// Name: CMyD3DApplication()
// Desc: Application constructor. Sets attributes for the app.
//-----------------------------------------------------------------------------
CMyD3DApplication::CMyD3DApplication()                
{
    m_strWindowTitle  = TEXT( "Chapter 5 chj v" ) _T(VER_STR);
	m_bAppUseZBuffer  = TRUE;
    
	m_fnConfirmDevice = NULL;
	m_bShowStats = TRUE;

	pCMyApp = this;

	m_ppbox = NULL;
	m_x = m_y = m_z = 0;
	m_camX_waggle = 0;
}


//-----------------------------------------------------------------------------
// Name: Cleanup3DEnvironment()
// Desc: Cleanup scene objects
//-----------------------------------------------------------------------------
VOID CMyD3DApplication::Cleanup3DEnvironment()
{
    SetbActive(FALSE);
    SetbReady(FALSE);

    if( GetFramework() )
    {
        DeleteDeviceObjects();
        DeleteFramework();

        FinalCleanup();
    }

    D3DEnum_FreeResources();
}


LRESULT CMyD3DApplication::MsgProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	HMENU hMenu;

	m_hWnd = hWnd;
		
	hMenu = GetMenu( hWnd );

    switch( uMsg )
    {{
	case WM_CREATE:
	{
		HINSTANCE hInst = GetModuleHandle(NULL);
		m_ppbox = new ParamDialog();
		HWND hdlgbox = m_ppbox->CreateModeless(hInst, MAKEINTRESOURCE(IDD_PARAMS), hWnd);
		assert(hdlgbox);

		RECT rcmain = {};
		GetWindowRect(hWnd, &rcmain);
		int xbox = rcmain.left + RECTcx(rcmain);
		m_ppbox->Show(TRUE, xbox, rcmain.top);
		m_pParamDlgbox = m_ppbox;
		break;
	}
	case WM_ACTIVATEAPP:
		RRAppActive = (BOOL)wParam; // 1=activated , 0=deactivated
		break;

	case WM_INITMENUPOPUP:
		CheckMenuItem (hMenu, MENU_LIGHT_SOURCES, bEnableLighting ? MF_CHECKED : MF_UNCHECKED);

		if(ShadeMode == D3DSHADE_FLAT)
		{
			CheckMenuItem (hMenu, MENU_FLAT,	MF_CHECKED);
			CheckMenuItem (hMenu, MENU_GOURAUD, MF_UNCHECKED);
		}
		if(ShadeMode == D3DSHADE_GOURAUD)
		{
			CheckMenuItem (hMenu, MENU_FLAT,	MF_UNCHECKED);
			CheckMenuItem (hMenu, MENU_GOURAUD, MF_CHECKED);
		}

		break;

	case WM_COMMAND:
		switch( LOWORD(wParam) )
		{
		case MENU_ABOUT:
			Pause(TRUE);
			DialogBox(hInstApp, MAKEINTRESOURCE(IDD_ABOUT), hWnd, (DLGPROC)AppAbout);
			Pause(FALSE);
			break;

			// Shading selection		 
		case MENU_FLAT_SHADING:
			CheckMenuItem (hMenu, MENU_FLAT,	MF_CHECKED);
			CheckMenuItem (hMenu, MENU_GOURAUD, MF_UNCHECKED);
			ShadeMode = D3DSHADE_FLAT;
			m_pd3dDevice->SetRenderState( D3DRENDERSTATE_SHADEMODE,
				D3DSHADE_FLAT );
			break;

		case MENU_GOURAUD_SHADING:
			CheckMenuItem (hMenu, MENU_FLAT,	MF_UNCHECKED);
			CheckMenuItem (hMenu, MENU_GOURAUD, MF_CHECKED);
			ShadeMode = D3DSHADE_GOURAUD;
			m_pd3dDevice->SetRenderState( D3DRENDERSTATE_SHADEMODE,
				D3DSHADE_GOURAUD );
			break;


		case MENU_LIGHT_SOURCES:
			bEnableLighting = !bEnableLighting;
			CheckMenuItem (hMenu, MENU_LIGHT_SOURCES, bEnableLighting ? MF_CHECKED : MF_UNCHECKED);
			break;

		case IDM_EXIT:
//			PrintMessage(NULL, "MsgProc - IDM_EXIT", NULL, LOGFILE_ONLY);

			// Destroy modeless child first
			DestroyWindow(m_ppbox->GetHwnd());
			m_pParamDlgbox = m_ppbox = NULL;

			Cleanup3DEnvironment();
			SendMessage( hWnd, WM_CLOSE, 0, 0 );
			DestroyWindow( hWnd );

			PostQuitMessage(0);
			exit(0);

		default:
			return CD3DApplication::MsgProc( hWnd, uMsg, wParam, lParam );           
		}
		break;

	case WM_GETMINMAXINFO:
		((MINMAXINFO*)lParam)->ptMinTrackSize.x = 100;
		((MINMAXINFO*)lParam)->ptMinTrackSize.y = 100;
		break;

	case WM_CLOSE:
//		PrintMessage(NULL, "MsgProc - WM_CLOSE", NULL, LOGFILE_ONLY);
		Cleanup3DEnvironment();
		DestroyWindow( hWnd );
		PostQuitMessage(0);
		return 0;

	default:
		return CD3DApplication::MsgProc( hWnd, uMsg, wParam, lParam );
	}}

    return DefWindowProc( hWnd, uMsg, wParam, lParam );
}  


//-----------------------------------------------------------------------------
// Name: Render3DEnvironment()
// Desc: Draws the scene.
//-----------------------------------------------------------------------------
HRESULT CMyD3DApplication::Render3DEnvironment()
{
    HRESULT hr;

    // Check the cooperative level before rendering
    if( FAILED( hr = m_pDD->TestCooperativeLevel() ) )
    {
        switch( hr )
        {
            case DDERR_EXCLUSIVEMODEALREADYSET:
            case DDERR_NOEXCLUSIVEMODE:
                // Do nothing because some other app has exclusive mode
                return S_OK;

            case DDERR_WRONGMODE:
                // The display mode changed on us. Resize accordingly
                if( m_pDeviceInfo->bWindowed )
                    return Change3DEnvironment();
                break;
        }
        return hr;
    }

	// Get the relative time, in seconds
	FLOAT fTime = ( timeGetTime() - GetBaseTime() ) * 0.001f;

	// FrameMove (animate) the scene
	if( GetbFrameMoving() || GetSingleStep() )
	{
		if( FAILED( hr = FrameMove( fTime ) ) )
			return hr;

		SetSingleStep(FALSE);
	}

	// If the display is in a stereo mode, render the scene from the left eye
	// first, then the right eye.
	if( m_bAppUseStereo && m_pDeviceInfo->bStereo && !m_pDeviceInfo->bWindowed )
	{
		// Render the scene from the left eye
		m_pd3dDevice->SetTransform( D3DTRANSFORMSTATE_VIEW, &m_matLeftView );
		if( FAILED( hr = m_pd3dDevice->SetRenderTarget( m_pddsRenderTargetLeft, 0 ) ) )
			return hr;
		if( FAILED( hr = Render() ) )
			return hr;

		//Render the scene from the right eye
		m_pd3dDevice->SetTransform( D3DTRANSFORMSTATE_VIEW, &m_matRightView );
		if( FAILED( hr = m_pd3dDevice->SetRenderTarget( m_pddsRenderTarget, 0 ) ) )
			return hr;
		if( FAILED( hr = Render() ) )
			return hr;
	} 
	else 
	{
		// Set center viewing matrix if app is NOT stereo-enabled
		m_pd3dDevice->SetTransform( D3DTRANSFORMSTATE_VIEW, &m_matView );

		// Render the scene as normal
		
		hr = Render();
		
		if( FAILED(hr) )
			return hr;
	}

    // Show the frame rate, etc.
    if( m_bShowStats )
        ShowStats();

    // Show the frame on the primary surface.
	CD3DFramework7 *pf = GetFramework();
    hr = pf->ShowFrame();
    return hr;
}



//-----------------------------------------------------------------------------
// Name: OneTimeSceneInit()
// Desc: Called during initial app startup, this function performs all the
//       permanent initialization.
//-----------------------------------------------------------------------------
HRESULT CMyD3DApplication::OneTimeSceneInit()
{
	WORD i, j, ind;

	// Seed the random number generator
	srand( (unsigned int)time(0) );

	// Generate a square mesh in XZ plane from (0,0) to (1,1) for the walls
	for( i=0; i<WALL_MESH_SIZE; i++ )
	{
		for( j=0; j<WALL_MESH_SIZE; j++ )
		{
			FLOAT      x = i / (FLOAT)(WALL_MESH_SIZE-1);  // [0 .. 1]
			FLOAT      z = j / (FLOAT)(WALL_MESH_SIZE-1);  // [0 .. 1]
			D3DVERTEX* v = &m_WallVertices[i*WALL_MESH_SIZE+j];
			
			(*v) = D3DVERTEX( 
				10.0f * D3DVECTOR(x, 0.0f, z), // 3D position, // [0 .. 10]
				D3DVECTOR(0.0f, 1.0f, 0.0f),   // Normal, all straight upward
				x, z // texture U,V [0 .. 1]
				); 
		}
	}

	// Generate the wall indices
	for( i=ind=0; i<WALL_MESH_SIZE-1; i++ )
	{
		for( j=0; j<WALL_MESH_SIZE-1; j++ )
		{
			m_WallIndices[ind++] = (i+0)*WALL_MESH_SIZE + (j+0);
			m_WallIndices[ind++] = (i+0)*WALL_MESH_SIZE + (j+1);
			m_WallIndices[ind++] = (i+1)*WALL_MESH_SIZE + (j+0);

			m_WallIndices[ind++] = (i+1)*WALL_MESH_SIZE + (j+0);
			m_WallIndices[ind++] = (i+0)*WALL_MESH_SIZE + (j+1);
			m_WallIndices[ind++] = (i+1)*WALL_MESH_SIZE + (j+1);
		}
	}

	// Generate the sphere data
	FLOAT dj = g_PI / (SPHERE_MESH_SIZE_Y + 1);
	FLOAT di = g_PI*2 / SPHERE_MESH_SIZE_X;

	// Vertices 0 and 1 are the north and south poles
	m_SphereVertices[0] = D3DVERTEX( 
		D3DVECTOR(0.0f, 1.0f, 0.0f), 
		D3DVECTOR(0.0f,-1.0f, 0.0f), 
		rnd(), rnd() 
		);
	m_SphereVertices[1] = D3DVERTEX( 
		D3DVECTOR(0.0f,-1.0f, 0.0f), 
		D3DVECTOR(0.0f, 1.0f, 0.0f), 
		rnd(), rnd() 
		);

	const int IDX_NP = 0; // north-pole VTX index
	const int IDX_SP = 1; // south-pole VTX index

	for( j=0; j<SPHERE_MESH_SIZE_Y; j++ )
	{
		for( i=0; i<SPHERE_MESH_SIZE_X; i++ ) 
		{
			D3DVECTOR   p;

			// j = latitude
			FLOAT radj = (j+1) * dj; // 1pi/5, 2pi/5, 3pi/5, 4pi/5

			// i = longitude
			FLOAT radi = i *di; // 0pi/8, 1pi/8, 2pi/8 ... 6ip/8, 7pi/8

			p.y = (FLOAT)cos(radj);  
			p.x = (FLOAT)( sin(radj) * sin(radi));
			p.z = (FLOAT)( sin(radj) * cos(radi) );

			m_SphereVertices[NPSP_2 + j*SPHERE_MESH_SIZE_X+i] = 
				D3DVERTEX(p, -p, rnd(), rnd());
		}
	}

	// Now generate the triangle indices. Strip around north pole first
	for( i=0; i<SPHERE_MESH_SIZE_X; i++ )
	{
		// Chj: each triangle involves 3 vertices(=costs 3 indices)
		m_SphereIndices[3*i+0] = 0;
		m_SphereIndices[3*i+1] = i+ NPSP_2;
		m_SphereIndices[3*i+2] = i+ NPSP_2+1;

		if( i == SPHERE_MESH_SIZE_X-1 )
			m_SphereIndices[3*i+2] = NPSP_2;
	}

	// Now all the middle strips
	WORD v = 0;
	for( j=0; j<SPHERE_MESH_SIZE_Y-1; j++ )
	{
		v = NPSP_2 + j*SPHERE_MESH_SIZE_X;
		ind = 3*SPHERE_MESH_SIZE_X + j*6*SPHERE_MESH_SIZE_X;

		for( i=0; i<SPHERE_MESH_SIZE_X; i++ )
		{
			m_SphereIndices[6*i+0+ind] = v+i;
			m_SphereIndices[6*i+2+ind] = v+i + 1;         // (longitude easter) LE
			m_SphereIndices[6*i+1+ind] = v+i + SPHERE_MESH_SIZE_X;

			m_SphereIndices[6*i+0+ind+3] = v+i + SPHERE_MESH_SIZE_X;
			m_SphereIndices[6*i+2+ind+3] = v+i + 1;                          // LE
			m_SphereIndices[6*i+1+ind+3] = v+i + SPHERE_MESH_SIZE_X +1;   // LE
			
			// Chj: adjust LE wrapping
			if( i == SPHERE_MESH_SIZE_X-1 )
			{
				m_SphereIndices[6*i+2+ind+0] = v;
				m_SphereIndices[6*i+2+ind+3] = v;
				m_SphereIndices[6*i+1+ind+3] = v + SPHERE_MESH_SIZE_X;
			}
		}
	}

	// Finally strip around south pole
	v   = NUM_SPHERE_VERTICES - SPHERE_MESH_SIZE_X;
	ind = NUM_SPHERE_INDICES-3 * SPHERE_MESH_SIZE_X;
	for( i=0; i<SPHERE_MESH_SIZE_X; i++ )
	{
		m_SphereIndices[3*i+0+ind] = 1;
		m_SphereIndices[3*i+1+ind] = v+i+1;
		m_SphereIndices[3*i+2+ind] = v+i;
		
		if( i == SPHERE_MESH_SIZE_X-1 )
			m_SphereIndices[3*i+1+ind] = v;
	}

	return S_OK;
}


//-----------------------------------------------------------------------------
// Name: InitDeviceObjects()
// Desc: Initialize 3D scene objects.
//-----------------------------------------------------------------------------
HRESULT CMyD3DApplication::InitDeviceObjects()
{
	// Set the transform matrices
	D3DMATRIX matWorld, matProj;
	D3DUtil_SetIdentityMatrix( matWorld );
	m_pd3dDevice->SetTransform( D3DTRANSFORMSTATE_WORLD,      &matWorld );

	// [2026-03-06] Chj: Update projection-matrix according to varying viewport aspect-ratio.
	FLOAT ratio = GetFramework()->GetAspectRatio();
	D3DUtil_SetProjectionMatrix( matProj, 1.57f, ratio, 1.0f, 100.0f );
	m_pd3dDevice->SetTransform( D3DTRANSFORMSTATE_PROJECTION, &matProj );

	//
	// Chap5_chj below
	//

	// Create and set up the object material
	D3DMATERIAL7 mtrl;
	D3DUtil_InitMaterial( mtrl, 1.0f, 1.0f, 1.0f );
	m_pd3dDevice->SetMaterial( &mtrl );

	// Turn on lighting. Light will be set during FrameMove() call
	m_pd3dDevice->LightEnable( 0, TRUE );
	m_pd3dDevice->SetRenderState( D3DRENDERSTATE_LIGHTING, TRUE );
	m_pd3dDevice->SetRenderState( D3DRENDERSTATE_AMBIENT,  0xFF202020 ); // 0xAARRGGBB

	//	m_pd3dDevice->SetRenderState(D3DRENDERSTATE_FILLMODE, D3DFILL_WIREFRAME); // debug purpose

	return S_OK;
}


//-----------------------------------------------------------------------------
// Name: FrameMove()
// Desc: Called once per frame, the call is the entry point for animating
//       the scene.
//-----------------------------------------------------------------------------
HRESULT CMyD3DApplication::FrameMove( FLOAT fTimeKey )
{
	// Chj: fTimeKey increases 1 each second.

	// Chj: Use light-type according to parameter dlgbox.
	m_dltType = m_ppbox->m_lighttype;

	// Make sure light is supported by the device
	DWORD dwCaps = m_pDeviceInfo->ddDeviceDesc.dwVertexProcessingCaps;

	if( 0 == ( dwCaps & D3DVTXPCAPS_POSITIONALLIGHTS ) )
	{
		if( m_dltType == D3DLIGHT_POINT || m_dltType == D3DLIGHT_SPOT )
			m_dltType = D3DLIGHT_DIRECTIONAL;
	}

	// Values for the light position, direction, and color

	FLOAT newx = sinf( fTimeKey*2.000f );
	FLOAT newy = sinf( fTimeKey*2.246f );
	FLOAT newz = sinf( fTimeKey*2.640f );

	FLOAT newz2= cosf( fTimeKey*2.000f ); // use same radius as newx

	if(m_ppbox->mc_LightAnimation.GetState()==BST_CHECKED)
	{
		m_x = newx; m_y = newy; m_z = newz; 
		m_z2 = newz2;
	}

	// Set up the light parameters
	D3DLIGHT7 &light = m_light; 

	ZeroMemory( &light, sizeof(light) );
	light.dltType = m_dltType;

	FLOAT &x = m_x, &y = m_y, &z = m_z;
	light.dcvDiffuse.r  = 0.5f + 0.5f * x;
	light.dcvDiffuse.g  = 0.5f + 0.5f * y;
	light.dcvDiffuse.b  = 0.5f + 0.5f * z;
	light.dvRange       = D3DLIGHT_RANGE_MAX;

	switch( m_dltType )
	{{
	case D3DLIGHT_POINT:
	{
		if(!m_ppbox->m_isPointLightLatitude) // book default
		{
			light.dvPosition = 4.5f * D3DVECTOR( x, y, z );
		}
		else
		{
			light.dvPosition = D3DVECTOR(
				m_ppbox->mc_PointLightRadius.GetValue() * x, 
				m_ppbox->m_PointLightHeight,
				m_ppbox->mc_PointLightRadius.GetValue() * m_z2
				);
		}
		light.dvAttenuation1 = 0.4f;
		break;
	}

	case D3DLIGHT_DIRECTIONAL:
		light.dvDirection    = D3DVECTOR( x, y, z );
		break;

	case D3DLIGHT_SPOT:
		light.dvDirection    = D3DVECTOR( x, y, z );
		light.dvFalloff      = 100.0f;
		light.dvTheta        =   0.8f;
		light.dvPhi          =   1.0f;
		light.dvAttenuation0 =   1.0f;
	}}

	// Set the light
	m_pd3dDevice->SetLight( 0, &light );

	//
	// Move the camera position around
	//
	if( m_ppbox->mc_CameraAnimation.IsChecked() ) {
		m_camX_waggle = newx;
	}

	FLOAT OrbitCenterDegree = m_ppbox->mc_CameraOrbitDegree.GetValue();
	FLOAT OribitWaggleDegreeMax = m_ppbox->mc_CameraWaggleDegreeMax.GetValue();
	FLOAT OrbitDegreeLive = OrbitCenterDegree + (OribitWaggleDegreeMax/2) * m_camX_waggle;
	m_ppbox->SetGui_CameraOrbitDegreeLive(OrbitDegreeLive) ;
	
	FLOAT toc = Deg2Rad(OrbitDegreeLive);

	D3DVECTOR vFrom( 
		cosf(toc) * m_ppbox->mc_CameraDistance.GetValue(), 
		m_ppbox->mc_CameraHeight.GetValue(), 
		sinf(toc) * m_ppbox->mc_CameraDistance.GetValue() );
	// -- camera around a latitude arc
	
	D3DVECTOR vAt( 0.0f, 0.0f, 0.0f );
	// -- camera looking at world space origin

	D3DVECTOR vUp( 0.0f, 1.0f, 0.0f );
	SetViewParams( &vFrom, &vAt, &vUp, 0.1f );

	return S_OK;
}


//-----------------------------------------------------------------------------
// Defines, constants, and global variables
//-----------------------------------------------------------------------------

// (chj: already in RoadRage.hpp)


//-----------------------------------------------------------------------------
// Name: Render()
// Desc: Called once per frame, the call is the entry point for 3d
//       rendering. This function sets up render states, clears the
//       viewport, and renders the scene.
//-----------------------------------------------------------------------------
HRESULT CMyD3DApplication::Render()
{
	// Clear the viewport
	m_pd3dDevice->Clear( 0, NULL, 
		D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, // Chj: D3DCLEAR_ZBUFFER must be clear to use D3DRENDERSTATE_ZENABLE
		0x00000000, 1.0f, 0L );

	// Begin the scene
	if( SUCCEEDED( m_pd3dDevice->BeginScene() ) )
	{
		D3DMATRIX matWorld, matTrans, matRotate;
		D3DPRIMITIVETYPE primtype;

		// Zbuffer type
		D3DZBUFFERTYPE zbtype = (D3DZBUFFERTYPE) m_ppbox->mc_Zbuffer.GetActiveIndex();
		zbtype = _MID_(D3DZB_FALSE, zbtype, D3DZB_USEW);
		m_pd3dDevice->SetRenderState(D3DRENDERSTATE_ZENABLE, zbtype);

		// Draw the bottom wall
		D3DUtil_SetRotateYMatrix( matRotate, 0.0f ); // 0.0 = no rotate
		D3DUtil_SetTranslateMatrix( matTrans,-5.0f, -5.0f, -5.0f );
		D3DMath_MatrixMultiply( matWorld, matRotate, matTrans );
		m_pd3dDevice->SetTransform(D3DTRANSFORMSTATE_WORLD, &matWorld );
		m_pd3dDevice->DrawIndexedPrimitive( D3DPT_TRIANGLELIST, D3DFVF_VERTEX,
			m_WallVertices, NUM_WALL_VERTICES,
			m_WallIndices, NUM_WALL_INDICES, 0 );

		// Draw the right-Side wall
		D3DUtil_SetRotateZMatrix( matRotate, g_PI/2 );
		D3DUtil_SetTranslateMatrix( matTrans, 5.0f,-5.0f, -5.0f );
		D3DMath_MatrixMultiply( matWorld, matRotate, matTrans );
		m_pd3dDevice->SetTransform( D3DTRANSFORMSTATE_WORLD, &matWorld );
		primtype = m_ppbox->mc_IsRightWallGrid.IsChecked() ? D3DPT_LINESTRIP : D3DPT_TRIANGLELIST;
		m_pd3dDevice->DrawIndexedPrimitive( primtype, D3DFVF_VERTEX,
			m_WallVertices, NUM_WALL_VERTICES,
			m_WallIndices, NUM_WALL_INDICES, 0 );

		// Draw the Back wall
		D3DUtil_SetRotateXMatrix( matRotate,  -g_PI/2 );
		D3DUtil_SetTranslateMatrix( matTrans, -5.0f, -5.0f, 5.0f );
		D3DMath_MatrixMultiply( matWorld, matRotate, matTrans );
		m_pd3dDevice->SetTransform( D3DTRANSFORMSTATE_WORLD, &matWorld );
		primtype = m_ppbox->mc_IsBackWallGrid.IsChecked() ? D3DPT_LINESTRIP : D3DPT_TRIANGLELIST;
		m_pd3dDevice->DrawIndexedPrimitive( primtype, D3DFVF_VERTEX,
			m_WallVertices, NUM_WALL_VERTICES,
			m_WallIndices, NUM_WALL_INDICES, 0 );

		// Draw sphere at light's position
		D3DLIGHT7 light;
		m_pd3dDevice->GetLight( 0, &light );
		
		D3DUtil_SetTranslateMatrix( matWorld, light.dvPosition.x,
			light.dvPosition.y, light.dvPosition.z );
		
		m_pd3dDevice->SetTransform( D3DTRANSFORMSTATE_WORLD, &matWorld );
		
		primtype = m_ppbox->mc_IsBallGrid.IsChecked() ? D3DPT_LINESTRIP : D3DPT_TRIANGLELIST;
		m_pd3dDevice->DrawIndexedPrimitive( primtype, D3DFVF_VERTEX, 
			m_SphereVertices, NUM_SPHERE_VERTICES,
			m_SphereIndices, NUM_SPHERE_INDICES, 0 );

		// Output the name of the light type
		switch( m_dltType )
		{
		case D3DLIGHT_POINT:
			OutputText(  0, 20, TEXT("Point Light") );
			break;
		case D3DLIGHT_SPOT:
			OutputText( 0, 20, TEXT("Spot Light") );
			break;
		case D3DLIGHT_DIRECTIONAL:
			OutputText( 0, 20, TEXT("Directional Light") );
			break;
		}
	}

	// End the scene.
	m_pd3dDevice->EndScene();

	return S_OK;
}


//-----------------------------------------------------------------------------
// Name: FinalCleanup()
// Desc: Called before the app exits, this function gives the app the chance
//       to cleanup after itself.
//-----------------------------------------------------------------------------
HRESULT CMyD3DApplication::FinalCleanup()
{
	return S_OK;
}


//-----------------------------------------------------------------------------
// Name: DeleteDeviceObjects()
// Desc: Called when the app is exiting, or the device is being changed,
//       this function deletes any device-dependent objects.
//-----------------------------------------------------------------------------
HRESULT CMyD3DApplication::DeleteDeviceObjects()
{
	return S_OK;
}


//-----------------------------------------------------------------------------
// Name: WinMain()
// Desc: Entry point to the program. Initializes everything, and goes into a
//       message-processing loop. Idle time is used to render the scene.
//-----------------------------------------------------------------------------
INT WINAPI _tWinMain( HINSTANCE hInst, HINSTANCE, LPTSTR strCmdLine, INT )
{
	
	CMyD3DApplication d3dApp;

	d3dApp.hInstApp = hInst;

    if( FAILED( d3dApp.Create( hInst, strCmdLine ) ) )
        return 0;

    d3dApp.Run();		
	
	CoUninitialize();
	return TRUE;
}
