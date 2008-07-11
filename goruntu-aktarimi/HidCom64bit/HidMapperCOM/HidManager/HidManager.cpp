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
// $Archive: /Lorado/HidMapperCOM/HidManager/HidManager.cpp $ 
//  
// $Author: Nmnguye1 $
//         
// $Date: 2/27/08 12:19p $
//                                                                            
////////////////////////////////////////////////////////////////////////////////
//
// HidManager.cpp : Implementation of the IHidManager interface and CHidManager class.
//

#include "stdafx.h"
#include "HidManager.h"

#define HID_REPORT_ID_LED			1	// single-LED output report [0c, 01]
#define HID_REPORT_ID_MCE_BUTTON	1	// MCE button input report [0c, 01]
#define HID_REPORT_ID_MULTILED		2	// multi-LED output report
#define HID_REPORT_ID_CE_BUTTON		2	// CE programmable button input [0c, 03]
#define HID_REPORT_ID_KEYBOARD		3	// desktop keyboard [01, 06]
#define HID_REPORT_ID_TABLET_BUTTON	4	// tablet PC buttons [01, 09]
#define HID_REPORT_ID_GPIO			5	// virtual button feature report [0c, 03]
#define HID_REPORT_ID_STETHOSCOPE	6	// stethoscope button input [40, 02]

// =================================================================================
// ButtonParams
// =================================================================================

// ---------------------------------------------------------------------------------
// Constructor
// ---------------------------------------------------------------------------------

CButtonParams::CButtonParams() :
	m_nRef(0)
{
	m_uHidPage = 0;
	m_uHidUsage = 0;
	m_uButtonUsage = 0;
	m_bstrButtonType = NULL;
}

// ---------------------------------------------------------------------------------
// Destructor
// ---------------------------------------------------------------------------------

CButtonParams::~CButtonParams()
{
	if (NULL != m_bstrButtonType)
	{
		::SysFreeString(m_bstrButtonType);
	}
}

// ---------------------------------------------------------------------------------
// IButtonParams methods
// ---------------------------------------------------------------------------------

HRESULT CButtonParams::Initialize(unsigned short uHidPage, unsigned short uHidUsage, unsigned short uButtonUsage, WCHAR *wszButtonType)
{
	m_uHidPage = uHidPage;
	m_uHidUsage = uHidUsage;
	m_uButtonUsage = uButtonUsage;
	m_bstrButtonType = ::SysAllocString(wszButtonType);
	return S_OK;
}

//
// put_HIDPage
//
HRESULT CButtonParams::put_HIDPage(unsigned short uHidPage)
{
	m_uHidPage = uHidPage;
	return S_OK;
}

//
// get_HIDPage
//
HRESULT CButtonParams::get_HIDPage(unsigned short *puHidPage)
{
	if (NULL == puHidPage)
	{
		return E_POINTER;
	}
	*puHidPage = m_uHidPage;
	return S_OK;
}

//
// put_HIDUsage
//
HRESULT CButtonParams::put_HIDUsage(unsigned short uHidUsage)
{
	m_uHidUsage = uHidUsage;
	return S_OK;
}

//
// get_HIDUsage
//
HRESULT CButtonParams::get_HIDUsage(unsigned short *puHidUsage)
{
	if (NULL == puHidUsage)
	{
		return E_POINTER;
	}
	*puHidUsage = m_uHidUsage;
	return S_OK;
}

//
// put_ButtonUsage
//
HRESULT CButtonParams::put_ButtonUsage(unsigned short uButtonUsage)
{
	m_uButtonUsage = uButtonUsage;
	return S_OK;
}

//
// get_ButtonUsage
//
HRESULT CButtonParams::get_ButtonUsage(unsigned short *puButtonUsage)
{
	if (NULL == puButtonUsage)
	{
		return E_POINTER;
	}
	*puButtonUsage = m_uButtonUsage;
	return S_OK;
}

//
// put_ButtonType
//
HRESULT CButtonParams::put_ButtonType(BSTR bstrButtonType)
{
	m_bstrButtonType = ::SysAllocString(bstrButtonType);
	return S_OK;
}

//
// get_ButtonType
//
HRESULT CButtonParams::get_ButtonType(BSTR *pbstrButtonType)
{
	if (NULL == pbstrButtonType)
	{
		return E_POINTER;
	}
	*pbstrButtonType = ::SysAllocString(m_bstrButtonType);
	return S_OK;
}

// =================================================================================
// HidManager
// =================================================================================

// ---------------------------------------------------------------------------------
// Constructor
// ---------------------------------------------------------------------------------

CHidManager::CHidManager() :
	m_nRef(0)
{
	m_bDeviceDetected = false;
	m_uDeviceVendorId = VID_INTEL;
	m_uDeviceProductId = PID_ACPIHID;
	m_dlgProc = (WNDPROC) NULL;
	//
	// Enable events
	//
	VARIANT_BOOL bSuccess;
	HRESULT hr = RegisterForEvents(&bSuccess);
}

// ---------------------------------------------------------------------------------
// Destructor
// ---------------------------------------------------------------------------------

CHidManager::~CHidManager()
{
	HRESULT hr = UnregisterForEvents();
}

// ---------------------------------------------------------------------------------
// IHidManager methods
// ---------------------------------------------------------------------------------

//
// FindHidDevice
//
HRESULT CHidManager::FindHidDevice(unsigned short uVendorId, unsigned short uProductId, VARIANT_BOOL *pbResult)
{
	m_uDeviceVendorId = uVendorId;
	m_uDeviceProductId = uProductId;
	m_bDeviceDetected = FindAndFillDeviceCapabilities(uVendorId, uProductId);
	if (NULL != pbResult)
	{
		*pbResult = (VARIANT_BOOL) m_bDeviceDetected;
		return S_OK;
	}
	return E_INVALIDARG;
}

//
// GetButtonStatus
//
HRESULT CHidManager::GetButtonStatus(int nButtonNumber, int *pButtonStatus)
{
	if (false == m_bDeviceDetected)
	{
		return E_HANDLE;
	}
	unsigned char *pFeatureReportBuffer = (unsigned char *) ::CoTaskMemAlloc(m_hidcapsFeature.FeatureReportByteLength);
	if (NULL == pFeatureReportBuffer)
	{
		return E_OUTOFMEMORY;
	}
	pFeatureReportBuffer[0] = HID_REPORT_ID_GPIO;
	pFeatureReportBuffer[1] = (nButtonNumber & 0xff);
	BOOLEAN bSuccess = ::HidD_GetFeature(m_hHidFileFeature, (PVOID) pFeatureReportBuffer, m_hidcapsFeature.FeatureReportByteLength);
	if (bSuccess)
	{
		*pButtonStatus = (int) pFeatureReportBuffer[m_hidcapsFeature.FeatureReportByteLength - 1];
		::CoTaskMemFree(pFeatureReportBuffer);
		return S_OK;
	}
	else
	{
		::CoTaskMemFree(pFeatureReportBuffer);
		return ::GetLastError();
	}
}

