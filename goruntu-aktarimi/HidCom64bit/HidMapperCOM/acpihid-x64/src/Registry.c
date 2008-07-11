////////////////////////////////////////////////////////////////////////////////
//      Copyright (c) 2001-2007, Intel Corporation.  All Rights Reserved.     //
//                                                                            //
//   Portions used from the WinDDK are Copyright (c) Microsoft Corporation.   //
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
//                                                                            //
// Module Name: Registry.c                                                    //
//                                                                            //
// Abstract: Get driver configuration settings from the registry.             //
//                                                                            //                                                                            //
// Authors: Babu Pallithanam                                                  //
//          Jim Trethewey                                                     //
//                                                                            //
// Environment: kernel mode only                                              //
//                                                                            //
// Revision History:                                                          //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////


#include "Main.h"
#include "Registry.h"
#include "Acpi.h"


#ifdef ALLOC_PRAGMA
	#pragma alloc_text( PAGE,  CheckRegistryForDescriptor )
	#pragma alloc_text( PAGE,  ReadDescriptorFromRegistry )
	#pragma alloc_text( PAGE,  CheckNumberOfButtonsFromRegistry )
	#pragma alloc_text( PAGE,  ReadButtonInfoFromRegistry )
	#pragma alloc_text( PAGE,  CheckNumberOfLedsFromRegistry )
	#pragma alloc_text( PAGE,  CheckNumberOfVirtualButtonsFromRegistry )
	#pragma alloc_text( PAGE,  ReadVirtualButtonInfoFromRegistry )
	#pragma alloc_text( PAGE,  InitializeVirtualButtonsFromRegistry )
#endif



NTSTATUS
CheckRegistryForDescriptor(
        IN PDEVICE_OBJECT  DeviceObject
        )
/*++

Routine Description:

    Read "ReadFromRegistry" key value from device parameters in the registry.

Arguments:

    DeviceObject - pointer to a device object.

Return Value:

    NT status code.

--*/

{
    NTSTATUS           ntStatus = STATUS_SUCCESS;
    PDEVICE_OBJECT     pdo;
    HANDLE             handle;
    ULONG              length = 0;
    ULONG              resultLength = 0;
    PUCHAR             ReadFromRegistryKeyValue = NULL;
    UNICODE_STRING     ReadFromRegistryKeyValueName;
    ULONG              ReadFromRegistry;

	PAGED_CODE();

	DebugPrint(("[acpihid.sys] CheckRegistryForDescriptor Entry: DeviceObject=0x%lx\n", DeviceObject));

    pdo = GET_PHYSICAL_DEVICE_OBJECT(DeviceObject);

//
// Get a handle to the Device Parameters registry key
//

    ntStatus = IoOpenDeviceRegistryKey(
        pdo,
        PLUGPLAY_REGKEY_DEVICE,
        KEY_QUERY_VALUE,
        &handle
        );

    if ( ! NT_SUCCESS( ntStatus ))
    {
        DebugPrint(("[acpihid.sys] IoOpenDeviceRegistryKey failed\nCheckRegistryForDescriptor Exit: status=0x%x\n", ntStatus));
        return ntStatus;
    }

//
// Query the "ReadFromRegistry" key value in the parameters key
//

    RtlInitUnicodeString(&ReadFromRegistryKeyValueName, L"ReadFromRegistry");

    ntStatus = ZwQueryValueKey(
        handle,
        &ReadFromRegistryKeyValueName,
        KeyValuePartialInformation ,
        (PVOID) ReadFromRegistryKeyValue,
        length,
        &resultLength
        );

//
// Query the registry again using resultlength as buffer length
//
    if ( STATUS_BUFFER_TOO_SMALL == ntStatus )
    {
        length = resultLength;
        ReadFromRegistryKeyValue = ExAllocatePoolWithTag(
            PagedPool,
            length,
            VHID_POOL_TAG
            );

        if ( ! ReadFromRegistryKeyValue )
        {
            DebugPrint(("[acpihid.sys] Allocating to keyValueInformation failed\n"));
            ntStatus = STATUS_UNSUCCESSFUL;
            goto query_end;
        }

        ntStatus= ZwQueryValueKey(
                handle,
                &ReadFromRegistryKeyValueName ,
                KeyValuePartialInformation ,
                (PVOID)ReadFromRegistryKeyValue,
                length,
                &resultLength
                );

        if(!NT_SUCCESS(ntStatus)){
           DebugPrint(("[acpihid.sys] ZwQueryValueKey for ReadFromRegistry failed status:0x%x\n", ntStatus));
           goto query_end;
        }
    }
    else
    {
        if ( ! NT_SUCCESS( ntStatus ))
        {
           DebugPrint(("[acpihid.sys] ZwQueryValueKey for ReadFromRegistry failed status:0x%x\n", ntStatus));
           goto query_end;
        }
    }

//
// if ReadFromRegsitry is set to 0 then we do not need to read
// the descriptor from regsistry. Just return failure.
//
    ReadFromRegistry = ((PKEY_VALUE_PARTIAL_INFORMATION)
        ReadFromRegistryKeyValue)->Data[0] ;

    if ( ! ReadFromRegistry )
    {
        ntStatus = STATUS_UNSUCCESSFUL;
        goto query_end;
    }

query_end:

    ZwClose(handle);

    if ( ReadFromRegistryKeyValue )
        ExFreePool(ReadFromRegistryKeyValue);

	DebugPrint(("[acpihid.sys] Exiting CheckRegistryForDescriptor\n"));

    return ntStatus;

}


