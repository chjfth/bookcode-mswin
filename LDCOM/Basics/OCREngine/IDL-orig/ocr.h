/* this ALWAYS GENERATED file contains the definitions for the interfaces */


/* File created by MIDL compiler version 5.01.0164 */
/* at Sun Oct 25 16:51:14 1998
 */
/* Compiler settings for ocr.idl:
    Oicf (OptLev=i2), W1, Zp8, env=Win32, ms_ext, c_ext
    error checks: allocation ref bounds_check enum stub_data 
*/
//@@MIDL_FILE_HEADING(  )


/* verify that the <rpcndr.h> version is high enough to compile this file*/
#ifndef __REQUIRED_RPCNDR_H_VERSION__
#define __REQUIRED_RPCNDR_H_VERSION__ 440
#endif

#include "rpc.h"
#include "rpcndr.h"

#ifndef __RPCNDR_H_VERSION__
#error this stub requires an updated version of <rpcndr.h>
#endif // __RPCNDR_H_VERSION__

#ifndef COM_NO_WINDOWS_H
#include "windows.h"
#include "ole2.h"
#endif /*COM_NO_WINDOWS_H*/

#ifndef __ocr_h__
#define __ocr_h__

#ifdef __cplusplus
extern "C"{
#endif 

/* Forward Declarations */ 

#ifndef __IOcr_FWD_DEFINED__
#define __IOcr_FWD_DEFINED__
typedef interface IOcr IOcr;
#endif 	/* __IOcr_FWD_DEFINED__ */


#ifndef __ISpell_FWD_DEFINED__
#define __ISpell_FWD_DEFINED__
typedef interface ISpell ISpell;
#endif 	/* __ISpell_FWD_DEFINED__ */


#ifndef __CoOCREngine_FWD_DEFINED__
#define __CoOCREngine_FWD_DEFINED__

#ifdef __cplusplus
typedef class CoOCREngine CoOCREngine;
#else
typedef struct CoOCREngine CoOCREngine;
#endif /* __cplusplus */

#endif 	/* __CoOCREngine_FWD_DEFINED__ */


/* header files for imported files */
#include "unknwn.h"

void __RPC_FAR * __RPC_USER MIDL_user_allocate(size_t);
void __RPC_USER MIDL_user_free( void __RPC_FAR * ); 

/* interface __MIDL_itf_ocr_0000 */
/* [local] */ 

struct  Zone
    {
    short m_sX;
    short m_sY;
    short m_sWidth;
    short m_sHeight;
    };


extern RPC_IF_HANDLE __MIDL_itf_ocr_0000_v0_0_c_ifspec;
extern RPC_IF_HANDLE __MIDL_itf_ocr_0000_v0_0_s_ifspec;

#ifndef __IOcr_INTERFACE_DEFINED__
#define __IOcr_INTERFACE_DEFINED__

/* interface IOcr */
/* [uuid][object] */ 


EXTERN_C const IID IID_IOcr;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("D9F23D61-A647-11d1-ABCD-00207810D5FE")
    IOcr : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE OcrImage( 
            /* [in] */ long lImageSize,
            /* [size_is][in] */ byte __RPC_FAR *pbImage,
            /* [string][out] */ wchar_t __RPC_FAR *__RPC_FAR *pwszOcrText) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE OcrZone( 
            /* [in] */ long lImageSize,
            /* [size_is][in] */ byte __RPC_FAR *pbImage,
            /* [in] */ struct Zone zone,
            /* [string][out] */ wchar_t __RPC_FAR *__RPC_FAR *pwszOcrText) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IOcrVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *QueryInterface )( 
            IOcr __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void __RPC_FAR *__RPC_FAR *ppvObject);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *AddRef )( 
            IOcr __RPC_FAR * This);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *Release )( 
            IOcr __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *OcrImage )( 
            IOcr __RPC_FAR * This,
            /* [in] */ long lImageSize,
            /* [size_is][in] */ byte __RPC_FAR *pbImage,
            /* [string][out] */ wchar_t __RPC_FAR *__RPC_FAR *pwszOcrText);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *OcrZone )( 
            IOcr __RPC_FAR * This,
            /* [in] */ long lImageSize,
            /* [size_is][in] */ byte __RPC_FAR *pbImage,
            /* [in] */ struct Zone zone,
            /* [string][out] */ wchar_t __RPC_FAR *__RPC_FAR *pwszOcrText);
        
        END_INTERFACE
    } IOcrVtbl;

    interface IOcr
    {
        CONST_VTBL struct IOcrVtbl __RPC_FAR *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IOcr_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IOcr_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IOcr_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IOcr_OcrImage(This,lImageSize,pbImage,pwszOcrText)	\
    (This)->lpVtbl -> OcrImage(This,lImageSize,pbImage,pwszOcrText)

#define IOcr_OcrZone(This,lImageSize,pbImage,zone,pwszOcrText)	\
    (This)->lpVtbl -> OcrZone(This,lImageSize,pbImage,zone,pwszOcrText)

#endif /* COBJMACROS */


#endif 	/* C style interface */



HRESULT STDMETHODCALLTYPE IOcr_OcrImage_Proxy( 
    IOcr __RPC_FAR * This,
    /* [in] */ long lImageSize,
    /* [size_is][in] */ byte __RPC_FAR *pbImage,
    /* [string][out] */ wchar_t __RPC_FAR *__RPC_FAR *pwszOcrText);


void __RPC_STUB IOcr_OcrImage_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IOcr_OcrZone_Proxy( 
    IOcr __RPC_FAR * This,
    /* [in] */ long lImageSize,
    /* [size_is][in] */ byte __RPC_FAR *pbImage,
    /* [in] */ struct Zone zone,
    /* [string][out] */ wchar_t __RPC_FAR *__RPC_FAR *pwszOcrText);


void __RPC_STUB IOcr_OcrZone_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IOcr_INTERFACE_DEFINED__ */


/* interface __MIDL_itf_ocr_0006 */
/* [local] */ 

struct  PossibleWords
    {
    wchar_t wszOne[ 16 ];
    wchar_t wszTwo[ 16 ];
    };


extern RPC_IF_HANDLE __MIDL_itf_ocr_0006_v0_0_c_ifspec;
extern RPC_IF_HANDLE __MIDL_itf_ocr_0006_v0_0_s_ifspec;

#ifndef __ISpell_INTERFACE_DEFINED__
#define __ISpell_INTERFACE_DEFINED__

/* interface ISpell */
/* [uuid][object] */ 


EXTERN_C const IID IID_ISpell;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("D9F23D63-A647-11d1-ABCD-00207810D5FE")
    ISpell : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE Check( 
            /* [string][in] */ wchar_t __RPC_FAR *pwszWord,
            /* [out] */ struct PossibleWords __RPC_FAR *pWords) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct ISpellVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *QueryInterface )( 
            ISpell __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void __RPC_FAR *__RPC_FAR *ppvObject);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *AddRef )( 
            ISpell __RPC_FAR * This);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *Release )( 
            ISpell __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Check )( 
            ISpell __RPC_FAR * This,
            /* [string][in] */ wchar_t __RPC_FAR *pwszWord,
            /* [out] */ struct PossibleWords __RPC_FAR *pWords);
        
        END_INTERFACE
    } ISpellVtbl;

    interface ISpell
    {
        CONST_VTBL struct ISpellVtbl __RPC_FAR *lpVtbl;
    };

    

