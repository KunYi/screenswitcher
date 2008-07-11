

/* this ALWAYS GENERATED file contains the definitions for the interfaces */


 /* File created by MIDL compiler version 6.00.0366 */
/* at Thu Jun 26 10:49:55 2008
 */
/* Compiler settings for _HidManager.idl:
    Oicf, W1, Zp8, env=Win32 (32b run)
    protocol : dce , ms_ext, c_ext, robust
    error checks: allocation ref bounds_check enum stub_data 
    VC __declspec() decoration level: 
         __declspec(uuid()), __declspec(selectany), __declspec(novtable)
         DECLSPEC_UUID(), MIDL_INTERFACE()
*/
//@@MIDL_FILE_HEADING(  )

#pragma warning( disable: 4049 )  /* more than 64k source lines */


/* verify that the <rpcndr.h> version is high enough to compile this file*/
#ifndef __REQUIRED_RPCNDR_H_VERSION__
#define __REQUIRED_RPCNDR_H_VERSION__ 475
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

#ifndef ___HidManager_h__
#define ___HidManager_h__

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

/* Forward Declarations */ 

#ifndef __IButtonParams_FWD_DEFINED__
#define __IButtonParams_FWD_DEFINED__
typedef interface IButtonParams IButtonParams;
#endif 	/* __IButtonParams_FWD_DEFINED__ */


#ifndef __IHidManager_FWD_DEFINED__
#define __IHidManager_FWD_DEFINED__
typedef interface IHidManager IHidManager;
#endif 	/* __IHidManager_FWD_DEFINED__ */


#ifndef __IHidEvent_FWD_DEFINED__
#define __IHidEvent_FWD_DEFINED__
typedef interface IHidEvent IHidEvent;
#endif 	/* __IHidEvent_FWD_DEFINED__ */


#ifndef __CButtonParams_FWD_DEFINED__
#define __CButtonParams_FWD_DEFINED__

#ifdef __cplusplus
typedef class CButtonParams CButtonParams;
#else
typedef struct CButtonParams CButtonParams;
#endif /* __cplusplus */

#endif 	/* __CButtonParams_FWD_DEFINED__ */


#ifndef __CHidManager_FWD_DEFINED__
#define __CHidManager_FWD_DEFINED__

#ifdef __cplusplus
typedef class CHidManager CHidManager;
#else
typedef struct CHidManager CHidManager;
#endif /* __cplusplus */

#endif 	/* __CHidManager_FWD_DEFINED__ */


/* header files for imported files */
#include "prsht.h"
#include "mshtml.h"
#include "mshtmhst.h"
#include "exdisp.h"
#include "objsafe.h"