NTSTATUS
ReadDescriptorFromRegistry(
        IN PDEVICE_OBJECT  DeviceObject
        )
/*++

Routine Description:

    Read HID report descriptor from registry

Arguments:

    DeviceObject - pointer to a device object.

Return Value:

    NT status code.

--*/
{
    NTSTATUS             ntStatus = STATUS_SUCCESS;
    PDEVICE_OBJECT       pdo;
    PDEVICE_EXTENSION    deviceInfo;
    HANDLE               handle;
    ULONG                length = 0;
    ULONG                resultLength = 0;
    ULONG                RegistryReportDescriptorLength = 0;
    PUCHAR               RegistryReportDescriptor= NULL;
    PUCHAR               keyValueInformation = NULL;
    UNICODE_STRING       valueName;

	PAGED_CODE();

    //
    // Get physical device object
    //

    pdo = GET_PHYSICAL_DEVICE_OBJECT(DeviceObject);

    //
    // Get the hid mini driver device extension
    //

    deviceInfo = GET_MINIDRIVER_DEVICE_EXTENSION(DeviceObject);

    //
    // Get a handle to the Device registry key
    //

    ntStatus = IoOpenDeviceRegistryKey(pdo,
                                       PLUGPLAY_REGKEY_DEVICE,
                                       KEY_QUERY_VALUE,
                                       &handle
                                       );

    if(!NT_SUCCESS(ntStatus)){

       DebugPrint(("[acpihid.sys] ReadDescriptorFromRegistry IoOpenDeviceRegistryKey failed status:0x%x\n", ntStatus));
       return ntStatus;

    }

    //
    // Query the Descriptor value in the parameters key
    //
    RtlInitUnicodeString(&valueName, L"HidReportDescriptor");

    ntStatus= ZwQueryValueKey(
                handle,
                &valueName,
                KeyValuePartialInformation,
                (PVOID)keyValueInformation,
                length,
                &resultLength
                );

    if(STATUS_BUFFER_TOO_SMALL == ntStatus){
        length = resultLength;
        keyValueInformation = ExAllocatePoolWithTag(
                                    PagedPool,
                                    length,
                                    VHID_POOL_TAG
                                    );

        if(!keyValueInformation){

            DebugPrint(("[acpihid.sys] ReadDescriptorFromRegistry Allocating to keyValueInformation failed\n"));
            ntStatus = STATUS_UNSUCCESSFUL;
            goto query_end;

        }
        ntStatus= ZwQueryValueKey(
                handle,
                &valueName ,
                KeyValuePartialInformation ,
                (PVOID)keyValueInformation,
                length,
                &resultLength
                );

        if(!NT_SUCCESS(ntStatus)){

            DebugPrint(("[acpihid.sys] ReadDescriptorFromRegistry ZwQueryValueKey failed status:0x%x\n", ntStatus));
            goto query_end;

        }
        //
        // Allocate memory for Report Descriptor
        //
        RegistryReportDescriptorLength =
                  ((PKEY_VALUE_PARTIAL_INFORMATION)
                  keyValueInformation)->DataLength;

        RegistryReportDescriptor =
                  ExAllocatePoolWithTag(NonPagedPool,
                                        RegistryReportDescriptorLength,
                                        VHID_POOL_TAG
                                        );

        if(!RegistryReportDescriptor){

            DebugPrint(("[acpihid.sys] ReadDescriptorFromRegistry Memory allocation for Report descriptor failed\n"));
            ntStatus = STATUS_UNSUCCESSFUL;
            goto query_end;

        }

        //
        // Copy report descriptor from registry
        //
        RtlCopyMemory(RegistryReportDescriptor,
                      ((PKEY_VALUE_PARTIAL_INFORMATION)
                      keyValueInformation)->Data,
                      RegistryReportDescriptorLength
                      );

        DebugPrint(("[acpihid.sys] ReadDescriptorFromRegistry Number of report descriptor bytes copied: %d\n",
                    RegistryReportDescriptorLength
                    ));

        //
        // Store the registry report descriptor in the device extension
        //
        deviceInfo->ReadReportDescFromRegistry = TRUE;
        deviceInfo->ReportDescriptor = RegistryReportDescriptor;
        deviceInfo->HidDescriptor.DescriptorList[0].wReportLength =
                                   (USHORT)RegistryReportDescriptorLength;

    }
    else {
        if(!NT_SUCCESS(ntStatus)) {
            DebugPrint(("[acpihid.sys] ReadDescriptorFromRegistry ZwQueryValueKey failed status:0x%x\n", ntStatus));
        }
    }

    query_end:

    ZwClose(handle);

    if(keyValueInformation)
        ExFreePool(keyValueInformation);

    return ntStatus;

}




NTSTATUS
CheckNumberOfButtonsFromRegistry(
        IN PDEVICE_OBJECT  DeviceObject
        )
