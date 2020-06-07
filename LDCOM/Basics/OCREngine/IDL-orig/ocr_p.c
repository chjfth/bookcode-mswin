

/* this ALWAYS GENERATED file contains the proxy stub code */


 /* File created by MIDL compiler version 7.00.0555 */
/* at Sun Jun 07 12:37:14 2020
 */
/* Compiler settings for ocr.idl:
    Oicf, W1, Zp8, env=Win32 (32b run), target_arch=X86 7.00.0555 
    protocol : dce , ms_ext, c_ext, robust
    error checks: allocation ref bounds_check enum stub_data 
    VC __declspec() decoration level: 
         __declspec(uuid()), __declspec(selectany), __declspec(novtable)
         DECLSPEC_UUID(), MIDL_INTERFACE()
*/
/* @@MIDL_FILE_HEADING(  ) */

#if !defined(_M_IA64) && !defined(_M_AMD64)


#pragma warning( disable: 4049 )  /* more than 64k source lines */
#if _MSC_VER >= 1200
#pragma warning(push)
#endif

#pragma warning( disable: 4211 )  /* redefine extern to static */
#pragma warning( disable: 4232 )  /* dllimport identity*/
#pragma warning( disable: 4024 )  /* array to pointer mapping*/
#pragma warning( disable: 4152 )  /* function/data pointer conversion in expression */
#pragma warning( disable: 4100 ) /* unreferenced arguments in x86 call */

#pragma optimize("", off ) 

#define USE_STUBLESS_PROXY


/* verify that the <rpcproxy.h> version is high enough to compile this file*/
#ifndef __REDQ_RPCPROXY_H_VERSION__
#define __REQUIRED_RPCPROXY_H_VERSION__ 475
#endif


#include "rpcproxy.h"
#ifndef __RPCPROXY_H_VERSION__
#error this stub requires an updated version of <rpcproxy.h>
#endif /* __RPCPROXY_H_VERSION__ */


#include "ocr.h"

#define TYPE_FORMAT_STRING_SIZE   65                                
#define PROC_FORMAT_STRING_SIZE   145                               
#define EXPR_FORMAT_STRING_SIZE   1                                 
#define TRANSMIT_AS_TABLE_SIZE    0            
#define WIRE_MARSHAL_TABLE_SIZE   0            

typedef struct _ocr_MIDL_TYPE_FORMAT_STRING
    {
    short          Pad;
    unsigned char  Format[ TYPE_FORMAT_STRING_SIZE ];
    } ocr_MIDL_TYPE_FORMAT_STRING;

typedef struct _ocr_MIDL_PROC_FORMAT_STRING
    {
    short          Pad;
    unsigned char  Format[ PROC_FORMAT_STRING_SIZE ];
    } ocr_MIDL_PROC_FORMAT_STRING;

typedef struct _ocr_MIDL_EXPR_FORMAT_STRING
    {
    long          Pad;
    unsigned char  Format[ EXPR_FORMAT_STRING_SIZE ];
    } ocr_MIDL_EXPR_FORMAT_STRING;


static const RPC_SYNTAX_IDENTIFIER  _RpcTransferSyntax = 
{{0x8A885D04,0x1CEB,0x11C9,{0x9F,0xE8,0x08,0x00,0x2B,0x10,0x48,0x60}},{2,0}};


extern const ocr_MIDL_TYPE_FORMAT_STRING ocr__MIDL_TypeFormatString;
extern const ocr_MIDL_PROC_FORMAT_STRING ocr__MIDL_ProcFormatString;
extern const ocr_MIDL_EXPR_FORMAT_STRING ocr__MIDL_ExprFormatString;


extern const MIDL_STUB_DESC Object_StubDesc;


extern const MIDL_SERVER_INFO IOcr_ServerInfo;
extern const MIDL_STUBLESS_PROXY_INFO IOcr_ProxyInfo;


extern const MIDL_STUB_DESC Object_StubDesc;


extern const MIDL_SERVER_INFO ISpell_ServerInfo;
extern const MIDL_STUBLESS_PROXY_INFO ISpell_ProxyInfo;



#if !defined(__RPC_WIN32__)
#error  Invalid build platform for this stub.
#endif

#if !(TARGET_IS_NT50_OR_LATER)
#error You need Windows 2000 or later to run this stub because it uses these features:
#error   /robust command line switch.
#error However, your C/C++ compilation flags indicate you intend to run this app on earlier systems.
#error This app will fail with the RPC_X_WRONG_STUB_VERSION error.
#endif


