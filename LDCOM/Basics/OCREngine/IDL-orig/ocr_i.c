

/* this ALWAYS GENERATED file contains the IIDs and CLSIDs */

/* link this file in with the server and any clients */


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

#pragma warning( disable: 4049 )  /* more than 64k source lines */


#ifdef __cplusplus
extern "C"{
#endif 


#include <rpc.h>
#include <rpcndr.h>

#ifdef _MIDL_USE_GUIDDEF_

#ifndef INITGUID
#define INITGUID
#include <guiddef.h>
#undef INITGUID
#else
#include <guiddef.h>
#endif

#define MIDL_DEFINE_GUID(type,name,l,w1,w2,b1,b2,b3,b4,b5,b6,b7,b8) \
        DEFINE_GUID(name,l,w1,w2,b1,b2,b3,b4,b5,b6,b7,b8)

#else // !_MIDL_USE_GUIDDEF_

#ifndef __IID_DEFINED__
#define __IID_DEFINED__

typedef struct _IID
{
    unsigned long x;
    unsigned short s1;
    unsigned short s2;
    unsigned char  c[8];
} IID;

#endif // __IID_DEFINED__

#ifndef CLSID_DEFINED
#define CLSID_DEFINED
typedef IID CLSID;
#endif // CLSID_DEFINED

#define MIDL_DEFINE_GUID(type,name,l,w1,w2,b1,b2,b3,b4,b5,b6,b7,b8) \
        const type name = {l,w1,w2,{b1,b2,b3,b4,b5,b6,b7,b8}}

#endif !_MIDL_USE_GUIDDEF_

MIDL_DEFINE_GUID(IID, IID_IOcr,0xD9F23D61,0xA647,0x11d1,0xAB,0xCD,0x00,0x20,0x78,0x10,0xD5,0xFE);


MIDL_DEFINE_GUID(IID, IID_ISpell,0xD9F23D63,0xA647,0x11d1,0xAB,0xCD,0x00,0x20,0x78,0x10,0xD5,0xFE);


MIDL_DEFINE_GUID(IID, LIBID_OCREngineLib,0x36EFD0B1,0xB326,0x11d1,0xAB,0xDE,0x00,0x20,0x78,0x10,0xD5,0xFE);


MIDL_DEFINE_GUID(CLSID, CLSID_CoOCREngine,0xDF22A6B2,0xA58A,0x11D1,0xAB,0xCC,0x00,0x20,0x78,0x10,0xd5,0xfe);

#undef MIDL_DEFINE_GUID

#ifdef __cplusplus
}
#endif



