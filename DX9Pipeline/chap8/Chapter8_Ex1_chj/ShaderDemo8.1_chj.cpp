//-----------------------------------------------------------------------------
// File: VertexShader8.1_chj.cpp
//
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
#include "../BookCommon/chj_d3d9_dump.h"

//-----------------------------------------------------------------------------
// Name: class CMyD3DApplication
// Desc: Application class. The base class (CD3DApplication) provides the 
//       generic functionality needed in all Direct3D samples. CMyD3DApplication 
//       adds functionality specific to this sample program.
//-----------------------------------------------------------------------------
class CMyD3DApplication : public CD3DApplication
{
	// texture vertex shader
	LPDIRECT3DVERTEXSHADER9        m_pVS_Texture;
	LPD3DXCONSTANTTABLE            m_pTexture_ConstantTable;
	LPDIRECT3DVERTEXSHADER9        m_pVS_Glow;
	LPD3DXCONSTANTTABLE            m_pGlow_ConstantTable;

	// Scene background
	LPDIRECT3DTEXTURE9 m_pBackgroundTexture;    
	LPDIRECT3DVERTEXBUFFER9 m_pVBBackground;

	// mesh
	LPD3DXMESH           m_pMesh;           // Our mesh object in sysmem	
	D3DMATERIAL9*        m_arMeshMaterials;  // Materials for our mesh
	LPDIRECT3DTEXTURE9*  m_arMeshTextures;   // Textures for our mesh
	DWORD                m_dwNumMaterials;  // Number of mesh materials

	// Font for drawing text
	CD3DFont* m_pFont;
	CD3DFont* m_pFontSmall;

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
	void SafeReleaseDevice()
	{
		SAFE_RELEASE(m_pVS_Texture);
		SAFE_RELEASE(m_pTexture_ConstantTable);
		SAFE_RELEASE(m_pVS_Glow);
		SAFE_RELEASE(m_pGlow_ConstantTable);
		SAFE_RELEASE(m_pVBBackground);
	}

	void SafeReleaseMesh()
	{
		SAFE_RELEASE(m_pBackgroundTexture);	// -

		SAFE_RELEASE( m_pMesh );			// -

		if( m_arMeshMaterials != NULL ) 
		{
			delete[] m_arMeshMaterials;
			m_arMeshMaterials = NULL;		//-
		}

		if( m_arMeshTextures )
		{
			for( DWORD i = 0; i < m_dwNumMaterials; i++ )
			{
				SAFE_RELEASE( m_arMeshTextures[i] );
			}
			delete[] m_arMeshTextures;
			m_arMeshTextures = NULL;		// -
		}
	}

	void ChjRestoreSceneInit();
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
	m_pVS_Texture  = NULL;
	m_pVS_Glow 	   = NULL;
	m_pTexture_ConstantTable  = NULL;
	m_pGlow_ConstantTable	  = NULL;

	m_pBackgroundTexture = NULL;
	m_pVBBackground      = NULL;

	m_pMesh          = NULL; // Our mesh object in sysmem	
	m_arMeshMaterials = NULL; // Materials for our mesh
	m_arMeshTextures  = NULL; // Textures for our mesh
	m_dwNumMaterials = 0L;   // Number of mesh materials

	m_strWindowTitle    = _T("Ex8-1 chj VertexShader");
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

	return S_OK;
}