static const ocr_MIDL_PROC_FORMAT_STRING ocr__MIDL_ProcFormatString =
    {
        0,
        {

	/* Procedure OcrImage */

			0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/*  2 */	NdrFcLong( 0x0 ),	/* 0 */
/*  6 */	NdrFcShort( 0x3 ),	/* 3 */
/*  8 */	NdrFcShort( 0x14 ),	/* x86 Stack size/offset = 20 */
/* 10 */	NdrFcShort( 0x8 ),	/* 8 */
/* 12 */	NdrFcShort( 0x8 ),	/* 8 */
/* 14 */	0x47,		/* Oi2 Flags:  srv must size, clt must size, has return, has ext, */
			0x4,		/* 4 */
/* 16 */	0x8,		/* 8 */
			0x5,		/* Ext Flags:  new corr desc, srv corr check, */
/* 18 */	NdrFcShort( 0x0 ),	/* 0 */
/* 20 */	NdrFcShort( 0x1 ),	/* 1 */
/* 22 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter lImageSize */

/* 24 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 26 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 28 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter pbImage */

/* 30 */	NdrFcShort( 0x10b ),	/* Flags:  must size, must free, in, simple ref, */
/* 32 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 34 */	NdrFcShort( 0x6 ),	/* Type Offset=6 */

	/* Parameter pwszOcrText */

/* 36 */	NdrFcShort( 0x2013 ),	/* Flags:  must size, must free, out, srv alloc size=8 */
/* 38 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 40 */	NdrFcShort( 0x12 ),	/* Type Offset=18 */

	/* Return value */

/* 42 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 44 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 46 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure OcrZone */

/* 48 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 50 */	NdrFcLong( 0x0 ),	/* 0 */
/* 54 */	NdrFcShort( 0x4 ),	/* 4 */
/* 56 */	NdrFcShort( 0x1c ),	/* x86 Stack size/offset = 28 */
/* 58 */	NdrFcShort( 0x20 ),	/* 32 */
/* 60 */	NdrFcShort( 0x8 ),	/* 8 */
/* 62 */	0x47,		/* Oi2 Flags:  srv must size, clt must size, has return, has ext, */
			0x5,		/* 5 */
/* 64 */	0x8,		/* 8 */
			0x5,		/* Ext Flags:  new corr desc, srv corr check, */
/* 66 */	NdrFcShort( 0x0 ),	/* 0 */
/* 68 */	NdrFcShort( 0x1 ),	/* 1 */
/* 70 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter lImageSize */

/* 72 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 74 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 76 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter pbImage */

/* 78 */	NdrFcShort( 0x10b ),	/* Flags:  must size, must free, in, simple ref, */
/* 80 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 82 */	NdrFcShort( 0x6 ),	/* Type Offset=6 */

	/* Parameter zone */

/* 84 */	NdrFcShort( 0x8a ),	/* Flags:  must free, in, by val, */
/* 86 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 88 */	NdrFcShort( 0x1a ),	/* Type Offset=26 */

	/* Parameter pwszOcrText */

/* 90 */	NdrFcShort( 0x2013 ),	/* Flags:  must size, must free, out, srv alloc size=8 */
/* 92 */	NdrFcShort( 0x14 ),	/* x86 Stack size/offset = 20 */
/* 94 */	NdrFcShort( 0x12 ),	/* Type Offset=18 */

	/* Return value */

/* 96 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 98 */	NdrFcShort( 0x18 ),	/* x86 Stack size/offset = 24 */
/* 100 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure Check */

/* 102 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 104 */	NdrFcLong( 0x0 ),	/* 0 */
/* 108 */	NdrFcShort( 0x3 ),	/* 3 */
/* 110 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 112 */	NdrFcShort( 0x0 ),	/* 0 */
/* 114 */	NdrFcShort( 0x8c ),	/* 140 */
/* 116 */	0x46,		/* Oi2 Flags:  clt must size, has return, has ext, */
			0x3,		/* 3 */
/* 118 */	0x8,		/* 8 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 120 */	NdrFcShort( 0x0 ),	/* 0 */
/* 122 */	NdrFcShort( 0x0 ),	/* 0 */
/* 124 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter pwszWord */

/* 126 */	NdrFcShort( 0x10b ),	/* Flags:  must size, must free, in, simple ref, */
/* 128 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 130 */	NdrFcShort( 0x26 ),	/* Type Offset=38 */

	/* Parameter pWords */

/* 132 */	NdrFcShort( 0x112 ),	/* Flags:  must free, out, simple ref, */
/* 134 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 136 */	NdrFcShort( 0x32 ),	/* Type Offset=50 */

	/* Return value */

/* 138 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 140 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 142 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

			0x0
        }
    };

static const ocr_MIDL_TYPE_FORMAT_STRING ocr__MIDL_TypeFormatString =
    {
        0,
        {
			NdrFcShort( 0x0 ),	/* 0 */
/*  2 */	
			0x11, 0x0,	/* FC_RP */
/*  4 */	NdrFcShort( 0x2 ),	/* Offset= 2 (6) */
/*  6 */	
			0x1b,		/* FC_CARRAY */
			0x0,		/* 0 */
/*  8 */	NdrFcShort( 0x1 ),	/* 1 */
/* 10 */	0x28,		/* Corr desc:  parameter, FC_LONG */
			0x0,		/*  */
/* 12 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 14 */	NdrFcShort( 0x1 ),	/* Corr flags:  early, */
/* 16 */	0x1,		/* FC_BYTE */
			0x5b,		/* FC_END */
/* 18 */	
			0x11, 0x14,	/* FC_RP [alloced_on_stack] [pointer_deref] */
/* 20 */	NdrFcShort( 0x2 ),	/* Offset= 2 (22) */
/* 22 */	
			0x13, 0x8,	/* FC_OP [simple_pointer] */
/* 24 */	
			0x25,		/* FC_C_WSTRING */
			0x5c,		/* FC_PAD */
/* 26 */	
			0x15,		/* FC_STRUCT */
			0x1,		/* 1 */
/* 28 */	NdrFcShort( 0x8 ),	/* 8 */
/* 30 */	0x6,		/* FC_SHORT */
			0x6,		/* FC_SHORT */
/* 32 */	0x6,		/* FC_SHORT */
			0x6,		/* FC_SHORT */
/* 34 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 36 */	
			0x11, 0x8,	/* FC_RP [simple_pointer] */
/* 38 */	
			0x25,		/* FC_C_WSTRING */
			0x5c,		/* FC_PAD */
/* 40 */	
			0x11, 0x0,	/* FC_RP */
/* 42 */	NdrFcShort( 0x8 ),	/* Offset= 8 (50) */
/* 44 */	
			0x1d,		/* FC_SMFARRAY */
			0x1,		/* 1 */
/* 46 */	NdrFcShort( 0x20 ),	/* 32 */
/* 48 */	0x5,		/* FC_WCHAR */
			0x5b,		/* FC_END */
/* 50 */	
			0x15,		/* FC_STRUCT */
			0x1,		/* 1 */
/* 52 */	NdrFcShort( 0x40 ),	/* 64 */
/* 54 */	0x4c,		/* FC_EMBEDDED_COMPLEX */
			0x0,		/* 0 */
/* 56 */	NdrFcShort( 0xfff4 ),	/* Offset= -12 (44) */
/* 58 */	0x4c,		/* FC_EMBEDDED_COMPLEX */
			0x0,		/* 0 */
/* 60 */	NdrFcShort( 0xfff0 ),	/* Offset= -16 (44) */
/* 62 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */

			0x0
        }
    };


