

/* this ALWAYS GENERATED file contains the definitions for the interfaces */


 /* File created by MIDL compiler version 8.01.0628 */
/* at Tue Jan 19 11:14:07 2038
 */
/* Compiler settings for message_host_impl.idl:
    Oicf, W1, Zp8, env=Win64 (32b run), target_arch=AMD64 8.01.0628 
    protocol : all , ms_ext, c_ext, robust
    error checks: allocation ref bounds_check enum stub_data 
    VC __declspec() decoration level: 
         __declspec(uuid()), __declspec(selectany), __declspec(novtable)
         DECLSPEC_UUID(), MIDL_INTERFACE()
*/
/* @@MIDL_FILE_HEADING(  ) */



/* verify that the <rpcndr.h> version is high enough to compile this file*/
#ifndef __REQUIRED_RPCNDR_H_VERSION__
#define __REQUIRED_RPCNDR_H_VERSION__ 500
#endif

#include "rpc.h"
#include "rpcndr.h"

#ifndef __RPCNDR_H_VERSION__
#error this stub requires an updated version of <rpcndr.h>
#endif /* __RPCNDR_H_VERSION__ */


#ifndef __message_host_impl_h_h__
#define __message_host_impl_h_h__

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

#ifndef DECLSPEC_XFGVIRT
#if defined(_CONTROL_FLOW_GUARD_XFG)
#define DECLSPEC_XFGVIRT(base, func) __declspec(xfg_virtual(base, func))
#else
#define DECLSPEC_XFGVIRT(base, func)
#endif
#endif

/* Forward Declarations */ 

#ifndef __Ihost_object_sample_FWD_DEFINED__
#define __Ihost_object_sample_FWD_DEFINED__
typedef interface Ihost_object_sample Ihost_object_sample;

#endif 	/* __Ihost_object_sample_FWD_DEFINED__ */


#ifndef __host_object_sample_FWD_DEFINED__
#define __host_object_sample_FWD_DEFINED__

#ifdef __cplusplus
typedef class host_object_sample host_object_sample;
#else
typedef struct host_object_sample host_object_sample;
#endif /* __cplusplus */

#endif 	/* __host_object_sample_FWD_DEFINED__ */


/* header files for imported files */
#include "oaidl.h"
#include "ocidl.h"

#ifdef __cplusplus
extern "C"{
#endif 



#ifndef __hostlibrary_LIBRARY_DEFINED__
#define __hostlibrary_LIBRARY_DEFINED__

/* library hostlibrary */
/* [version][uuid] */ 


EXTERN_C const IID LIBID_hostlibrary;

#ifndef __Ihost_object_sample_INTERFACE_DEFINED__
#define __Ihost_object_sample_INTERFACE_DEFINED__

/* interface Ihost_object_sample */
/* [local][object][uuid] */ 


EXTERN_C const IID IID_Ihost_object_sample;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("3a14c9c0-bc3e-453f-a314-4ce4a0ec81d8")
    Ihost_object_sample : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE testfunc( 
            /* [in] */ FLOAT value) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE TestFunStr( 
            /* [in] */ BSTR str) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE QuitGame( void) = 0;
        
    };
    
    
#else 	/* C style interface */

    typedef struct Ihost_object_sampleVtbl
    {
        BEGIN_INTERFACE
        
        DECLSPEC_XFGVIRT(IUnknown, QueryInterface)
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            Ihost_object_sample * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            _COM_Outptr_  void **ppvObject);
        
        DECLSPEC_XFGVIRT(IUnknown, AddRef)
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            Ihost_object_sample * This);
        
        DECLSPEC_XFGVIRT(IUnknown, Release)
        ULONG ( STDMETHODCALLTYPE *Release )( 
            Ihost_object_sample * This);
        
        DECLSPEC_XFGVIRT(Ihost_object_sample, testfunc)
        HRESULT ( STDMETHODCALLTYPE *testfunc )( 
            Ihost_object_sample * This,
            /* [in] */ FLOAT value);
        
        DECLSPEC_XFGVIRT(Ihost_object_sample, TestFunStr)
        HRESULT ( STDMETHODCALLTYPE *TestFunStr )( 
            Ihost_object_sample * This,
            /* [in] */ BSTR str);
        
        DECLSPEC_XFGVIRT(Ihost_object_sample, QuitGame)
        HRESULT ( STDMETHODCALLTYPE *QuitGame )( 
            Ihost_object_sample * This);
        
        END_INTERFACE
    } Ihost_object_sampleVtbl;

    interface Ihost_object_sample
    {
        CONST_VTBL struct Ihost_object_sampleVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define Ihost_object_sample_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define Ihost_object_sample_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define Ihost_object_sample_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define Ihost_object_sample_testfunc(This,value)	\
    ( (This)->lpVtbl -> testfunc(This,value) ) 

#define Ihost_object_sample_TestFunStr(This,str)	\
    ( (This)->lpVtbl -> TestFunStr(This,str) ) 

#define Ihost_object_sample_QuitGame(This)	\
    ( (This)->lpVtbl -> QuitGame(This) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __Ihost_object_sample_INTERFACE_DEFINED__ */


EXTERN_C const CLSID CLSID_host_object_sample;

#ifdef __cplusplus

class DECLSPEC_UUID("637abc45-11f7-4dde-84b4-317d62a638d3")
host_object_sample;
#endif
#endif /* __hostlibrary_LIBRARY_DEFINED__ */

/* Additional Prototypes for ALL interfaces */

/* end of Additional Prototypes */

#ifdef __cplusplus
}
#endif

#endif


