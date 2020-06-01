/* this ALWAYS GENERATED file contains the definitions for the interfaces */


/* File created by MIDL compiler version 5.01.0164 */
/* at Sun Oct 25 19:10:51 1998
 */
/* Compiler settings for contain.idl:
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

#ifndef __contain_h__
#define __contain_h__

#ifdef __cplusplus
extern "C"{
#endif 

/* Forward Declarations */ 

#ifndef __IDictionary_FWD_DEFINED__
#define __IDictionary_FWD_DEFINED__
typedef interface IDictionary IDictionary;
#endif 	/* __IDictionary_FWD_DEFINED__ */


/* header files for imported files */
#include "unknwn.h"

void __RPC_FAR * __RPC_USER MIDL_user_allocate(size_t);
void __RPC_USER MIDL_user_free( void __RPC_FAR * ); 

#ifndef __IDictionary_INTERFACE_DEFINED__
#define __IDictionary_INTERFACE_DEFINED__

/* interface IDictionary */
/* [uuid][object] */ 


EXTERN_C const IID IID_IDictionary;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("F122AFB0-C465-11d1-83B4-006008CDD9AE")
    IDictionary : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE LookUp( void) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IDictionaryVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *QueryInterface )( 
            IDictionary __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void __RPC_FAR *__RPC_FAR *ppvObject);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *AddRef )( 
            IDictionary __RPC_FAR * This);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *Release )( 
            IDictionary __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *LookUp )( 
            IDictionary __RPC_FAR * This);
        
        END_INTERFACE
    } IDictionaryVtbl;

    interface IDictionary
    {
        CONST_VTBL struct IDictionaryVtbl __RPC_FAR *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IDictionary_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IDictionary_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IDictionary_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IDictionary_LookUp(This)	\
    (This)->lpVtbl -> LookUp(This)

#endif /* COBJMACROS */


#endif 	/* C style interface */



HRESULT STDMETHODCALLTYPE IDictionary_LookUp_Proxy( 
    IDictionary __RPC_FAR * This);


void __RPC_STUB IDictionary_LookUp_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IDictionary_INTERFACE_DEFINED__ */


/* interface __MIDL_itf_contain_0006 */
/* [local] */ 

DEFINE_GUID(CLSID_Dictionary, 0x80da2d21, 0xc466, 0x11d1, 0x83, 0xb4, 0x0, 0x60, 0x8, 0xcd, 0xd9, 0xae);
DEFINE_GUID(IID_IDictionary, 0xf122afb0, 0xc465, 0x11d1, 0x83, 0xb4, 0x0, 0x60, 0x8, 0xcd, 0xd9, 0xae);


extern RPC_IF_HANDLE __MIDL_itf_contain_0006_v0_0_c_ifspec;
extern RPC_IF_HANDLE __MIDL_itf_contain_0006_v0_0_s_ifspec;

/* Additional Prototypes for ALL interfaces */

/* end of Additional Prototypes */

#ifdef __cplusplus
}
#endif

#endif