/* Standard interface: __MIDL_itf_ocr_0000_0000, ver. 0.0,
   GUID={0x00000000,0x0000,0x0000,{0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00}} */


/* Object interface: IUnknown, ver. 0.0,
   GUID={0x00000000,0x0000,0x0000,{0xC0,0x00,0x00,0x00,0x00,0x00,0x00,0x46}} */


/* Object interface: IOcr, ver. 0.0,
   GUID={0xD9F23D61,0xA647,0x11d1,{0xAB,0xCD,0x00,0x20,0x78,0x10,0xD5,0xFE}} */

#pragma code_seg(".orpc")
static const unsigned short IOcr_FormatStringOffsetTable[] =
    {
    0,
    48
    };

static const MIDL_STUBLESS_PROXY_INFO IOcr_ProxyInfo =
    {
    &Object_StubDesc,
    ocr__MIDL_ProcFormatString.Format,
    &IOcr_FormatStringOffsetTable[-3],
    0,
    0,
    0
    };


static const MIDL_SERVER_INFO IOcr_ServerInfo = 
    {
    &Object_StubDesc,
    0,
    ocr__MIDL_ProcFormatString.Format,
    &IOcr_FormatStringOffsetTable[-3],
    0,
    0,
    0,
    0};
CINTERFACE_PROXY_VTABLE(5) _IOcrProxyVtbl = 
{
    &IOcr_ProxyInfo,
    &IID_IOcr,
    IUnknown_QueryInterface_Proxy,
    IUnknown_AddRef_Proxy,
    IUnknown_Release_Proxy ,
    (void *) (INT_PTR) -1 /* IOcr::OcrImage */ ,
    (void *) (INT_PTR) -1 /* IOcr::OcrZone */
};

