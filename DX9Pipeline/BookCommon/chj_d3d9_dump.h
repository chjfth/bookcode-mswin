#ifndef __chj_d3d9_dump_h_20260101_
#define __chj_d3d9_dump_h_20260101_


#include <d3d9.h>

#include <stdarg.h>
#include <fsapi.h>
#include <mmlogfile.h>

#include "chjshare.h"


class IDbgDump
{
public:
	virtual void vaDbg(const TCHAR *fmt, ...) = 0;
};

class FileDbgDump : public IDbgDump
{
public:
	FileDbgDump(const TCHAR *logfile, bool isAppend=true, int bufmax=8192) :
		m_mmlogfile(logfile, isAppend, bufmax)
	{}

	virtual void vaDbg(const TCHAR *fmt, ...)
	{
		va_list args;
		va_start(args, fmt);
		m_mmlogfile.mmlog(_T("%w"), MM_WPAIR_PARAM(fmt, args));
		va_end(args);
	}

private:
	CMmLogfile m_mmlogfile;
};


/////////////////

void dumpRenderState(IDirect3DDevice9 *pd, const TCHAR *fmt_prolog=nullptr, ...);

void dumpSamplerState(IDirect3DDevice9 *pd, int iSampler, const TCHAR *fmt_prolog=nullptr, ...);

void dumpTextureStageState(IDirect3DDevice9 *pd, int iStage, const TCHAR *fmt_prolog, ...);

// Ex8-2
void dumpMeshVertex_with_Format_0x112(IDbgDump *dump, ID3DXMesh *pMesh);
void dumpMeshVertex_Ex8_2_tangent(IDbgDump *dump, ID3DXMesh *pMesh);


/*
////////////////////////////////////////////////////////////////////////////
 ___                 _                           _        _   _             
|_ _|_ __ ___  _ __ | | ___ _ __ ___   ___ _ __ | |_ __ _| |_(_) ___  _ __  
 | || '_ ` _ \| '_ \| |/ _ \ '_ ` _ \ / _ \ '_ \| __/ _` | __| |/ _ \| '_ \ 
 | || | | | | | |_) | |  __/ | | | | |  __/ | | | || (_| | |_| | (_) | | | |
|___|_| |_| |_| .__/|_|\___|_| |_| |_|\___|_| |_|\__\__,_|\__|_|\___/|_| |_|
              |_|                                                           
////////////////////////////////////////////////////////////////////////////
*/
// ++++++++++++++++++ Implementation Below ++++++++++++++++++
//
#if defined(chj_d3d9_dump_IMPL) || (defined CHHI_ALL_IMPL && !defined CHHI_ALL_IMPL_HIDE_chj_d3d9_dump) // [IMPL]


static void vaDbgTs_herr(HRESULT herr, const TCHAR *prefix)
{
	vaDbgTs(_T("%s fail, herr=%s"), prefix, ITCSvn(herr, itc::DxErr));
}

#define DUMP_RenderState_bool(pd, which) dumpRenderState_bool(pd, which, _T(#which))

void dumpRenderState_bool(IDirect3DDevice9 *pd, D3DRENDERSTATETYPE which, const TCHAR *whichname)
{
	DWORD val = -1;
	HRESULT herr = pd->GetRenderState(which, &val);
	if(!herr) {
		vaDbgTs(_T("%s = %d (%s)"), whichname, 
			val, 
			val?_T("true"):_T("false"));
	} else {
		vaDbgTs_herr(herr, whichname);
	}
}

#define DUMP_RenderState_int(pd, which) dumpRenderState_int(pd, which, _T(#which))

void dumpRenderState_int(IDirect3DDevice9 *pd, D3DRENDERSTATETYPE which, const TCHAR *whichname)
{
	DWORD val = -1;
	HRESULT herr = pd->GetRenderState(which, &val);
	if(!herr) {
		vaDbgTs(_T("%s = %d"), whichname, val);
	} else {
		vaDbgTs_herr(herr, whichname);
	}
}

#define DUMP_RenderState_hex(pd, which) dumpRenderState_hex(pd, which, _T(#which))

void dumpRenderState_hex(IDirect3DDevice9 *pd, D3DRENDERSTATETYPE which, const TCHAR *whichname)
{
	DWORD val = -1;
	HRESULT herr = pd->GetRenderState(which, &val);
	if(!herr) {
		vaDbgTs(_T("%s = 0x%X"), whichname, val);
	} else {
		vaDbgTs_herr(herr, whichname);
	}
}

