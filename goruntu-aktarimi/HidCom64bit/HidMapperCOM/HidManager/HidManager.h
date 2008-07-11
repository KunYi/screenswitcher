////////////////////////////////////////////////////////////////////////////////
//      Copyright (c) 2008, Intel Corporation.  All Rights Reserved.	      //	
//                                                                            //
//   Portions used from the MSDN are Copyright (c) Microsoft Corporation.     //
//                                                                            //
//              INTEL CORPORATION PROPRIETARY INFORMATION                     //
//                                                                            //
// The source code contained or described herein and all documents related to //
// the source code (Material) are owned by Intel Corporation or its suppliers //
// or licensors. Title to the Material remains with Intel Corporation or its  //
// suppliers and licensors. The Material contains trade secrets and           //
// proprietary and confidential information of Intel or its suppliers and     //
// licensors. The Material is protected by worldwide copyright and trade      //
// secret laws and treaty provisions. No part of the Material may be used,    //
// copied, reproduced, modified, published, uploaded, posted, transmitted,    //
// distributed, or disclosed in any way without Intel’s prior express written //
// permission.                                                                //
//                                                                            //
// No license under any patent, copyright, trade secret or other intellectual //
// property right is granted to or conferred upon you by disclosure or        //
// delivery of the Materials, either expressly, by implication, inducement,   //
// estoppel or otherwise. Any license under such intellectual property rights //
// must be express and approved by Intel in writing.                          //
//                                                                            //
// THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY      //
// KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE        //
// IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR      //
// PURPOSE.                                                                   //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////
//
// $Archive: /Lorado/HidMapperCOM/HidManager/HidManager.h $ 
//  
// $Author: Nmnguye1 $
//         
// $Date: 2/27/08 12:19p $
//                                                                            
////////////////////////////////////////////////////////////////////////////////

//
// HidManager.h : Declaration of the IHidManager interface and CHidManager class.
//

//
// Windows Header Files
//
#include <windows.h>
#include <strsafe.h>
#define MAX_STRING_SIZE 256
extern "C" 
{
#ifdef WIN64
#include "winddk64/hidsdi.h"
#else
#include "winddk/hidsdi.h"
#endif
}
#include <setupapi.h>

// PnP Vendor ID for Intel devices, registered with USB Implementer's Forum
#define VID_INTEL			0x8086  //
// PnP Product IDs for Intel devices, registered with brad.w.hosler@intel.com
#define PID_ACPIHID			0x1018
#define PID_LED_ARRAY_LEFT	0x1019
#define PID_LED_ARRAY_RIGHT	0x101a
#define PID_ACCELEROMETER	0x101b
#define PID_EINK_DISPLAY	0x101c
#define PID_VFD_DISPLAY		0x101d

#define KEYMOD_RIGHT_GUI	0x80
#define KEYMOD_RIGHT_ALT	0x40
#define KEYMOD_RIGHT_SHIFT	0x20
#define KEYMOD_RIGHT_CTRL	0x10
#define KEYMOD_LEFT_GUI		0x08
#define KEYMOD_LEFT_ALT		0x04
#define KEYMOD_LEFT_SHIFT	0x02
#define KEYMOD_LEFT_CTRL	0x01

//
// IButtonParams interface
//
[
	object,
	uuid = "C8B15683-FE3E-4a49-864B-ED9DAAA28966",
	dual,
	helpstring = "ButtonParams Interface",
	pointer_default(unique)
]
__interface IButtonParams : IDispatch
{
	[id(1), helpstring("Initialize method")] HRESULT Initialize([in] unsigned short uHidPage, [in] unsigned short uHidUsage, [in] unsigned short uButtonUsage, [in] WCHAR *wszButtonType);
	[id(2), propput, helpstring("HIDPage property")] HRESULT HIDPage([in] unsigned short uHidPage);
	[id(2), propget, helpstring("HIDPage property")] HRESULT HIDPage([out, retval] unsigned short *puHidPage);
	[id(3), propput, helpstring("HIDUsage property")] HRESULT HIDUsage([in] unsigned short uHidUsage);
	[id(3), propget, helpstring("HIDUsage property")] HRESULT HIDUsage([out, retval] unsigned short *puHidUsage);
	[id(4), propput, helpstring("ButtonUsage property")] HRESULT ButtonUsage([in] unsigned short uButtonUsage);
	[id(4), propget, helpstring("ButtonUsage property")] HRESULT ButtonUsage([out, retval] unsigned short *puButtonUsage);
	[id(5), propput, helpstring("ButtonType property")] HRESULT ButtonType([in] BSTR bstrButtonType);
	[id(5), propget, helpstring("ButtonType property")] HRESULT ButtonType([out, retval] BSTR *pbstrButtonType);
};

