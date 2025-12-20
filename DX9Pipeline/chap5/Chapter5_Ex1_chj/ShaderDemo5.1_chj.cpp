//-----------------------------------------------------------------------------
// ShaderDemo5.1_chj.cpp
// Example code showing how to write simple pixel shader in D3D.
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

//-----------------------------------------------------------------------------
// Name: class CMyD3DApplication
// Desc: Application class. The base class (CD3DApplication) provides the 
//       generic functionality needed in all Direct3D samples. CMyD3DApplication 
//       adds functionality specific to this sample program.
//-----------------------------------------------------------------------------
class CMyD3DApplication : public CD3DApplication
{
	LPDIRECT3DPIXELSHADER9         m_pAsm_PS;
	LPDIRECT3DVERTEXSHADER9        m_pAsm_VS;
	LPDIRECT3DVERTEXDECLARATION9   m_pVertexDeclaration;
	LPDIRECT3DTEXTURE9             m_pTexture;
	
	// A structure for our custom vertex type
	struct CUSTOMVERTEX
	{
		float x, y, z;   
		float u,v;
	};

	LPDIRECT3DVERTEXBUFFER9 m_pVB; 

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
		SAFE_RELEASE(m_pAsm_VS);
		SAFE_RELEASE(m_pAsm_PS);
		SAFE_RELEASE(m_pVertexDeclaration);
		SAFE_RELEASE(m_pTexture);
		SAFE_RELEASE(m_pVB);
	}
};