#define DUMP_RenderState_float(pd, which) dumpRenderState_float(pd, which, _T(#which))

void dumpRenderState_float(IDirect3DDevice9 *pd, D3DRENDERSTATETYPE which, const TCHAR *whichname)
{
	DWORD val = -1;
	HRESULT herr = pd->GetRenderState(which, &val);
	if(!herr) {
		vaDbgTs(_T("%s = %.3f"), whichname, *(float*)&val);
	} else {
		vaDbgTs_herr(herr, whichname);
	}
}

void dumpRenderState(IDirect3DDevice9 *pd, const TCHAR *fmt_prolog, ...)
{
	if(fmt_prolog)
	{
		va_list args;
		va_start(args, fmt_prolog);
		vlDbgTs(fmt_prolog, args);
		va_end(args);
	}

	HRESULT herr = 0;
	DWORD val = 0;

	herr = pd->GetRenderState(D3DRS_ZENABLE, &val);
	if(!herr) {
		vaDbgTs(_T("D3DRS_ZENABLE = %s"), ITCSvn(val, itc::D3DZBUFFERTYPE));
	} else {
		vaDbgTs_herr(herr, _T("D3DRS_ZENABLE"));
	}

	herr = pd->GetRenderState(D3DRS_FILLMODE, &val);
	if(!herr) {
		vaDbgTs(_T("D3DRS_FILLMODE = %s"), ITCSvn(val, itc::D3DFILLMODE));
	} else {
		vaDbgTs_herr(herr, _T("D3DRS_FILLMODE"));
	}

	herr = pd->GetRenderState(D3DRS_SHADEMODE, &val);
	if(!herr) {
		vaDbgTs(_T("D3DRS_SHADEMODE = %s"), ITCSvn(val, itc::D3DSHADEMODE));
	} else {
		vaDbgTs_herr(herr, _T("D3DRS_SHADEMODE"));
	}

	DUMP_RenderState_bool(pd, D3DRS_ZWRITEENABLE);
	DUMP_RenderState_bool(pd, D3DRS_ALPHATESTENABLE);
	DUMP_RenderState_bool(pd, D3DRS_LASTPIXEL);

	herr = pd->GetRenderState(D3DRS_SRCBLEND, &val);
	if(!herr) {
		vaDbgTs(_T("D3DRS_SRCBLEND = %s"), ITCSvn(val, itc::D3DBLEND));
	} else {
		vaDbgTs_herr(herr, _T("D3DRS_SRCBLEND"));
	}

	herr = pd->GetRenderState(D3DRS_DESTBLEND, &val);
	if(!herr) {
		vaDbgTs(_T("D3DRS_DESTBLEND = %s"), ITCSvn(val, itc::D3DBLEND));
	} else {
		vaDbgTs_herr(herr, _T("D3DRS_DESTBLEND"));
	}

	herr = pd->GetRenderState(D3DRS_CULLMODE, &val);
	if(!herr) {
		vaDbgTs(_T("D3DRS_CULLMODE = %s"), ITCSvn(val, itc::D3DCULL));
	} else {
		vaDbgTs_herr(herr, _T("D3DRS_CULLMODE"));
	}

	herr = pd->GetRenderState(D3DRS_ZFUNC, &val);
	if(!herr) {
		vaDbgTs(_T("D3DRS_ZFUNC = %s"), ITCSvn(val, itc::D3DCMPFUNC));
	} else {
		vaDbgTs_herr(herr, _T("D3DRS_ZFUNC"));
	}

	DUMP_RenderState_hex(pd, D3DRS_ALPHAREF);

	herr = pd->GetRenderState(D3DRS_ALPHAFUNC, &val);
	if(!herr) {
		vaDbgTs(_T("D3DRS_ALPHAFUNC = %s"), ITCSvn(val, itc::D3DCMPFUNC));
	} else {
		vaDbgTs_herr(herr, _T("D3DRS_ALPHAFUNC"));
	}

	DUMP_RenderState_bool(pd, D3DRS_DITHERENABLE);
	DUMP_RenderState_bool(pd, D3DRS_ALPHABLENDENABLE);
	DUMP_RenderState_bool(pd, D3DRS_FOGENABLE);
	DUMP_RenderState_bool(pd, D3DRS_SPECULARENABLE);

	DUMP_RenderState_hex(pd, D3DRS_FOGCOLOR);

	herr = pd->GetRenderState(D3DRS_FOGTABLEMODE, &val);
	if(!herr) {
		vaDbgTs(_T("D3DRS_FOGTABLEMODE = %s"), ITCSvn(val, itc::D3DFOGMODE));
	} else {
		vaDbgTs_herr(herr, _T("D3DRS_FOGTABLEMODE"));
	}

	DUMP_RenderState_float(pd, D3DRS_FOGSTART);
	DUMP_RenderState_float(pd, D3DRS_FOGEND);
	DUMP_RenderState_float(pd, D3DRS_FOGDENSITY);

	DUMP_RenderState_bool(pd, D3DRS_RANGEFOGENABLE);
	DUMP_RenderState_bool(pd, D3DRS_STENCILENABLE);

	herr = pd->GetRenderState(D3DRS_STENCILFAIL, &val);
	if(!herr) {
		vaDbgTs(_T("D3DRS_STENCILFAIL = %s"), ITCSvn(val, itc::D3DSTENCILOP));
	} else {
		vaDbgTs_herr(herr, _T("D3DRS_STENCILFAIL"));
	}

	herr = pd->GetRenderState(D3DRS_STENCILZFAIL, &val);
	if(!herr) {
		vaDbgTs(_T("D3DRS_STENCILZFAIL = %s"), ITCSvn(val, itc::D3DSTENCILOP));
	} else {
		vaDbgTs_herr(herr, _T("D3DRS_STENCILZFAIL"));
	}

	herr = pd->GetRenderState(D3DRS_STENCILPASS, &val);
	if(!herr) {
		vaDbgTs(_T("D3DRS_STENCILPASS = %s"), ITCSvn(val, itc::D3DSTENCILOP));
	} else {
		vaDbgTs_herr(herr, _T("D3DRS_STENCILPASS"));
	}

	herr = pd->GetRenderState(D3DRS_STENCILFUNC, &val);
	if(!herr) {
		vaDbgTs(_T("D3DRS_STENCILFUNC = %s"), ITCSvn(val, itc::D3DCMPFUNC));
	} else {
		vaDbgTs_herr(herr, _T("D3DRS_STENCILFUNC"));
	}

	DUMP_RenderState_int(pd, D3DRS_STENCILREF);

	DUMP_RenderState_hex(pd, D3DRS_STENCILMASK);
	DUMP_RenderState_hex(pd, D3DRS_STENCILWRITEMASK);

	DUMP_RenderState_hex(pd, D3DRS_TEXTUREFACTOR);

	DUMP_RenderState_hex(pd, D3DRS_WRAP0);
	DUMP_RenderState_hex(pd, D3DRS_WRAP1);
	DUMP_RenderState_hex(pd, D3DRS_WRAP2);
	DUMP_RenderState_hex(pd, D3DRS_WRAP3);

	DUMP_RenderState_bool(pd, D3DRS_CLIPPING);
	DUMP_RenderState_bool(pd, D3DRS_LIGHTING);

	DUMP_RenderState_hex(pd, D3DRS_AMBIENT); 

	herr = pd->GetRenderState(D3DRS_FOGVERTEXMODE, &val);
	if(!herr) {
		vaDbgTs(_T("D3DRS_FOGVERTEXMODE = %s"), ITCSvn(val, itc::D3DFOGMODE));
	} else {
		vaDbgTs_herr(herr, _T("D3DRS_FOGVERTEXMODE"));
	}

	DUMP_RenderState_bool(pd, D3DRS_COLORVERTEX);
	DUMP_RenderState_bool(pd, D3DRS_LOCALVIEWER);
	DUMP_RenderState_bool(pd, D3DRS_NORMALIZENORMALS);

	herr = pd->GetRenderState(D3DRS_DIFFUSEMATERIALSOURCE, &val);
	if(!herr) {
		vaDbgTs(_T("D3DRS_DIFFUSEMATERIALSOURCE = %s"), ITCSvn(val, itc::D3DMATERIALCOLORSOURCE));
	} else {
		vaDbgTs_herr(herr, _T("D3DRS_DIFFUSEMATERIALSOURCE"));
	}
	herr = pd->GetRenderState(D3DRS_SPECULARMATERIALSOURCE, &val);
	if(!herr) {
		vaDbgTs(_T("D3DRS_SPECULARMATERIALSOURCE = %s"), ITCSvn(val, itc::D3DMATERIALCOLORSOURCE));
	} else {
		vaDbgTs_herr(herr, _T("D3DRS_SPECULARMATERIALSOURCE"));
	}
	herr = pd->GetRenderState(D3DRS_AMBIENTMATERIALSOURCE, &val);
	if(!herr) {
		vaDbgTs(_T("D3DRS_AMBIENTMATERIALSOURCE = %s"), ITCSvn(val, itc::D3DMATERIALCOLORSOURCE));
	} else {
		vaDbgTs_herr(herr, _T("D3DRS_AMBIENTMATERIALSOURCE"));
	}
	herr = pd->GetRenderState(D3DRS_EMISSIVEMATERIALSOURCE, &val);
	if(!herr) {
		vaDbgTs(_T("D3DRS_EMISSIVEMATERIALSOURCE = %s"), ITCSvn(val, itc::D3DMATERIALCOLORSOURCE));
	} else {
		vaDbgTs_herr(herr, _T("D3DRS_EMISSIVEMATERIALSOURCE"));
	}

	herr = pd->GetRenderState(D3DRS_VERTEXBLEND, &val);
	if(!herr) {
		vaDbgTs(_T("D3DRS_VERTEXBLEND = %s"), ITCSvn(val, itc::D3DVERTEXBLENDFLAGS));
	} else {
		vaDbgTs_herr(herr, _T("D3DRS_VERTEXBLEND"));
	}

	DUMP_RenderState_hex(pd, D3DRS_CLIPPLANEENABLE);

	DUMP_RenderState_float(pd, D3DRS_POINTSIZE);
	DUMP_RenderState_float(pd, D3DRS_POINTSIZE_MIN);
	DUMP_RenderState_float(pd, D3DRS_POINTSIZE_MAX);

	DUMP_RenderState_bool(pd, D3DRS_POINTSPRITEENABLE);
	DUMP_RenderState_bool(pd, D3DRS_POINTSCALEENABLE);

	DUMP_RenderState_float(pd, D3DRS_POINTSCALE_A);
	DUMP_RenderState_float(pd, D3DRS_POINTSCALE_B);
	DUMP_RenderState_float(pd, D3DRS_POINTSCALE_C);

	DUMP_RenderState_bool(pd, D3DRS_MULTISAMPLEANTIALIAS);

	DUMP_RenderState_hex(pd, D3DRS_MULTISAMPLEMASK);

	herr = pd->GetRenderState(D3DRS_PATCHEDGESTYLE, &val);
	if(!herr) {
		vaDbgTs(_T("D3DRS_PATCHEDGESTYLE = %s"), ITCSvn(val, itc::D3DPATCHEDGESTYLE));
	} else {
		vaDbgTs_herr(herr, _T("D3DRS_PATCHEDGESTYLE"));
	}

	// skip D3DRS_DEBUGMONITORTOKEN 

	DUMP_RenderState_bool(pd, D3DRS_INDEXEDVERTEXBLENDENABLE);

	DUMP_RenderState_hex(pd, D3DRS_COLORWRITEENABLE );

	DUMP_RenderState_float(pd, D3DRS_TWEENFACTOR);

	herr = pd->GetRenderState(D3DRS_BLENDOP, &val);
	if(!herr) {
		vaDbgTs(_T("D3DRS_BLENDOP = %s"), ITCSvn(val, itc::D3DBLENDOP));
	} else {
		vaDbgTs_herr(herr, _T("D3DRS_BLENDOP"));
	}

	herr = pd->GetRenderState(D3DRS_POSITIONDEGREE, &val);
	if(!herr) {
		vaDbgTs(_T("D3DRS_POSITIONDEGREE  = %s"), ITCSvn(val, itc::D3DDEGREETYPE));
	} else {
		vaDbgTs_herr(herr, _T("D3DRS_POSITIONDEGREE"));
	}
	//
	herr = pd->GetRenderState(D3DRS_NORMALDEGREE, &val);
	if(!herr) {
		vaDbgTs(_T("D3DRS_NORMALDEGREE = %s"), ITCSvn(val, itc::D3DDEGREETYPE));
	} else {
		vaDbgTs_herr(herr, _T("D3DRS_NORMALDEGREE"));
	}

	DUMP_RenderState_bool(pd, D3DRS_SCISSORTESTENABLE);

	DUMP_RenderState_int(pd, D3DRS_SLOPESCALEDEPTHBIAS);

	DUMP_RenderState_bool(pd, D3DRS_ANTIALIASEDLINEENABLE);

	DUMP_RenderState_float(pd, D3DRS_MINTESSELLATIONLEVEL);
	DUMP_RenderState_float(pd, D3DRS_MAXTESSELLATIONLEVEL);

	DUMP_RenderState_float(pd, D3DRS_ADAPTIVETESS_X);
	DUMP_RenderState_float(pd, D3DRS_ADAPTIVETESS_Y);
	DUMP_RenderState_float(pd, D3DRS_ADAPTIVETESS_Z);
	DUMP_RenderState_float(pd, D3DRS_ADAPTIVETESS_W);

	DUMP_RenderState_bool(pd, D3DRS_ENABLEADAPTIVETESSELLATION);

	DUMP_RenderState_bool(pd, D3DRS_TWOSIDEDSTENCILMODE);

	herr = pd->GetRenderState(D3DRS_CCW_STENCILFAIL, &val);
	if(!herr) {
		vaDbgTs(_T("D3DRS_CCW_STENCILFAIL = %s"), ITCSvn(val, itc::D3DSTENCILOP));
	} else {
		vaDbgTs_herr(herr, _T("D3DRS_CCW_STENCILFAIL"));
	}

	herr = pd->GetRenderState(D3DRS_CCW_STENCILZFAIL, &val);
	if(!herr) {
		vaDbgTs(_T("D3DRS_CCW_STENCILZFAIL = %s"), ITCSvn(val, itc::D3DSTENCILOP));
	} else {
		vaDbgTs_herr(herr, _T("D3DRS_CCW_STENCILZFAIL"));
	}

	herr = pd->GetRenderState(D3DRS_CCW_STENCILPASS, &val);
	if(!herr) {
		vaDbgTs(_T("D3DRS_CCW_STENCILPASS = %s"), ITCSvn(val, itc::D3DSTENCILOP));
	} else {
		vaDbgTs_herr(herr, _T("D3DRS_CCW_STENCILPASS"));
	}

	herr = pd->GetRenderState(D3DRS_CCW_STENCILFUNC, &val);
	if(!herr) {
		vaDbgTs(_T("D3DRS_CCW_STENCILFUNC = %s"), ITCSvn(val, itc::D3DCMPFUNC));
	} else {
		vaDbgTs_herr(herr, _T("D3DRS_CCW_STENCILFUNC"));
	}

	// Skip D3DRS_COLORWRITEENABLE1 , D3DRS_COLORWRITEENABLE2, D3DRS_COLORWRITEENABLE3

	DUMP_RenderState_hex(pd, D3DRS_BLENDFACTOR);

	DUMP_RenderState_bool(pd, D3DRS_SRGBWRITEENABLE);

	DUMP_RenderState_float(pd, D3DRS_DEPTHBIAS);

	DUMP_RenderState_bool(pd, D3DRS_SEPARATEALPHABLENDENABLE);

	herr = pd->GetRenderState(D3DRS_SRCBLENDALPHA, &val);
	if(!herr) {
		vaDbgTs(_T("D3DRS_SRCBLENDALPHA = %s"), ITCSvn(val, itc::D3DBLEND));
	} else {
		vaDbgTs_herr(herr, _T("D3DRS_SRCBLENDALPHA"));
	}

	herr = pd->GetRenderState(D3DRS_DESTBLENDALPHA, &val);
	if(!herr) {
		vaDbgTs(_T("D3DRS_DESTBLENDALPHA = %s"), ITCSvn(val, itc::D3DBLEND));
	} else {
		vaDbgTs_herr(herr, _T("D3DRS_DESTBLENDALPHA"));
	}

	DUMP_RenderState_bool(pd, D3DRS_BLENDOPALPHA);

}


