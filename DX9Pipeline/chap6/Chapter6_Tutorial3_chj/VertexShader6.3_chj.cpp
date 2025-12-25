//-----------------------------------------------------------------------------
// File: VertexShader6.3_chj.cpp
//
// Desc: Example code showing how to do vertex shaders in D3D.
//
// Copyright (c) Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------
#define STRICT
#include <Windows.h>
#include <commctrl.h>
#include <stdio.h>
#include <math.h>
#include <D3DX9.h>
#include "DXUtil.h"
#include "D3DEnumeration.h"
#include "D3DSettings.h"
#include "D3DApp.h"
#include "D3DFile.h"
#include "D3DFont.h"
#include "D3DUtil.h"
#include "resource.h"

#include "../BookCommon/chjshare.h"

struct CUSTOM_VERTEX
{
	float		x,y,z;
	float		tu,tv;
};

//-----------------------------------------------------------------------------
// Name: class CMyD3DApplication
// Desc: Application class. The base class (CD3DApplication) provides the 
//       generic functionality needed in all Direct3D samples. CMyD3DApplication 
//       adds functionality specific to this sample program.
//-----------------------------------------------------------------------------
class CMyD3DApplication : public CD3DApplication
{
	LPDIRECT3DVERTEXSHADER9			m_pHLL_VS;
	LPDIRECT3DPIXELSHADER9			m_pHLL_PS;
	LPDIRECT3DVERTEXDECLARATION9	m_pVertexDeclaration;
	LPD3DXCONSTANTTABLE				m_pConstantTable;
	LPDIRECT3DTEXTURE9				m_pTexture;

	LPDIRECT3DVERTEXBUFFER9			m_pVBSphere;
	DWORD							m_dwNumSphereSegments;
	DWORD							m_dwNumSphereRings;

	// Font for drawing text
	CD3DFont* m_pFont;
	CD3DFont* m_pFontSmall;

	// Scene

	// Transforms
	D3DXMATRIXA16  m_matPosition;
	D3DXMATRIXA16  m_matWorld;
	D3DXMATRIXA16  m_matView;
	D3DXMATRIXA16  m_matProj;

	// Navigation
	BYTE        m_bKey[256];
	FLOAT       m_fSpeed;
	FLOAT       m_fAngularSpeed;
	BOOL        m_bShowHelp;

	D3DXVECTOR3 m_vVelocity;
	D3DXVECTOR3 m_vAngularVelocity;

protected:
	HRESULT OneTimeSceneInit();
	HRESULT InitDeviceObjects();
	HRESULT RestoreDeviceObjects();
	HRESULT InvalidateDeviceObjects();
	HRESULT DeleteDeviceObjects();
	HRESULT FinalCleanup();
	HRESULT Render();
	HRESULT FrameMove();
	HRESULT ConfirmDevice( D3DCAPS9* pCaps, DWORD dwBehavior, 
		D3DFORMAT adapterFormat, D3DFORMAT backBufferFormat );
	LRESULT MsgProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );

public:
	CMyD3DApplication();

protected:
	void SafeReleaseAll()
	{
		SAFE_RELEASE(m_pHLL_VS);
		SAFE_RELEASE(m_pHLL_PS);
		SAFE_RELEASE(m_pVertexDeclaration);
		SAFE_RELEASE(m_pConstantTable);
		SAFE_RELEASE(m_pTexture);
		SAFE_RELEASE(m_pVBSphere);
	}
};


static void ChjInit()
{
	int argc = __argc;

#ifdef UNICODE
	PCTSTR* argv = (PCTSTR*) CommandLineToArgvW(GetCommandLine(), &argc);
#else
	PCTSTR* argv = (PCTSTR*) __argv;
#endif

// 	if(argc>1)
// 		g_rings = _MAX_(3, _ttoi(argv[1]));
// 
// 	if(argc>2)
// 		g_segments = _MAX_(3, _ttoi(argv[2]));

	vaDbg_set_vsnprintf(my_mm_vsnprintf); // Chj
}