/*++

Routine Description:

    Read "ReadFromRegistry" key value from device parameters in the registry.

Arguments:

    DeviceObject - pointer to a device object.

Return Value:

    NT status code.

--*/

{

    NTSTATUS           ntStatus = STATUS_SUCCESS;
    PDEVICE_OBJECT     pdo;
    HANDLE             handle;
    ULONG              length = 0, resultLength = 0;
    PUCHAR             ReadFromRegistryKeyValue = NULL;
    UNICODE_STRING     ReadFromRegistryKeyValueName;

    PDEVICE_EXTENSION    deviceInfo;

	PAGED_CODE();

 	//
    // Get physical device object
    //

    pdo = GET_PHYSICAL_DEVICE_OBJECT(DeviceObject);


    deviceInfo = GET_MINIDRIVER_DEVICE_EXTENSION(DeviceObject);

    //
    // Get a handle to the Device Parameters registry key
    //

    ntStatus = IoOpenDeviceRegistryKey(
                         pdo,
                         PLUGPLAY_REGKEY_DEVICE,
                         KEY_QUERY_VALUE,
                         &handle
                         );

    if (!NT_SUCCESS(ntStatus)) {
       DebugPrint(("[acpihid.sys] CheckNumberOfButtonsFromRegistry IoOpenDeviceRegistryKey failed status: 0x%x\n", ntStatus));
       return ntStatus;
    }

    //
    //Query the "NumberOfButtons" key value in the parameters key
    //

    RtlInitUnicodeString(&ReadFromRegistryKeyValueName, L"NumberOfButtons");

    ntStatus= ZwQueryValueKey(
                handle,
                &ReadFromRegistryKeyValueName,
                KeyValuePartialInformation ,
                (PVOID)ReadFromRegistryKeyValue,
                length,
                &resultLength
                );

    if (STATUS_BUFFER_TOO_SMALL == ntStatus) {
        //
        // Query the registry again using resultlength as buffer length
        //

        length = resultLength;
        ReadFromRegistryKeyValue = ExAllocatePoolWithTag(
                                                    PagedPool,
                                                    length,
                                                    VHID_POOL_TAG
                                                    );

        if (!ReadFromRegistryKeyValue) {
            DebugPrint(("[acpihid.sys] CheckNumberOfButtonsFromRegistry Allocating to keyValueInformation failed\n"));
            ntStatus = STATUS_UNSUCCESSFUL;
            goto query_end;
        }

        ntStatus= ZwQueryValueKey(
                handle,
                &ReadFromRegistryKeyValueName ,
                KeyValuePartialInformation ,
                (PVOID)ReadFromRegistryKeyValue,
                length,
                &resultLength
                );

        if (!NT_SUCCESS(ntStatus)) {
           DebugPrint(("[acpihid.sys] CheckNumberOfButtonsFromRegistry ZwQueryValueKey for ReadFromRegistry failed status: 0x%x\n", ntStatus));
           goto query_end;
        }
    } else {
        if (!NT_SUCCESS(ntStatus)) {
           DebugPrint(("[acpihid.sys] CheckNumberOfButtonsFromRegistry ZwQueryValueKey for ReadFromRegistry failed status: 0x%x\n", ntStatus));
           goto query_end;
        }
    }

    deviceInfo->NumberOfButtons = ((PKEY_VALUE_PARTIAL_INFORMATION) ReadFromRegistryKeyValue)->Data[0] ;

query_end:

    ZwClose(handle);

    if(ReadFromRegistryKeyValue)
        ExFreePool(ReadFromRegistryKeyValue);

    return ntStatus;

}

NTSTATUS
ReadButtonInfoFromRegistry(
        IN PDEVICE_OBJECT  DeviceObject
        )
