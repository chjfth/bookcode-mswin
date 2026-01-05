//-----------------------------------------------------------------------------
// File: VertexShader8.2.cpp
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




//-----------------------------------------------------------------------------
// Name: class CMyD3DApplication
// Desc: Application class. The base class (CD3DApplication) provides the 
//       generic functionality needed in all Direct3D samples. CMyD3DApplication 
//       adds functionality specific to this sample program.
//-----------------------------------------------------------------------------
class CMyD3DApplication : public CD3DApplication
{
	LPDIRECT3DVERTEXSHADER9        m_pVS;
	LPD3DXCONSTANTTABLE            m_pVSConstantTable;
	LPDIRECT3DPIXELSHADER9         m_pPS;
	LPD3DXCONSTANTTABLE            m_pPSConstantTable;

    LPDIRECT3DCUBETEXTURE9	       m_pEnvironmentMap;    
    LPDIRECT3DVOLUMETEXTURE9	   m_pNoiseMap;    

	// Mesh
	TCHAR m_strInitialDir[256];
	LPD3DXMESH           m_pMesh;           // Our mesh object in sysmem	
	D3DMATERIAL9*        m_pMeshMaterials;  // Materials for our mesh
	LPDIRECT3DTEXTURE9*  m_pMeshTextures;   // Textures for our mesh
	DWORD                m_dwNumMaterials;  // Number of mesh materials

	// Font for drawing text
    CD3DFont* m_pFont;
    CD3DFont* m_pFontSmall;

    // Transforms
    D3DXMATRIXA16  m_matPosition;
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
	m_pVS	           = NULL;
	m_pPS	           = NULL;
	m_pVSConstantTable = NULL;
    m_pPSConstantTable = NULL;
	
	m_pEnvironmentMap  = NULL;    
    m_pNoiseMap        = NULL;    

	m_pMesh            = NULL; // Our mesh object in sysmem	
	m_pMeshMaterials   = NULL; // Materials for our mesh
	m_pMeshTextures    = NULL; // Textures for our mesh
	m_dwNumMaterials   = 0L;   // Number of mesh materials

	m_strWindowTitle   = _T("VertexShader");
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

    if( m_bKey[VK_LEFT] || m_bKey[VK_NUMPAD1] )                 vT.x -= 1.0f; // Slide Left
    if( m_bKey[VK_RIGHT] || m_bKey[VK_NUMPAD3] )                vT.x += 1.0f; // Slide Right
    if( m_bKey[VK_DOWN] )                                       vT.y -= 1.0f; // Slide Down
    if( m_bKey[VK_UP] )                                         vT.y += 1.0f; // Slide Up
    if( m_bKey['W'] )                                           vT.z -= 2.0f; // Move Forward
    if( m_bKey['S'] )                                           vT.z += 2.0f; // Move Backward
    if( m_bKey['A'] || m_bKey[VK_NUMPAD8] )                     vR.x -= 1.0f; // Pitch Down
    if( m_bKey['Z'] || m_bKey[VK_NUMPAD2] )                     vR.x += 1.0f; // Pitch Up
    if( m_bKey['E'] || m_bKey[VK_NUMPAD6] )                     vR.y -= 1.0f; // Turn Right
    if( m_bKey['Q'] || m_bKey[VK_NUMPAD4] )                     vR.y += 1.0f; // Turn Left
    if( m_bKey[VK_NUMPAD9] )                                    vR.z -= 2.0f; // Roll CW
    if( m_bKey[VK_NUMPAD7] )                                    vR.z += 2.0f; // Roll CCW

    m_vVelocity        = m_vVelocity * 0.9f + vT * 0.1f;
    m_vAngularVelocity = m_vAngularVelocity * 0.9f + vR * 0.1f;

    // Update position and view matricies
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
		// Initialize the uniform constants
		m_pVSConstantTable->SetDefaults(m_pd3dDevice); 
		m_pPSConstantTable->SetDefaults(m_pd3dDevice); 

		// Initialize the shader matrices using application matrices
		m_pVSConstantTable->SetMatrix(m_pd3dDevice, "WorldView", &m_matView); 
        m_pVSConstantTable->SetMatrix(m_pd3dDevice, "Projection", &m_matProj); 