//
// IHidEvent interface
//
[
	dispinterface,
	uuid("18976042-0890-4dd6-AC79-C042AB95894C"),
	helpstring("HidEvent Interface")
]
__interface IHidEvent
{
	[id(1), helpstring("method ButtonPressed")] HRESULT ButtonPressed(IButtonParams *piButtonParams);
};

//
// IHidManager interface
//
[
	object,
	uuid = "{C1607EF3-CA5E-44bb-A0BC-18224670DB31}",
	dual,
	helpstring = "HidManager Interface",
	pointer_default(unique)
]
__interface IHidManager : IDispatch
{
	[id(1), helpstring("method FindHidDevice")] HRESULT FindHidDevice([in] unsigned short uVendorId, [in] unsigned short uProductId, [out, retval] VARIANT_BOOL *pbResult);
	[id(2), helpstring("method GetButtonStatus")] HRESULT GetButtonStatus([in] int nButtonNumber, [out, retval] int *pButtonStatus);
	[id(3), helpstring("method SetButtonStatus")] HRESULT SetButtonStatus([in] int nButtonNumber, [in] int nButtonStatus);
	[id(4), helpstring("method GetLedStatus")] HRESULT GetLedStatus([in] int nLedNumber, [out, retval] int *pLedStatus);
	[id(5), helpstring("method SetLedStatus")] HRESULT SetLedStatus([in] int nLedNumber, [in] int nLedStatus);
	[id(6), helpstring("method SetMultiLedStatus")] HRESULT SetMultiLedStatus([in] int nMultiLedMask);
	[id(7), helpstring("method SimulateLoopback")] HRESULT SimulateLoopback([in] unsigned char *pBuffer);
	[id(8), helpstring("method SimulateHidKeycode")] HRESULT SimulateHidKeycode([in] unsigned short uHidPage, [in] unsigned short uHidUsage, [in] unsigned short uHidButtonUsage, [in] unsigned short uKeyModifiers);
	[id(9), helpstring("method FireOnButtonPressed")] HRESULT FireOnButtonPressed([in] IButtonParams *piButtonParams);
};

//
// CButtonParams class
//
[
	coclass,
	threading(apartment),
	vi_progid("ButtonParams"),
	progid("ButtonParams.1"),
	version(1.0),
	uuid("D0CE718F-574B-41a3-B157-4496FBC40738"),
	helpstring("ButtonParams Class")
]
class CButtonParams :
	public IButtonParams
{
	private:
		long				m_nRef;
		unsigned short		m_uHidPage;
		unsigned short		m_uHidUsage;
		unsigned short		m_uButtonUsage;
		BSTR				m_bstrButtonType;
	public:
		CButtonParams();
		virtual ~CButtonParams();
		//
		// IUnknown methods
		//
		//STDMETHOD(QueryInterface)(REFIID riid, LPVOID* ppvObject);
		//STDMETHOD_(ULONG, AddRef)();
		//STDMETHOD_(ULONG, Release)();
		//
		// IDispatch methods
		//
		//STDMETHOD(GetTypeInfoCount)(UINT *pcTInfo);
		//STDMETHOD(GetTypeInfo)(UINT iTInfo, LCID lcid, ITypeInfo **ppTInfo);
		//STDMETHOD(GetIDsOfNames)(const IID& riid, LPOLESTR *rgszNames, UINT cNames, LCID lcid, DISPID *rgDispId);
		//STDMETHOD(Invoke)(DISPID dispidMember, const IID& riid, LCID lcid, WORD wFlags, DISPPARAMS *pDispParams, VARIANT *pVarResult, EXCEPINFO *pExcepInfo, UINT *puArgErr);
		//
		// IButtonParams methods
		//
		STDMETHOD(Initialize)(unsigned short uHidPage, unsigned short uHidUsage, unsigned short uButtonUsage, WCHAR *wszButtonType);
		STDMETHOD(put_HIDPage)(unsigned short uHidPage);
		STDMETHOD(get_HIDPage)(unsigned short *puHidPage);
		STDMETHOD(put_HIDUsage)(unsigned short uHidUsage);
		STDMETHOD(get_HIDUsage)(unsigned short *puHidUsage);
		STDMETHOD(put_ButtonUsage)(unsigned short uButtonUsage);
		STDMETHOD(get_ButtonUsage)(unsigned short *puButtonUsage);
		STDMETHOD(put_ButtonType)(BSTR bstrButtonType);
		STDMETHOD(get_ButtonType)(BSTR *pbstrButtonType);
};