void dumpSamplerState(IDirect3DDevice9 *pd, int iSampler, const TCHAR *fmt_prolog, ...)
{
	static D3DSAMPLERSTATETYPE arTypes[] = {
		D3DSAMP_ADDRESSU      ,
		D3DSAMP_ADDRESSV      ,
		D3DSAMP_ADDRESSW      ,
		D3DSAMP_BORDERCOLOR   ,
		D3DSAMP_MAGFILTER     ,
		D3DSAMP_MINFILTER     ,
		D3DSAMP_MIPFILTER     ,
		D3DSAMP_MIPMAPLODBIAS ,
		D3DSAMP_MAXMIPLEVEL   ,
		D3DSAMP_MAXANISOTROPY ,
		D3DSAMP_SRGBTEXTURE   ,
		D3DSAMP_ELEMENTINDEX  ,
		D3DSAMP_DMAPOFFSET    ,
	}; (void)arTypes;

	if(fmt_prolog)
	{
		va_list args;
		va_start(args, fmt_prolog);
		vlDbgTs(fmt_prolog, args);
		va_end(args);
	}

	HRESULT herr = 0;
	DWORD val = 0;

	herr = pd->GetSamplerState(iSampler, D3DSAMP_ADDRESSU, &val);
	if(!herr) {
		vaDbgTs(_T("D3DSAMP_ADDRESSU = %s"), ITCSvn(val, itc::D3DTEXTUREADDRESS));
	} else {
		vaDbgTs_herr(herr, _T("D3DSAMP_ADDRESSU"));
	}

	herr = pd->GetSamplerState(iSampler, D3DSAMP_ADDRESSV, &val);
	if(!herr) {
		vaDbgTs(_T("D3DSAMP_ADDRESSV = %s"), ITCSvn(val, itc::D3DTEXTUREADDRESS));
	} else {
		vaDbgTs_herr(herr, _T("D3DSAMP_ADDRESSV"));
	}

	herr = pd->GetSamplerState(iSampler, D3DSAMP_ADDRESSW, &val);
	if(!herr) {
		vaDbgTs(_T("D3DSAMP_ADDRESSW = %s"), ITCSvn(val, itc::D3DTEXTUREADDRESS));
	} else {
		vaDbgTs_herr(herr, _T("D3DSAMP_ADDRESSW"));
	}

	herr = pd->GetSamplerState(iSampler, D3DSAMP_BORDERCOLOR, &val);
	if(!herr) {
		vaDbgTs(_T("D3DSAMP_BORDERCOLOR (D3DCOLOR) = 0x%08X"), &val);
	} else {
		vaDbgTs_herr(herr, _T("D3DSAMP_BORDERCOLOR"));
	}

	herr = pd->GetSamplerState(iSampler, D3DSAMP_MAGFILTER, &val);
	if(!herr) {
		vaDbgTs(_T("D3DSAMP_MAGFILTER = %s"), ITCSvn(val, itc::D3DTEXTUREFILTERTYPE));
	} else {
		vaDbgTs_herr(herr, _T("D3DSAMP_MAGFILTER"));
	}

	herr = pd->GetSamplerState(iSampler, D3DSAMP_MINFILTER, &val);
	if(!herr) {
		vaDbgTs(_T("D3DSAMP_MINFILTER = %s"), ITCSvn(val, itc::D3DTEXTUREFILTERTYPE));
	} else {
		vaDbgTs_herr(herr, _T("D3DSAMP_MINFILTER"));
	}

	herr = pd->GetSamplerState(iSampler, D3DSAMP_MIPFILTER, &val);
	if(!herr) {
		vaDbgTs(_T("D3DSAMP_MIPFILTER = %s"), ITCSvn(val, itc::D3DTEXTUREFILTERTYPE));
	} else {
		vaDbgTs_herr(herr, _T("D3DSAMP_MIPFILTER"));
	}

	herr = pd->GetSamplerState(iSampler, D3DSAMP_MIPMAPLODBIAS, &val);
	if(!herr) {
		vaDbgTs(_T("D3DSAMP_MIPMAPLODBIAS = %d"), val);
	} else {
		vaDbgTs_herr(herr, _T("D3DSAMP_MIPMAPLODBIAS"));
	}

	herr = pd->GetSamplerState(iSampler, D3DSAMP_MAXMIPLEVEL, &val);
	if(!herr) {
		vaDbgTs(_T("D3DSAMP_MAXMIPLEVEL = %d"), val);
	} else {
		vaDbgTs_herr(herr, _T("D3DSAMP_MAXMIPLEVEL"));
	}

	herr = pd->GetSamplerState(iSampler, D3DSAMP_MAXANISOTROPY, &val);
	if(!herr) {
		vaDbgTs(_T("D3DSAMP_MAXANISOTROPY = %d"), val);
	} else {
		vaDbgTs_herr(herr, _T("D3DSAMP_MAXANISOTROPY"));
	}

	herr = pd->GetSamplerState(iSampler, D3DSAMP_SRGBTEXTURE , &val);
	if(!herr) {
		vaDbgTs(_T("D3DSAMP_SRGBTEXTURE = %d"), val);
	} else {
		vaDbgTs_herr(herr, _T("D3DSAMP_SRGBTEXTURE"));
	}

	herr = pd->GetSamplerState(iSampler, D3DSAMP_ELEMENTINDEX , &val);
	if(!herr) {
		vaDbgTs(_T("D3DSAMP_ELEMENTINDEX = %d"), val);
	} else {
		vaDbgTs_herr(herr, _T("D3DSAMP_ELEMENTINDEX"));
	}

	herr = pd->GetSamplerState(iSampler, D3DSAMP_DMAPOFFSET, &val);
	if(!herr) {
		vaDbgTs(_T("D3DSAMP_DMAPOFFSET = %d"), val);
	} else {
		vaDbgTs_herr(herr, _T("D3DSAMP_DMAPOFFSET"));
	}
}


