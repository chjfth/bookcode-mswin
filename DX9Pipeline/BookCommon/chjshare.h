#pragma once

#include <assert.h>
#include <commdefs.h>
#include <_MINMAX_.h>
#include <sdring.h>
#include <makeTsdring.h>

#include <EnsureClnup_mswin.h>
#include <mm_snprintf.h>

#include <vaDbgTs.h>
#include <vaDbgTs_util.h>

#include <InterpretConst.h>

using namespace itc;
#include <DirectX\d3d9.itc.h>
#include <DirectX\d3dx9.itc.h>
#include <DirectX\directx_errors.itc.h>
#include <DirectX\d3d9caps.itc.h>
#include <DirectX\d3d9types.itc.h>


inline void ComPtr_Release(IUnknown *puk)
{ 
	puk->Release(); 
}
MakeDelega_CleanupPtr(Cec_Release, void, ComPtr_Release, IUnknown*);


inline int my_mm_vsnprintf(TCHAR *buf, size_t bufchars, const TCHAR *fmt, va_list args)
{
	return mm_vsnprintf(buf, (int)bufchars, fmt, args);
}


void mm_DebugProgress(void *ctx_user, const TCHAR *psz_dbginfo)
{
	static int si = 0;
	if(si==0)
	{
		si++;

		AllocConsole();

		freopen("CONOUT$", "w", stdout);
		freopen("CONIN$", "r", stdin);
		freopen("CONOUT$", "w", stderr);
	}

	_tprintf(_T("%s"), psz_dbginfo);
}

HRESULT D3DXCompileShaderFromFile_dbg(
	const TCHAR*                    pSrcFile,
	CONST D3DXMACRO*                pDefines,
	LPD3DXINCLUDE                   pInclude,
	const char*                    pFunctionName,
	const char*                    pTarget,
	DWORD                           Flags,
	LPD3DXBUFFER*                   ppShader,
	LPD3DXBUFFER*                   ppErrorMsgs, // user should pass in NULL
	LPD3DXCONSTANTTABLE*            ppConstantTable)
{
	// This wrapper grabs error-messages and vaDbgTs outputs them.
	// So the caller is hassle-free from ppErrorMsgs buffer.

	assert(ppErrorMsgs==NULL);

	LPD3DXBUFFER pErrBuf = NULL;	

	// Compile the vertex shader
	HRESULT hr = D3DXCompileShaderFromFile(
		pSrcFile,
		pDefines,
		pInclude,
		pFunctionName, // "VS_HLL_EX1",
		pTarget,       // "vs_1_1",
		Flags,
		ppShader, 
		&pErrBuf, 
		ppConstantTable);
	Cec_Release cec_errmsg = pErrBuf;

	if( FAILED(hr) )
	{
		const char *errmsg = 
			pErrBuf
				? (const char*)pErrBuf->GetBufferPointer()
				: NULL ;
		sdring<TCHAR> s1 = makeTsdring(errmsg);
		
		sdring<TCHAR> tFunctionName = makeTsdring(pFunctionName);
		sdring<TCHAR> tTarget = makeTsdring(pTarget);

//		mm_set_DebugProgressCallback(mm_DebugProgress, nullptr);
		vaDbgTs(
			_T("D3DXCompileShaderFromFile() failed.\n")
			_T("  Shader-file: %s\n")
			_T("  Function-name: %s\n")
			_T("  Target: %s\n")
			_T("  HRESULT = %s\n")
			_T("  ErrMsg: %s")
			,
			pSrcFile,
			tFunctionName.c_str(),
			tTarget.c_str(),
			ITCSvn(hr, DxErr), // HRESULT
			s1.c_str() ? s1.c_str() : _T("No textual error message from D3DXCompileShaderFromFile().")
			);
	}

	return hr;
}