void CMyD3DApplication::ChjRestoreSceneInit()
{
	// As Sub-function in RestoreDeviceObjects()

	//
	// Set up initial world matrix
	//
	D3DXMatrixIdentity( &m_matWorld );

	//
	// Setup the view matrix
	//
	D3DXVECTOR3 vEye = D3DXVECTOR3( 2.5f, 0.5f, 0.0f ); 
	D3DXVECTOR3 vAt  = D3DXVECTOR3( 0.0f, 0.0f, 0.0f );
	D3DXVECTOR3 vUp  = D3DXVECTOR3( 0.0f, 1.0f, 0.0f );
	D3DXMatrixLookAtLH( &m_matView, &vEye, &vAt, &vUp );

	// Set the position matrix
	D3DXMatrixInverse( &m_matPosition, NULL, &m_matView );

	//
	// Set up the projection matrix
	//
	FLOAT fAspectRatio = (FLOAT)m_d3dsdBackBuffer.Width / (FLOAT)m_d3dsdBackBuffer.Height;
	D3DXMatrixPerspectiveFovLH( &m_matProj, D3DXToRadian(60.0f), fAspectRatio, 0.1f, 100.0f );
	m_pd3dDevice->SetTransform( D3DTS_PROJECTION, &m_matProj );
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
		ChjRestoreSceneInit();
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
#if 1
		// Draw the background
		if( m_pBackgroundTexture != NULL )
		{
			m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP, D3DTOP_SELECTARG1 );
			m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );

			// Render background image
			m_pd3dDevice->SetTexture(0, m_pBackgroundTexture);
			m_pd3dDevice->SetRenderState( D3DRS_ZENABLE, FALSE );
			m_pd3dDevice->SetRenderState( D3DRS_FILLMODE, D3DFILL_SOLID );

			m_pd3dDevice->SetFVF( D3DFVF_XYZRHW | D3DFVF_TEX1);
			m_pd3dDevice->SetStreamSource( 0, m_pVBBackground, 0, 6*sizeof(float) ); // 6 floats in s_Verts[]
			m_pd3dDevice->DrawPrimitive( D3DPT_TRIANGLESTRIP, 0, 2 );
			
			m_pd3dDevice->SetTexture(0, NULL);
			m_pd3dDevice->SetRenderState( D3DRS_ZENABLE, TRUE );

			m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP, D3DTOP_DISABLE );
			m_pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP, D3DTOP_DISABLE );
		}
#endif

		DWORD i;

		// Draw the solid tiger and the glow

		if(m_pTexture_ConstantTable)
		{
			// Initialize the texture constants
			D3DXMATRIXA16 matWorldView;
			D3DXMatrixMultiply(&matWorldView, &m_matWorld, &m_matView);
			m_pTexture_ConstantTable->SetMatrix(m_pd3dDevice, "WorldView", &matWorldView);
			m_pTexture_ConstantTable->SetMatrix(m_pd3dDevice, "Projection", &m_matProj); 

#if 2
			// Dry up the texture and light blending
			m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP, D3DTOP_MODULATE );
			m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_DIFFUSE );
			m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG2, D3DTA_TEXTURE );

			// Set the textured vertex shader
			m_pd3dDevice->SetVertexShader( m_pVS_Texture );

			// Render the tiger with a mesh drawing loop 
			for( i=0; i < m_dwNumMaterials; i++ )
			{
				// Set the material and texture for this subset
				m_pd3dDevice->SetMaterial( &m_arMeshMaterials[i] ); // useless bcz we use custom shader
				m_pd3dDevice->SetTexture( 0, m_arMeshTextures[i] );
			
				// Draw the mesh subset
				m_pMesh->DrawSubset( i );
			}

			// Turn off the texture stage
			m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP, D3DTOP_DISABLE );
			m_pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP, D3DTOP_DISABLE );
			// Drawing solid tiger is done
#endif

#if 3
			// Prepare to draw the glow

			// Enable alpha blend between the frame buffer, and the glow color 
			m_pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, TRUE );

			// Set up the render states and texture blend states 
			m_pd3dDevice->SetRenderState( D3DRS_SRCBLEND,  D3DBLEND_ONE );
			m_pd3dDevice->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_ONE );

			m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP, D3DTOP_SELECTARG2 );
			m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG2, D3DTA_DIFFUSE );

			// Chj: this two lines are useless, bcz we used D3DBLEND_ONE for D3DRS_SRCBLEND and D3DRS_DESTBLEND.
			//m_pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP, D3DTOP_SELECTARG2 );
			//m_pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG2, D3DTA_DIFFUSE );

			// [2026-01-05] Chj: Note a trick here: We should code m_pGlow_ConstantTable->SetMatrix(...) assignment
			// here, but the author omits it and the glow still works. 
			// This is bcz m_pTexture_ConstantTable->SetMatrix(...) and m_pGlow_ConstantTable->SetMatrix(...)
			// would assign to the same set of shader constant registers(c0, c1, c2 etc) and those register values
			// preserve across switching shader.

			// Set the glow vertex shader
			m_pd3dDevice->SetVertexShader( m_pVS_Glow );

			// Meshes are divided into subsets, one for each material. Render them in
			// a loop
			for( i=0; i < m_dwNumMaterials; i++ )
			{
				// Set the material and texture for this subset
				m_pd3dDevice->SetMaterial( &m_arMeshMaterials[i] ); // useless bcz we use custom shader
//				m_pd3dDevice->SetTexture( 0, m_arMeshTextures[i] ); // useless bcz VS_HLSL_Glow does NOT refer to texture
			
				// Draw the mesh subset
				m_pMesh->DrawSubset(i);
			}