/*++

Routine Description:

    Read HID report descriptor from registry

Arguments:

    DeviceObject - pointer to a device object.

Return Value:

    NT status code.

--*/
{
    NTSTATUS             ntStatus = STATUS_SUCCESS;
    PDEVICE_OBJECT       pdo;
    PDEVICE_EXTENSION    deviceInfo;
    HANDLE               handle;
    ULONG                length = 0, NoButtons = 0;
    ULONG                resultLength = 0;
    ULONG                RegistryButtonInfoLength = 0;
    PUCHAR               RegistryButtonInfo = NULL;
    PUCHAR               keyValueInformation = NULL;
    UNICODE_STRING       valueName[MAX_NO_BUTTONS + 1];

	PCWSTR               Button[] = {	L"HoldButton",
										L"Button1",	L"Button2",	L"Button3",	L"Button4",
										L"Button5", L"Button6",	L"Button7",	L"Button8",
										L"Button9",	L"Button10",L"Button11",L"Button12",
										L"Button13",L"Button14",L"Button15",L"Button16",
										L"Button17",L"Button18",L"Button19",L"Button20",
										L"Button21",L"Button22",L"Button23",L"Button24",
										L"Button25",L"Button26",L"Button27",L"Button28",
										L"Button29",L"Button30",L"Button31",L"Button32",
										L"Button33",L"Button34",L"Button35",L"Button36",
										L"Button37",L"Button38",L"Button39",L"Button40",
										L"Button41",L"Button42",L"Button43",L"Button44",
										L"Button45",L"Button46",L"Button47",L"Button48",
										L"Button49",L"Button50",L"Button51",L"Button52",
										L"Button53",L"Button54",L"Button55",L"Button56",
										L"Button57",L"Button58",L"Button59",L"Button60",
										L"Button61",L"Button62",L"Button63",L"Button64"
									};


	PAGED_CODE();

    //
    // Get physical device object
    //

    pdo = GET_PHYSICAL_DEVICE_OBJECT(DeviceObject);

    //
    // Get the hid mini driver device extension
    //
	//return ntStatus;

    deviceInfo = GET_MINIDRIVER_DEVICE_EXTENSION(DeviceObject);

    //
    // Get a handle to the Device registry key
    //


	if (deviceInfo->NumberOfButtons > MAX_NO_BUTTONS) {
        DebugPrint(("[acpihid.sys] ReadButtonInfoFromRegistry Registry Number of buttons %d is above driver's hard-coded limit %d\n", deviceInfo->NumberOfButtons, MAX_NO_BUTTONS));
		return   STATUS_UNSUCCESSFUL;
	}


	for (NoButtons = 0; NoButtons <= deviceInfo->NumberOfButtons; NoButtons++) {
		ntStatus = IoOpenDeviceRegistryKey(pdo,
				                           PLUGPLAY_REGKEY_DEVICE,
				                           KEY_QUERY_VALUE,
					                       &handle
						                   );

		if (!NT_SUCCESS(ntStatus)) {
			DebugPrint(("[acpihid.sys] ReadButtonInfoFromRegistry IoOpenDeviceRegistryKey failed status:0x%x\n", ntStatus));
			return ntStatus;
		}

		RtlInitUnicodeString(&valueName[NoButtons],  Button[NoButtons]);

		ntStatus = ZwQueryValueKey(
								 handle,
								 &valueName[NoButtons],
								 KeyValuePartialInformation,
								 (PVOID)keyValueInformation,
								 length,
								 &resultLength
								);

		if (STATUS_BUFFER_TOO_SMALL == ntStatus) {
			length = resultLength;
			keyValueInformation = ExAllocatePoolWithTag(
                                    PagedPool,
                                    length,
                                    VHID_POOL_TAG
                                    );

            if (!keyValueInformation) {
               DebugPrint(("[acpihid.sys] ReadButtonInfoFromRegistry Allocating %ld bytes to keyValueInformation failed\n", length));
               ntStatus = STATUS_UNSUCCESSFUL;
               goto query_end;
			}

		    ntStatus= ZwQueryValueKey(
									handle,
									&valueName[NoButtons] ,
									KeyValuePartialInformation ,
									(PVOID)keyValueInformation,
									length,
									&resultLength
									);

			if(!NT_SUCCESS(ntStatus)){
				DebugPrint(("[acpihid.sys] ReadButtonInfoFromRegistry ZwQueryValueKey (key=%ls) failed status:0x%x\n",
					Button[NoButtons], ntStatus));
				goto query_end;
			}

	    	//
        	// Allocate memory for  button info
        	//
			RegistryButtonInfoLength = ((PKEY_VALUE_PARTIAL_INFORMATION) keyValueInformation)->DataLength;

			if (RegistryButtonInfoLength != sizeof(BUTTON_INFO)) {
				DebugPrint(("[acpihid.sys] ReadButtonInfoFromRegistry Invalid RegistryButtonInfoLength: 0x%lx, sizeof(BUTTON_INFO): 0x%lx\n", RegistryButtonInfoLength, sizeof(BUTTON_INFO)));
				goto query_end;
			}

        	//
        	// Copy  the button info from the registry
        	//

            RtlCopyMemory(&deviceInfo->Buttons[NoButtons].ReportID,
						  &((PKEY_VALUE_PARTIAL_INFORMATION)
						  keyValueInformation)->Data[0],
                          sizeof(UCHAR)
                         );

            RtlCopyMemory(&deviceInfo->Buttons[NoButtons].UsagePageMsb,
						  &((PKEY_VALUE_PARTIAL_INFORMATION)
						  keyValueInformation)->Data[1],
                          sizeof(UCHAR)
                         );
            RtlCopyMemory(&deviceInfo->Buttons[NoButtons].UsagePageLsb,
						  &((PKEY_VALUE_PARTIAL_INFORMATION)
						  keyValueInformation)->Data[2],
                          sizeof(UCHAR)
                         );

            RtlCopyMemory(&deviceInfo->Buttons[NoButtons].UsageMsb,
						  &((PKEY_VALUE_PARTIAL_INFORMATION)
						  keyValueInformation)->Data[3],
                          sizeof(UCHAR)
                         );
            RtlCopyMemory(&deviceInfo->Buttons[NoButtons].UsageLsb,
						  &((PKEY_VALUE_PARTIAL_INFORMATION)
						  keyValueInformation)->Data[4],
                          sizeof(UCHAR)
                         );

            RtlCopyMemory(&deviceInfo->Buttons[NoButtons].ButtonUsageMsb,
						  &((PKEY_VALUE_PARTIAL_INFORMATION)
						  keyValueInformation)->Data[5],
                          sizeof(UCHAR)
                         );

            RtlCopyMemory(&deviceInfo->Buttons[NoButtons].ButtonUsageLsb,
						  &((PKEY_VALUE_PARTIAL_INFORMATION)
						  keyValueInformation)->Data[6],
                          sizeof(UCHAR)
                         );

            RtlCopyMemory(&deviceInfo->Buttons[NoButtons].KeyStatus,
						  &((PKEY_VALUE_PARTIAL_INFORMATION)
						  keyValueInformation)->Data[7],
                          sizeof(UCHAR)
                         );

			DebugPrint(("[acpihid.sys]    %ls: 0x%lx, 0x%04lx, 0x%04lx, 0x%04lx, 0x%lx\n",
						   Button[NoButtons],
						   deviceInfo->Buttons[NoButtons].ReportID,
						   (deviceInfo->Buttons[NoButtons].UsagePageMsb << 8) | deviceInfo->Buttons[NoButtons].UsagePageLsb,
						   (deviceInfo->Buttons[NoButtons].UsageMsb << 8) | deviceInfo->Buttons[NoButtons].UsageLsb,
						   (deviceInfo->Buttons[NoButtons].ButtonUsageMsb << 8) | deviceInfo->Buttons[NoButtons].ButtonUsageLsb,
						   deviceInfo->Buttons[NoButtons].KeyStatus));

			// RtlFreeUnicodeString(&valueName);
		 } else {
              if(!NT_SUCCESS(ntStatus)) {
                 DebugPrint(("[acpihid.sys] ReadButtonInfoFromRegistry ZwQueryValueKey (key=%ls) failed status:0x%x\n",
					Button[NoButtons], ntStatus));
			  }
		 }

    query_end:

    	ZwClose(handle);

    	if(keyValueInformation)
        	ExFreePool(keyValueInformation);

    	length = 0;
    	resultLength = 0;
    	RegistryButtonInfoLength = 0;
    	RegistryButtonInfo = NULL;
    	keyValueInformation = NULL;

	} // End of for loop

    return ntStatus;
}