//
// SetButtonStatus
//
HRESULT CHidManager::SetButtonStatus(int nButtonNumber, int nButtonStatus)
{
	if (false == m_bDeviceDetected)
	{
		return E_HANDLE;
	}
	unsigned char *pFeatureReportBuffer = (unsigned char *) ::CoTaskMemAlloc(m_hidcapsFeature.FeatureReportByteLength);
	if (NULL == pFeatureReportBuffer)
	{
		return E_OUTOFMEMORY;
	}
	pFeatureReportBuffer[0] = HID_REPORT_ID_GPIO;
	pFeatureReportBuffer[1] = ((nButtonStatus & 0x01) << 7) | (nButtonNumber & 0x7f);  // Silicon Labs format
	BOOLEAN bSuccess = ::HidD_SetFeature(m_hHidFileFeature, (PVOID) pFeatureReportBuffer, m_hidcapsFeature.FeatureReportByteLength);
	::CoTaskMemFree(pFeatureReportBuffer);
	if (bSuccess)
	{
		return S_OK;
	}
	else
	{
		return ::GetLastError();
	}
}

//
// GetLedStatus
//
HRESULT CHidManager::GetLedStatus(int nLedNumber, int *pLedStatus)
{
	if (false == m_bDeviceDetected)
	{
		return E_HANDLE;
	}
	unsigned char *pInputReportBuffer = (unsigned char *) ::CoTaskMemAlloc(m_hidcapsInputOutput.InputReportByteLength);
	if (NULL == pInputReportBuffer)
	{
		return E_OUTOFMEMORY;
	}
	pInputReportBuffer[0] = HID_REPORT_ID_LED;
	pInputReportBuffer[1] = (nLedNumber & 0xff);
	BOOLEAN bSuccess = ::HidD_GetInputReport(m_hHidFileInputOutput, (PVOID) pInputReportBuffer, m_hidcapsInputOutput.InputReportByteLength);
	if (bSuccess)
	{
		*pLedStatus = (int) pInputReportBuffer[m_hidcapsInputOutput.InputReportByteLength - 1];
		::CoTaskMemFree(pInputReportBuffer);
		return S_OK;
	}
	else
	{
		::CoTaskMemFree(pInputReportBuffer);
		return ::GetLastError();
	}
}

//
// SetLedStatus
//
HRESULT CHidManager::SetLedStatus(int nLedNumber, int nLedStatus)
{
	DWORD dwBytesWritten = 0;
	if (false == m_bDeviceDetected)
	{
		return E_HANDLE;
	}
	unsigned char *pOutputReportBuffer = (unsigned char *) ::CoTaskMemAlloc(m_hidcapsInputOutput.OutputReportByteLength);
	if (NULL == pOutputReportBuffer)
	{
		return E_OUTOFMEMORY;
	}
	pOutputReportBuffer[0] = HID_REPORT_ID_LED;
	pOutputReportBuffer[1] = ((nLedStatus & 0x01) << 7) | (nLedNumber & 0x7f);  // Silicon Labs format
	BOOLEAN bSuccess = ::WriteFile(m_hHidFileInputOutput, (LPCVOID) pOutputReportBuffer, m_hidcapsInputOutput.OutputReportByteLength, &dwBytesWritten, NULL);
	::CoTaskMemFree(pOutputReportBuffer);
	if (bSuccess)
	{
		return S_OK;
	}
	else
	{
		return ::GetLastError();
	}
}

//
// SetMultiLedStatus
//
HRESULT CHidManager::SetMultiLedStatus(int nMultiLedMask)
{
	DWORD dwBytesWritten = 0;
	if (false == m_bDeviceDetected)
	{
		return E_HANDLE;
	}
	unsigned char *pOutputReportBuffer = (unsigned char *) ::CoTaskMemAlloc(m_hidcapsInputOutput.OutputReportByteLength);
	if (NULL == pOutputReportBuffer)
	{
		return E_OUTOFMEMORY;
	}
	pOutputReportBuffer[0] = HID_REPORT_ID_MULTILED;
	pOutputReportBuffer[1] = (unsigned char) (nMultiLedMask & 0xff);
	pOutputReportBuffer[2] = (unsigned char) ((nMultiLedMask >> 8) & 0xff);
	pOutputReportBuffer[3] = (unsigned char) ((nMultiLedMask >> 16) & 0xff);
	BOOLEAN bSuccess = ::WriteFile(m_hHidFileInputOutput, (LPCVOID) pOutputReportBuffer, m_hidcapsInputOutput.OutputReportByteLength, &dwBytesWritten, NULL);
	::CoTaskMemFree(pOutputReportBuffer);
	if (bSuccess)
	{
		return S_OK;
	}
	else
	{
		return ::GetLastError();
	}
}

//
// SimulateLoopback
//
HRESULT CHidManager::SimulateLoopback(unsigned char *pBuffer)
{
	if (false == m_bDeviceDetected)
	{
		return E_HANDLE;
	}
	BOOLEAN bSuccess = ::HidD_SetOutputReport(m_hHidFileLoopback, (PVOID) pBuffer, m_hidcapsLoopback.OutputReportByteLength);
	if (bSuccess)
	{
		return S_OK;
	}
	else
	{
		return ::GetLastError();
	}
}