//-----------------------------------------------------------------------------
// Name: WinMain()
// Desc: Entry point to the program. Initializes everything, and goes into a
//       message-processing loop. Idle time is used to render the scene.
//-----------------------------------------------------------------------------
INT WINAPI WinMain( HINSTANCE hInst, HINSTANCE, LPSTR, INT )
{
	ChjInit();

	CMyD3DApplication d3dApp;

	InitCommonControls();
	if( FAILED( d3dApp.Create( hInst ) ) )
		return 0;

	return d3dApp.Run();
}


//-----------------------------------------------------------------------------
// Name: CMyD3DApplication()
// Desc: Application constructor. Sets attributes for the app.
//-----------------------------------------------------------------------------
CMyD3DApplication::CMyD3DApplication()
{
	m_pHLL_VS = NULL;
	m_pHLL_PS = NULL;

	m_pVertexDeclaration  = NULL;
	m_pConstantTable      = NULL;
	m_pTexture            = NULL;

	m_pVBSphere             = NULL;
	m_dwNumSphereSegments	= 24;
	m_dwNumSphereRings		= m_dwNumSphereSegments;

	m_strWindowTitle    = _T("Ex6-3-chj VertexShader");
	m_d3dEnumeration.AppUsesDepthBuffer   = TRUE;

	m_pFont            = new CD3DFont( _T("Arial"), 12, D3DFONT_BOLD );
	m_pFontSmall       = new CD3DFont( _T("Arial"),  9, D3DFONT_BOLD );

	ZeroMemory( m_bKey, sizeof(m_bKey) );
	m_fSpeed           = 5.0f;
	m_fAngularSpeed    = 1.0f;
	m_bShowHelp        = FALSE;

	m_vVelocity        = D3DXVECTOR3( 0.0f, 0.0f, 0.0f );
	m_vAngularVelocity = D3DXVECTOR3( 0.0f, 0.0f, 0.0f );


}


//-----------------------------------------------------------------------------
// Name: OneTimeSceneInit()
// Desc: Called during initial app startup, this function performs all the
//       permanent initialization.
//-----------------------------------------------------------------------------
HRESULT CMyD3DApplication::OneTimeSceneInit()
{
	D3DXVECTOR3 from( 0.0f, 0.0f, -3.0f );
	D3DXVECTOR3 at( 0.0f, 0.0f, 0.0f );
	D3DXVECTOR3 up( 0.0f, 1.0f, 0.0f );
	D3DXMatrixIdentity( &m_matView );
	D3DXMatrixLookAtLH( &m_matView, &from, &at, &up );
	D3DXMatrixInverse( &m_matPosition, NULL, &m_matView );

	return S_OK;
}