NTSTATUS
CheckNumberOfLedsFromRegistry(
        IN PDEVICE_OBJECT  DeviceObject
        )
/*++

Routine Description:

    Read "ReadFromRegistry" key value from device parameters in the registry.

Arguments:

    DeviceObject - pointer to a device object.

Return Value:

    NT status code.

--*/

{

    NTSTATUS           ntStatus = STATUS_SUCCESS;
    PDEVICE_OBJECT     pdo;
    HANDLE             handle;
    ULONG              length = 0, resultLength = 0;
    PUCHAR             ReadFromRegistryKeyValue = NULL;
    UNICODE_STRING     ReadFromRegistryKeyValueName;

    PDEVICE_EXTENSION    deviceInfo;

	PAGED_CODE();

 	//
    // Get physical device object
    //

    pdo = GET_PHYSICAL_DEVICE_OBJECT(DeviceObject);


    deviceInfo = GET_MINIDRIVER_DEVICE_EXTENSION(DeviceObject);

    //
    // Get a handle to the Device Parameters registry key
    //

    ntStatus = IoOpenDeviceRegistryKey(
                         pdo,
                         PLUGPLAY_REGKEY_DEVICE,
                         KEY_QUERY_VALUE,
                         &handle
                         );

    if (!NT_SUCCESS(ntStatus)) {
       DebugPrint(("[acpihid.sys] CheckNumberOfLedsFromRegistry IoOpenDeviceRegistryKey failed status: 0x%x\n", ntStatus));
       return ntStatus;
    }

    //
    //Query the "NumberOfLeds" key value in the parameters key
    //

    RtlInitUnicodeString(&ReadFromRegistryKeyValueName, L"NumberOfLeds");

    ntStatus= ZwQueryValueKey(
                handle,
                &ReadFromRegistryKeyValueName,
                KeyValuePartialInformation ,
                (PVOID)ReadFromRegistryKeyValue,
                length,
                &resultLength
                );

    if (STATUS_BUFFER_TOO_SMALL == ntStatus) {
        //
        // Query the registry again using resultlength as buffer length
        //

        length = resultLength;
        ReadFromRegistryKeyValue = ExAllocatePoolWithTag(
                                                    PagedPool,
                                                    length,
                                                    VHID_POOL_TAG
                                                    );

        if (!ReadFromRegistryKeyValue) {
            DebugPrint(("[acpihid.sys] CheckNumberOfLedsFromRegistry Allocating to keyValueInformation failed\n"));
            ntStatus = STATUS_UNSUCCESSFUL;
            goto query_end;
        }

        ntStatus= ZwQueryValueKey(
                handle,
                &ReadFromRegistryKeyValueName ,
                KeyValuePartialInformation ,
                (PVOID)ReadFromRegistryKeyValue,
                length,
                &resultLength
                );

        if (!NT_SUCCESS(ntStatus)) {
           DebugPrint(("[acpihid.sys] CheckNumberOfLedsFromRegistry ZwQueryValueKey for ReadFromRegistry failed status: 0x%x\n", ntStatus));
           goto query_end;
        }
    } else {
        if (!NT_SUCCESS(ntStatus)) {
           DebugPrint(("[acpihid.sys] CheckNumberOfLedsFromRegistry ZwQueryValueKey for ReadFromRegistry failed status: 0x%x\n", ntStatus));
           goto query_end;
        }
    }

    deviceInfo->NumberOfLeds = ((PKEY_VALUE_PARTIAL_INFORMATION) ReadFromRegistryKeyValue)->Data[0] ;

query_end:

    ZwClose(handle);

    if( ReadFromRegistryKeyValue )
	{
        ExFreePool(ReadFromRegistryKeyValue);
	}

	DebugPrint(("[acpihid.sys] CheckNumberOfLedsFromRegistry Exit: status= 0x%x\n", ntStatus));
    return ntStatus;
}