//
// SimulateHidKeycode
//
HRESULT CHidManager::SimulateHidKeycode(unsigned short uHidPage, unsigned short uHidUsage, unsigned short uHidButtonUsage, unsigned short uKeyModifiers)
{
	if (false == m_bDeviceDetected)
	{
		return E_HANDLE;
	}
	if ((uHidPage == HID_USAGE_PAGE_GENERIC) && (uHidUsage == HID_USAGE_GENERIC_KEYBOARD)) // desktop keyboard
	{
		struct loopbackBuffer {
			unsigned char reportId;
			unsigned char hidKeyModifiers;
			unsigned char hidReserved;
			unsigned char hidKeystroke1;
			unsigned char hidKeystroke2;
			unsigned char hidKeystroke3;
			unsigned char hidKeystroke4;
			unsigned char hidKeystroke5;
			unsigned char hidKeystroke6;
		} *pBuffer;
		pBuffer = (struct loopbackBuffer *) ::CoTaskMemAlloc(sizeof(struct loopbackBuffer));
		if (NULL == pBuffer)
		{
			return E_OUTOFMEMORY;
		}
		pBuffer->reportId = (unsigned char) HID_REPORT_ID_KEYBOARD;
		pBuffer->hidKeyModifiers = (unsigned char) uKeyModifiers & 0xff;
		pBuffer->hidReserved = (unsigned char) 0;
		pBuffer->hidKeystroke1 = (unsigned char) uHidButtonUsage & 0xff;
		pBuffer->hidKeystroke2 = (unsigned char) 0;
		pBuffer->hidKeystroke3 = (unsigned char) 0;
		pBuffer->hidKeystroke4 = (unsigned char) 0;
		pBuffer->hidKeystroke5 = (unsigned char) 0;
		pBuffer->hidKeystroke6 = (unsigned char) 0;
		int nLoopbackResult = SimulateLoopback((unsigned char *) pBuffer);
		::CoTaskMemFree((LPVOID)pBuffer);
		return nLoopbackResult;
	}
	else if ((uHidPage == HID_USAGE_PAGE_GENERIC) && (uHidUsage == 0x09)) // Tablet PC button
	{
		struct loopbackBuffer {
			unsigned char reportId;
			unsigned char hidButtonMask8to1;
			unsigned char hidButtonMask16to9;
			unsigned char hidButtonMask24to17;
			unsigned char hidButtonMask32to25;
		} *pBuffer;
		pBuffer = (struct loopbackBuffer *) ::CoTaskMemAlloc(sizeof(struct loopbackBuffer));
		if (NULL == pBuffer)
		{
			return E_OUTOFMEMORY;
		}
		pBuffer->reportId = (unsigned char) HID_REPORT_ID_TABLET_BUTTON;
		pBuffer->hidButtonMask8to1 = (unsigned char) 0;
		pBuffer->hidButtonMask16to9 = (unsigned char) 0;
		pBuffer->hidButtonMask24to17 = (unsigned char) 0;
		pBuffer->hidButtonMask32to25 = (unsigned char) 0;
		if (uHidButtonUsage > 0)
		{
			unsigned long hidButtonMask = (unsigned long) (1 << (uHidButtonUsage - 1)) & 0xffff;
			pBuffer->hidButtonMask8to1 |= (unsigned char) hidButtonMask & 0xff;
			pBuffer->hidButtonMask16to9 |= (unsigned char) (hidButtonMask >> 8) & 0xff;
			pBuffer->hidButtonMask24to17 |= (unsigned char) (hidButtonMask >> 16) & 0xff;
			pBuffer->hidButtonMask32to25 |= (unsigned char) (hidButtonMask >> 24) & 0xff;
		}
		// for Tablet PC buttons, consider the uKeyModifiers to be a "chorded" button
		if (uKeyModifiers > 0)
		{
			unsigned long hidButtonMask = (unsigned long) (1 << (uKeyModifiers - 1)) & 0xffff;
			pBuffer->hidButtonMask8to1 |= (unsigned char) hidButtonMask & 0xff;
			pBuffer->hidButtonMask16to9 |= (unsigned char) (hidButtonMask >> 8) & 0xff;
			pBuffer->hidButtonMask24to17 |= (unsigned char) (hidButtonMask >> 16) & 0xff;
			pBuffer->hidButtonMask32to25 |= (unsigned char) (hidButtonMask >> 24) & 0xff;
		}
		int nLoopbackResult = SimulateLoopback((unsigned char *) pBuffer);
		::CoTaskMemFree((LPVOID)pBuffer);
		return nLoopbackResult;
	}
	else if ((uHidPage == HID_USAGE_PAGE_CONSUMER) && (uHidUsage == 0x01)) // MCE button
	{
		struct loopbackBuffer {
			unsigned char reportId;
			unsigned char hidButtonUsageLsb;
			unsigned char hidButtonUsageMsb;
		} *pBuffer;
		pBuffer = (struct loopbackBuffer *) ::CoTaskMemAlloc(sizeof(struct loopbackBuffer));
		if (NULL == pBuffer)
		{
			return E_OUTOFMEMORY;
		}
		pBuffer->reportId = (unsigned char) HID_REPORT_ID_MCE_BUTTON;
		pBuffer->hidButtonUsageLsb = (unsigned char) uHidButtonUsage & 0xff;
		pBuffer->hidButtonUsageMsb = (unsigned char) (uHidButtonUsage >> 8) & 0xff;
		int nLoopbackResult = SimulateLoopback((unsigned char *) pBuffer);
		::CoTaskMemFree((LPVOID)pBuffer);
		return nLoopbackResult;
	}
	else if ((uHidPage == HID_USAGE_PAGE_CONSUMER) && (uHidUsage == 0x03)) // CE programmable button
	{
		struct loopbackBuffer {
			unsigned char reportId;
			unsigned char hidButtonUsageLsb;
			unsigned char hidButtonUsageMsb;
		} *pBuffer;
		pBuffer = (struct loopbackBuffer *) ::CoTaskMemAlloc(sizeof(struct loopbackBuffer));
		if (NULL == pBuffer)
		{
			return E_OUTOFMEMORY;
		}
		pBuffer->reportId = (unsigned char) HID_REPORT_ID_CE_BUTTON;
		pBuffer->hidButtonUsageLsb = (unsigned char) uHidButtonUsage & 0xff;
		pBuffer->hidButtonUsageMsb = (unsigned char) (uHidButtonUsage >> 8) & 0xff;
		int nLoopbackResult = SimulateLoopback((unsigned char *) pBuffer);
		::CoTaskMemFree((LPVOID)pBuffer);
		return nLoopbackResult;
	}
	else if ((uHidPage == 0x40) && (uHidUsage == 0x02)) // stethoscope button
	{
		struct loopbackBuffer {
			unsigned char reportId;
			unsigned char hidButtonUsageLsb;
			unsigned char hidButtonUsageMsb;
		} *pBuffer;
		pBuffer = (struct loopbackBuffer *) ::CoTaskMemAlloc(sizeof(struct loopbackBuffer));
		if (NULL == pBuffer)
		{
			return E_OUTOFMEMORY;
		}
		pBuffer->reportId = (unsigned char) HID_REPORT_ID_STETHOSCOPE;
		pBuffer->hidButtonUsageLsb = (unsigned char) uHidButtonUsage & 0xff;
		pBuffer->hidButtonUsageMsb = (unsigned char) (uHidButtonUsage >> 8) & 0xff;
		int nLoopbackResult = SimulateLoopback((unsigned char *) pBuffer);
		::CoTaskMemFree((LPVOID)pBuffer);
		return nLoopbackResult;
	}
	return E_NOTIMPL;
}

// ---------------------------------------------------------------------------------
// Private methods
// ---------------------------------------------------------------------------------