#ifdef __cplusplus
extern "C"{
#endif 

void * __RPC_USER MIDL_user_allocate(size_t);
void __RPC_USER MIDL_user_free( void * ); 

#ifndef __IButtonParams_INTERFACE_DEFINED__
#define __IButtonParams_INTERFACE_DEFINED__

/* interface IButtonParams */
/* [unique][helpstring][dual][uuid][object] */ 


EXTERN_C const IID IID_IButtonParams;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("C8B15683-FE3E-4a49-864B-ED9DAAA28966")
    IButtonParams : public IDispatch
    {
    public:
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE Initialize( 
            /* [in] */ unsigned short uHidPage,
            /* [in] */ unsigned short uHidUsage,
            /* [in] */ unsigned short uButtonUsage,
            /* [in] */ WCHAR *wszButtonType) = 0;
        
        virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_HIDPage( 
            /* [in] */ unsigned short uHidPage) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_HIDPage( 
            /* [retval][out] */ unsigned short *puHidPage) = 0;
        
        virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_HIDUsage( 
            /* [in] */ unsigned short uHidUsage) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_HIDUsage( 
            /* [retval][out] */ unsigned short *puHidUsage) = 0;
        
        virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_ButtonUsage( 
            /* [in] */ unsigned short uButtonUsage) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_ButtonUsage( 
            /* [retval][out] */ unsigned short *puButtonUsage) = 0;
        
        virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_ButtonType( 
            /* [in] */ BSTR bstrButtonType) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_ButtonType( 
            /* [retval][out] */ BSTR *pbstrButtonType) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IButtonParamsVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IButtonParams * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IButtonParams * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IButtonParams * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            IButtonParams * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            IButtonParams * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            IButtonParams * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            IButtonParams * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS *pDispParams,
            /* [out] */ VARIANT *pVarResult,
            /* [out] */ EXCEPINFO *pExcepInfo,
            /* [out] */ UINT *puArgErr);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *Initialize )( 
            IButtonParams * This,
            /* [in] */ unsigned short uHidPage,
            /* [in] */ unsigned short uHidUsage,
            /* [in] */ unsigned short uButtonUsage,
            /* [in] */ WCHAR *wszButtonType);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE *put_HIDPage )( 
            IButtonParams * This,
            /* [in] */ unsigned short uHidPage);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_HIDPage )( 
            IButtonParams * This,
            /* [retval][out] */ unsigned short *puHidPage);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE *put_HIDUsage )( 
            IButtonParams * This,
            /* [in] */ unsigned short uHidUsage);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_HIDUsage )( 
            IButtonParams * This,
            /* [retval][out] */ unsigned short *puHidUsage);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE *put_ButtonUsage )( 
            IButtonParams * This,
            /* [in] */ unsigned short uButtonUsage);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_ButtonUsage )( 
            IButtonParams * This,
            /* [retval][out] */ unsigned short *puButtonUsage);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE *put_ButtonType )( 
            IButtonParams * This,
            /* [in] */ BSTR bstrButtonType);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_ButtonType )( 
            IButtonParams * This,
            /* [retval][out] */ BSTR *pbstrButtonType);
        
        END_INTERFACE
    } IButtonParamsVtbl;

    interface IButtonParams
    {
        CONST_VTBL struct IButtonParamsVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IButtonParams_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IButtonParams_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IButtonParams_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IButtonParams_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)

#define IButtonParams_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)

#define IButtonParams_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)

#define IButtonParams_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)


#define IButtonParams_Initialize(This,uHidPage,uHidUsage,uButtonUsage,wszButtonType)	\
    (This)->lpVtbl -> Initialize(This,uHidPage,uHidUsage,uButtonUsage,wszButtonType)

#define IButtonParams_put_HIDPage(This,uHidPage)	\
    (This)->lpVtbl -> put_HIDPage(This,uHidPage)

#define IButtonParams_get_HIDPage(This,puHidPage)	\
    (This)->lpVtbl -> get_HIDPage(This,puHidPage)

#define IButtonParams_put_HIDUsage(This,uHidUsage)	\
    (This)->lpVtbl -> put_HIDUsage(This,uHidUsage)

#define IButtonParams_get_HIDUsage(This,puHidUsage)	\
    (This)->lpVtbl -> get_HIDUsage(This,puHidUsage)

#define IButtonParams_put_ButtonUsage(This,uButtonUsage)	\
    (This)->lpVtbl -> put_ButtonUsage(This,uButtonUsage)

#define IButtonParams_get_ButtonUsage(This,puButtonUsage)	\
    (This)->lpVtbl -> get_ButtonUsage(This,puButtonUsage)

#define IButtonParams_put_ButtonType(This,bstrButtonType)	\
    (This)->lpVtbl -> put_ButtonType(This,bstrButtonType)

#define IButtonParams_get_ButtonType(This,pbstrButtonType)	\
    (This)->lpVtbl -> get_ButtonType(This,pbstrButtonType)

#endif /* COBJMACROS */


#endif 	/* C style interface */



/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IButtonParams_Initialize_Proxy( 
    IButtonParams * This,
    /* [in] */ unsigned short uHidPage,
    /* [in] */ unsigned short uHidUsage,
    /* [in] */ unsigned short uButtonUsage,
    /* [in] */ WCHAR *wszButtonType);


void __RPC_STUB IButtonParams_Initialize_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE IButtonParams_put_HIDPage_Proxy( 
    IButtonParams * This,
    /* [in] */ unsigned short uHidPage);


