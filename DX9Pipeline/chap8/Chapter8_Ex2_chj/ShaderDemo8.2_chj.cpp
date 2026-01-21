//-----------------------------------------------------------------------------
// File: VertexShader8.2_chj.cpp
// Updates:
// * Input parameter to tell which .fx file to use, instead of default HLSL_MetallicFlakes.fx .
// * Some error messages dumps to DbgView .
// * Verbose debugging data dumps into DXP9-Ex8-2.log, always.
// * For D3DXCreateVolumeTexture(), noisepx is reduce to 16 (was 32).
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

//#include <fsapi.h>
//#include <mmlogfile.h>
//#include <utf8wchar.h>

#include "../BookCommon/chjshare.h"
#include "../BookCommon/chj_d3d9_dump.h"


FileDbgDump gfdump(_T("DXP9-Ex8-2.log"), false, 8192);

const TCHAR *g_filepath_fx = _T("HLSL_MetallicFlakes.fx");


//-----------------------------------------------------------------------------
// Name: class CMyD3DApplication
// Desc: Application class. The base class (CD3DApplication) provides the 
//       generic functionality needed in all Direct3D samples. CMyD3DApplication 
//       adds functionality specific to this sample program.
//-----------------------------------------------------------------------------
class CMyD3DApplication : public CD3DApplication
{
	// texture vertex shader
	LPDIRECT3DVERTEXSHADER9        m_pVS;
	LPD3DXCONSTANTTABLE            m_pVSConstantTable;
	LPDIRECT3DPIXELSHADER9         m_pPS;
	LPD3DXCONSTANTTABLE            m_pPSConstantTable;

	// env & noise map
	LPDIRECT3DCUBETEXTURE9	       m_pEnvironmentMap;    
	LPDIRECT3DVOLUMETEXTURE9	   m_pNoiseMap;    

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
		SAFE_RELEASE(m_pVS);
		SAFE_RELEASE(m_pVSConstantTable);
		SAFE_RELEASE(m_pPS);
		SAFE_RELEASE(m_pPSConstantTable);
		