#define DUMP_TextureStageState_D3DTA(pd, iStage, which) dumpTextureStageState_D3DTA(pd, iStage, which, _T(#which))

void dumpTextureStageState_D3DTA(IDirect3DDevice9 *pd, int iStage, 
	D3DTEXTURESTAGESTATETYPE which, const TCHAR *whichname)
{
	DWORD val = -1;
	HRESULT herr = pd->GetTextureStageState(iStage, which, &val);
	if(!herr) {
		vaDbgTs(_T("%s = %s"), whichname, ITCSvn(val, itc::D3DTA_xxx));
	} else {
		vaDbgTs_herr(herr, whichname);
	}
}

#define DUMP_TextureStageState_float(pd, iStage, which) dumpTextureStageState_float(pd, iStage, which, _T(#which))

void dumpTextureStageState_float(IDirect3DDevice9 *pd, int iStage, 
	D3DTEXTURESTAGESTATETYPE which, const TCHAR *whichname)
{
	DWORD val = -1;
	HRESULT herr = pd->GetTextureStageState(iStage, which, &val);
	if(!herr) {
		vaDbgTs(_T("%s = %.3f"), whichname, *(float*)&val);
	} else {
		vaDbgTs_herr(herr, whichname);
	}
}


void dumpTextureStageState(IDirect3DDevice9 *pd, int iStage, const TCHAR *fmt_prolog, ...)
{
	if(fmt_prolog)
	{
		va_list args;
		va_start(args, fmt_prolog);
		vlDbgTs(fmt_prolog, args);
		va_end(args);
	}

	HRESULT herr = 0;
	DWORD val = 0;

	herr = pd->GetTextureStageState(iStage, D3DTSS_COLOROP, &val);
	if(!herr) {
		vaDbgTs(_T("D3DTSS_COLOROP = %s"), ITCSvn(val, itc::D3DTEXTUREOP));
	} else {
		vaDbgTs_herr(herr, _T("D3DTSS_COLOROP"));
	}

	DUMP_TextureStageState_D3DTA(pd, iStage, D3DTSS_COLORARG1);
	DUMP_TextureStageState_D3DTA(pd, iStage, D3DTSS_COLORARG2);

	herr = pd->GetTextureStageState(iStage, D3DTSS_ALPHAOP, &val);
	if(!herr) {
		vaDbgTs(_T("D3DTSS_ALPHAOP = %s"), ITCSvn(val, itc::D3DTEXTUREOP));
	} else {
		vaDbgTs_herr(herr, _T("D3DTSS_ALPHAOP"));
	}

	DUMP_TextureStageState_D3DTA(pd, iStage, D3DTSS_ALPHAARG1);
	DUMP_TextureStageState_D3DTA(pd, iStage, D3DTSS_ALPHAARG2);

	// skip D3DTSS_BUMPENVMAT00, D3DTSS_BUMPENVMAT01, D3DTSS_BUMPENVMAT10, D3DTSS_BUMPENVMAT11

	herr = pd->GetTextureStageState(iStage, D3DTSS_TEXCOORDINDEX, &val);
	if(!herr) {
		vaDbgTs(_T("D3DTSS_TEXCOORDINDEX = 0x%X"), val);
		// todo: Need to further interpret into D3DTSS_TCI_xxx
	} else {
		vaDbgTs_herr(herr, _T("D3DTSS_TEXCOORDINDEX"));
	}

	DUMP_TextureStageState_float(pd, iStage, D3DTSS_BUMPENVLSCALE);
	DUMP_TextureStageState_float(pd, iStage, D3DTSS_BUMPENVLOFFSET);

	herr = pd->GetTextureStageState(iStage, D3DTSS_TEXTURETRANSFORMFLAGS, &val);
	if(!herr) {
		vaDbgTs(_T("D3DTSS_TEXTURETRANSFORMFLAGS = %s"), ITCSvn(val, itc::D3DTEXTURETRANSFORMFLAGS));
	} else {
		vaDbgTs_herr(herr, _T("D3DTSS_TEXTURETRANSFORMFLAGS "));
	}

	DUMP_TextureStageState_D3DTA(pd, iStage, D3DTSS_COLORARG0);
	DUMP_TextureStageState_D3DTA(pd, iStage, D3DTSS_ALPHAARG0);

	DUMP_TextureStageState_D3DTA(pd, iStage, D3DTSS_RESULTARG);
	
	DUMP_TextureStageState_D3DTA(pd, iStage, D3DTSS_CONSTANT);

}