NTSTATUS
CheckNumberOfVirtualButtonsFromRegistry(
        IN PDEVICE_OBJECT  DeviceObject
        )
/*++

Routine Description:

    Read "ReadFromRegistry" key value from device parameters in the registry.

Arguments:

    DeviceObject - pointer to a device object.

Return Value:

    NT status code.

--*/

{

    NTSTATUS           ntStatus = STATUS_SUCCESS;
    PDEVICE_OBJECT     pdo;
    HANDLE             handle;
    ULONG              length = 0, resultLength = 0;
    PUCHAR             ReadFromRegistryKeyValue = NULL;
    UNICODE_STRING     ReadFromRegistryKeyValueName;

    PDEVICE_EXTENSION    deviceInfo;

	PAGED_CODE();

 	//
    // Get physical device object
    //

    pdo = GET_PHYSICAL_DEVICE_OBJECT(DeviceObject);


    deviceInfo = GET_MINIDRIVER_DEVICE_EXTENSION(DeviceObject);

    //
    // Get a handle to the Device Parameters registry key
    //

    ntStatus = IoOpenDeviceRegistryKey(
                         pdo,
                         PLUGPLAY_REGKEY_DEVICE,
                         KEY_QUERY_VALUE,
                         &handle
                         );

    if (!NT_SUCCESS(ntStatus)) {
       DebugPrint(("[acpihid.sys] CheckNumberOfVirtualButtonsFromRegistry IoOpenDeviceRegistryKey failed status: 0x%x\n", ntStatus));
       return ntStatus;
    }

    //
    //Query the "NumberOfVirtualButtons" key value in the parameters key
    //

    RtlInitUnicodeString(&ReadFromRegistryKeyValueName, L"NumberOfVirtualButtons");

    ntStatus= ZwQueryValueKey(
                handle,
                &ReadFromRegistryKeyValueName,
                KeyValuePartialInformation ,
                (PVOID)ReadFromRegistryKeyValue,
                length,
                &resultLength
                );

    if (STATUS_BUFFER_TOO_SMALL == ntStatus) {
        //
        // Query the registry again using resultlength as buffer length
        //

        length = resultLength;
        ReadFromRegistryKeyValue = ExAllocatePoolWithTag(
                                                    PagedPool,
                                                    length,
                                                    VHID_POOL_TAG
                                                    );

        if (!ReadFromRegistryKeyValue) {
            DebugPrint(("[acpihid.sys] CheckNumberOfVirtualButtonsFromRegistry Allocating to keyValueInformation failed\n"));
            ntStatus = STATUS_UNSUCCESSFUL;
            goto query_end;
        }

        ntStatus= ZwQueryValueKey(
                handle,
                &ReadFromRegistryKeyValueName ,
                KeyValuePartialInformation ,
                (PVOID)ReadFromRegistryKeyValue,
                length,
                &resultLength
                );

        if (!NT_SUCCESS(ntStatus)) {
           DebugPrint(("[acpihid.sys] CheckNumberOfVirtualButtonsFromRegistry ZwQueryValueKey for ReadFromRegistry failed status: 0x%x\n", ntStatus));
           goto query_end;
        }
    } else {
        if (!NT_SUCCESS(ntStatus)) {
           DebugPrint(("[acpihid.sys] CheckNumberOfVirtualButtonsFromRegistry ZwQueryValueKey for ReadFromRegistry failed status: 0x%x\n", ntStatus));
           goto query_end;
        }
    }

    deviceInfo->NumberOfVirtualButtons = ((PKEY_VALUE_PARTIAL_INFORMATION) ReadFromRegistryKeyValue)->Data[0] ;

query_end:

    ZwClose(handle);

    if(ReadFromRegistryKeyValue)
        ExFreePool(ReadFromRegistryKeyValue);

    return ntStatus;

}

NTSTATUS
ReadVirtualButtonInfoFromRegistry(
        IN PDEVICE_OBJECT  DeviceObject
        )
