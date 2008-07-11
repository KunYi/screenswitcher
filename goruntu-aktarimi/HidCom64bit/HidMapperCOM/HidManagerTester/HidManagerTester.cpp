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
// $Archive: /Lorado/HidMapperCOM/HidManagerTester/HidManagerTester.cpp $ 
//  
// $Author: Nmnguye1 $
//         
// $Date: 2/27/08 12:23p $
//                                                                            
////////////////////////////////////////////////////////////////////////////////

// HidManagerTester.cpp : Defines the entry point for the application.
//

#include "stdafx.h"
#include "HidManagerTester.h"

// Notes: Use #import "HidManager.tlb" to get the Auto COM support
#include "..\HidManager\_HidManager.h"
#include <time.h>

#define MAX_LOADSTRING	100
#define HUGE_STRING		32768

// Global Variables:
HINSTANCE hInst;								// current instance
TCHAR szTitle[MAX_LOADSTRING];					// The title bar text
TCHAR szWindowClass[MAX_LOADSTRING];			// the main window class name
HWND g_hWnd;
HWND g_hDlg;

// Forward declarations of functions included in this code module:
ATOM				MyRegisterClass(HINSTANCE hInstance);
BOOL				InitInstance(HINSTANCE, int);
LRESULT CALLBACK	WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK	About(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK	DlgProc(HWND, UINT, WPARAM, LPARAM);

IHidManager *g_pHidManager;

class CHidManagerEventSink : public IHidEvent
{
private:
	long m_nRef;
public:
	CHidManagerEventSink() :
		m_nRef(0)
	{
	}
	virtual ~CHidManagerEventSink()
	{
	}
	// IUnknown
	HRESULT __stdcall QueryInterface(REFIID riid, void** ppv)
	{
		if (IID_IUnknown == riid)
		{
			*ppv = static_cast<IUnknown *>(this);
		}
		else if (IID_IDispatch == riid)
		{
			*ppv = static_cast<IDispatch *>(this);
		}
		else if (DIID_IHidEvent == riid)
		{
			*ppv = static_cast<IHidEvent *>(this);
		}
		else
		{
			*ppv = NULL;
			return E_NOINTERFACE;
		}
		reinterpret_cast<IUnknown *>(*ppv)->AddRef();
		return S_OK;
	}
	ULONG __stdcall AddRef()
	{
		return ::InterlockedIncrement(&m_nRef);
	}
	ULONG __stdcall Release()
	{
		if (0 == ::InterlockedDecrement(&m_nRef))
		{
			delete this;
			return 0;
		}
		return m_nRef;
	}
	// IDispatch
	HRESULT __stdcall GetTypeInfoCount(UINT *pcTInfo)
	{
		*pcTInfo = 0;  // sorry, I'm not providing any type info for them, but they won't call it.
		return S_OK;
	}
	HRESULT __stdcall GetTypeInfo(UINT iTInfo, LCID lcid, ITypeInfo **ppTInfo)
	{
		*ppTInfo = NULL;
		if (0 != iTInfo)
		{
			return DISP_E_BADINDEX;
		}
		// sorry, I'm not providing any type info for them, but they won't call it.
		return E_NOTIMPL;
	}
	HRESULT __stdcall GetIDsOfNames(const IID& riid, LPOLESTR *rgszNames, UINT cNames, LCID lcid, DISPID *rgDispId)
	{
		if (IID_NULL != riid)
		{
			return DISP_E_UNKNOWNINTERFACE;
		}
		if ((1 == cNames) && (0 == wcscmp(rgszNames[0], L"ButtonPressed")))
		{
			rgDispId[0] = (DISPID) 1;  // this DISPID comes from HidManager.h
			return S_OK;
		}
		return DISP_E_BADINDEX;
	}
	HRESULT __stdcall Invoke(DISPID dispidMember, const IID& riid, LCID lcid, WORD wFlags, DISPPARAMS *pDispParams, VARIANT *pVarResult, EXCEPINFO *pExcepInfo, UINT *puArgErr)
	{
		if (IID_NULL != riid)
		{
			return DISP_E_UNKNOWNINTERFACE;
		}
		if ((DISPID) 1 != dispidMember)  // this DISPID comes from HidManager.h
		{
			return DISP_E_BADINDEX;
		}
		if ((1 != pDispParams->cArgs) || (NULL == pDispParams->rgvarg))
		{
			return E_INVALIDARG;
		}
		VARIANTARG varg = pDispParams->rgvarg[0];
		if (VT_DISPATCH != varg.vt) 
		{
			return E_INVALIDARG;  // They better send us an (IDispatch *) to IButtonParams as the argument!
		}
		return ButtonPressed((IButtonParams *) varg.pdispVal); 
	}
	// IHidEvent
	HRESULT __stdcall ButtonPressed(IButtonParams *piButtonParams)
	{
		WCHAR wszString[MAX_LOADSTRING];
		unsigned short uHidPage;
		unsigned short uHidUsage;
		unsigned short uHidButtonUsage;
		BSTR bstrButtonType;
		HRESULT hr;
		WCHAR wszOldLog[HUGE_STRING];
		WCHAR wszNewLog[HUGE_STRING];
		time_t ltime;
		WCHAR wszLongTime[MAX_LOADSTRING];
		WCHAR wszShortTime[MAX_LOADSTRING];

		hr = piButtonParams->get_HIDPage(&uHidPage);
		hr = piButtonParams->get_HIDUsage(&uHidUsage);
		hr = piButtonParams->get_ButtonUsage(&uHidButtonUsage);
		hr = piButtonParams->get_ButtonType(&bstrButtonType);
		swprintf_s(wszString, MAX_LOADSTRING, TEXT("0x%04x"), uHidPage & 0xffff);
		::SetDlgItemText(g_hDlg, IDC_EDIT_HIDPAGE, wszString);
		swprintf_s(wszString, MAX_LOADSTRING, TEXT("0x%04x"), uHidUsage & 0xffff);
		::SetDlgItemText(g_hDlg, IDC_EDIT_HIDUSAGE, wszString);
		swprintf_s(wszString, MAX_LOADSTRING, TEXT("0x%04x"), uHidButtonUsage & 0xffff);
		::SetDlgItemText(g_hDlg, IDC_EDIT_BUTTONUSAGE, wszString);
		::SetDlgItemText(g_hDlg, IDC_EDIT_BUTTONTYPE, bstrButtonType);
		wcscpy_s(wszOldLog, HUGE_STRING, L"");
		::GetDlgItemText(g_hDlg, IDC_EDIT_LOG, wszOldLog, HUGE_STRING);
		time (&ltime);
		wcscpy_s(wszLongTime, MAX_LOADSTRING, _wctime(&ltime));
		wcsncpy_s(wszShortTime, MAX_LOADSTRING, &wszLongTime[11], 8);
		swprintf_s(wszNewLog, MAX_LOADSTRING, TEXT("%ls   0x%04x 0x%04x 0x%04x   %ls\r\n"), 
			wszShortTime,
			uHidPage & 0xffff,
			uHidUsage & 0xffff,
			uHidButtonUsage & 0xffff,
			bstrButtonType);
		wcscat_s(wszNewLog, HUGE_STRING, wszOldLog);
		::SetDlgItemText(g_hDlg, IDC_EDIT_LOG, wszNewLog);
		
		return S_OK;
	}
};

CHidManagerEventSink *g_pSink = NULL;

int APIENTRY _tWinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPTSTR    lpCmdLine,
                     int       nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

 	// TODO: Place code here.
	MSG msg;
	HACCEL hAccelTable;

	::CoInitializeEx(NULL, COINIT_APARTMENTTHREADED);

	// Initialize global strings
	LoadString(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	LoadString(hInstance, IDC_HIDMANAGERTESTER, szWindowClass, MAX_LOADSTRING);
	MyRegisterClass(hInstance);

	// Perform application initialization:
	if (!InitInstance (hInstance, nCmdShow))
	{
		return FALSE;
	}

	hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_HIDMANAGERTESTER));

	//
    // Create the  dialog
    //
    g_hDlg = ::CreateDialog(hInstance,
                          MAKEINTRESOURCE(IDD_DIALOG_TEST), 
                          NULL, 
                          &DlgProc);
	if (NULL != g_hDlg)
	{
        ::ShowWindow(g_hDlg, SW_SHOW);
	}
	else
	{
		HRESULT hr = ::GetLastError();
		::MessageBox(g_hWnd, TEXT("Could not create dialog box IDD_DIALOG_TEST"), TEXT("HidManagerTester Error"), MB_OK | MB_ICONEXCLAMATION);
	}

	// Main message loop:
	while (GetMessage(&msg, NULL, 0, 0))
	{
		if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

	return (int) msg.wParam;
}