//
// FireOnMotion
//
HRESULT CHidManager::FireOnButtonPressed(IButtonParams *piButtonParams)
{
	// 1. Get ConnectionPointContainer for the current HidManager object
	IConnectionPointContainer *pICPC = NULL;
	this->QueryInterface(IID_IConnectionPointContainer, (void **)&pICPC);
	if (NULL == pICPC)
	{
		return E_FAIL;
	}
	// 2. Get ConnectionPoint for IHidEvent interface
	IConnectionPoint *pCP = NULL;
	pICPC->FindConnectionPoint(DIID_IHidEvent, &pCP);
	if (NULL == pCP)
	{
		// Ack!  No IHidEvent connection point?!?
		return E_FAIL;
	}
	// 3. Cycle through all the Advised connections and fire each one
	IEnumConnections *pEnum = NULL;
	HRESULT hr = pCP->EnumConnections(&pEnum);
	if (FAILED(hr) || (NULL == pEnum))
	{
		return E_FAIL;
	}
	ULONG nConnectionIndex = 0;
	CONNECTDATA connectData;
	ULONG cFetched = 0;
	hr = pEnum->Reset();
	for (nConnectionIndex = 0; nConnectionIndex < 10; nConnectionIndex++)
	{
		hr = pEnum->Next(1, &connectData, &cFetched);
		if (FAILED(hr) || (1 != cFetched))
		{
			// why did it fail?
		}
		else
		{
			IUnknown *pSink = connectData.pUnk;
			if (NULL != pSink)
			{
				IDispatch *piEvents;
				hr = pSink->QueryInterface(IID_IDispatch, (void **)&piEvents);
				if (!FAILED(hr) && (NULL != piEvents))
				{
					// FIRE THE EVENT!  Woo-hoo!  -- using IDispatch::Invoke()
					VARIANTARG varg;
					varg.vt = VT_DISPATCH;
					varg.pdispVal = (IDispatch *) piButtonParams;
					DISPPARAMS params;
					params.cArgs = 1;
					params.cNamedArgs = 0;
					params.rgdispidNamedArgs = NULL;
					params.rgvarg = &varg;
					piEvents->Invoke(1,  // this DISPID comes from the IDL for ButtonPressed event
						IID_NULL,
						::GetUserDefaultLCID(),
						DISPATCH_METHOD,
						&params,
						NULL,
						NULL,
						NULL);
					piEvents->Release();
				}
			}
		}
	}
	pEnum->Release();
	pCP->Release();
	pICPC->Release();
	return S_OK;
}

