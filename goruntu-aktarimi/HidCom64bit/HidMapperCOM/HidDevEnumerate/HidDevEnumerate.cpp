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
// $Archive: /Lorado/HidMapperCOM/HidDevEnumerate/HidDevEnumerate.cpp $ 
//  
// $Author: Nmnguye1 $
//         
// $Date: 2/27/08 12:22p $
//                                                                            
////////////////////////////////////////////////////////////////////////////////

// HidDevEnumerate.cpp : Defines the entry point for the application.
// originally posted on microsoft.public.development.device.drivers
//
//

#include "stdafx.h"
#include "HidDevEnumerate.h"

#define MAX_LOADSTRING 100

// Global Variables:
HINSTANCE hInst;								// current instance
TCHAR szTitle[MAX_LOADSTRING];					// The title bar text
TCHAR szWindowClass[MAX_LOADSTRING];			// the main window class name

// Forward declarations of functions included in this code module:
ATOM				MyRegisterClass(HINSTANCE hInstance);
BOOL				InitInstance(HINSTANCE, int);
LRESULT CALLBACK	WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK	About(HWND, UINT, WPARAM, LPARAM);

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

	// Initialize global strings
	LoadString(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	LoadString(hInstance, IDC_HIDDEVENUMERATE, szWindowClass, MAX_LOADSTRING);
	MyRegisterClass(hInstance);

	// Perform application initialization:
	if (!InitInstance (hInstance, nCmdShow))
	{
		return FALSE;
	}

	hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_HIDDEVENUMERATE));

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
	wcex.hIcon			= LoadIcon(hInstance, MAKEINTRESOURCE(IDI_HIDDEVENUMERATE));
	wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground	= (HBRUSH)(COLOR_WINDOW+1);
	wcex.lpszMenuName	= MAKEINTRESOURCE(IDC_HIDDEVENUMERATE);
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
   HWND hWnd;

   hInst = hInstance; // Store instance handle in our global variable

   hWnd = CreateWindow(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
      CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, NULL, NULL, hInstance, NULL);

   if (!hWnd)
   {
      return FALSE;
   }

   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

   return TRUE;
}

BOOL	GetEditLines( void);

HINSTANCE	hInstance;
HWND		hwndMain = NULL,
			hwndEdit = NULL;
int			cEditLines;
TCHAR		*szAppName,
			szAppPath[ MAX_PATH];


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
	int			 wmId, wmEvent;
	PAINTSTRUCT  ps;
	HDC			 hdc;
	RECT		 rect;
	LOGFONT		 lf;
	static HFONT hFont = NULL;

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

	case WM_CREATE:
		GetClientRect( hWnd, &rect);
		// Create Windows
		hwndEdit = CreateWindow( TEXT("EDIT"), TEXT(""),
			ES_MULTILINE|ES_NOHIDESEL|ES_READONLY|
			WS_VSCROLL|WS_HSCROLL|WS_VISIBLE|WS_CHILD,
			0, 0, rect.right-rect.left, rect.bottom-rect.top,
			hWnd, NULL, hInstance, 0);
		if (!hwndEdit)
		{
			MbErr( FALSE, TEXT("Couldn't create edit control"));
			return -1;
		}

		lf.lfHeight			= 0;
		lf.lfWidth			= 0;
		lf.lfEscapement		= 0;
		lf.lfOrientation	= 0;
		lf.lfWeight			= FW_DONTCARE;
		lf.lfItalic			= FALSE;
		lf.lfUnderline		= FALSE;
		lf.lfStrikeOut		= FALSE;
		lf.lfCharSet		= DEFAULT_CHARSET;
		lf.lfOutPrecision	= OUT_DEFAULT_PRECIS;
		lf.lfClipPrecision	= CLIP_DEFAULT_PRECIS;
		lf.lfQuality		= DEFAULT_QUALITY;
		lf.lfPitchAndFamily	= FIXED_PITCH;
		lf.lfFaceName[0]	= '\0';

		hFont = CreateFontIndirect( &lf);

		if (!hFont)
		{
			MbErr( FALSE, TEXT("Couldn't create font"));
			return -1;
		}
		SendMessage( hwndEdit, WM_SETFONT, (WPARAM)hFont, TRUE);

		if (!GetEditLines())
			return -1;

		if (SetTimer( hWnd, 1, 50, NULL) == 0)
		{
			MbErr( FALSE, TEXT("Couldn't create timer"));
			return -1;
		}
		
		return 0;

	case WM_SIZE:
		GetClientRect( hWnd, &rect);
		MoveWindow( hwndEdit, 0, 0, rect.right - rect.left,
			rect.bottom - rect.top, TRUE);
		GetEditLines();
		break;

	case WM_DESTROY:
		if (hFont) DeleteObject( hFont);
		DestroyWindow( hwndEdit);
		PostQuitMessage( 0);
		return 0;

	case WM_TIMER:
		if (wParam == 1)
		{
			KillTimer( hWnd, 1);
			HidDevEnumFunction();
			RefreshDeviceList();
			return 0;
		}
		break;
	
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	} // switch

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