//-----------------------------------------------------------------------------
// Name: FrameMove()
// Desc: Called once per frame, the call is the entry point for animating
//       the scene.
//-----------------------------------------------------------------------------
HRESULT CMyD3DApplication::FrameMove()
{
	FLOAT fSecsPerFrame = m_fElapsedTime;

	// Process keyboard input
	D3DXVECTOR3 vT( 0.0f, 0.0f, 0.0f );
	D3DXVECTOR3 vR( 0.0f, 0.0f, 0.0f );

	if( m_bKey[VK_LEFT] || m_bKey[VK_NUMPAD1] )    vT.x -= 1.0f; // Slide Left
	if( m_bKey[VK_RIGHT] || m_bKey[VK_NUMPAD3] )   vT.x += 1.0f; // Slide Right
	if( m_bKey[VK_DOWN] )                          vT.y -= 1.0f; // Slide Down
	if( m_bKey[VK_UP] )                            vT.y += 1.0f; // Slide Up
	if( m_bKey['W'] )                              vT.z -= 2.0f; // Move Forward
	if( m_bKey['S'] )                              vT.z += 2.0f; // Move Backward
	if( m_bKey['A'] || m_bKey[VK_NUMPAD8] )        vR.x -= 1.0f; // Pitch Down
	if( m_bKey['Z'] || m_bKey[VK_NUMPAD2] )        vR.x += 1.0f; // Pitch Up
	if( m_bKey['E'] || m_bKey[VK_NUMPAD6] )        vR.y -= 1.0f; // Turn Right
	if( m_bKey['Q'] || m_bKey[VK_NUMPAD4] )        vR.y += 1.0f; // Turn Left
	if( m_bKey[VK_NUMPAD9] )                       vR.z -= 2.0f; // Roll CW
	if( m_bKey[VK_NUMPAD7] )                       vR.z += 2.0f; // Roll CCW

	m_vVelocity        = m_vVelocity * 0.9f + vT * 0.1f;
	m_vAngularVelocity = m_vAngularVelocity * 0.9f + vR * 0.1f;

	// Update position and view matrices
	D3DXMATRIXA16     matT, matR;
	D3DXQUATERNION qR;

	vT = m_vVelocity * fSecsPerFrame * m_fSpeed;
	vR = m_vAngularVelocity * fSecsPerFrame * m_fAngularSpeed;

	D3DXMatrixTranslation( &matT, vT.x, vT.y, vT.z);
	D3DXMatrixMultiply( &m_matPosition, &matT, &m_matPosition );

	D3DXQuaternionRotationYawPitchRoll( &qR, vR.y, vR.x, vR.z );
	D3DXMatrixRotationQuaternion( &matR, &qR );

	D3DXMatrixMultiply( &m_matPosition, &matR, &m_matPosition );
	D3DXMatrixInverse( &m_matView, NULL, &m_matPosition );

	// Chj test code >>>
	if (m_bKey['1'] || m_bKey['2']) 
	{
		// Rotate the earth along Y-axis, by changing m_matWorld.
		int sign = m_bKey['1'] ? 1 : -1;

		D3DXMATRIXA16 roty;
		D3DXMatrixRotationY(&roty, sign * D3DX_PI / 5 * fSecsPerFrame);

		D3DXMatrixMultiply(&m_matWorld, &m_matWorld, &roty);
	}
	else if (m_bKey[VK_HOME] || m_bKey[VK_END])
	{
		// Rotate the earth along X-axis.
		int sign = m_bKey[VK_HOME] ? 1 : -1;

		D3DXMATRIXA16 rotx;
		D3DXMatrixRotationX(&rotx, sign * D3DX_PI /5 * fSecsPerFrame);

		D3DXMatrixMultiply(&m_matWorld, &m_matWorld, &rotx);
	}
	else if (m_bKey['0'])
	{
		D3DXMatrixIdentity(&m_matWorld);
	}
	// Chj test code <<<

	return S_OK;
}


//-----------------------------------------------------------------------------
// Name: Render()
// Desc: Called once per frame, the call is the entry point for 3d
//       rendering. This function sets up render states, clears the
//       viewport, and renders the scene.
//-----------------------------------------------------------------------------
HRESULT CMyD3DApplication::Render()
{
	// Clear the backbuffer
	m_pd3dDevice->Clear( 0L, NULL, D3DCLEAR_TARGET|D3DCLEAR_ZBUFFER,
		0x000000ff, 1.0f, 0L );

	// Begin the scene
	if( SUCCEEDED( m_pd3dDevice->BeginScene() ) )
	{
		HRESULT hr;

		// Draw a sphere using a vertex shader and a pixel shader
		if(m_pConstantTable)
		{
			// Setup vertex shader
			m_pd3dDevice->SetVertexShader( m_pHLL_VS );
			m_pd3dDevice->SetVertexDeclaration( m_pVertexDeclaration );
			m_pd3dDevice->SetStreamSource( 0, m_pVBSphere, 0, 
				sizeof(CUSTOM_VERTEX) );

			D3DXMATRIX compMat;
			D3DXMatrixMultiply( &compMat, &m_matWorld, &m_matView);
			D3DXMatrixMultiply( &compMat, &compMat, &m_matProj);
			m_pConstantTable->SetMatrix(m_pd3dDevice, "WorldViewProj", 
				&compMat); 

			// Setup pixel shader
			m_pd3dDevice->SetPixelShader( m_pHLL_PS );
			m_pd3dDevice->SetTexture( 0, m_pTexture );

			// Draw sphere
			DWORD dwNumSphereVerts = 
				2*m_dwNumSphereRings*(m_dwNumSphereSegments+1);
			m_pd3dDevice->DrawPrimitive( D3DPT_TRIANGLESTRIP, 0, 
				dwNumSphereVerts - 2 );

			// Release the vertex shader before using the FVF vertex pipe
			m_pd3dDevice->SetTexture( 0, NULL );
			m_pd3dDevice->SetPixelShader( NULL );
			m_pd3dDevice->SetVertexShader( NULL );

			// Draw hint text 
			m_pFontSmall->DrawText( 2,  0, D3DCOLOR_ARGB(255,255,255,0), 
				_T("Press 1/2/Home/End to rotate. 0 to reset.") );
		}					


		// End the scene
		hr = m_pd3dDevice->EndScene();
	}

	return S_OK;
}