/*++

Routine Description:

    Read HID report descriptor from registry

Arguments:

    DeviceObject - pointer to a device object.

Return Value:

    NT status code.

--*/
{
    NTSTATUS             ntStatus = STATUS_SUCCESS;
    PDEVICE_OBJECT       pdo;
    PDEVICE_EXTENSION    deviceInfo;
    HANDLE               handle;
    ULONG                length = 0, NoVirtualButtons = 0;
    ULONG                resultLength = 0;
    ULONG                RegistryVirtualButtonInfoLength = 0;
    PUCHAR               RegistryVirtualButtonInfo = NULL;
    PUCHAR               keyValueInformation = NULL;
    UNICODE_STRING       valueName[MAX_NO_VIRTUAL_BUTTONS + 1];

	PCWSTR               VirtualButton[] = {	L"HoldButton",
										L"VirtualButton1", L"VirtualButton2", L"VirtualButton3", L"VirtualButton4",
										L"VirtualButton5", L"VirtualButton6", L"VirtualButton7", L"VirtualButton8",
										L"VirtualButton9", L"VirtualButton10",L"VirtualButton11",L"VirtualButton12",
										L"VirtualButton13",L"VirtualButton14",L"VirtualButton15",L"VirtualButton16",
										L"VirtualButton17",L"VirtualButton18",L"VirtualButton19",L"VirtualButton20",
										L"VirtualButton21",L"VirtualButton22",L"VirtualButton23",L"VirtualButton24",
										L"VirtualButton25",L"VirtualButton26",L"VirtualButton27",L"VirtualButton28",
										L"VirtualButton29",L"VirtualButton30",L"VirtualButton31",L"VirtualButton32",
										L"VirtualButton33",L"VirtualButton34",L"VirtualButton35",L"VirtualButton36",
										L"VirtualButton37",L"VirtualButton38",L"VirtualButton39",L"VirtualButton40",
										L"VirtualButton41",L"VirtualButton42",L"VirtualButton43",L"VirtualButton44",
										L"VirtualButton45",L"VirtualButton46",L"VirtualButton47",L"VirtualButton48",
										L"VirtualButton49",L"VirtualButton50",L"VirtualButton51",L"VirtualButton52",
										L"VirtualButton53",L"VirtualButton54",L"VirtualButton55",L"VirtualButton56",
										L"VirtualButton57",L"VirtualButton58",L"VirtualButton59",L"VirtualButton60",
										L"VirtualButton61",L"VirtualButton62",L"VirtualButton63",L"VirtualButton64"
									};


	PAGED_CODE();

    //
    // Get physical device object
    //

    pdo = GET_PHYSICAL_DEVICE_OBJECT(DeviceObject);

    //
    // Get the hid mini driver device extension
    //
	//return ntStatus;

    deviceInfo = GET_MINIDRIVER_DEVICE_EXTENSION(DeviceObject);

    //
    // Get a handle to the Device registry key
    //


	if (deviceInfo->NumberOfVirtualButtons > MAX_NO_VIRTUAL_BUTTONS) {
        DebugPrint(("[acpihid.sys] ReadVirtualButtonInfoFromRegistry Registry Number of virtual buttons %d is above driver's hard-coded limit %d\n",
        	deviceInfo->NumberOfVirtualButtons, MAX_NO_VIRTUAL_BUTTONS));
		return   STATUS_UNSUCCESSFUL;
	}


	for (NoVirtualButtons = 1; NoVirtualButtons <= (deviceInfo->NumberOfVirtualButtons + deviceInfo->NumberOfButtons); NoVirtualButtons++) {  // skip the HoldButton
		ntStatus = IoOpenDeviceRegistryKey(pdo,
				                           PLUGPLAY_REGKEY_DEVICE,
				                           KEY_QUERY_VALUE,
					                       &handle
						                   );

		if (!NT_SUCCESS(ntStatus)) {
			DebugPrint(("[acpihid.sys] ReadVirtualButtonInfoFromRegistry IoOpenDeviceRegistryKey failed status:0x%x\n", ntStatus));
			return ntStatus;
		}

		RtlInitUnicodeString(&valueName[NoVirtualButtons],  VirtualButton[NoVirtualButtons]);

		ntStatus = ZwQueryValueKey(
								 handle,
								 &valueName[NoVirtualButtons],
								 KeyValuePartialInformation,
								 (PVOID)keyValueInformation,
								 length,
								 &resultLength
								);

		if (STATUS_BUFFER_TOO_SMALL == ntStatus) {
			length = resultLength;
			keyValueInformation = ExAllocatePoolWithTag(
                                    PagedPool,
                                    length,
                                    VHID_POOL_TAG
                                    );

            if (!keyValueInformation) {
               DebugPrint(("[acpihid.sys] ReadVirtualButtonInfoFromRegistry Allocating %ld bytes to keyValueInformation failed\n", length));
               ntStatus = STATUS_UNSUCCESSFUL;
               goto query_end;
			}

		    ntStatus= ZwQueryValueKey(
									handle,
									&valueName[NoVirtualButtons] ,
									KeyValuePartialInformation ,
									(PVOID)keyValueInformation,
									length,
									&resultLength
									);

			if(!NT_SUCCESS(ntStatus)){
				DebugPrint(("[acpihid.sys] ReadVirtualButtonInfoFromRegistry ZwQueryValueKey (key=%ls) failed status:0x%x\n",
					VirtualButton[NoVirtualButtons], ntStatus));
				deviceInfo->VirtualButtons[NoVirtualButtons].KeyStatus = 0;
				ntStatus = STATUS_SUCCESS;  // -- it's ok to have missing virtual buttons
				goto query_end;  // -- if not there, it's ok, just skip it.
			}

	    	//
        	// Allocate memory for  button info
        	//
			RegistryVirtualButtonInfoLength = ((PKEY_VALUE_PARTIAL_INFORMATION) keyValueInformation)->DataLength;

			if (RegistryVirtualButtonInfoLength != sizeof(VIRTUAL_BUTTON_INFO)) {
				DebugPrint(("[acpihid.sys] ReadVirtualButtonInfoFromRegistry Invalid RegistryVirtualButtonInfoLength: 0x%lx, sizeof(VIRTUAL_BUTTON_INFO): 0x%lx\n",
					RegistryVirtualButtonInfoLength, sizeof(VIRTUAL_BUTTON_INFO)));
				goto query_end;
			}

        	//
        	// Copy  the button info from the registry
        	//

            RtlCopyMemory(&deviceInfo->VirtualButtons[NoVirtualButtons].InitialValue,
						  &((PKEY_VALUE_PARTIAL_INFORMATION)
						  keyValueInformation)->Data[0],
                          sizeof(UCHAR)
                         );

            RtlCopyMemory(&deviceInfo->VirtualButtons[NoVirtualButtons].KeyStatus,
						  &((PKEY_VALUE_PARTIAL_INFORMATION)
						  keyValueInformation)->Data[1],
                          sizeof(UCHAR)
                         );

			DebugPrint(("[acpihid.sys]    %ls: 0x%lx, 0x%lx\n",
						   VirtualButton[NoVirtualButtons],
						   deviceInfo->VirtualButtons[NoVirtualButtons].InitialValue,
						   deviceInfo->VirtualButtons[NoVirtualButtons].KeyStatus));

			// RtlFreeUnicodeString(&valueName);
		 } else {
         	if(!NT_SUCCESS(ntStatus)) {
				DebugPrint(("[acpihid.sys] ReadVirtualButtonInfoFromRegistry ZwQueryValueKey (key=%ls) failed status:0x%x\n",
					VirtualButton[NoVirtualButtons], ntStatus));
				ntStatus = STATUS_SUCCESS;  // -- it's ok to have missing virtual buttons
			}
		 }

    query_end:

    	ZwClose(handle);

    	if(keyValueInformation)
        	ExFreePool(keyValueInformation);

    	length = 0;
    	resultLength = 0;
    	RegistryVirtualButtonInfoLength = 0;
    	RegistryVirtualButtonInfo = NULL;
    	keyValueInformation = NULL;

	} // End of for loop

    return ntStatus;
}