//-----------------------------------------------------------------------------
// Name: WinMain()
// Desc: Entry point to the program. Initializes everything, and goes into a
//       message-processing loop. Idle time is used to render the scene.
//-----------------------------------------------------------------------------
INT WINAPI WinMain( HINSTANCE hInst, HINSTANCE, LPSTR, INT )
{
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
	m_pAsm_PS				= NULL;
	m_pAsm_VS				= NULL;
	m_pVertexDeclaration	= NULL;
	m_pVB					= NULL;
	m_pTexture				= NULL;


	m_strWindowTitle   = _T("Ex5-1 PixelShader");
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
		D3DXMATRIX compMat; 
		D3DXMatrixMultiply(&compMat, &m_matWorld, &m_matView);
		D3DXMatrixMultiply(&compMat, &compMat, &m_matProj);
		D3DXMatrixTranspose( &compMat, &compMat );
		m_pd3dDevice->SetVertexShaderConstantF( 0, (float*)&compMat, 4 );

		m_pd3dDevice->SetVertexDeclaration( m_pVertexDeclaration);
		m_pd3dDevice->SetVertexShader(m_pAsm_VS);
		m_pd3dDevice->SetStreamSource(0, m_pVB, 0, sizeof(CUSTOMVERTEX));

		m_pd3dDevice->SetPixelShader(m_pAsm_PS);
		m_pd3dDevice->DrawPrimitive( D3DPT_TRIANGLEFAN, 0, 2 );

		m_pd3dDevice->SetVertexShader(NULL);
		m_pd3dDevice->SetPixelShader(NULL);

		// End the scene
		m_pd3dDevice->EndScene();
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
	HRESULT hr = S_OK;

	LPD3DXBUFFER pShader = NULL;
	LPD3DXBUFFER pErrBuf = NULL;
	Cec_Release cec_ShaderCode = pShader;
	Cec_Release cec_errmsg = pErrBuf;

	const char* strAsmPixelShader = 
"ps_1_1            // version instruction\n"

"def c0, 0,0,0,0   \n"
"def c1, 1,1,1,1   \n"
"def c2, 1.0, 0.5, 0,       0 \n"
"def c3, 0,  -0.5, -0.25,   0 \n"

"tex t0             // sample texture at stage 0,\n" 
"                   // with texture coordinate set 0\n"

"mov r0, t0         // output texture color\n"

//"mov r0, 1 - t0   // output inverted texture color\n"

//"add r0, t0, c2   // add more reds and greens\n"

//"add r0, t0, c3   // subtract greens and blues\n"

//"mov r0, c2       // output solid pixel color\n"
"";

	hr = D3DXAssembleShader(
		strAsmPixelShader,
		(UINT)strlen(strAsmPixelShader),
		NULL, // A NULL terminated array of D3DXMACROs
		NULL, // A #include handler
		D3DXSHADER_DEBUG, 
		&pShader, 
		&pErrBuf // error messages 
		);
 	cec_ShaderCode = pShader;
	cec_errmsg = pErrBuf;

	if( FAILED(hr) ) 
	{
		if(pErrBuf)
		{
			const char *errmsg = (const char*)pErrBuf->GetBufferPointer();
			sdring<TCHAR> s1 = makeTsdring(errmsg);
			vaDbgTs(_T("D3DXAssembleShader() pixel-shader fail with hr=%s: %s"), ITCSvn(hr, DxErr), s1.c_str());
		}
		goto ERROR_END;
	}

	// Create the pixel shader
	hr = m_pd3dDevice->CreatePixelShader( 
		(DWORD*)pShader->GetBufferPointer(), &m_pAsm_PS );

	if( FAILED(hr) ) {
		goto ERROR_END;
	}


	const char* strAsmVertexShader = 
"vs_1_1                // version instruction\n"
"dcl_position v0       // bind position data in register v0\n"
"dcl_texcoord v1       // bind texture coordinate data in register v1\n"
"m4x4 oPos, v0, c0     // transform with view/projection matrix\n"
"mov oT0.xzw, v1.xzw   // output xzw texture coordinates (chj: zw just useless)\n"
"mov oT0.y, -v1.y      // output and invert y texture coordinate\n"
"";

	hr = D3DXAssembleShader(
		strAsmVertexShader,
		(UINT)strlen(strAsmVertexShader),
		NULL, // A NULL terminated string of D3DXMACROs
		NULL, // A #include handler
		D3DXSHADER_DEBUG, 
		&pShader, 
		&pErrBuf // error messages 
		);
	cec_ShaderCode = pShader; // will auto-release old pShader
	cec_errmsg = pErrBuf;

	if( FAILED(hr) ) 
	{
		if(pErrBuf)
		{
			const char *errmsg = (const char*)pErrBuf->GetBufferPointer();
			sdring<TCHAR> s1 = makeTsdring(errmsg);
			vaDbgTs(_T("D3DXAssembleShader() vertex-shader fail with hr=%s: %s"), ITCSvn(hr, DxErr), s1.c_str());
		}
		goto ERROR_END;
	}

	// Create the vertex shader
	hr = m_pd3dDevice->CreateVertexShader( 
		(DWORD*)pShader->GetBufferPointer(), &m_pAsm_VS );

	if( FAILED(hr) ) {
		goto ERROR_END;
	}


	// Declare the vertex data
	CUSTOMVERTEX vertices[] =
	{
		//  x      y      z    u, v
		{ -1.0f, -1.0f, 0.0f,  0, 0 },  // lower left
		{ +1.0f, -1.0f, 0.0f,  1, 0 },  // lower right
		{ +1.0f, +1.0f, 0.0f,  1, 1 },  // upper right
		{ -1.0f, +1.0f, 0.0f,  0, 1 },  // upper left
	};

	// Create the vertex buffer. Allocate enough memory
	// (from the default pool) to hold 4 custom vertices. 
	if( FAILED( hr = m_pd3dDevice->CreateVertexBuffer( 
		sizeof(vertices), 0, 0, D3DPOOL_DEFAULT, 
		&m_pVB, NULL ) ) )
	{
		goto ERROR_END;
	}

	// Now we fill the vertex buffer. To do this, Lock() 
	// the VB to gain access to the vertices.
	if(1)
	{
		VOID* pVertices = NULL;;
		if( FAILED( hr = m_pVB->Lock( 0, sizeof(vertices), 
			(VOID**)&pVertices, 0 ) ) )
		{
			goto ERROR_END;
		}
		memcpy( pVertices, vertices, sizeof(vertices) );
		hr = m_pVB->Unlock();
	}


	// Create the vertex declaration
	D3DVERTEXELEMENT9 decl[] =
	{
		{ 0, 0,  D3DDECLTYPE_FLOAT3,   D3DDECLMETHOD_DEFAULT, 
			D3DDECLUSAGE_POSITION, 0 },
		{ 0, 12,  D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT, 
			D3DDECLUSAGE_TEXCOORD, 0 },
		D3DDECL_END()
	};

	if( FAILED( hr = m_pd3dDevice->CreateVertexDeclaration( decl, 
		&m_pVertexDeclaration ) ) )
	{
		goto ERROR_END;
	}

	//
	// Load a texture
	//

	TCHAR szEarth[MAX_PATH] = {};
	hr = DXUtil_FindMediaFileCch(szEarth, ARRAYSIZE(szEarth), _T("earth.bmp"));
	if( FAILED(hr) ) {
		goto ERROR_END;
	}

	hr = D3DXCreateTextureFromFile( m_pd3dDevice, szEarth, &m_pTexture);
	if( FAILED(hr) ) {
		goto ERROR_END;
	}

	m_pd3dDevice->SetTexture( 0, m_pTexture );
	hr = m_pd3dDevice->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_POINT);
	hr = m_pd3dDevice->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_POINT);
	hr = m_pd3dDevice->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_POINT);


	m_pFont->RestoreDeviceObjects();
	m_pFontSmall->RestoreDeviceObjects();

	// Setup render states
	m_pd3dDevice->SetRenderState( D3DRS_LIGHTING, FALSE );
	m_pd3dDevice->SetRenderState( D3DRS_CULLMODE, D3DCULL_NONE );

	// Set up the world matrix
	D3DXMatrixIdentity( &m_matWorld );

	// Set up the projection matrix
	D3DXMatrixPerspectiveFovLH( &m_matProj, D3DX_PI/4, 
		1.0f, 0.1f, 100.0f );

	return S_OK;

ERROR_END:
	SafeReleaseAll();
	return hr;
}


//-----------------------------------------------------------------------------
// Name: InvalidateDeviceObjects()
// Desc:
//-----------------------------------------------------------------------------
HRESULT CMyD3DApplication::InvalidateDeviceObjects()
{
	m_pFont->InvalidateDeviceObjects();
	m_pFontSmall->InvalidateDeviceObjects();

	SafeReleaseAll();

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