//-----------------------------------------------------------------------------
// Name: InitDeviceObjects()
// Desc: Initialize scene objects.
//-----------------------------------------------------------------------------
HRESULT CMyD3DApplication::InitDeviceObjects()
{
	m_pFont->InitDeviceObjects( m_pd3dDevice );
	m_pFontSmall->InitDeviceObjects( m_pd3dDevice );

	return S_OK;
}


//-----------------------------------------------------------------------------
// Name: RestoreDeviceObjects()
// Desc: Initialize scene objects.
//-----------------------------------------------------------------------------
HRESULT CMyD3DApplication::RestoreDeviceObjects()
{
	HRESULT hr = 0;

	LPD3DXBUFFER pShader = NULL;
	Cec_Release cec_ShaderCode;

	const TCHAR *fxfile = _T("Ex6-3-chj.fx");

	DWORD shader_flags = D3DXSHADER_DEBUG | D3DXSHADER_SKIPOPTIMIZATION;

	// Compile the vertex shader
	hr = D3DXCompileShaderFromFile_dbg(
		fxfile,
		NULL,
		NULL,
		"VS_HLL_EX1",
		"vs_1_1",
		shader_flags,
		&pShader, 
		NULL, 
		&m_pConstantTable );
	cec_ShaderCode = pShader;
	if( FAILED(hr) ) {
		goto ERROR_END;
	}

	// Create the vertex shader
	hr = m_pd3dDevice->CreateVertexShader( 
		(DWORD*)pShader->GetBufferPointer(), &m_pHLL_VS );
	if( FAILED(hr) ) {
		goto ERROR_END;
	}

	// Compile the pixel shader
	hr = D3DXCompileShaderFromFile_dbg(
		fxfile,
		NULL,
		NULL,
		"PS_HLL_EX1",
		"ps_1_1",
		shader_flags,
		&pShader, 
		NULL, 
		NULL );
	cec_ShaderCode = pShader;
	if( FAILED(hr) ) {
		goto ERROR_END;
	}

	// Create the pixel shader
	hr = m_pd3dDevice->CreatePixelShader( 
		(DWORD*)pShader->GetBufferPointer(), &m_pHLL_PS );

	if( FAILED(hr) ) {
		goto ERROR_END;
	}

	// Compile the procedural texture (new in Ex6-3)
	hr = D3DXCompileShaderFromFile_dbg(
		fxfile,
		NULL, 
		NULL, 
		"TX_HLL_EX1",  
		"tx_1_0", 
		shader_flags,
		&pShader, 
		NULL, 
		NULL );
	cec_ShaderCode = pShader;
	if( FAILED(hr) ) {
		goto ERROR_END;
	}


	// Create the procedural texture (new in Ex6-3)
	hr = D3DXCreateTexture(
		m_pd3dDevice,
		64, 64,           // width, height
		1,                // mip levels
		0,                // usage
		D3DFMT_UNKNOWN,   // format
		D3DPOOL_MANAGED,  // memory pool
		&m_pTexture);

	if(FAILED(hr)) {
		goto ERROR_END;
	}


	// Procedurally fill the texture (new in Ex6-3)
	hr = D3DXFillTextureTX(m_pTexture, 
		(CONST DWORD*)pShader->GetBufferPointer(), NULL, 0);

	if( FAILED(hr) ) {
		goto ERROR_END;
	}

	// Set the sampler state for the procedural texture
	m_pd3dDevice->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_POINT);
	m_pd3dDevice->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_POINT);
	m_pd3dDevice->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_POINT);

	// Create the vertex declaration
	D3DVERTEXELEMENT9 decl[] =
	{
		{ 0, 0, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, 
			D3DDECLUSAGE_POSITION, 0 },
		{ 0, 12, D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT, 
			D3DDECLUSAGE_TEXCOORD, 0 },
		D3DDECL_END()
	};

	if( FAILED( hr = m_pd3dDevice->CreateVertexDeclaration( decl, 
		&m_pVertexDeclaration ) ) )
	{
		goto ERROR_END;
	}


	DWORD dwNumSphereVerts = 2*m_dwNumSphereRings*(m_dwNumSphereSegments+1);
	// Once for the top, once for the bottom vertices

	if(FAILED(m_pd3dDevice->CreateVertexBuffer(
		dwNumSphereVerts*sizeof(CUSTOM_VERTEX),
		0, 
		0, // don't need an FVF code
		D3DPOOL_DEFAULT, 
		&m_pVBSphere, NULL)	) )
	{
		goto ERROR_END;
	}


	CUSTOM_VERTEX* pVertices;
	hr = m_pVBSphere->Lock(0, dwNumSphereVerts*sizeof(CUSTOM_VERTEX), 
		(VOID**)&pVertices, 0);
	if(SUCCEEDED(hr))
	{
		FLOAT fDeltaRingAngle = ( D3DX_PI / m_dwNumSphereRings );
		FLOAT fDeltaSegAngle  = ( 2.0f * D3DX_PI / m_dwNumSphereSegments );

		// Generate the group of rings for the sphere
		for( DWORD ring = 0; ring < m_dwNumSphereRings; ring++ )
		{
			FLOAT r0 = sinf( (ring+0) * fDeltaRingAngle );
			FLOAT r1 = sinf( (ring+1) * fDeltaRingAngle );
			FLOAT y0 = cosf( (ring+0) * fDeltaRingAngle );
			FLOAT y1 = cosf( (ring+1) * fDeltaRingAngle );

			// Generate the group of segments for the current ring
			for( DWORD seg = 0; seg < (m_dwNumSphereSegments+1); seg++ )
			{
				FLOAT x0 =  r0 * sinf( seg * fDeltaSegAngle );
				FLOAT z0 =  r0 * cosf( seg * fDeltaSegAngle );
				FLOAT x1 =  r1 * sinf( seg * fDeltaSegAngle );
				FLOAT z1 =  r1 * cosf( seg * fDeltaSegAngle );

				// Add two vertices to the strip which makes up the sphere
				// (using the transformed normal to generate texture coords)
				pVertices->x = x0;
				pVertices->y = y0;
				pVertices->z = z0;
				pVertices->tu = -((FLOAT)seg)/m_dwNumSphereSegments;
				pVertices->tv = (ring+0)/(FLOAT)m_dwNumSphereRings;
				pVertices++;

				pVertices->x = x1;
				pVertices->y = y1;
				pVertices->z = z1;
				pVertices->tu = -((FLOAT)seg)/m_dwNumSphereSegments;
				pVertices->tv = (ring+1)/(FLOAT)m_dwNumSphereRings;
				pVertices++;
			}

		}

		hr = m_pVBSphere->Unlock();
	}


	// Set up the world matrix
	D3DXMatrixIdentity( &m_matWorld );
	D3DXMatrixRotationY(&m_matWorld, 5.0f*3.14f/8.0f);


	// Set up the projection matrix
	D3DXMatrixIdentity( &m_matProj );
	FLOAT fAspectRatio = 
		(FLOAT)m_d3dsdBackBuffer.Width / (FLOAT)m_d3dsdBackBuffer.Height;
	D3DXMatrixPerspectiveFovLH( &m_matProj, D3DX_PI/4, fAspectRatio, 
		0.5f, 1000.0f );

	m_pFont->RestoreDeviceObjects();
	m_pFontSmall->RestoreDeviceObjects();

	// Set up render states
	m_pd3dDevice->SetRenderState( D3DRS_LIGHTING, FALSE );
	m_pd3dDevice->SetRenderState( D3DRS_CULLMODE, D3DCULL_NONE );

	return S_OK;