#endif
			// Reset the render states on exit 
			m_pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, FALSE );

			m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP, D3DTOP_DISABLE );
			m_pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP, D3DTOP_DISABLE );
			// --[2026-01-01] Chj: Above two lines can be omitted, bcz we've done using texture 0.
		}					

		// Output statistics
		m_pFont->DrawText( 2,  0, D3DCOLOR_ARGB(255,255,255,0), m_strFrameStats );
		m_pFont->DrawText( 2, 20, D3DCOLOR_ARGB(255,255,255,0), m_strDeviceStats );

		if( m_bShowHelp )
		{
			m_pFontSmall->DrawText(  2, 40, D3DCOLOR_ARGB(255,255,255,255),
									_T("Keyboard controls:") );
			m_pFontSmall->DrawText( 20, 60, D3DCOLOR_ARGB(255,255,255,255),
									_T("Far,Near\nTurn\nPitch\nSlide\n")
									_T("Rotate tiger\n") );
			m_pFontSmall->DrawText( 210, 60, D3DCOLOR_ARGB(255,255,255,255),
									_T("W,S\nE,Q\nA,Z\nArrow keys\n")
									_T("1,2,Home,End,0\n") );
		}
		else
		{
			m_pFontSmall->DrawText(  2, 40, D3DCOLOR_ARGB(255,255,255,255), 
							   _T("Press F1 for help") );
		}


		// End the scene.
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
	// Chj: SafeReleaseMesh(); reveals what resources are allocated in InitDeviceObjects().

	dumpRenderState(m_pd3dDevice,          _T("In InitDeviceObjects(), dumpRenderState:"));
	dumpSamplerState(m_pd3dDevice,      0, _T("In InitDeviceObjects(), dumpSamplerState(iSample=0):"));
	dumpTextureStageState(m_pd3dDevice, 0, _T("In InitDeviceObjects(), dumpTextureStageState(iStage=0):"));

	// Load the texture for the background image
	if( FAILED( D3DUtil_CreateTexture( m_pd3dDevice, _T("Lake.bmp"),
									   &m_pBackgroundTexture )))
		return D3DAPPERR_MEDIANOTFOUND;

	TCHAR        strMediaPath[512] = {};
	LPD3DXBUFFER l_pD3DXMtrlBuffer = NULL;
	HRESULT hr = 0;

   // Find the path to the mesh
	hr = DXUtil_FindMediaFileCch( strMediaPath, ARRAYSIZE(strMediaPath), 
		TEXT("Tiger.x") // Tiger.x refers to Tiger.bmp inside.
		);
	if( FAILED(hr) )
		return D3DAPPERR_MEDIANOTFOUND;
	
	// Load the mesh from the specified file
	hr = D3DXLoadMeshFromX( strMediaPath, D3DXMESH_SYSTEMMEM, 
		m_pd3dDevice, NULL, &l_pD3DXMtrlBuffer, NULL, 
		&m_dwNumMaterials, &m_pMesh );
	Cec_Release cec_pD3DXMtrlBuffer = l_pD3DXMtrlBuffer;
	if( FAILED(hr) ) {
		goto ERROR_END;
	}

	DWORD opt = m_pMesh->GetOptions();
	DWORD dw32BitFlag = (opt & D3DXMESH_32BIT);

	// We need to extract the material properties and texture names from 
	// the l_pD3DXMtrlBuffer
	D3DXMATERIAL* d3dxMaterials = (D3DXMATERIAL*)l_pD3DXMtrlBuffer->GetBufferPointer();

	m_arMeshMaterials = new D3DMATERIAL9[m_dwNumMaterials];
	m_arMeshTextures  = new LPDIRECT3DTEXTURE9[m_dwNumMaterials];
	memset(m_arMeshMaterials, 0, m_dwNumMaterials*sizeof(D3DMATERIAL9));
	memset(m_arMeshTextures,  0, m_dwNumMaterials*sizeof(LPDIRECT3DTEXTURE9));

	for( DWORD i=0; i < m_dwNumMaterials; i++ ) // only 1 Material{...} from tiger.x
	{
		sdring<TCHAR> tsTextureFilename = makeTsdring(d3dxMaterials[i].pTextureFilename);

		// Copy the material
		m_arMeshMaterials[i] = d3dxMaterials[i].MatD3D;

		// Set the ambient color for the material (D3DX does not do this)
		// Chj: I see .Diffuse = { .r=.g.=b=0.694118, .a=1.0 } from tiger.x
		m_arMeshMaterials[i].Ambient = m_arMeshMaterials[i].Diffuse;

		if( d3dxMaterials[i].pTextureFilename != NULL && 
			lstrlen(tsTextureFilename) > 0 )
		{
			// Find the path to the texture and create that texture
			DXUtil_FindMediaFileCch(strMediaPath, ARRAYSIZE(strMediaPath), tsTextureFilename);
			
			// Create the texture
			if( FAILED( D3DXCreateTextureFromFile( m_pd3dDevice, 
				strMediaPath, &m_arMeshTextures[i] ) ) )
			{
				goto ERROR_END;
			}

		}
	}

	if ( !(m_pMesh->GetFVF() & D3DFVF_NORMAL) )
	{
		LPD3DXMESH   l_pTempMesh = NULL;

		hr = m_pMesh->CloneMeshFVF( dw32BitFlag | D3DXMESH_MANAGED, 
			m_pMesh->GetFVF() | D3DFVF_NORMAL, m_pd3dDevice, 
			&l_pTempMesh );
		
		if (FAILED(hr))	{
			assert(!l_pTempMesh);
			goto ERROR_END;
		}

		D3DXComputeNormals( l_pTempMesh, NULL );

		m_pMesh->Release();
		m_pMesh = l_pTempMesh;
	}

	// used for debugging
	//D3DVERTEXELEMENT9 meshDeclaration[MAX_FVF_DECL_SIZE];
	//m_pMesh->GetDeclaration( meshDeclaration);


	m_pFont->InitDeviceObjects( m_pd3dDevice );
	m_pFontSmall->InitDeviceObjects( m_pd3dDevice );

	ChjRestoreSceneInit();

	return S_OK;