// For Ex8-2

struct VertexFVF_0x112
{
	float x, y, z;     // D3DFVF_XYZ
	float nx, ny, nz;  // D3DFVF_NORMAL
	float u, v;        // D3DFVF_TEX1 (TEXCOORD0)
};

void dumpMeshVertex_with_Format_0x112(IDbgDump *dump, ID3DXMesh *pMesh)
{
	int nVertex = pMesh->GetNumVertices();
	int nStride = pMesh->GetNumBytesPerVertex();

	dump->vaDbg(_T("Vertex count: %d"), nVertex);

	IDirect3DVertexBuffer9 *pVB = NULL;
	pMesh->GetVertexBuffer(&pVB);
	Cec_Release cec_vb = pVB;

	VertexFVF_0x112 *varray = nullptr;
	pVB->Lock(0, 0, (void**)&varray, D3DLOCK_READONLY);

	for(int i=0; i<nVertex; i++)
	{
		VertexFVF_0x112 &vt = varray[i];
		dump->vaDbg(
			_T("[v#%d] Pos:(% .-3f,% .-3f,% .-3f) Normal:(% .-3f,% .-3f,% .-3f) Texture@(% .-3f,% .-3f)")
			, 
			i,
			vt.x, vt.y, vt.z,
			vt.nx, vt.ny, vt.nz,
			vt.u, vt.v
			);
	}

	pVB->Unlock();

	D3DVERTEXBUFFER_DESC desc = {};
	pVB->GetDesc(&desc);
}