//
// HidEquivalentOfVkey
//
unsigned short HidEquivalentOfVkey(unsigned short vKey)
{
	unsigned short HidKey;
	switch (vKey)
	{
		case 0x08:	HidKey = 0x2a;	break;	// Backspace
		case 0x09:	HidKey = 0x2b;	break;	// Tab
		case 0x0c:	HidKey = 0x9c;	break;	// Clear
		case 0x0d:	HidKey = 0x28;	break;	// Return
		case 0x10:	HidKey = 0xe1;	break;	// Shift
		case 0x11:	HidKey = 0xe0;	break;	// Ctrl
		case 0x12:	HidKey = 0xe2;	break;	// Alt
		case 0x13:	HidKey = 0x48;	break;	// Pause
		case 0x14:	HidKey = 0x39;	break;	// Caps Lock
		case 0x1b:	HidKey = 0x29;	break;	// Escape
		case 0x20:	HidKey = 0x2c;	break;	// Space
		case 0x21:	HidKey = 0x4b;	break;	// Page Up
		case 0x22:	HidKey = 0x4e;	break;	// Page Down
		case 0x23:	HidKey = 0x4d;	break;	// End
		case 0x24:	HidKey = 0x4a;	break;	// Home
		case 0x25:	HidKey = 0x50;	break;	// Left arrow
		case 0x26:	HidKey = 0x52;	break;	// Up arrow
		case 0x27:	HidKey = 0x4f;	break;	// Right arrow
		case 0x28:	HidKey = 0x51;	break;	// Down arrow
		case 0x29:	HidKey = 0x77;	break;	// Select
		case 0x2a:	HidKey = 0x46;	break;	// Print
		case 0x2b:	HidKey = 0x74;	break;	// Execute
		case 0x2c:	HidKey = 0x46;	break;	// Print Screen
		case 0x2d:	HidKey = 0x49;	break;	// Insert
		case 0x2e:	HidKey = 0x4c;	break;	// Delete
		case 0x2f:	HidKey = 0x75;	break;	// Help
		case 0x30:	HidKey = 0x27;	break;	// 0
		case 0x31:	HidKey = 0x1e;	break;	// 1
		case 0x32:	HidKey = 0x1f;	break;	// 2
		case 0x33:	HidKey = 0x20;	break;	// 3
		case 0x34:	HidKey = 0x21;	break;	// 4
		case 0x35:	HidKey = 0x22;	break;	// 5
		case 0x36:	HidKey = 0x23;	break;	// 6
		case 0x37:	HidKey = 0x24;	break;	// 7
		case 0x38:	HidKey = 0x25;	break;	// 8
		case 0x39:	HidKey = 0x26;	break;	// 9
		case 0x41:	HidKey = 0x04;	break;	// A
		case 0x42:	HidKey = 0x05;	break;	// B
		case 0x43:	HidKey = 0x06;	break;	// C
		case 0x44:	HidKey = 0x07;	break;	// D
		case 0x45:	HidKey = 0x08;	break;	// E
		case 0x46:	HidKey = 0x09;	break;	// F
		case 0x47:	HidKey = 0x0a;	break;	// G
		case 0x48:	HidKey = 0x0b;	break;	// H
		case 0x49:	HidKey = 0x0c;	break;	// I
		case 0x4a:	HidKey = 0x0d;	break;	// J
		case 0x4b:	HidKey = 0x0e;	break;	// K
		case 0x4c:	HidKey = 0x0f;	break;	// L
		case 0x4d:	HidKey = 0x10;	break;	// M
		case 0x4e:	HidKey = 0x11;	break;	// N
		case 0x4f:	HidKey = 0x12;	break;	// O
		case 0x50:	HidKey = 0x13;	break;	// P
		case 0x51:	HidKey = 0x14;	break;	// Q
		case 0x52:	HidKey = 0x15;	break;	// R
		case 0x53:	HidKey = 0x16;	break;	// S
		case 0x54:	HidKey = 0x17;	break;	// T
		case 0x55:	HidKey = 0x18;	break;	// U
		case 0x56:	HidKey = 0x19;	break;	// V
		case 0x57:	HidKey = 0x1a;	break;	// W
		case 0x58:	HidKey = 0x1b;	break;	// X
		case 0x59:	HidKey = 0x1c;	break;	// Y
		case 0x5a:	HidKey = 0x1d;	break;	// Z
		case 0x5b:	HidKey = 0xe3;	break;	// Left Windows key
		case 0x5c:	HidKey = 0xe7;	break;	// Right Windows key
		case 0x5d:	HidKey = 0x65;	break;	// Applications/Menu key (like a right-click)
		case 0x5f:	HidKey = 0x66;	break;	// Sleep
		case 0x60:	HidKey = 0x62;	break;	// numeric pad 0
		case 0x61:	HidKey = 0x59;	break;	// numeric pad 1
		case 0x62:	HidKey = 0x5a;	break;	// numeric pad 2
		case 0x63:	HidKey = 0x5b;	break;	// numeric pad 3
		case 0x64:	HidKey = 0x5c;	break;	// numeric pad 4
		case 0x65:	HidKey = 0x5d;	break;	// numeric pad 5
		case 0x66:	HidKey = 0x5e;	break;	// numeric pad 6
		case 0x67:	HidKey = 0x5f;	break;	// numeric pad 7
		case 0x68:	HidKey = 0x60;	break;	// numeric pad 8
		case 0x69:	HidKey = 0x61;	break;	// numeric pad 9
		case 0x6a:	HidKey = 0x55;	break;	// Multiply key
		case 0x6b:	HidKey = 0x57;	break;	// Add key
		case 0x6c:	HidKey = 0x85;	break;	// Separator key (nominally comma)
		case 0x6d:	HidKey = 0x56;	break;	// Subtract key
		case 0x6e:	HidKey = 0x63;	break;	// Decimal key (nominally dot)
		case 0x6f:	HidKey = 0x54;	break;	// Divide key
		case 0x70:	HidKey = 0x3a;	break;	// F1
		case 0x71:	HidKey = 0x3b;	break;	// F2
		case 0x72:	HidKey = 0x3c;	break;	// F3
		case 0x73:	HidKey = 0x3d;	break;	// F4
		case 0x74:	HidKey = 0x3e;	break;	// F5
		case 0x75:	HidKey = 0x3f;	break;	// F6
		case 0x76:	HidKey = 0x40;	break;	// F7
		case 0x77:	HidKey = 0x41;	break;	// F8
		case 0x78:	HidKey = 0x42;	break;	// F9
		case 0x79:	HidKey = 0x43;	break;	// F10
		case 0x7a:	HidKey = 0x44;	break;	// F11
		case 0x7b:	HidKey = 0x45;	break;	// F12
		case 0x7c:	HidKey = 0x68;	break;	// F13
		case 0x7d:	HidKey = 0x69;	break;	// F14
		case 0x7e:	HidKey = 0x6a;	break;	// F15
		case 0x7f:	HidKey = 0x6b;	break;	// F16
		case 0x80:	HidKey = 0x6c;	break;	// F17
		case 0x81:	HidKey = 0x6d;	break;	// F18
		case 0x82:	HidKey = 0x6e;	break;	// F19
		case 0x83:	HidKey = 0x6f;	break;	// F20
		case 0x84:	HidKey = 0x70;	break;	// F21
		case 0x85:	HidKey = 0x71;	break;	// F22
		case 0x86:	HidKey = 0x72;	break;	// F23
		case 0x87:	HidKey = 0x73;	break;	// F24
		case 0x90:	HidKey = 0x53;	break;	// Num Lock
		case 0x91:	HidKey = 0x47;	break;	// Scroll Lock
		case 0x92:	HidKey = 0x67;	break;	// Equals key (OEM specific, dubious)
		case 0x93:	HidKey = 0xb0;	break;	// 00 (OEM specific)
		case 0x94:	HidKey = 0xb1;	break;	// 000 (OEM specific)
		case 0x95:	HidKey = 0xb4;	break;	// Pound/Euro (OEM specific)
		case 0x96:	HidKey = 0xb5;	break;	// Cents (OEM specific)
		case 0xa0:	HidKey = 0xe1;	break;	// Left Shift
		case 0xa1:	HidKey = 0xe5;	break;	// Right Shift
		case 0xa2:	HidKey = 0xe0;	break;	// Left Control
		case 0xa3:	HidKey = 0xe4;	break;	// Right Control
		case 0xa4:	HidKey = 0xe2;	break;	// Left Menu
		case 0xa5:	HidKey = 0xe6;	break;	// Right Menu
		case 0xa6:	HidKey = 0x0224;break;	// Browser Back -- for HID, must be on Consumer Controls page
		case 0xa7:	HidKey = 0x0225;break;	// Browser Forward -- for HID, must be on Consumer Controls page
		case 0xa8:	HidKey = 0x0227;break;	// Browser Refresh -- for HID, must be on Consumer Controls page
		case 0xa9:	HidKey = 0x0226;break;	// Browser Stop -- for HID, must be on Consumer Controls page
		case 0xaa:	HidKey = 0x0221;break;	// Browser Search -- for HID, must be on Consumer Controls page
		case 0xab:	HidKey = 0x022a;break;	// Browser Favorites -- for HID, must be on Consumer Controls page
		case 0xac:	HidKey = 0x0223;break;	// Browser Home -- for HID, must be on Consumer Controls page
		case 0xad:	HidKey = 0x7f;	break;	// Volume Mute
		case 0xae:	HidKey = 0x81;	break;	// Volume Down
		case 0xaf:	HidKey = 0x80;	break;	// Volume Up
		case 0xb0:	HidKey = 0x00;	break;	// Next Track -- for HID, must be on Consumer Controls page
		case 0xb1:	HidKey = 0x00;	break;	// Previous Track -- for HID, must be on Consumer Controls page
		case 0xb2:	HidKey = 0x00;	break;	// Stop -- for HID, must be on Consumer Controls page
		case 0xb3:	HidKey = 0x00;	break;	// Play/Pause -- for HID, must be on Consumer Controls page
		case 0xb4:	HidKey = 0x018a;break;	// Mail -- for HID, must be on Consumer Controls page
		case 0xb5:	HidKey = 0x0193;break;	// Media Select -- for HID, must be on Consumer Controls page
		case 0xb6:	HidKey = 0x0192;break;	// AppLaunch 1 -- for HID, must be on Consumer Controls page
		case 0xb7:	HidKey = 0x0196;break;	// AppLaunch 2 -- for HID, must be on Consumer Controls page
		case 0xba:	HidKey = 0x33;	break;	// : and ;
		case 0xbb:	HidKey = 0x2e;	break;	// = and +
		case 0xbc:	HidKey = 0x36;	break;	// , and <
		case 0xbd:	HidKey = 0x2d;	break;	// - and _
		case 0xbe:	HidKey = 0x37;	break;	// . and >
		case 0xbf:	HidKey = 0x38;	break;	// / and ?
		case 0xc0:	HidKey = 0x35;	break;	// ` and ~
		case 0xdb:	HidKey = 0x2f;	break;	// [ and {
		case 0xdc:	HidKey = 0x31;	break;	// \ and |
		case 0xdd:	HidKey = 0x30;	break;	// ] and }
		case 0xde:	HidKey = 0x34;	break;	// ' and "
		case 0xe3:	HidKey = 0x75;	break;	// Help key (OEM specific)
		case 0xe4:	HidKey = 0xb0;	break;	// 00 key (OEM specific)
		case 0xe5:	HidKey = 0x91;	break;	// IME Process key
		case 0xf6:	HidKey = 0x9a;	break;	// Attention
		case 0xf7:	HidKey = 0xa3;	break;	// CrSel/Props
		case 0xf8:	HidKey = 0xa4;	break;	// ExSel
		case 0xf9:	HidKey = 0x99;	break;	// Erase EOF
		case 0xfa:	HidKey = 0x00;	break;	// Play -- for HID, must be on Consumer Controls page
		case 0xfb:	HidKey = 0x00;	break;	// Zoom -- for HID, must be on Consumer Controls page
		case 0xfc:	HidKey = 0xa5;	break;	// Noname
		case 0xfd:	HidKey = 0xa6;	break;	// PA1
		case 0xfe:	HidKey = 0x9c;	break;	// Clear

		default:	HidKey = 0x00;	break;
	}
	return HidKey;
}