void __RPC_STUB IButtonParams_put_HIDPage_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IButtonParams_get_HIDPage_Proxy( 
    IButtonParams * This,
    /* [retval][out] */ unsigned short *puHidPage);


void __RPC_STUB IButtonParams_get_HIDPage_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE IButtonParams_put_HIDUsage_Proxy( 
    IButtonParams * This,
    /* [in] */ unsigned short uHidUsage);


void __RPC_STUB IButtonParams_put_HIDUsage_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IButtonParams_get_HIDUsage_Proxy( 
    IButtonParams * This,
    /* [retval][out] */ unsigned short *puHidUsage);


void __RPC_STUB IButtonParams_get_HIDUsage_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE IButtonParams_put_ButtonUsage_Proxy( 
    IButtonParams * This,
    /* [in] */ unsigned short uButtonUsage);


void __RPC_STUB IButtonParams_put_ButtonUsage_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IButtonParams_get_ButtonUsage_Proxy( 
    IButtonParams * This,
    /* [retval][out] */ unsigned short *puButtonUsage);


void __RPC_STUB IButtonParams_get_ButtonUsage_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE IButtonParams_put_ButtonType_Proxy( 
    IButtonParams * This,
    /* [in] */ BSTR bstrButtonType);


void __RPC_STUB IButtonParams_put_ButtonType_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IButtonParams_get_ButtonType_Proxy( 
    IButtonParams * This,
    /* [retval][out] */ BSTR *pbstrButtonType);


void __RPC_STUB IButtonParams_get_ButtonType_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IButtonParams_INTERFACE_DEFINED__ */


#ifndef __IHidManager_INTERFACE_DEFINED__
#define __IHidManager_INTERFACE_DEFINED__

/* interface IHidManager */
/* [unique][helpstring][dual][uuid][object] */ 


EXTERN_C const IID IID_IHidManager;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("C1607EF3-CA5E-44bb-A0BC-18224670DB31")
    IHidManager : public IDispatch
    {
    public:
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE FindHidDevice( 
            /* [in] */ unsigned short uVendorId,
            /* [in] */ unsigned short uProductId,
            /* [retval][out] */ VARIANT_BOOL *pbResult) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetButtonStatus( 
            /* [in] */ int nButtonNumber,
            /* [retval][out] */ int *pButtonStatus) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE SetButtonStatus( 
            /* [in] */ int nButtonNumber,
            /* [in] */ int nButtonStatus) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetLedStatus( 
            /* [in] */ int nLedNumber,
            /* [retval][out] */ int *pLedStatus) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE SetLedStatus( 
            /* [in] */ int nLedNumber,
            /* [in] */ int nLedStatus) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE SetMultiLedStatus( 
            /* [in] */ int nMultiLedMask) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE SimulateLoopback( 
            /* [in] */ unsigned char *pBuffer) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE SimulateHidKeycode( 
            /* [in] */ unsigned short uHidPage,
            /* [in] */ unsigned short uHidUsage,
            /* [in] */ unsigned short uHidButtonUsage,
            /* [in] */ unsigned short uKeyModifiers) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE FireOnButtonPressed( 
            /* [in] */ IButtonParams *piButtonParams) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IHidManagerVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IHidManager * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IHidManager * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IHidManager * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            IHidManager * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            IHidManager * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            IHidManager * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            IHidManager * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS *pDispParams,
            /* [out] */ VARIANT *pVarResult,
            /* [out] */ EXCEPINFO *pExcepInfo,
            /* [out] */ UINT *puArgErr);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *FindHidDevice )( 
            IHidManager * This,
            /* [in] */ unsigned short uVendorId,
            /* [in] */ unsigned short uProductId,
            /* [retval][out] */ VARIANT_BOOL *pbResult);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *GetButtonStatus )( 
            IHidManager * This,
            /* [in] */ int nButtonNumber,
            /* [retval][out] */ int *pButtonStatus);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *SetButtonStatus )( 
            IHidManager * This,
            /* [in] */ int nButtonNumber,
            /* [in] */ int nButtonStatus);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *GetLedStatus )( 
            IHidManager * This,
            /* [in] */ int nLedNumber,
            /* [retval][out] */ int *pLedStatus);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *SetLedStatus )( 
            IHidManager * This,
            /* [in] */ int nLedNumber,
            /* [in] */ int nLedStatus);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *SetMultiLedStatus )( 
            IHidManager * This,
            /* [in] */ int nMultiLedMask);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *SimulateLoopback )( 
            IHidManager * This,
            /* [in] */ unsigned char *pBuffer);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *SimulateHidKeycode )( 
            IHidManager * This,
            /* [in] */ unsigned short uHidPage,
            /* [in] */ unsigned short uHidUsage,
            /* [in] */ unsigned short uHidButtonUsage,
            /* [in] */ unsigned short uKeyModifiers);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *FireOnButtonPressed )( 
            IHidManager * This,
            /* [in] */ IButtonParams *piButtonParams);
        
        END_INTERFACE
    } IHidManagerVtbl;

    interface IHidManager
    {
        CONST_VTBL struct IHidManagerVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IHidManager_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IHidManager_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IHidManager_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IHidManager_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)