//
// CHidManager class
//
[
	coclass,
	event_source(com),
	threading(apartment),
	vi_progid("HidManager"),
	progid("HidManager.1"),
	version(1.0),
	uuid("E0425E75-6644-45dd-87FB-503240B2136A"),
	helpstring("HidManager Class")
]
class CHidManager :
	public IHidManager
{
	private:
		bool FindAndFillDeviceCapabilities(unsigned short uVendorId, unsigned short uProductId);
		int FindDevicesFromGuid(GUID* pHidGuid, TCHAR *pszDevicePathname[]);
		STDMETHOD(RegisterForEvents)(VARIANT_BOOL *pbSuccess);
		STDMETHOD(UnregisterForEvents)(void);

		long				m_nRef;
		bool				m_bDeviceDetected;
		unsigned short		m_uDeviceVendorId;
		unsigned short		m_uDeviceProductId;
		HIDD_ATTRIBUTES		m_HidAttributes;
		HANDLE				m_hHidFileInputOutput;
		HIDP_CAPS			m_hidcapsInputOutput;
		HANDLE				m_hHidFileFeature;
		HIDP_CAPS			m_hidcapsFeature;
		HANDLE				m_hHidFileLoopback;
		HIDP_CAPS			m_hidcapsLoopback;
		HWND				m_hwnd;
		WNDPROC				m_dlgProc;

	public:
		CHidManager();
		virtual ~CHidManager();
		//
		// IUnknown methods
		//
		//STDMETHOD(QueryInterface)(REFIID riid, LPVOID* ppvObject);
		//STDMETHOD_(ULONG, AddRef)();
		//STDMETHOD_(ULONG, Release)();
		//
		// IDispatch methods
		//
		//STDMETHOD(GetTypeInfoCount)(UINT *pcTInfo);
		//STDMETHOD(GetTypeInfo)(UINT iTInfo, LCID lcid, ITypeInfo **ppTInfo);
		//STDMETHOD(GetIDsOfNames)(const IID& riid, LPOLESTR *rgszNames, UINT cNames, LCID lcid, DISPID *rgDispId);
		//STDMETHOD(Invoke)(DISPID dispidMember, const IID& riid, LCID lcid, WORD wFlags, DISPPARAMS *pDispParams, VARIANT *pVarResult, EXCEPINFO *pExcepInfo, UINT *puArgErr);
		//
		// IHidManager methods
		//
		STDMETHOD(FindHidDevice)(unsigned short uVendorId, unsigned short uProductId, VARIANT_BOOL *pbResult);
		STDMETHOD(GetButtonStatus)(int nButtonNumber, int *pButtonStatus);
		STDMETHOD(SetButtonStatus)(int nButtonNumber, int nButtonStatus);
		STDMETHOD(GetLedStatus)(int nLedNumber, int *pLedStatus);
		STDMETHOD(SetLedStatus)(int nLedNumber, int nLedStatus);
		STDMETHOD(SetMultiLedStatus)(int nMultiLedMask);
		STDMETHOD(SimulateLoopback)(unsigned char *pBuffer);
		STDMETHOD(SimulateHidKeycode)(unsigned short uHidPage, unsigned short uHidUsage, unsigned short uHidButtonUsage, unsigned short uKeyModifiers);
		//
		// IHidEvent methods
		//
		__event __interface IHidEvent;
		STDMETHOD(FireOnButtonPressed)(IButtonParams *piButtonParams);
};

class DECLSPEC_UUID("D0CE718F-574B-41a3-B157-4496FBC40738") CButtonParams;
class DECLSPEC_UUID("E0425E75-6644-45dd-87FB-503240B2136A") CHidManager;

const IID LIBID_HidManager = {0x17a600b7, 0x4dd1, 0x4d8d, { 0xba, 0xf8, 0xe4, 0xff, 0xc5, 0x92, 0xb9, 0x99 } };
const IID IID_IButtonParams = {0xc8b15683, 0xfe3e, 0x4a49, { 0x86, 0x4b, 0xed, 0x9d, 0xaa, 0xa2, 0x89, 0x66 } };
const CLSID CLSID_CButtonParams = {0xd0ce718f, 0x574b, 0x41a3, { 0xb1, 0x57, 0x44, 0x96, 0xfb, 0xc4, 0x7, 0x38 } };
const IID DIID_IHidEvent = {0x18976042, 0x890, 0x4dd6, { 0xac, 0x79, 0xc0, 0x42, 0xab, 0x95, 0x89, 0x4c } };
const IID IID_IHidManager = {0xc1607ef3, 0xca5e, 0x44bb, { 0xa0, 0xbc, 0x18, 0x22, 0x46, 0x70, 0xdb, 0x31 } };
const CLSID CLSID_CHidManager = {0xe0425e75, 0x6644, 0x45dd, { 0x87, 0xfb, 0x50, 0x32, 0x40, 0xb2, 0x13, 0x6a } };