//
// onButtonPressHandler
//
LRESULT APIENTRY onButtonPressHandler(HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam)
{
    switch(msg)
    {
	case WM_INPUT:  // process keyboard input; especially interested in RAW HID keycodes
		{
			unsigned int dwSize;
			::GetRawInputData((HRAWINPUT)lParam, RID_INPUT, NULL, &dwSize, sizeof(RAWINPUTHEADER));
			unsigned char *pBuffer = (unsigned char *) ::CoTaskMemAlloc((size_t)dwSize);
			if (NULL == pBuffer)
			{
				break;
			}
			::GetRawInputData((HRAWINPUT)lParam, RID_INPUT, pBuffer, &dwSize, sizeof(RAWINPUTHEADER));
			RAWINPUT *pRaw = (RAWINPUT *) pBuffer;
			if (RIM_TYPEKEYBOARD == pRaw->header.dwType)  // It was HID KEYBOARD
			{
				// craft a new ButtonParams object
				CComObject<CButtonParams> *pButtonParams = NULL;
				CComObject<CButtonParams>::CreateInstance(&pButtonParams);
				if (NULL == pButtonParams)
				{
					return 0;
				}
				pButtonParams->AddRef();
				// retrieve the HidManager object associated with this dialog window
				IHidManager *pHidManager = (IHidManager *) ::GetProp(hDlg, "PROP_HIDMANAGER");
				if (NULL == pHidManager)
				{
					return 0;
				}
				// do VKEY to HIDcode conversion here
				pButtonParams->Initialize(
					HID_USAGE_PAGE_GENERIC, 
					HID_USAGE_GENERIC_KEYBOARD, 
					HidEquivalentOfVkey(pRaw->data.keyboard.VKey), 
					(pRaw->data.keyboard.Flags == RI_KEY_MAKE) ? L"KEY MAKE" : L"KEY BREAK");
				// Fire the motion event
				IButtonParams *piButtonParams = NULL;
				pButtonParams->QueryInterface(IID_IDispatch, (void **) &piButtonParams);
				HRESULT hr = pHidManager->FireOnButtonPressed(piButtonParams);
			}
			else if (RIM_TYPEHID == pRaw->header.dwType)  // It was RAWHID, not keyboard or mouse
			{
				// craft a new ButtonParams object
				CComObject<CButtonParams> *pButtonParams = NULL;
				CComObject<CButtonParams>::CreateInstance(&pButtonParams);
				if (NULL == pButtonParams)
				{
					return 0;
				}
				pButtonParams->AddRef();
				// retrieve the HidManager object associated with this dialog window
				IHidManager *pHidManager = (IHidManager *) ::GetProp(hDlg, "PROP_HIDMANAGER");
				if (NULL == pHidManager)
				{
					return 0;
				}
				// stuff HID code info here
				struct myHidBuffer {
					unsigned char reportId;
					unsigned char hidButtonUsageLsb;
					unsigned char hidButtonUsageMsb;
					unsigned char hidExtra[10];
				} *pHidBuffer = (struct myHidBuffer *) &(pRaw->data.hid.bRawData);
				unsigned short usButtonUsage = ((pHidBuffer->hidButtonUsageMsb << 8) | pHidBuffer->hidButtonUsageLsb);
				RID_DEVICE_INFO deviceInfo;
				UINT cbSize;
				cbSize = sizeof(RID_DEVICE_INFO);
				HRESULT hr = ::GetRawInputDeviceInfo(pRaw->header.hDevice, RIDI_DEVICEINFO, &deviceInfo, &cbSize);
				WCHAR wszTypeString[14]; // L"HID xxxx xxxx"
				swprintf_s(wszTypeString, 14, L"HID %04x %04x", 
					deviceInfo.hid.dwVendorId,
					deviceInfo.hid.dwProductId);
				pButtonParams->Initialize(
					deviceInfo.hid.usUsagePage, 
					deviceInfo.hid.usUsage, 
					usButtonUsage, 
					wszTypeString);
				// Fire the motion event
				IButtonParams *piButtonParams = NULL;
				pButtonParams->QueryInterface(IID_IDispatch, (void **) &piButtonParams);
				hr = pHidManager->FireOnButtonPressed(piButtonParams);
			}
			::CoTaskMemFree((LPVOID)pBuffer);
		}
		break;

	default:
		break;
    }

	// Pass messages to previous WNDPROC for processing.
	WNDPROC prevDlgProc = (WNDPROC) ::GetProp(hDlg, "PROP_PREV_DLGPROC");
	if (NULL == prevDlgProc)
	{
		return ::DefWindowProc(hDlg, msg, wParam, lParam);
	}
	return ::CallWindowProc(prevDlgProc, hDlg, msg, wParam, lParam);
}

//
// myDefWndProc -- default WndProc for the HID Catcher Window Class.
//
LRESULT APIENTRY myDefWndProc(HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam)
{
	return ::DefWindowProc(hDlg, msg, wParam, lParam);
}