void MbErr( BOOL bKillApp, TCHAR *szMsg)
{
	MessageBox( NULL, szMsg, TEXT("Error"), MB_OK|MB_ICONERROR);
	if (bKillApp) ExitProcess( 1);
}


int logargs( TCHAR *szFormat, va_list args)
{
	int		len, curLine, lineCount;
	TCHAR	szBuf[ BUFSIZE], szBuf2[ BUFSIZE], *pSrc, *pDest;

	if ((!szFormat) || (!_tcslen(szFormat))) return 0;
	len = (int)SendMessage( hwndEdit, WM_GETTEXTLENGTH, 0, 0);
	SendMessage( hwndEdit, EM_SETSEL, len, len+1);

	pSrc = szFormat;
	pDest = szBuf2;
	while (*pSrc && (pDest - szBuf2 < BUFSIZE-2))
	{
		if (*pSrc != '\n')
			*pDest = *pSrc;
		else	{
			*pDest = '\x0D';
			pDest++;
			*pDest = '\x0A';
			}
		pSrc++;
		pDest++;
	} // while

	*pDest = '\0';
	if (pDest-szBuf2 >= BUFSIZE-2)
		return 0;
	len = _vsntprintf_s( szBuf, BUFSIZE-2, BUFSIZE, szBuf2, args);
	if (len < BUFSIZE-2)
		SendMessage( hwndEdit, EM_REPLACESEL, FALSE, (LPARAM)szBuf);
	else
		return 0;

	curLine		= (int) SendMessage( hwndEdit, EM_GETFIRSTVISIBLELINE, 0, 0);
	lineCount	= (int) SendMessage( hwndEdit, EM_GETLINECOUNT, 0, 0);

	if (lineCount - curLine > cEditLines)
		SendMessage( hwndEdit, EM_LINESCROLL, 0,
			lineCount - cEditLines - curLine);

	return len;
}


int	log( TCHAR *szFormat, ...)
{
	va_list	args;
	int	len=0;

	if (szFormat && _tcslen(szFormat))
	{
		va_start( args, szFormat);
		len = logargs( szFormat, args);
		va_end( args);
	}
	return len;
}


int	logln( TCHAR *szFormat, ...)
{
	va_list	args;
	int	len=0;

	if (szFormat && _tcslen(szFormat))
	{
		va_start( args, szFormat);
		len = logargs( szFormat, args);
		va_end( args);
	}
	return (log(TEXT("\n"))?len+2:len);
}

int	logwe( TCHAR *szFormat, ...)
{
	va_list	args;
	int	len=0;
	DWORD	lastError = GetLastError();

	if (szFormat && _tcslen(szFormat))
	{
		va_start( args, szFormat);
		len = logerrargs( lastError, szFormat, args);
		va_end( args);
	}
	return len;
}



int	logerr( DWORD dwWinError, TCHAR *szFormat, ...)
{
	va_list	args;
	int	len;

	if (szFormat && _tcslen(szFormat))
	{
		va_start( args, szFormat);
		len = logerrargs( dwWinError, szFormat, args);
		va_end( args);
	}

	return len;
}



int	logerrargs( DWORD dwWinError, TCHAR *szFormat, va_list args)
{
	TCHAR	szBuf[ BUFSIZE];
	int	len, len2;

	_tcscpy_s( szBuf, BUFSIZE, TEXT("\n\n*** Error ***\n"));
	len = (int) _tcslen( szBuf);

	if (szFormat && _tcslen(szFormat))
	{
		if ((len2 = (int) _tcslen( szFormat)) + len >= BUFSIZE - 2)
			return 0;
		_tcscat_s( szBuf, BUFSIZE, szFormat);
		len += len2;
		if (szBuf[len]!='\n')
		{
			if (len+1 >= BUFSIZE -2)
				return 0;
			_tcscat_s( szBuf, BUFSIZE, TEXT("\n"));
			len++;
		}
	}

	len2 =_sntprintf_s( &szBuf[len], BUFSIZE-2-len, BUFSIZE,
		TEXT("Windows says: (%u) "), dwWinError);
	if (!len2) 
		return 0;
	len = (int) _tcslen( szBuf);
	len2 = FormatMessage(
		FORMAT_MESSAGE_IGNORE_INSERTS | FORMAT_MESSAGE_FROM_SYSTEM,
		0, dwWinError, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		&szBuf[len], BUFSIZE - len, NULL);

	if (len2 == BUFSIZE - len)
		return 0;
	// done
	return logargs( szBuf, args);
}



BOOL GetEditLines( void)
{
	HDC		hdc;
	TEXTMETRIC	tm;
	RECT		rect;

	GetClientRect( hwndEdit, &rect);
	if ((hdc = GetDC( hwndEdit)) == NULL)
		MbErr( TRUE, TEXT("Couldn't get DC for edit control"));
	if (!GetTextMetrics( hdc, &tm))
		MbErr( TRUE, TEXT("Couldn't GetTextMetrics for edit control"));

	cEditLines = ((rect.bottom - rect.top) / tm.tmHeight) +
			 ( ((rect.bottom - rect.top) % tm.tmHeight) ? 1:0);

	ReleaseDC( hwndEdit, hdc);
	return TRUE;
}


HWND	GetMainWnd( void)
{ 
	return hwndMain; 
}

// EOF