const CInterfaceStubVtbl _IOcrStubVtbl =
{
    &IID_IOcr,
    &IOcr_ServerInfo,
    5,
    0, /* pure interpreted */
    CStdStubBuffer_METHODS
};


/* Standard interface: __MIDL_itf_ocr_0000_0001, ver. 0.0,
   GUID={0x00000000,0x0000,0x0000,{0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00}} */


/* Object interface: ISpell, ver. 0.0,
   GUID={0xD9F23D63,0xA647,0x11d1,{0xAB,0xCD,0x00,0x20,0x78,0x10,0xD5,0xFE}} */

#pragma code_seg(".orpc")
static const unsigned short ISpell_FormatStringOffsetTable[] =
    {
    102
    };

static const MIDL_STUBLESS_PROXY_INFO ISpell_ProxyInfo =
    {
    &Object_StubDesc,
    ocr__MIDL_ProcFormatString.Format,
    &ISpell_FormatStringOffsetTable[-3],
    0,
    0,
    0
    };


static const MIDL_SERVER_INFO ISpell_ServerInfo = 
    {
    &Object_StubDesc,
    0,
    ocr__MIDL_ProcFormatString.Format,
    &ISpell_FormatStringOffsetTable[-3],
    0,
    0,
    0,
    0};
CINTERFACE_PROXY_VTABLE(4) _ISpellProxyVtbl = 
{
    &ISpell_ProxyInfo,
    &IID_ISpell,
    IUnknown_QueryInterface_Proxy,
    IUnknown_AddRef_Proxy,
    IUnknown_Release_Proxy ,
    (void *) (INT_PTR) -1 /* ISpell::Check */
};

const CInterfaceStubVtbl _ISpellStubVtbl =
{
    &IID_ISpell,
    &ISpell_ServerInfo,
    4,
    0, /* pure interpreted */
    CStdStubBuffer_METHODS
};


/* Standard interface: __MIDL_itf_ocr_0000_0002, ver. 0.0,
   GUID={0x00000000,0x0000,0x0000,{0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00}} */

static const MIDL_STUB_DESC Object_StubDesc = 
    {
    0,
    NdrOleAllocate,
    NdrOleFree,
    0,
    0,
    0,
    0,
    0,
    ocr__MIDL_TypeFormatString.Format,
    1, /* -error bounds_check flag */
    0x50002, /* Ndr library version */
    0,
    0x700022b, /* MIDL Version 7.0.555 */
    0,
    0,
    0,  /* notify & notify_flag routine table */
    0x1, /* MIDL flag */
    0, /* cs routines */
    0,   /* proxy/server info */
    0
    };

const CInterfaceProxyVtbl * const _ocr_ProxyVtblList[] = 
{
    ( CInterfaceProxyVtbl *) &_IOcrProxyVtbl,
    ( CInterfaceProxyVtbl *) &_ISpellProxyVtbl,
    0
};

const CInterfaceStubVtbl * const _ocr_StubVtblList[] = 
{
    ( CInterfaceStubVtbl *) &_IOcrStubVtbl,
    ( CInterfaceStubVtbl *) &_ISpellStubVtbl,
    0
};

PCInterfaceName const _ocr_InterfaceNamesList[] = 
{
    "IOcr",
    "ISpell",
    0
};


#define _ocr_CHECK_IID(n)	IID_GENERIC_CHECK_IID( _ocr, pIID, n)

int __stdcall _ocr_IID_Lookup( const IID * pIID, int * pIndex )
{
    IID_BS_LOOKUP_SETUP

    IID_BS_LOOKUP_INITIAL_TEST( _ocr, 2, 1 )
    IID_BS_LOOKUP_RETURN_RESULT( _ocr, 2, *pIndex )
    
}

const ExtendedProxyFileInfo ocr_ProxyFileInfo = 
{
    (PCInterfaceProxyVtblList *) & _ocr_ProxyVtblList,
    (PCInterfaceStubVtblList *) & _ocr_StubVtblList,
    (const PCInterfaceName * ) & _ocr_InterfaceNamesList,
    0, /* no delegation */
    & _ocr_IID_Lookup, 
    2,
    2,
    0, /* table of [async_uuid] interfaces */
    0, /* Filler1 */
    0, /* Filler2 */
    0  /* Filler3 */
};
#pragma optimize("", on )
#if _MSC_VER >= 1200
#pragma warning(pop)
#endif


#endif /* !defined(_M_IA64) && !defined(_M_AMD64)*/