#define IHidManager_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)

#define IHidManager_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)

#define IHidManager_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)


#define IHidManager_FindHidDevice(This,uVendorId,uProductId,pbResult)	\
    (This)->lpVtbl -> FindHidDevice(This,uVendorId,uProductId,pbResult)

#define IHidManager_GetButtonStatus(This,nButtonNumber,pButtonStatus)	\
    (This)->lpVtbl -> GetButtonStatus(This,nButtonNumber,pButtonStatus)

#define IHidManager_SetButtonStatus(This,nButtonNumber,nButtonStatus)	\
    (This)->lpVtbl -> SetButtonStatus(This,nButtonNumber,nButtonStatus)

#define IHidManager_GetLedStatus(This,nLedNumber,pLedStatus)	\
    (This)->lpVtbl -> GetLedStatus(This,nLedNumber,pLedStatus)

#define IHidManager_SetLedStatus(This,nLedNumber,nLedStatus)	\
    (This)->lpVtbl -> SetLedStatus(This,nLedNumber,nLedStatus)

#define IHidManager_SetMultiLedStatus(This,nMultiLedMask)	\
    (This)->lpVtbl -> SetMultiLedStatus(This,nMultiLedMask)

#define IHidManager_SimulateLoopback(This,pBuffer)	\
    (This)->lpVtbl -> SimulateLoopback(This,pBuffer)

#define IHidManager_SimulateHidKeycode(This,uHidPage,uHidUsage,uHidButtonUsage,uKeyModifiers)	\
    (This)->lpVtbl -> SimulateHidKeycode(This,uHidPage,uHidUsage,uHidButtonUsage,uKeyModifiers)

#define IHidManager_FireOnButtonPressed(This,piButtonParams)	\
    (This)->lpVtbl -> FireOnButtonPressed(This,piButtonParams)

#endif /* COBJMACROS */


#endif 	/* C style interface */



/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IHidManager_FindHidDevice_Proxy( 
    IHidManager * This,
    /* [in] */ unsigned short uVendorId,
    /* [in] */ unsigned short uProductId,
    /* [retval][out] */ VARIANT_BOOL *pbResult);


void __RPC_STUB IHidManager_FindHidDevice_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IHidManager_GetButtonStatus_Proxy( 
    IHidManager * This,
    /* [in] */ int nButtonNumber,
    /* [retval][out] */ int *pButtonStatus);


void __RPC_STUB IHidManager_GetButtonStatus_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IHidManager_SetButtonStatus_Proxy( 
    IHidManager * This,
    /* [in] */ int nButtonNumber,
    /* [in] */ int nButtonStatus);


void __RPC_STUB IHidManager_SetButtonStatus_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IHidManager_GetLedStatus_Proxy( 
    IHidManager * This,
    /* [in] */ int nLedNumber,
    /* [retval][out] */ int *pLedStatus);