//
//  FUNCTION: MyRegisterClass()
//
//  PURPOSE: Registers the window class.
//
//  COMMENTS:
//
//    This function and its usage are only necessary if you want this code
//    to be compatible with Win32 systems prior to the 'RegisterClassEx'
//    function that was added to Windows 95. It is important to call this function
//    so that the application will get 'well formed' small icons associated
//    with it.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
	WNDCLASSEX wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style			= CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc	= WndProc;
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= 0;
	wcex.hInstance		= hInstance;
	wcex.hIcon			= LoadIcon(hInstance, MAKEINTRESOURCE(IDI_HIDMANAGERTESTER));
	wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground	= (HBRUSH)(COLOR_WINDOW+1);
	wcex.lpszMenuName	= MAKEINTRESOURCE(IDC_HIDMANAGERTESTER);
	wcex.lpszClassName	= szWindowClass;
	wcex.hIconSm		= LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

	return RegisterClassEx(&wcex);
}

//
//   FUNCTION: InitInstance(HINSTANCE, int)
//
//   PURPOSE: Saves instance handle and creates main window
//
//   COMMENTS:
//
//        In this function, we save the instance handle in a global variable and
//        create and display the main program window.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{

   hInst = hInstance; // Store instance handle in our global variable

   g_hWnd = CreateWindow(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
      0, 0, 300, 100, NULL, NULL, hInstance, NULL);

   if (!g_hWnd)
   {
      return FALSE;
   }

   ShowWindow(g_hWnd, SW_SHOW);
   UpdateWindow(g_hWnd);

   return TRUE;
}