//
// RegisterForMotion
//
HRESULT CHidManager::RegisterForEvents(VARIANT_BOOL *pbSuccess)
{
	BOOL bPropSuccess;

	if (NULL == pbSuccess)
	{
		return E_POINTER;
	}
	*pbSuccess = VARIANT_FALSE;
	// We need a window to catch RAW HID input reports.  Register a window class and then create the window.
	TCHAR *ctstrClassName = TEXT("HidManager");
	HINSTANCE hInstance = _AtlBaseModule.GetModuleInstance();
	WNDCLASSEX wcex;
	wcex.cbSize			= sizeof(WNDCLASSEX);
	wcex.style			= CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc	= myDefWndProc;
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= 0;
	wcex.hInstance		= hInstance;
	wcex.hIcon			= NULL;
	wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground	= (HBRUSH)(COLOR_WINDOW+1);
	wcex.lpszMenuName	= NULL;
	wcex.lpszClassName	= ctstrClassName;
	wcex.hIconSm		= NULL;
	ATOM aWinClass = ::RegisterClassEx(&wcex);
	m_hwnd = ::CreateWindow(ctstrClassName,
		TEXT("HID Catcher Window"),
		WS_POPUP | WS_CAPTION | WS_SYSMENU,  // not WS_VISIBLE
		0, 0,
		200, 100,
		NULL,
		NULL,
		hInstance,
		NULL
		);
	if (NULL == m_hwnd)
	{
		return ::GetLastError();
	}
	// Add our own subclass WindowProc to this window
#ifdef WIN64
	m_dlgProc = (WNDPROC) ::SetWindowLongPtr(m_hwnd, GWLP_WNDPROC, (LONG_PTR) onButtonPressHandler);
#else
	m_dlgProc = (WNDPROC) ::SetWindowLongPtr(m_hwnd, GWL_WNDPROC, (LONG_PTR) onButtonPressHandler);
#endif

	if (NULL == m_dlgProc)
	{
		return ::GetLastError();
	}
	// Remember which HidManager object instance is associated with this window
	bPropSuccess = ::SetProp(m_hwnd, "PROP_HIDMANAGER", (HANDLE) this);
	bPropSuccess = ::SetProp(m_hwnd, "PROP_PREV_DLGPROC", (HANDLE) m_dlgProc);
	// Register our interest in receiving RAWHID reports via WM_INPUT.
	RAWINPUTDEVICE rawInputDevice[8];
	rawInputDevice[0].usUsagePage = (unsigned short) HID_USAGE_PAGE_CONSUMER;	// Consumer control
	rawInputDevice[0].usUsage = (unsigned short) 0x01;  // Consumer buttons
	rawInputDevice[0].dwFlags = RIDEV_INPUTSINK;
	rawInputDevice[0].hwndTarget = m_hwnd;
	rawInputDevice[1].usUsagePage = (unsigned short) HID_USAGE_PAGE_CONSUMER;	// Consumer control
	rawInputDevice[1].usUsage = (unsigned short) 0x03;  // Programmable buttons
	rawInputDevice[1].dwFlags = RIDEV_INPUTSINK;
	rawInputDevice[1].hwndTarget = m_hwnd;
	rawInputDevice[2].usUsagePage = (unsigned short) HID_USAGE_PAGE_GENERIC;	// Desktop
	rawInputDevice[2].usUsage = (unsigned short) HID_USAGE_GENERIC_KEYBOARD;  // Keyboard
	rawInputDevice[2].dwFlags = RIDEV_INPUTSINK;
	rawInputDevice[2].hwndTarget = m_hwnd;
	rawInputDevice[3].usUsagePage = (unsigned short) HID_USAGE_PAGE_GENERIC;	// Desktop
	rawInputDevice[3].usUsage = (unsigned short) HID_USAGE_GENERIC_KEYPAD;  // Keypad
	rawInputDevice[3].dwFlags = RIDEV_INPUTSINK;
	rawInputDevice[3].hwndTarget = m_hwnd;
	rawInputDevice[4].usUsagePage = (unsigned short) HID_USAGE_PAGE_GENERIC;	// Desktop
	rawInputDevice[4].usUsage = (unsigned short) 0x09;  // Tablet PC buttons
	rawInputDevice[4].dwFlags = RIDEV_INPUTSINK;
	rawInputDevice[4].hwndTarget = m_hwnd;
	rawInputDevice[5].usUsagePage = (unsigned short) 0x40;	// Medical devices
	rawInputDevice[5].usUsage = (unsigned short) 0x02;  // Bluetooth stethoscope
	rawInputDevice[5].dwFlags = RIDEV_INPUTSINK;
	rawInputDevice[5].hwndTarget = m_hwnd;

	rawInputDevice[6].usUsagePage = (unsigned short) 0xffbc;	// Media Center Edition remote control
	rawInputDevice[6].usUsage = (unsigned short) 0x88;  // Media category buttons
	rawInputDevice[6].dwFlags = RIDEV_INPUTSINK;
	rawInputDevice[6].hwndTarget = m_hwnd;
	rawInputDevice[7].usUsagePage = (unsigned short) 0xffbc;	// Media Center Edition remote control
	rawInputDevice[7].usUsage = (unsigned short) 0x89;  // Green jewel button
	rawInputDevice[7].dwFlags = RIDEV_INPUTSINK;
	rawInputDevice[7].hwndTarget = m_hwnd;
	BOOLEAN bSuccess = ::RegisterRawInputDevices(rawInputDevice, 8, sizeof(RAWINPUTDEVICE));
	if (!bSuccess)
	{
		// Ack!  We couldn't register for RAW HID keycode reception.
		DWORD dwError = ::GetLastError();
		::MessageBox(HWND_DESKTOP, TEXT("Cannot register for RAWHID"), TEXT("HidManager Error"), MB_OK | MB_ICONERROR);
	}
	*pbSuccess = bSuccess;
	return S_OK;
}

//
// UnregisterForMotion
//
HRESULT CHidManager::UnregisterForEvents()
{
	HANDLE hPrevProp = NULL;

	if (NULL == m_dlgProc)
	{
		return E_HANDLE;
	}
	// Remove the property we added earlier
	hPrevProp = ::RemoveProp(m_hwnd, "PROP_PREV_DLGPROC");
	hPrevProp = ::RemoveProp(m_hwnd, "PROP_HIDMANAGER");

	// Put back the original WindowProc
#ifdef WIN64
	::SetWindowLongPtr(m_hwnd, GWLP_WNDPROC, (LONG_PTR) m_dlgProc);
#else
	::SetWindowLongPtr(m_hwnd, GWL_WNDPROC, (LONG_PTR) m_dlgProc);
#endif

	// Get rid of our catcher window
	::DestroyWindow(m_hwnd);
	// Unregister the window class
	TCHAR *ctstrClassName = TEXT("HidManager");
	HINSTANCE hInstance = _AtlBaseModule.GetModuleInstance();
	::UnregisterClass(ctstrClassName, hInstance);
	return S_OK;
}

