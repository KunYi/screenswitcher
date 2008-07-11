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
// $Archive: /Lorado/HidMapperCOM/HidDevEnumerate/main.cpp $ 
//  
// $Author: Nmnguye1 $
//         
// $Date: 2/27/08 12:22p $
//                                                                            
////////////////////////////////////////////////////////////////////////////////

#include <InitGuid.h>

#include "stdafx.h"

void	SprintGuid( TCHAR *szBuf, size_t size, GUID *pGuid);
DWORD	GetDevices( GUID *pGuid);


// Function to enumerate the device
int HidDevEnumFunction( void)
{
	TCHAR	szBuf1[128] = {0};

	logln(_T("Start HID Device Enumerate\n"));

	// Do the HID Class
	SprintGuid( szBuf1, 128, (GUID*)&GUID_DEVCLASS_HIDCLASS);
	logln( TEXT("\nGUID Human interface devices = %s"), szBuf1);
	GetDevices( (GUID*)&GUID_DEVCLASS_HIDCLASS);

	// Do the MOUSE Class
	SprintGuid( szBuf1, 128, (GUID*)&GUID_DEVCLASS_MOUSE);
	logln( TEXT("\nGUID Mice = %s"), szBuf1);
	GetDevices( (GUID*)&GUID_DEVCLASS_MOUSE);

	return 1;
}

// Function to print GUID
void SprintGuid( TCHAR *szBuf, size_t size, GUID *pGuid)
{
	// print out GUID value
	_stprintf_s( szBuf, size,
		TEXT("{%8.8X-%4.4X-%4.4X-%2.2X%2.2X-%2.2X%2.2X%2.2X%2.2X%2.2X%2.2X}"),
		pGuid->Data1, pGuid->Data2, pGuid->Data3,
		pGuid->Data4[0], pGuid->Data4[1],
		pGuid->Data4[2], pGuid->Data4[3],
		pGuid->Data4[4], pGuid->Data4[5],
		pGuid->Data4[6], pGuid->Data4[7]);
}


// Retrieve devices
DWORD GetDevices(GUID *pGuid)
{
	TCHAR						szBuf[128] = {0};
	HDEVINFO					devInfo = NULL;
	SP_DEVINFO_DATA				devInfoData;
	DWORD						cnt=0;
	BOOL						retval;

	// Extract 
	devInfo = SetupDiGetClassDevsEx(
		pGuid,
		NULL,
		NULL,
		DIGCF_PRESENT,
		NULL,
		NULL,
		NULL);

	if (NULL == devInfo)
	{
		logwe(_T("SetupDiGetClassDevsEx"));
		return 0;
	}

	do	{
		// clean memory
		memset(&devInfoData,0,sizeof(devInfoData));
		devInfoData.cbSize = sizeof(devInfoData);
		// Get device info - first call with 0 index for first member then enumerate
		if ((retval = SetupDiEnumDeviceInfo(devInfo, cnt, &devInfoData))!=FALSE)
		{
			logln(TEXT("Device[%u] (DevInst: %8.8Xh)"),
				cnt, devInfoData.DevInst);
			// Get Device registry
			if (SetupDiGetDeviceRegistryProperty(
					devInfo,
					&devInfoData,
					SPDRP_DEVICEDESC,
					NULL,
					(PBYTE)szBuf,
					sizeof(szBuf),
					NULL))

				log(_T(" Desc: %s\n"),szBuf);
			else	
				logwe(NULL);
			// 
			logln(NULL);
			cnt++;
		}
	} while (retval != FALSE);

	// Check for error
	if ((retval == FALSE) && (GetLastError()!=ERROR_NO_MORE_ITEMS))
	{
		logwe( TEXT("Problem with 'SetupDiEnumDeviceInterfaces'"));
		return 0;
	}

	log(TEXT("%u device(s) were enumerated\n\n"), cnt);

	return cnt;
}


int RefreshDeviceList(void)
{
	SP_DEVICE_INTERFACE_DATA devInfoData;
	PSP_DEVICE_INTERFACE_DETAIL_DATA detailData = NULL;
	bool LastDevice = FALSE;
	int MemberIndex = 0;
	LONG Result;
	HANDLE DeviceHandle = NULL;
	GUID HidGuid;
	bool MyDeviceDetected = FALSE;

	ULONG Length;
	HANDLE hDevInfo;
	ULONG Required;

	logln(_T("Start RefreshDeviceList\n"));
	// Get the HID CLASS
	HidD_GetHidGuid(&HidGuid);

	hDevInfo=SetupDiGetClassDevs(&HidGuid,
		NULL,
		NULL,
		DIGCF_PRESENT|DIGCF_INTERFACEDEVICE);

	devInfoData.cbSize = sizeof(devInfoData);
	MemberIndex = 0;
	LastDevice = FALSE;

	do
	{
		// Get the top of the device list
		Result=SetupDiEnumDeviceInterfaces(hDevInfo,
			0,
			&HidGuid,
			MemberIndex,
			&devInfoData);

		// A device has been detected, so get more information about it.
		if (Result != 0)
		{
			// First call to get buffer size
			Result = SetupDiGetDeviceInterfaceDetail(hDevInfo,
				&devInfoData,
				NULL,
				0,
				&Length,
				NULL);

			//Allocate memory for the hDevInfo structure, using the returned Length.
			detailData = (PSP_DEVICE_INTERFACE_DETAIL_DATA)malloc(
				sizeof(SP_DEVICE_INTERFACE_DETAIL_DATA)+ Length+1);
			// Must always be the size of detailData
			detailData->cbSize = sizeof(SP_DEVICE_INTERFACE_DETAIL_DATA);
			
			// Set cbSize in the detailData structure.
			Required = 0;	
			// 2nd Call to get the actual information
			Result = SetupDiGetDeviceInterfaceDetail(hDevInfo,
				&devInfoData,
				detailData,
				Length,   // DeviceInterfaceDetailDataSize
				&Required,
				NULL);
			
			// See if we can create device handle
			log(TEXT("Device Detail required %d - Device Path: %s\n"), Required, detailData->DevicePath);
			DeviceHandle=CreateFile(detailData->DevicePath,
				0,
				FILE_SHARE_READ|FILE_SHARE_WRITE,
				(LPSECURITY_ATTRIBUTES)NULL,
				OPEN_EXISTING,
				0,
				NULL);

			// close the handle
			if (DeviceHandle != INVALID_HANDLE_VALUE)
			{
				CloseHandle(DeviceHandle);
			}
			else
			{
				log(TEXT("Failed to Open Device - error: %8.8Xh\n"), GetLastError());
			}

			// deallocate memory
			free(detailData);

		} 
		else
		{
			// there are no more devices to check.
			LastDevice=TRUE;
		}
		
		MemberIndex = MemberIndex + 1;

	} //do
	while ((LastDevice == FALSE) && (MyDeviceDetected == FALSE));


	//Free the memory reserved for hDevInfo by SetupDiClassDevs.
	SetupDiDestroyDeviceInfoList(hDevInfo);

	// return the count
	return MemberIndex;
}