//
//  FUNCTION: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  PURPOSE:  Processes messages for the main window.
//
//  WM_COMMAND	- process the application menu
//  WM_PAINT	- Paint the main window
//  WM_DESTROY	- post a quit message and return
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	int wmId, wmEvent;
	PAINTSTRUCT ps;
	HDC hdc;

	switch (message)
	{
	case WM_COMMAND:
		wmId    = LOWORD(wParam);
		wmEvent = HIWORD(wParam);
		// Parse the menu selections:
		switch (wmId)
		{
		case IDM_ABOUT:
			DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
			break;
		case IDM_EXIT:
			DestroyWindow(hWnd);
			break;
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
		}
		break;
	case WM_PAINT:
		hdc = BeginPaint(hWnd, &ps);
		// TODO: Add any drawing code here...
		EndPaint(hWnd, &ps);
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

// Message handler for about box.
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(lParam);
	switch (message)
	{
	case WM_INITDIALOG:
		return (INT_PTR)TRUE;

	case WM_COMMAND:
		if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
		{
			EndDialog(hDlg, LOWORD(wParam));
			return (INT_PTR)TRUE;
		}
		break;
	}
	return (INT_PTR)FALSE;
}

// Message handler for dialog box.
INT_PTR CALLBACK DlgProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(lParam);
	switch (message)
	{
	case WM_INITDIALOG: {
		//
		// Connect to the accelerometer COM interface.
		//  The CLSID_ and IID_ are defined in the accelerometer.tlh file generated by .NET/COM Interop 
		//  (you can see this in the generated accelerometer.tlh file as a by-product of the #import).  
		//
		HRESULT hr = ::CoCreateInstance(CLSID_CHidManager,
			NULL,
			CLSCTX_INPROC_SERVER,
			IID_IHidManager,
			(void **)&g_pHidManager);
		if (FAILED(hr))
		{
			::MessageBox(HWND_DESKTOP, TEXT("Cannot CoCreateInstance HidManager COM object"), TEXT("HidManager Error"), MB_OK | MB_ICONERROR);
		}
		else
		{
			// Make sure the device is plugged in and visible.
			
			VARIANT_BOOL bFound;
			HRESULT hr = g_pHidManager->FindHidDevice(0x8086, 0x1018, &bFound);
			if (false == bFound)
			{
				::MessageBox(HWND_DESKTOP, TEXT("Cannot find ACPI-to-HID Mapper Driver"), TEXT("HidManager Error"), MB_OK | MB_ICONERROR);
//				return (INT_PTR)FALSE;   // we can still get HID inputs without this driver
			} 
			// Create the callback:
			// 1) Get IConnectionPointContainer.
			IConnectionPointContainer *pICPC = NULL;
			hr = g_pHidManager->QueryInterface(IID_IConnectionPointContainer, (void **)&pICPC);
			if (FAILED(hr))
			{
				::MessageBox(HWND_DESKTOP, TEXT("Cannot CoCreateInstance IConnectionPointContainer"), TEXT("HidManager Error"), MB_OK | MB_ICONERROR);
				return (INT_PTR)FALSE;
			}
			// 2) Find a connection point.
			IConnectionPoint *pCP = NULL;
			pICPC->FindConnectionPoint(DIID_IHidEvent, &pCP);  // The DIID_ also comes from HidManager.h
			// 3) Make a connection to this event source.
			g_pSink = new CHidManagerEventSink();
			ULONG cookie;
			pCP->Advise((IUnknown *)g_pSink, &cookie);
			// clean up
			pCP->Release();
			pICPC->Release();
		}
		}
		return (INT_PTR)TRUE;
		break;

	case WM_COMMAND:
        switch (LOWORD(wParam))
        {
        case IDCANCEL:  // Exit application
		case IDM_EXIT:
            {
                EndDialog(hDlg, 0);
                PostQuitMessage(0);
				return (INT_PTR)TRUE;
            }
            break;

		}

	default:
		return DefWindowProc(hDlg, message, wParam, lParam);
	}
	return (INT_PTR)FALSE;
}