NTSTATUS
InitializeVirtualButtonsFromRegistry(
        IN PDEVICE_OBJECT  DeviceObject
        )
/*++

Routine Description:

    Read HID report descriptor from registry

Arguments:

    DeviceObject - pointer to a device object.

Return Value:

    NT status code.

--*/
{
    NTSTATUS             ntStatus = STATUS_SUCCESS;
    PDEVICE_OBJECT       pdo;
    PDEVICE_EXTENSION    deviceInfo;
    ULONG                NoVirtualButtons = 0;

	PAGED_CODE();

    //
    // Get physical device object
    //

    pdo = GET_PHYSICAL_DEVICE_OBJECT(DeviceObject);

    //
    // Get the hid mini driver device extension
    //

    deviceInfo = GET_MINIDRIVER_DEVICE_EXTENSION(DeviceObject);

    //
    // Go through all the (present) virutal buttons and initialize them if we're asked to.
    // KeyStatus bitmask semantics:
    //   * bit 7 - 2: unused
    //   * bit 1: please initialize
    //   * bit 0: virtual button is present
    //

	DebugPrint(("[acpihid.sys] Virtual Button Initialization\n"));
	for (NoVirtualButtons = 1; NoVirtualButtons <= (deviceInfo->NumberOfVirtualButtons + deviceInfo->NumberOfButtons); NoVirtualButtons++) {  // skip the HoldButton
		if ((deviceInfo->VirtualButtons[NoVirtualButtons].KeyStatus & 0x01) == 0x01) {   // check if present
			if ((deviceInfo->VirtualButtons[NoVirtualButtons].KeyStatus & 0x02) == 0x02) {  // check if initialize request
				// yes, let's initialize it.
				DebugPrint(("[acpihid.sys]    Initializing VirtualButton%ld: 0x%lx\n",
					NoVirtualButtons,
					deviceInfo->VirtualButtons[NoVirtualButtons].InitialValue));
				ntStatus = AcpiButtonCtrl(deviceInfo,
					NoVirtualButtons,
					deviceInfo->VirtualButtons[NoVirtualButtons].InitialValue);
			} else {
				// no, skip initialization.
				DebugPrint(("[acpihid.sys]    Skipping VirtualButton%ls because status bit 1 'Initialize' not set.\n",
					NoVirtualButtons));
			}
		} else {
			// no, skip initialization.
			DebugPrint(("[acpihid.sys]    Skipping VirtualButton%ls because status bit 0 'Present' not set.\n",
				NoVirtualButtons));
		}
	} // End of for loop

    return ntStatus;
}