//
// FindAndFillDeviceCapabilities
//
bool CHidManager::FindAndFillDeviceCapabilities(unsigned short uVendorId, unsigned short uProductId)
{
	GUID HidGuid;
	TCHAR *pszDevicePathname[128];
	int iDevice;
	SECURITY_ATTRIBUTES Security;
	HANDLE hFile;
	HIDD_ATTRIBUTES HidAttributes;
	HIDP_CAPS HidCaps;
	BOOLEAN bSuccess;
	int nResult;
	int nCollectionsDetected = 0;

	// Values for the SECURITY_ATTRIBUTES structure:
	Security.lpSecurityDescriptor = NULL;
	Security.bInheritHandle = true;
	Security.nLength = sizeof(SECURITY_ATTRIBUTES);
	
	// HID DevInterface is {4D1E55B2-F16F-11CF-88CB-001111000030} - defined in the DDK
	::HidD_GetHidGuid(&HidGuid);  // should return {4D1E55B2-F16F-11CF-88CB-001111000030} 

	// Fill an array with the device path names of all attached HIDs.
	int nDevicesFound = FindDevicesFromGuid(&HidGuid, pszDevicePathname);
	if (nDevicesFound > 0)
	{
		// If there is at least one HID, attempt to read the Vendor ID and Product ID
		// of each device until there is a match or all devices have been examined.
		for (iDevice = 0; iDevice < nDevicesFound; iDevice++)
		{
			hFile = ::CreateFile((LPCTSTR) pszDevicePathname[iDevice],
							GENERIC_READ | GENERIC_WRITE,
							FILE_SHARE_READ | FILE_SHARE_WRITE,
							&Security,
							OPEN_EXISTING,
							0,
							0);
			if (INVALID_HANDLE_VALUE != hFile)
			{
				// The returned handle is valid,so find out if this is the device we're looking for.
				HidAttributes.Size = sizeof(HIDD_ATTRIBUTES);
				bSuccess = ::HidD_GetAttributes(hFile, &HidAttributes);
				if (bSuccess)
				{
					if ((HidAttributes.VendorID == uVendorId) && (HidAttributes.ProductID == uProductId))
					{
						m_HidAttributes = HidAttributes;

						// looks like the device we want, grab its capabilities.
						PHIDP_PREPARSED_DATA phidpPreparsedData;
						bSuccess = ::HidD_GetPreparsedData(hFile, &phidpPreparsedData);
						nResult = ::HidP_GetCaps(phidpPreparsedData, &HidCaps);
						bSuccess = ::HidD_FreePreparsedData(phidpPreparsedData);

						// check to see which collection (page/usage) it is
						if ((HidCaps.UsagePage == 0x0c) && (HidCaps.Usage == 0x01))  // Used for LED input and output reports
						{
							m_hHidFileInputOutput = hFile;
							m_hidcapsInputOutput = HidCaps;
							nCollectionsDetected++;
						}
						else if ((HidCaps.UsagePage == 0x0c) && (HidCaps.Usage == 0x03))  // Used for button feature reports
						{
							m_hHidFileFeature = hFile;
							m_hidcapsFeature = HidCaps;
							nCollectionsDetected++;
						}
						else if ((HidCaps.UsagePage == 0x40) && (HidCaps.Usage == 0x02))  // Used for loopbacks
						{
							m_hHidFileLoopback = hFile;
							m_hidcapsLoopback = HidCaps;
							nCollectionsDetected++;
						}
						else
						{
							::CloseHandle(hFile);
						}
					}
					else
					{
						::CloseHandle(hFile);
					}
				}
				else
				{
					::CloseHandle(hFile);
				}
			} // endif
		} // endfor
	} // endif
	return (nCollectionsDetected > 0);
}

//
// FindDevicesFromGuid
//
int CHidManager::FindDevicesFromGuid(GUID* pHidGuid, TCHAR *pszDevicePathname[])
{
	bool bLastDevice = false;
	SP_DEVICE_INTERFACE_DATA devInterfaceData;
	DWORD dwRequireSize, dwBufferSize = 0;
	PSP_DEVICE_INTERFACE_DETAIL_DATA pDevInterfaceDetailData = NULL;
	int nDevicesFound = 0;
	HRESULT hRes = S_OK;

	// Get the 
	HDEVINFO hDevInfoSet = ::SetupDiGetClassDevs(pHidGuid,
		NULL,
		0,
		DIGCF_PRESENT | DIGCF_DEVICEINTERFACE);
	// stop here and return 0
	if (!hDevInfoSet) 
		return nDevicesFound;

	int i = 0;
	do {
		devInterfaceData.cbSize = sizeof(SP_DEVICE_INTERFACE_DATA);
		BOOLEAN bSuccess = ::SetupDiEnumDeviceInterfaces(hDevInfoSet,
			0,
			pHidGuid,
			i,
			&devInterfaceData);

		if (bSuccess)
		{
			// 1st call is to find the buffer size <dwBufferSize>
			bSuccess = ::SetupDiGetDeviceInterfaceDetail(hDevInfoSet,
				&devInterfaceData,
				NULL,
				0,
				&dwBufferSize,
				NULL);

			// Allocate buffer enough for the fix part and variable path
			pDevInterfaceDetailData = (PSP_DEVICE_INTERFACE_DETAIL_DATA) 
					::CoTaskMemAlloc (sizeof(SP_DEVICE_INTERFACE_DETAIL_DATA)+ dwBufferSize);
			if (!pDevInterfaceDetailData) 
				break;  // Can't continue
			
			// Must be set to sizeof(SP_DEVICE_INTERFACE_DETAIL_DATA)
			pDevInterfaceDetailData->cbSize = sizeof(SP_DEVICE_INTERFACE_DETAIL_DATA);
			pDevInterfaceDetailData->DevicePath[0] = (TCHAR) 0;

			// Second call to get the device interface details
			bSuccess = ::SetupDiGetDeviceInterfaceDetail(hDevInfoSet,
				&devInterfaceData,
				pDevInterfaceDetailData,
				dwBufferSize,
				&dwRequireSize,
				NULL);

			if (!bSuccess) hRes = GetLastError();

			// set the device path pointer
			pszDevicePathname[i] = pDevInterfaceDetailData->DevicePath;
			nDevicesFound++;
			// reset for next one
			pDevInterfaceDetailData = NULL; 
		}
		else
		{
			bLastDevice = true;
		}
		i++;
	} while (!bLastDevice);

	// cleanup
	::SetupDiDestroyDeviceInfoList(hDevInfoSet);
	
	return nDevicesFound;
}