        m_pd3dDevice->SetTexture(0, m_pNoiseMap);
        m_pd3dDevice->SetTexture(1, m_pEnvironmentMap);

        // Set the shaders
		m_pd3dDevice->SetVertexShader( m_pVS );
		m_pd3dDevice->SetPixelShader( m_pPS );

		// Draw the mesh
		for( DWORD i=0; i < m_dwNumMaterials; i++ )
		{
            // Use mesh material colors to set the 
			// shader ambient and diffuse material colors
            m_pVSConstantTable->SetVector(m_pd3dDevice, "k_a", 
				(D3DXVECTOR4 *)(FLOAT *)D3DXCOLOR(m_pMeshMaterials[i].Ambient));
            m_pVSConstantTable->SetVector(m_pd3dDevice, "k_d", 
				(D3DXVECTOR4 *)(FLOAT *)D3DXCOLOR(m_pMeshMaterials[i].Diffuse));

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
                                    _T("Move\nTurn\nPitch\nSlide\n")
                                    _T("Help\nChange device\nExit") );
            m_pFontSmall->DrawText( 210, 60, D3DCOLOR_ARGB(255,255,255,255),
                                    _T("W,S\nE,Q\nA,Z\nArrow keys\n")
                                    _T("F1\nF2\nEsc") );
        }
        else
        {
            m_pFontSmall->DrawText(  2, 40, D3DCOLOR_ARGB(255,255,255,255), 
                               _T("Press F1 for help") );
        }


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
    TCHAR        strMediaPath[512];
	LPD3DXBUFFER l_pD3DXMtrlBuffer;
    LPD3DXMESH   l_pTempMesh;

   // Find the path to the mesh
    if( FAILED( DXUtil_FindMediaFileCb( strMediaPath, 
	     sizeof(strMediaPath), TEXT("bigship1.x") ) ) )
	{
		return D3DAPPERR_MEDIANOTFOUND;
	}

	// Load the mesh from the specified file
    if( FAILED( D3DXLoadMeshFromX( strMediaPath, D3DXMESH_SYSTEMMEM, 
                                   m_pd3dDevice, NULL, 
                                   &l_pD3DXMtrlBuffer, NULL, 
								   &m_dwNumMaterials, &m_pMesh ) ) )
	{
	    SAFE_RELEASE(l_pD3DXMtrlBuffer);
	    SAFE_RELEASE(m_pMesh);
	}

	DWORD        dw32BitFlag;
    dw32BitFlag = (m_pMesh->GetOptions() & D3DXMESH_32BIT);

    // Extract the mesh material properties and texture names
    D3DXMATERIAL* d3dxMaterials = 
		(D3DXMATERIAL*)l_pD3DXMtrlBuffer->GetBufferPointer();
    m_pMeshMaterials = new D3DMATERIAL9[m_dwNumMaterials];
    m_pMeshTextures  = new LPDIRECT3DTEXTURE9[m_dwNumMaterials];

    for( DWORD i=0; i < m_dwNumMaterials; i++ )
    {
        // Copy the material
        m_pMeshMaterials[i] = d3dxMaterials[i].MatD3D;

        // Set the ambient color for the material (D3DX does not do this)
        m_pMeshMaterials[i].Ambient = m_pMeshMaterials[i].Diffuse;

        m_pMeshTextures[i] = NULL;
        if( d3dxMaterials[i].pTextureFilename != NULL && 
            lstrlen(d3dxMaterials[i].pTextureFilename) > 0 )
        {
	        // Find the path to the texture and create that texture
		    DXUtil_FindMediaFileCb( strMediaPath, sizeof(strMediaPath), 
				d3dxMaterials[i].pTextureFilename );
            
			// Create the texture
	        if( FAILED( D3DXCreateTextureFromFile( m_pd3dDevice, 
				strMediaPath, &m_pMeshTextures[i] ) ) )
			{
	            m_pMeshTextures[i] = NULL;
			}

		}
	}

	HRESULT hr;

	// Useful for reading the mesh declaration
	//D3DVERTEXELEMENT9 declaration[MAX_FVF_DECL_SIZE];
	//m_pMesh->GetDeclaration(declaration);

	if ( !(m_pMesh->GetFVF() & D3DFVF_NORMAL) )
    {
        hr = m_pMesh->CloneMeshFVF( dw32BitFlag | D3DXMESH_MANAGED, 
			m_pMesh->GetFVF() | D3DFVF_NORMAL, 
			m_pd3dDevice, &l_pTempMesh );
        if (FAILED(hr))
		{
			SAFE_RELEASE( l_pTempMesh );
		}

        D3DXComputeNormals( l_pTempMesh, NULL );

        m_pMesh->Release();
        m_pMesh = l_pTempMesh;
    }

	// Expand the mesh to hold tangent data
    D3DVERTEXELEMENT9 decl[] =
    {
        { 0, 0, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0 }, 
        { 0, 12, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_NORMAL, 0 },  
        { 0, 24, D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 0 },
        { 0, 32, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TANGENT, 0 }, 
        D3DDECL_END()
    };

    hr = m_pMesh->CloneMesh( dw32BitFlag | D3DXMESH_MANAGED, decl, m_pd3dDevice, 
		&l_pTempMesh );
    if (FAILED(hr))
	{
		SAFE_RELEASE(l_pTempMesh);	
		return hr;
	}

    hr = D3DXComputeTangent( l_pTempMesh, // input mesh 
		0, // TexStageIndex 
		0, // TangentIndex 
		0, // BinormIndex 
		0, // Wrap 
		NULL // Adjacency 
		);

    m_pMesh->Release();
    m_pMesh = l_pTempMesh;

    // Done with the material buffer
    l_pD3DXMtrlBuffer->Release();
    l_pD3DXMtrlBuffer = NULL;


    m_pFont->InitDeviceObjects( m_pd3dDevice );
    m_pFontSmall->InitDeviceObjects( m_pd3dDevice );


	D3DXVECTOR3 vEye = D3DXVECTOR3( 15.0f, 0.0f, -10.0f ); 
    D3DXVECTOR3 vAt  = D3DXVECTOR3( 0.0f, 0.0f, 0.0f );
    D3DXVECTOR3 vUp  = D3DXVECTOR3( 0.0f, 1.0f, 0.0f );
    D3DXMatrixLookAtLH( &m_matView, &vEye, &vAt, &vUp );

    // Set the position matrix
    D3DXMatrixInverse( &m_matPosition, NULL, &m_matView );

    // Set up the projection matrix
    FLOAT fAspectRatio = 
		(FLOAT)m_d3dsdBackBuffer.Width / (FLOAT)m_d3dsdBackBuffer.Height;
    D3DXMatrixPerspectiveFovLH( &m_matProj, D3DXToRadian(60.0f), 
		fAspectRatio, 0.1f, 100.0f );
    m_pd3dDevice->SetTransform( D3DTS_PROJECTION, &m_matProj );

    return S_OK;
}




