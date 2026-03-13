
#ifndef __ROADRAGE_H
#define __ROADRAGE_H

#include "D3DFrame.h"
#include "D3DEnum.h"
#include "D3DUtil.h"
#include "D3DRes.h"

#include "world.hpp" // in share subdir

//-----------------------------------------------------------------------------
// Defines, constants, and global variables
//-----------------------------------------------------------------------------
 
#define WALL_MESH_SIZE      12
#define NUM_WALL_VERTICES   (WALL_MESH_SIZE*WALL_MESH_SIZE)
#define NUM_WALL_INDICES    ((WALL_MESH_SIZE-1)*(WALL_MESH_SIZE-1)*6)

#define NPSP_2   2  // north-pole and south-pole cost 2 vertices
#define SPHERE_MESH_SIZE_X    8
#define SPHERE_MESH_SIZE_Y    4
#define NUM_SPHERE_VERTICES (NPSP_2 + SPHERE_MESH_SIZE_X*SPHERE_MESH_SIZE_Y)
#define NUM_SPHERE_INDICES  ( \
	(SPHERE_MESH_SIZE_X*2 + SPHERE_MESH_SIZE_X*(SPHERE_MESH_SIZE_Y-1)*2) \
	* 3 ) // 3 vertices per triangle

#define rnd() ( ( ((FLOAT)rand())-((FLOAT)rand()) ) / RAND_MAX )

#define MAX_NUM_QUADS			20000
#define MAX_NUM_TRIANGLES		20000
#define MAX_NUM_FACE_INDICES	20000 
#define MAX_NUM_VERTICES		40000

class ParamDialog;


//-----------------------------------------------------------------------------
// Name: class CMyD3DApplication
// Desc: Application class. The base class provides just about all the
//       functionality we want, so we're just supplying stubs to interface with
//       the non-C++ functions of the app.
//-----------------------------------------------------------------------------

class CMyD3DApplication : public CD3DApplication
{

    D3DLIGHTTYPE m_dltType;
    VOID    Cleanup3DEnvironment();

	// Chj >>>
	
	ParamDialog *m_ppbox;

	FLOAT m_x, m_y, m_z; // to support animation on/off

	D3DLIGHT7 m_light;
	float m_camX_waggle; // in radian

	// Chj <<<

protected:
	HRESULT OneTimeSceneInit();  // Chap5
	HRESULT InitDeviceObjects(); // Init 3D objecs in scene
	HRESULT DeleteDeviceObjects();
	HRESULT Render();
	HRESULT FrameMove( FLOAT fTimeKey ); // Chap5
	HRESULT FinalCleanup();

public:
	
    CMyD3DApplication();

	BOOL	m_bBufferPaused;    
    BOOL    m_bWindowed;
    BOOL    m_bToggleFullscreen;
	BOOL	IsRenderingOk;
	void	DisplayCredits(HWND hwnd);
	void	DisplayRRStats(HWND hwnd);
	void	DisplayLegalInfo(HWND hwnd);

//  VOID    OutputText( DWORD x, DWORD y, TCHAR* str );
	HRESULT Render3DEnvironment();

	LRESULT MsgProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );

	// Chap5 >>>
	D3DVERTEX    m_WallVertices[NUM_WALL_VERTICES];
	WORD         m_WallIndices[NUM_WALL_INDICES];
	D3DVERTEX    m_SphereVertices[NUM_SPHERE_VERTICES];
	WORD         m_SphereIndices[NUM_SPHERE_INDICES];

	setupinfo_ptr setupinfo;

	BOOL RRAppActive;
	// Chap5 <<<

	static HRESULT hr;

	// Chap5+
	D3DVALUE		angx, angy, angz; 
	D3DVALUE		look_up_ang;
	D3DVECTOR       m_vEyePt;
	D3DVECTOR       m_vLookatPt;
	D3DVECTOR       m_vUpVec;
	FLOAT           m_fEyeDistance; // Offset of each eye from m_vEyePt
	BOOL            m_bUseViewPoints;

	int current_mode;
	int toggle_delay ;
	int current_screen_status; // default, in case rrsetup.ini 

	int curr_seq;
	char szDevice_debug[256];

	D3DMATRIX trans, roty;
	D3DMATRIX WorldMatrix, ViewMatrix, ProjMatrix;
	D3DVALUE posx, posy, posz; //Player position
	D3DVECTOR from, at, up; // Used to set view matrix
	D3DVALUE Z_NEAR;
	D3DVALUE Z_FAR;
	D3DVALUE FOV; 
	D3DVALUE ASPECT_RATIO;
	int num_dp_commands_in_scene;

	DWORD intensity;
	BOOL bEnableLighting;
	D3DSHADEMODE ShadeMode;

	HINSTANCE hInstApp;
};

#endif //__ROADRAGE_H