ERROR_END:

	SafeReleaseMesh();
	return hr;
}


//-----------------------------------------------------------------------------
// Name: RestoreDeviceObjects()
// Desc: Initialize scene objects.
//-----------------------------------------------------------------------------
HRESULT CMyD3DApplication::RestoreDeviceObjects()
{
	HRESULT hr = 0;

	LPD3DXBUFFER pShader = NULL;
	Cec_Release cec_Shader;

	const TCHAR *fxfile = _T("HLSL_Glow.fx");
	DWORD shader_flags = D3DXSHADER_DEBUG | D3DXSHADER_SKIPOPTIMIZATION;

	// Compile the texturing shader
	hr = D3DXCompileShaderFromFile_dbg(fxfile,
		NULL, // a NULL terminated array of D3DXMACROs
		NULL, // a #include handler
		"VS_HLSL_Texture",  
		"vs_1_1",
		shader_flags,
		&pShader, 
		NULL, // error messages 
		&m_pTexture_ConstantTable );
	cec_Shader = pShader;
	if(FAILED(hr)) {
		goto ERROR_END;
	}
	
	// Create the vertex shader
	hr = m_pd3dDevice->CreateVertexShader( 
			   (DWORD*)pShader->GetBufferPointer(), &m_pVS_Texture );
	if(FAILED(hr)) {
		goto ERROR_END;
	}

	// Compile the glow shader
	hr = D3DXCompileShaderFromFile_dbg(fxfile,
		NULL, // a NULL terminated array of D3DXMACROs
		NULL, // a #include handler
		"VS_HLSL_Glow",  
		"vs_1_1",
		shader_flags,
		&pShader, 
		NULL, // error messages 
		&m_pGlow_ConstantTable );
	cec_Shader = pShader;
	if(FAILED(hr)) {
		goto ERROR_END;
	}
	
	// Create the vertex shader
	hr = m_pd3dDevice->CreateVertexShader( 
			   (DWORD*)pShader->GetBufferPointer(), &m_pVS_Glow );
	if(FAILED(hr)) {
		goto ERROR_END;
	}


   // Build background image vertex buffer
	 hr = m_pd3dDevice->CreateVertexBuffer( 6*sizeof(float)*4, 0,
		D3DFVF_XYZRHW | D3DFVF_TEX1, D3DPOOL_MANAGED, &m_pVBBackground, NULL );
	if( FAILED(hr) ) {
		goto ERROR_END;
	}

	// Set up a set of points which represents the screen
	const float fPend = 750.0; // pending to set, casual init-value
	const float fHalf = 0.5;   // half-pixel/pixel-center tuning const
	const float fNoUse = 0; // bcz SetRenderState( D3DRS_ZENABLE, FALSE )
	static struct { float x,y,z,w; float u,v; } s_Verts[] =
	{
		{fPend,  -fHalf,   fNoUse, 1.0f,  1, 0}, // at (400, 0)
		{fPend,   fPend,   fNoUse, 1.0f,  1, 1}, // at (400, 300)
		{-fHalf, -fHalf,   fNoUse, 1.0f,  0, 0}, // at (0, 0)
		{-fHalf,  fPend,   fNoUse, 1.0f,  0, 1}, // at (0, 300)
	};

	s_Verts[0].x = (float)m_d3dsdBackBuffer.Width  - fHalf;
	s_Verts[1].x = (float)m_d3dsdBackBuffer.Width  - fHalf;
	s_Verts[1].y = (float)m_d3dsdBackBuffer.Height - fHalf;
	s_Verts[3].y = (float)m_d3dsdBackBuffer.Height - fHalf; 
 
	// Copy them into the buffer
	if(1)
	{
		void *pVerts = NULL;
		hr = m_pVBBackground->Lock( 0, sizeof(s_Verts), (void**)&pVerts, 0 );
		if ( FAILED(hr) )
			goto ERROR_END;

		memcpy( pVerts, s_Verts, sizeof(s_Verts) );
		m_pVBBackground->Unlock();
	}


	m_pFont->RestoreDeviceObjects();
	m_pFontSmall->RestoreDeviceObjects();

	// Setup render states (Chj: NOT required for this demo)
//	m_pd3dDevice->SetRenderState( D3DRS_LIGHTING, TRUE );
//	m_pd3dDevice->SetRenderState( D3DRS_CULLMODE, D3DCULL_NONE );
//	m_pd3dDevice->SetRenderState( D3DRS_ZWRITEENABLE, TRUE );

	// smooth out the texture map transitions at high magnification
	m_pd3dDevice->SetSamplerState( 0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR );
	m_pd3dDevice->SetSamplerState( 0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR );
	m_pd3dDevice->SetSamplerState( 0, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR );

	return S_OK;

ERROR_END:

	SafeReleaseDevice();
	return hr;
}


//-----------------------------------------------------------------------------
// Name: InvalidateDeviceObjects()
// Desc: Will be called after user drags window border to change window size.
//-----------------------------------------------------------------------------
HRESULT CMyD3DApplication::InvalidateDeviceObjects()
{
	SafeReleaseDevice();

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
	SafeReleaseDevice();
	SafeReleaseMesh();

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
	SafeReleaseDevice();
	SafeReleaseMesh();

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