		SAFE_RELEASE(m_pEnvironmentMap);
		SAFE_RELEASE(m_pNoiseMap);
	}

	void SafeReleaseMesh()
	{
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

	if(argc>1)
	{
		g_filepath_fx = argv[1];
		vaDbgTs(_T("Will use custom HLSL .fx file '%s'"), g_filepath_fx);
	}

//	gfdump.vaDbg(_T("Chj test %d"), 11);
//	gfdump.vaDbg(_T("Chj test %d"), 22);

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
	m_pVS	           = NULL;
	m_pPS	           = NULL;
	m_pVSConstantTable = NULL;
	m_pPSConstantTable = NULL;

	m_pEnvironmentMap  = NULL;    
	m_pNoiseMap        = NULL;    

	m_pMesh          = NULL; // Our mesh object in sysmem	
	m_arMeshMaterials = NULL; // Materials for our mesh
	m_arMeshTextures  = NULL; // Textures for our mesh
	m_dwNumMaterials = 0L;   // Number of mesh materials

	static TCHAR szTitle[MAX_PATH];
	_sntprintf_s(szTitle, _TRUNCATE, _T("Ex8-2 \"%s\""), g_filepath_fx);
	m_strWindowTitle    = szTitle;
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
	D3DXVECTOR3 vEye = D3DXVECTOR3( 15.0f, 0, -10.0f ); 
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
		// Initialize the uniform constants
		m_pVSConstantTable->SetDefaults(m_pd3dDevice); 
		m_pPSConstantTable->SetDefaults(m_pd3dDevice); 

		// Initialize the shader matrices using application matrices
		D3DXMATRIXA16 matWorldView;
		D3DXMatrixMultiply(&matWorldView, &m_matWorld, &m_matView);
		m_pVSConstantTable->SetMatrix(m_pd3dDevice, "WorldView", &matWorldView);
		m_pVSConstantTable->SetMatrix(m_pd3dDevice, "Projection", &m_matProj); 

		m_pd3dDevice->SetTexture(0, m_pNoiseMap);
		m_pd3dDevice->SetTexture(1, m_pEnvironmentMap);

		// Set the shaders
		m_pd3dDevice->SetVertexShader( m_pVS );
		m_pd3dDevice->SetPixelShader( m_pPS );

		// Draw the mesh
		for( DWORD i=0; i < m_dwNumMaterials; i++ ) // We see 5 materials from bigship1.x
		{
			// Use mesh material colors to set the 
			// shader ambient and diffuse material colors
			
			m_pVSConstantTable->SetVector(m_pd3dDevice, "k_a", 
				(D3DXVECTOR4 *)(FLOAT *)D3DXCOLOR(m_arMeshMaterials[i].Ambient));

			m_pVSConstantTable->SetVector(m_pd3dDevice, "k_d", 
				(D3DXVECTOR4 *)(FLOAT *)D3DXCOLOR(m_arMeshMaterials[i].Diffuse));

			// Draw the mesh subset
			m_pMesh->DrawSubset( i );
		}

		m_pd3dDevice->SetTexture(0, NULL);
		m_pd3dDevice->SetTexture(1, NULL);			

		// Output statistics
		m_pFont->DrawText( 2,  0, D3DCOLOR_ARGB(255,255,255,0), m_strFrameStats );
		m_pFont->DrawText( 2, 20, D3DCOLOR_ARGB(255,255,255,0), m_strDeviceStats );

		if( m_bShowHelp )
		{
			m_pFontSmall->DrawText(  2, 40, D3DCOLOR_ARGB(255,255,255,255),
									_T("Keyboard controls:") );
			m_pFontSmall->DrawText( 20, 60, D3DCOLOR_ARGB(255,255,255,255),
									_T("Far,Near\nTurn\nPitch\nSlide\n")
									_T("Rotate spaceship\n") );
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


	TCHAR        strMediaPath[512] = {};
	LPD3DXBUFFER l_pD3DXMtrlBuffer = NULL;
	HRESULT hr = 0;

   // Find the path to the mesh
	hr = DXUtil_FindMediaFileCch( strMediaPath, ARRAYSIZE(strMediaPath), 
		TEXT("bigship1.x")
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

	for( DWORD i=0; i < m_dwNumMaterials; i++ )
	{
		sdring<TCHAR> tsTextureFilename = makeTsdring(d3dxMaterials[i].pTextureFilename);

		// Copy the material
		m_arMeshMaterials[i] = d3dxMaterials[i].MatD3D;

		// Set the ambient color for the material (D3DX does not do this)
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

	LPD3DXMESH l_pTempMesh = NULL;
	bool force_recalculate_Normals = false; // fiddle-debug-purpose
	
	DWORD fvf = m_pMesh->GetFVF();

#if 11
	
	if ( !(fvf & D3DFVF_NORMAL) )
	{
		// Chj memo: The default bigship1.x has MeshNormals{...} data block, 
		// so this if-block will not be executed.
		// You can delete MeshNormals{...} from bigship1.x to activate this if-block.

		hr = m_pMesh->CloneMeshFVF( dw32BitFlag | D3DXMESH_MANAGED, 
			fvf | D3DFVF_NORMAL, 
			m_pd3dDevice, 
			&l_pTempMesh );
		
		if (FAILED(hr))	{
			assert(!l_pTempMesh);
			goto ERROR_END;
		}

		gfdump.vaDbg(_T("In CMyD3DApplication::InitDeviceObjects(), call D3DXComputeNormals()."));

		D3DXComputeNormals( l_pTempMesh, NULL );

		m_pMesh->Release();
		m_pMesh = l_pTempMesh;
		l_pTempMesh = NULL;
	}
	else if(force_recalculate_Normals)
	{
		// Overwrite MeshNormals{...} data from bigship1.x, but re-calculate Vertex-Normals
		// for each vertex, via triangle faces from bigship1.x .

		gfdump.vaDbg(_T("In CMyD3DApplication::InitDeviceObjects(), force call D3DXComputeNormals()."));

		D3DXComputeNormals(m_pMesh, NULL);
	}


	// Chj debug code >>>

	fvf = m_pMesh->GetFVF();
	assert(fvf == (D3DFVF_XYZ|D3DFVF_NORMAL|D3DFVF_TEX1)); // 0x112, bigship1.x should have these

	gfdump.vaDbg(_T("In CMyD3DApplication::InitDeviceObjects(), Dump Mesh with Normal elements."));
	dumpMeshVertex_with_Format_0x112(&gfdump, m_pMesh);

	// Chj debug code <<<
#endif


#if 12
	//
	// Expand the mesh to hold tangent data
	//

	// [2026-01-15] Chj memo: Bcz Tangent element is not supported in FVF (Flexible Vertex Format),
	// so in order to add Tangent data, we need to declare a new set of vertex format, and
	// call CloneMesh().  CloneMeshFVF() does NOT help here.
	
	D3DVERTEXELEMENT9 decl[] =
	{
		{ 0, 0, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0 }, 
		{ 0, 12, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_NORMAL, 0 },  
		{ 0, 24, D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 0 },
		
		{ 0, 32, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TANGENT, 0 }, 
		// -- Tangent is new in Ex8-2.

		D3DDECL_END()
	};

	assert(l_pTempMesh==NULL);

	hr = m_pMesh->CloneMesh( dw32BitFlag | D3DXMESH_MANAGED, decl, m_pd3dDevice, 
		&l_pTempMesh );
	if (FAILED(hr)) {
		goto ERROR_END;
	}

	hr = D3DXComputeTangent( l_pTempMesh, // input mesh 
		0, // TexStageIndex 
		0, // TangentIndex 
		0, // BinormIndex 
		0, // Wrap 
		NULL // Adjacency 
		);

	// Let m_pMesh points to the cloned-mesh, release old one.
	m_pMesh->Release();
	m_pMesh = l_pTempMesh;

	// Chj debug code >>>
	gfdump.vaDbg(_T("In CMyD3DApplication::InitDeviceObjects(), Dump Mesh with Tangent elements."));
	dumpMeshVertex_Ex8_2_tangent(&gfdump, m_pMesh);
	// Chj debug code <<<

#endif

	// used for debugging
	//D3DVERTEXELEMENT9 meshDeclaration[MAX_FVF_DECL_SIZE];
	//m_pMesh->GetDeclaration( meshDeclaration );

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

	const TCHAR *fxfile = g_filepath_fx;
	DWORD shader_flags = D3DXSHADER_DEBUG | D3DXSHADER_SKIPOPTIMIZATION;

	// Compile the vertex shader
	hr = D3DXCompileShaderFromFile_dbg(fxfile,
		NULL, // a NULL terminated array of D3DXMACROs
		NULL, // a #include handler
		"VS_Sparkle",  
		"vs_1_1",
		shader_flags,
		&pShader, 
		NULL, // error messages 
		&m_pVSConstantTable );
	cec_Shader = pShader;
	if(FAILED(hr)) {
		goto ERROR_END;
	}
	
	// Create the vertex shader
	hr = m_pd3dDevice->CreateVertexShader( 
			   (DWORD*)pShader->GetBufferPointer(), &m_pVS );
	if(FAILED(hr)) {
		goto ERROR_END;
	}

	// Compile the pixel shader
	hr = D3DXCompileShaderFromFile_dbg(fxfile,
		NULL, // a NULL terminated array of D3DXMACROs
		NULL, // a #include handler
		"PS_Sparkle",  
		"ps_1_1",
		shader_flags,
		&pShader, 
		NULL, // error messages 
		&m_pPSConstantTable );
	cec_Shader = pShader; // would Release() old shader-code
	if(FAILED(hr)) {
		goto ERROR_END;
	}
	
	// Create the pixel shader
	hr = m_pd3dDevice->CreatePixelShader( 
			   (DWORD*)pShader->GetBufferPointer(), &m_pPS );
	if(FAILED(hr)) {
		goto ERROR_END;
	}


	//// ==== NEW from Ex8-2 >>>

	// Create the noise map (procedural texture)
	const int noisepx = 16;
	hr = D3DXCreateVolumeTexture(
		m_pd3dDevice,
		noisepx, noisepx, noisepx,       // width, height, depth
		1,                // mip levels
		0,                // usage
		D3DFMT_UNKNOWN,   // format
		D3DPOOL_MANAGED,  // memory pool
		&m_pNoiseMap);
	if(FAILED(hr)) {
		goto ERROR_END;
	}

	hr = D3DXCompileShaderFromFile_dbg(fxfile,
		NULL, // a NULL terminated string of D3DXMACROs
		NULL, // a #include handler
		"GenerateSparkle",  
		"tx_1_0", 
		D3DXSHADER_DEBUG,
		&pShader, 
		NULL,    // error messages 
		NULL );  // constant table pointer
	cec_Shader = pShader; // would Release() old shader-code
	if(FAILED(hr)) {
		goto ERROR_END;
	}

	// Procedurally fill texture
	// Chj memo: Run GenerateSparkle() code to fill texture into m_pNoiseMap.
	hr = D3DXFillVolumeTextureTX(m_pNoiseMap, 
		(CONST DWORD*)pShader->GetBufferPointer(), NULL, 0);
	if( FAILED(hr) ) {
		goto ERROR_END;
	}

	dumpVolumeTexture(&gfdump, m_pNoiseMap); // Chj test code


	// Set the sampler state for the noise map
	m_pd3dDevice->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_POINT);
	m_pd3dDevice->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_POINT);
	m_pd3dDevice->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_POINT);

	// Create the cubic environment map
	TCHAR strMediaPath[512] = {};
	hr = DXUtil_FindMediaFileCch( strMediaPath, ARRAYSIZE(strMediaPath), 
		TEXT("lobbycube.dds") );
	if( FAILED(hr) ) {
		hr = D3DAPPERR_MEDIANOTFOUND;
		goto ERROR_END;
	}

	hr = D3DXCreateCubeTextureFromFile( m_pd3dDevice, strMediaPath, 
		&m_pEnvironmentMap);
	if( FAILED(hr) ) {
		goto ERROR_END;
	}

	// Set up the sampler state for the environment map
	m_pd3dDevice->SetSamplerState(1, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
	m_pd3dDevice->SetSamplerState(1, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
	m_pd3dDevice->SetSamplerState(1, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR);

	//// ==== NEW from Ex8-2 <<<


	m_pFont->RestoreDeviceObjects();
	m_pFontSmall->RestoreDeviceObjects();

	// Setup render states
	m_pd3dDevice->SetRenderState( D3DRS_LIGHTING, FALSE );
	m_pd3dDevice->SetRenderState( D3DRS_CULLMODE, D3DCULL_NONE );
	// m_pd3dDevice->SetRenderState( D3DRS_ZWRITEENABLE, TRUE ); // no this in Ex8-2

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

	if( pCaps->PixelShaderVersion < D3DPS_VERSION(1,1) ) // Ex8-2
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