void __RPC_STUB IHidManager_GetLedStatus_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IHidManager_SetLedStatus_Proxy( 
    IHidManager * This,
    /* [in] */ int nLedNumber,
    /* [in] */ int nLedStatus);


void __RPC_STUB IHidManager_SetLedStatus_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IHidManager_SetMultiLedStatus_Proxy( 
    IHidManager * This,
    /* [in] */ int nMultiLedMask);


void __RPC_STUB IHidManager_SetMultiLedStatus_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IHidManager_SimulateLoopback_Proxy( 
    IHidManager * This,
    /* [in] */ unsigned char *pBuffer);


void __RPC_STUB IHidManager_SimulateLoopback_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IHidManager_SimulateHidKeycode_Proxy( 
    IHidManager * This,
    /* [in] */ unsigned short uHidPage,
    /* [in] */ unsigned short uHidUsage,
    /* [in] */ unsigned short uHidButtonUsage,
    /* [in] */ unsigned short uKeyModifiers);


void __RPC_STUB IHidManager_SimulateHidKeycode_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IHidManager_FireOnButtonPressed_Proxy( 
    IHidManager * This,
    /* [in] */ IButtonParams *piButtonParams);


void __RPC_STUB IHidManager_FireOnButtonPressed_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IHidManager_INTERFACE_DEFINED__ */



#ifndef __HidManager_LIBRARY_DEFINED__
#define __HidManager_LIBRARY_DEFINED__

/* library HidManager */
/* [helpstring][uuid][version] */ 


EXTERN_C const IID LIBID_HidManager;

#ifndef __IHidEvent_DISPINTERFACE_DEFINED__
#define __IHidEvent_DISPINTERFACE_DEFINED__

/* dispinterface IHidEvent */
/* [helpstring][uuid] */ 


EXTERN_C const IID DIID_IHidEvent;

#if defined(__cplusplus) && !defined(CINTERFACE)

    MIDL_INTERFACE("18976042-0890-4dd6-AC79-C042AB95894C")
    IHidEvent : public IDispatch
    {
    };
    
#else 	/* C style interface */

    typedef struct IHidEventVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IHidEvent * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IHidEvent * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IHidEvent * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            IHidEvent * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            IHidEvent * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            IHidEvent * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            IHidEvent * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS *pDispParams,
            /* [out] */ VARIANT *pVarResult,
            /* [out] */ EXCEPINFO *pExcepInfo,
            /* [out] */ UINT *puArgErr);
        
        END_INTERFACE
    } IHidEventVtbl;

    interface IHidEvent
    {
        CONST_VTBL struct IHidEventVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IHidEvent_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IHidEvent_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IHidEvent_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IHidEvent_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)

#define IHidEvent_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)

#define IHidEvent_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)

#define IHidEvent_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)

#endif /* COBJMACROS */


#endif 	/* C style interface */


#endif 	/* __IHidEvent_DISPINTERFACE_DEFINED__ */


EXTERN_C const CLSID CLSID_CButtonParams;

#ifdef __cplusplus

class DECLSPEC_UUID("D0CE718F-574B-41a3-B157-4496FBC40738")
CButtonParams;
#endif

EXTERN_C const CLSID CLSID_CHidManager;

#ifdef __cplusplus

class DECLSPEC_UUID("E0425E75-6644-45dd-87FB-503240B2136A")
CHidManager;
#endif
#endif /* __HidManager_LIBRARY_DEFINED__ */

/* Additional Prototypes for ALL interfaces */

unsigned long             __RPC_USER  BSTR_UserSize(     unsigned long *, unsigned long            , BSTR * ); 
unsigned char * __RPC_USER  BSTR_UserMarshal(  unsigned long *, unsigned char *, BSTR * ); 
unsigned char * __RPC_USER  BSTR_UserUnmarshal(unsigned long *, unsigned char *, BSTR * ); 
void                      __RPC_USER  BSTR_UserFree(     unsigned long *, BSTR * ); 

/* end of Additional Prototypes */

#ifdef __cplusplus
}
#endif

#endif