ERROR_END:
	SafeReleaseAll();
	return hr;
}


//-----------------------------------------------------------------------------
// Name: InvalidateDeviceObjects()
// Desc: Will be called after user drags window border to change window size.
//-----------------------------------------------------------------------------
HRESULT CMyD3DApplication::InvalidateDeviceObjects()
{
	SafeReleaseAll();

	m_pFont->InvalidateDeviceObjects();
	m_pFontSmall->InvalidateDeviceObjects();

	return S_OK;
}


//-----------------------------------------------------------------------------
// Name: DeleteDeviceObjects()
// Desc: Called when the app is exiting, or the device is being changed,
//       this function deletes any device dependent objects.
//-----------------------------------------------------------------------------
HRESULT CMyD3DApplication::DeleteDeviceObjects()
{
	SafeReleaseAll();

	m_pFont->DeleteDeviceObjects();
	m_pFontSmall->DeleteDeviceObjects();

	return S_OK;
}


//-----------------------------------------------------------------------------
// Name: FinalCleanup()
// Desc: Called before the app exits, this function gives the app the chance
//       to cleanup after itself.
//-----------------------------------------------------------------------------
HRESULT CMyD3DApplication::FinalCleanup()
{
	SAFE_DELETE( m_pFont );
	SAFE_DELETE( m_pFontSmall );
	return S_OK;
}


