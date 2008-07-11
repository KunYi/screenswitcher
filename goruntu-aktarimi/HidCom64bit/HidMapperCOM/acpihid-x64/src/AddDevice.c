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
// Module Name: AddDevice.c                                                   //
//                                                                            //
// Abstract: AddDevice, locate ACPI device object                             //
//                                                                            //
// Authors: Babu Pallithanam                                                  //
//          Jim Trethewey                                                     //
//                                                                            //
// Environment: kernel mode only                                              //
//                                                                            //
// Revision History:                                                          //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////


#include "main.h"
#include "Acpi.h"

#define HBTN_POOL_TAG          'ntbH'


PDEVICE_OBJECT GetACPIDeviceObject(
	IN PDEVICE_OBJECT DeviceObject
	);

#ifdef ALLOC_PRAGMA
    #pragma alloc_text( PAGE,  GetACPIDeviceObject )
    #pragma alloc_text( PAGE,  AcpiHidAddDevice )
#endif


PDEVICE_OBJECT GetACPIDeviceObject( IN PDEVICE_OBJECT DeviceObject )
{
    UNICODE_STRING      deviceName;
	PDEVICE_OBJECT      pReturn;
    BOOLEAN             found;

	PAGED_CODE();

//    DbgBreakPoint();
    RtlInitUnicodeString(&deviceName, NULL);

	deviceName.MaximumLength = sizeof( L"\\Driver\\ACPI" ) + sizeof( UNICODE_NULL );
    deviceName.Buffer = ExAllocatePool( PagedPool, deviceName.MaximumLength);

	RtlZeroMemory( deviceName.Buffer, deviceName.MaximumLength);
    RtlAppendUnicodeToString( &deviceName, L"\\Driver\\ACPI");

	pReturn = DeviceObject;
    found = FALSE;
//
// this code just walks the device objects and looks for the one whose driver object is
// \Driver\ACPI  This is the driver we want as our "lower" device object
// we should never get into a situation where this loops endlessly if we are just walking
// the attached device stack, but just in case
//

    while ( NULL != pReturn )
	{
        if ( pReturn->DeviceType == FILE_DEVICE_ACPI )
        {
            PDRIVER_OBJECT p = pReturn->DriverObject;
            if ( p && ( 0 == RtlCompareUnicodeString( &p->DriverName, &deviceName, TRUE ) ))
            {
				found = TRUE;
				break;
            }
        }

        if ( NULL != pReturn->DeviceExtension )
        {
            pReturn = GET_NEXT_DEVICE_OBJECT( pReturn );
        }
        else
        {
            break;
        }
    }

	if ( deviceName.Buffer != 0 )
	{
        ExFreePool ( deviceName.Buffer );
    }

	if ( FALSE == found )
	{
        DebugPrint(( "[acpihid.sys] GetACPIDeviceObject: Error in obtaining ACPI device object\n" ));
	    return DeviceObject;
	}

	return pReturn;
}


NTSTATUS
AcpiHidAddDevice (
    IN PDRIVER_OBJECT DriverObject,
    IN PDEVICE_OBJECT FunctionalDeviceObject
    )
/*++

Routine Description:

    HidClass Driver calls our AddDevice routine after creating an FDO for us.
    We do not need to create a device object or attach it to the PDO.
    Hidclass driver will do it for us.

Arguments:

    DriverObject - pointer to the driver object.

    FunctionalDeviceObject -  pointer to the FDO created by the
                            Hidclass driver for us.

Return Value:

    NT status code.

--*/
{
    NTSTATUS            ntStatus = STATUS_SUCCESS;
    PDEVICE_EXTENSION   deviceInfo;

    deviceInfo = GET_MINIDRIVER_DEVICE_EXTENSION( FunctionalDeviceObject );

//
// Initialize all the members of device extension
//

    RtlZeroMemory( (PVOID)deviceInfo, sizeof( DEVICE_EXTENSION ));
    deviceInfo->LowerDevObj = GetACPIDeviceObject( GET_NEXT_DEVICE_OBJECT( FunctionalDeviceObject ));
    DebugPrint(( "[acpihid.sys] Enter AcpiHidAddDevice: deviceInfo->LowerDevObj = 0x%x\n", deviceInfo->LowerDevObj ));

    IoInitializeRemoveLock( &deviceInfo->RemoveLock, HBTN_POOL_TAG, 0, 10 );

// Used for ReadReport
    KeInitializeSpinLock( &deviceInfo->SpinLock );
    InitializeListHead( &deviceInfo->PendingIrpList );

    ntStatus = GetAcpiInterfaces( FunctionalDeviceObject, &deviceInfo->AcpiInterface );

//
// Set the initial state of the FDO
//

    INITIALIZE_PNP_STATE( deviceInfo );

    FunctionalDeviceObject->Flags &= ~DO_DEVICE_INITIALIZING;

    DebugPrint(("[acpihid.sys] Exit AcpiHidAddDevice, status = %lx\n", ntStatus));
    return ntStatus;
}