//-----------------------------------------------------------------------------
// Name: RestoreDeviceObjects()
// Desc: Initialize scene objects.
//-----------------------------------------------------------------------------
HRESULT CMyD3DApplication::RestoreDeviceObjects()
{
	HRESULT hr;
	LPD3DXBUFFER pShader = NULL;

	// Compile the vertex shader
	hr = D3DXCompileShaderFromResource(
        NULL,
        MAKEINTRESOURCE(ID_HLSL_METALLICFLAKES),
		NULL, // a NULL terminated string of D3DXMACROs
		NULL, // a #include handler
		"VS",  
		"vs_1_1",
		D3DXSHADER_DEBUG,
		&pShader, 
		NULL, // error messages 
		&m_pVSConstantTable );

	if(FAILED(hr))
	{
		SAFE_RELEASE(pShader);
		SAFE_RELEASE(m_pVSConstantTable);
		return hr;
	}

	// Create the vertex shader
    hr = m_pd3dDevice->CreateVertexShader( 
		(DWORD*)pShader->GetBufferPointer(), &m_pVS );

	if(FAILED(hr))
	{
		SAFE_RELEASE(pShader);
		SAFE_RELEASE(m_pVSConstantTable);
		SAFE_RELEASE(m_pVS);
		return hr;
	}

	// Compile the pixel shader
	hr = D3DXCompileShaderFromResource(
        NULL,
        MAKEINTRESOURCE(ID_HLSL_METALLICFLAKES),
		NULL, // a NULL terminated string of D3DXMACROs
		NULL, // a #include handler
		"PS",  
		"ps_1_1", 
		D3DXSHADER_DEBUG,
		&pShader, 
		NULL, // error messages 
		&m_pPSConstantTable );

	if(FAILED(hr))
	{
		SAFE_RELEASE(pShader);
		SAFE_RELEASE(m_pPSConstantTable);
		return hr;
	}

	// Create the pixel shader
    hr = m_pd3dDevice->CreatePixelShader( 
		(DWORD*)pShader->GetBufferPointer(), &m_pPS );

	if(FAILED(hr))
	{
		SAFE_RELEASE(pShader);
		SAFE_RELEASE(m_pPSConstantTable);
		SAFE_RELEASE(m_pPS);
		return hr;
	}

    // Create the noise map (procedural texture)
	hr = D3DXCreateVolumeTexture(
		m_pd3dDevice,
		32, 32, 32,       // width, height, depth
		1,                // mip levels
		0,                // usage
		D3DFMT_UNKNOWN,   // format
		D3DPOOL_MANAGED,  // memory pool
		&m_pNoiseMap);

	if(FAILED(hr))
	{
		SAFE_RELEASE(m_pNoiseMap);
		return hr;
	}


	hr = D3DXCompileShaderFromResource(
        NULL,
        MAKEINTRESOURCE(ID_HLSL_METALLICFLAKES),
		NULL, // a NULL terminated string of D3DXMACROs
		NULL, // a #include handler
		"GenerateSparkle",  
		"tx_1_0", 
		D3DXSHADER_DEBUG,
		&pShader, 
		NULL,    // error messages 
		NULL );  // constant table pointer

	if(FAILED(hr))
	{
		SAFE_RELEASE(pShader);
		return hr;
	}

	// Procedurally fill texture
    hr = D3DXFillVolumeTextureTX(m_pNoiseMap, 
		(CONST DWORD*)pShader->GetBufferPointer(), NULL, 0);

    if( FAILED(hr) )
	{
		SAFE_RELEASE(pShader);
		SAFE_RELEASE(m_pNoiseMap);
		return hr;
	}

	// Set the sampler state for the noise map
	m_pd3dDevice->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_POINT);
	m_pd3dDevice->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_POINT);
	m_pd3dDevice->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_POINT);

    // Create the cubic environment map
    TCHAR        strMediaPath[512];
    if( FAILED( DXUtil_FindMediaFileCb( strMediaPath, sizeof(strMediaPath), 
		  TEXT("lobbycube.dds") ) ) )
	{
		return D3DAPPERR_MEDIANOTFOUND;
	}

	hr = D3DXCreateCubeTextureFromFile( m_pd3dDevice, strMediaPath, 
		&m_pEnvironmentMap);
    if( FAILED(hr) )
	{
		SAFE_RELEASE(m_pEnvironmentMap);
		return hr;
	}

	// Set up the sampler state for the environment map
	m_pd3dDevice->SetSamplerState(1, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
	m_pd3dDevice->SetSamplerState(1, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
	m_pd3dDevice->SetSamplerState(1, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR);


	m_pFont->RestoreDeviceObjects();
    m_pFontSmall->RestoreDeviceObjects();

    // Set up render states
    m_pd3dDevice->SetRenderState( D3DRS_LIGHTING, FALSE );
    m_pd3dDevice->SetRenderState( D3DRS_CULLMODE, D3DCULL_NONE );

	return S_OK;
}




//-----------------------------------------------------------------------------
// Name: InvalidateDeviceObjects()
// Desc:
//-----------------------------------------------------------------------------
HRESULT CMyD3DApplication::InvalidateDeviceObjects()
{
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
	SAFE_RELEASE( m_pVS );
	SAFE_RELEASE( m_pVSConstantTable );
	SAFE_RELEASE( m_pPS );
	SAFE_RELEASE( m_pPSConstantTable );
	SAFE_RELEASE( m_pNoiseMap );
	SAFE_RELEASE( m_pEnvironmentMap );

	if( m_pMeshMaterials != NULL ) 
        delete[] m_pMeshMaterials;

    if( m_pMeshTextures )
    {
        for( DWORD i = 0; i < m_dwNumMaterials; i++ )
        {
            if( m_pMeshTextures[i] )
                m_pMeshTextures[i]->Release();
        }
        delete[] m_pMeshTextures;
    }
    if( m_pMesh != NULL )
        m_pMesh->Release();

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