//-----------------------------------------------------------------------------
// Name: ConfirmDevice()
// Desc: Called during device initialization, this code checks the device
//       for some minimum set of capabilities
//-----------------------------------------------------------------------------
HRESULT CMyD3DApplication::ConfirmDevice( D3DCAPS9* pCaps, DWORD dwBehavior,
	D3DFORMAT adapterFormat, D3DFORMAT backBufferFormat )
{
	if( dwBehavior & D3DCREATE_PUREDEVICE )
		return E_FAIL;

	if( pCaps->PixelShaderVersion < D3DPS_VERSION(1,1) )
		return E_FAIL;

	// If device doesn't support vs_1_1 in hardware, switch to 
	// software vertex processing
	if( pCaps->VertexShaderVersion < D3DVS_VERSION(1,1) )
	{
		if( (dwBehavior & D3DCREATE_SOFTWARE_VERTEXPROCESSING ) == 0 )
		{
			return E_FAIL;
		}
	}

	return S_OK;
}


//-----------------------------------------------------------------------------
// Name: MsgProc()
// Desc: Message proc function to handle key and menu input
//-----------------------------------------------------------------------------
LRESULT CMyD3DApplication::MsgProc( HWND hWnd, UINT uMsg, WPARAM wParam,
	LPARAM lParam )
{
	// Handle key presses
	switch( uMsg )
	{
	case WM_KEYUP:
		m_bKey[wParam] = 0;
		break;

	case WM_KEYDOWN:
		m_bKey[wParam] = 1;
		break;

	case WM_COMMAND:
		{
			switch( LOWORD(wParam) )
			{
			case IDM_TOGGLEHELP:
				m_bShowHelp = !m_bShowHelp;
				break;
			}
		}
	}

	// Pass remaining messages to default handler
	return CD3DApplication::MsgProc( hWnd, uMsg, wParam, lParam );
}
