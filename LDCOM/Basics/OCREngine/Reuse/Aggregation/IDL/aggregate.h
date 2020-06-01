/* this ALWAYS GENERATED file contains the definitions for the interfaces */


/* File created by MIDL compiler version 5.01.0164 */
/* at Sun Oct 25 19:24:18 1998
 */
/* Compiler settings for aggregate.idl:
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

#ifndef __aggregate_h__
#define __aggregate_h__

#ifdef __cplusplus
extern "C"{
#endif 

/* Forward Declarations */ 

#ifndef __IThesaurus_FWD_DEFINED__
#define __IThesaurus_FWD_DEFINED__
typedef interface IThesaurus IThesaurus;
#endif 	/* __IThesaurus_FWD_DEFINED__ */


/* header files for imported files */
#include "unknwn.h"

void __RPC_FAR * __RPC_USER MIDL_user_allocate(size_t);
void __RPC_USER MIDL_user_free( void __RPC_FAR * ); 

#ifndef __IThesaurus_INTERFACE_DEFINED__
#define __IThesaurus_INTERFACE_DEFINED__

/* interface IThesaurus */
/* [uuid][object] */ 


EXTERN_C const IID IID_IThesaurus;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("A0F35540-C5C1-11d1-9150-006008052F2D")
    IThesaurus : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE LookUp( void) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IThesaurusVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *QueryInterface )( 
            IThesaurus __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void __RPC_FAR *__RPC_FAR *ppvObject);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *AddRef )( 
            IThesaurus __RPC_FAR * This);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *Release )( 
            IThesaurus __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *LookUp )( 
            IThesaurus __RPC_FAR * This);
        
        END_INTERFACE
    } IThesaurusVtbl;

    interface IThesaurus
    {
        CONST_VTBL struct IThesaurusVtbl __RPC_FAR *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IThesaurus_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IThesaurus_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IThesaurus_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IThesaurus_LookUp(This)	\
    (This)->lpVtbl -> LookUp(This)

#endif /* COBJMACROS */


#endif 	/* C style interface */



HRESULT STDMETHODCALLTYPE IThesaurus_LookUp_Proxy( 
    IThesaurus __RPC_FAR * This);


void __RPC_STUB IThesaurus_LookUp_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IThesaurus_INTERFACE_DEFINED__ */


/* interface __MIDL_itf_aggregate_0006 */
/* [local] */ 

DEFINE_GUID(CLSID_Thesaurus, 0xa0f3554a, 0xc5c1, 0x11d1, 0x91, 0x50, 0x0, 0x60, 0x8, 0x5, 0x2f, 0x2d);
DEFINE_GUID(IID_IThesaurus, 0xa0f35540, 0xc5c1, 0x11d1, 0x91, 0x50, 0x0, 0x60, 0x8, 0x5, 0x2f, 0x2d);


extern RPC_IF_HANDLE __MIDL_itf_aggregate_0006_v0_0_c_ifspec;
extern RPC_IF_HANDLE __MIDL_itf_aggregate_0006_v0_0_s_ifspec;

/* Additional Prototypes for ALL interfaces */

/* end of Additional Prototypes */

#ifdef __cplusplus
}
#endif

#endif