#ifdef COBJMACROS


#define ISpell_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define ISpell_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define ISpell_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define ISpell_Check(This,pwszWord,pWords)	\
    (This)->lpVtbl -> Check(This,pwszWord,pWords)

#endif /* COBJMACROS */


#endif 	/* C style interface */



HRESULT STDMETHODCALLTYPE ISpell_Check_Proxy( 
    ISpell __RPC_FAR * This,
    /* [string][in] */ wchar_t __RPC_FAR *pwszWord,
    /* [out] */ struct PossibleWords __RPC_FAR *pWords);


void __RPC_STUB ISpell_Check_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __ISpell_INTERFACE_DEFINED__ */



#ifndef __OCREngineLib_LIBRARY_DEFINED__
#define __OCREngineLib_LIBRARY_DEFINED__

/* library OCREngineLib */
/* [version][uuid] */ 


EXTERN_C const IID LIBID_OCREngineLib;

EXTERN_C const CLSID CLSID_CoOCREngine;

#ifdef __cplusplus

class DECLSPEC_UUID("DF22A6B2-A58A-11D1-ABCC-00207810d5fe")
CoOCREngine;
#endif
#endif /* __OCREngineLib_LIBRARY_DEFINED__ */

/* interface __MIDL_itf_ocr_0007 */
/* [local] */ 

DEFINE_GUID(OCREngineLib, 0x36efd0b1, 0xb326, 0x11d1, 0xab, 0xde, 0x0, 0x20, 0x78, 0x10, 0xd5, 0xfe);
DEFINE_GUID(CLSID_OcrEngine, 0xdf22a6b2, 0xa58a, 0x11d1, 0xab, 0xcc, 0x0, 0x20, 0x78, 0x10, 0xd5, 0xfe);
DEFINE_GUID(IID_IOcr, 0xd9f23d61, 0xa647, 0x11d1, 0xab, 0xcd, 0x0, 0x20, 0x78, 0x10, 0xd5, 0xfe);
DEFINE_GUID(IID_ISpell, 0xd9f23d63, 0xa647, 0x11d1, 0xab, 0xcd, 0x0, 0x20, 0x78, 0x10, 0xd5, 0xfe);


extern RPC_IF_HANDLE __MIDL_itf_ocr_0007_v0_0_c_ifspec;
extern RPC_IF_HANDLE __MIDL_itf_ocr_0007_v0_0_s_ifspec;

/* Additional Prototypes for ALL interfaces */

/* end of Additional Prototypes */

#ifdef __cplusplus
}
#endif

#endif