struct Vertex_Ex8_2_tangent
{
	float x, y, z;     // D3DFVF_XYZ
	float nx, ny, nz;  // D3DFVF_NORMAL
	float u, v;        // D3DFVF_TEX1 (TEXCOORD0)
	float tx, ty, tz;  // tangent
};

void dumpMeshVertex_Ex8_2_tangent(IDbgDump *dump, ID3DXMesh *pMesh)
{
	int nVertex = pMesh->GetNumVertices();
	int nStride = pMesh->GetNumBytesPerVertex();

	dump->vaDbg(_T("Vertex count: %d"), nVertex);

	IDirect3DVertexBuffer9 *pVB = NULL;
	pMesh->GetVertexBuffer(&pVB);
	Cec_Release cec_vb = pVB;

	Vertex_Ex8_2_tangent *varray = nullptr;
	pVB->Lock(0, 0, (void**)&varray, D3DLOCK_READONLY);

	for(int i=0; i<nVertex; i++)
	{
		Vertex_Ex8_2_tangent &vt = varray[i];
		dump->vaDbg(
			_T("[v#%d] Pos:(% .-3f,% .-3f,% .-3f) Normal:(% .-3f,% .-3f,% .-3f) Texture@(% .-3f,% .-3f) Tangent:(% .-3f,% .-3f,% .-3f)")
			, 
			i,
			vt.x, vt.y, vt.z,
			vt.nx, vt.ny, vt.nz,
			vt.u, vt.v,
			vt.tx, vt.ty, vt.tz
			);
	}

	pVB->Unlock();
}



#endif // [IMPL]

#endif // __chj_d3d9_dump_h_20260101_
