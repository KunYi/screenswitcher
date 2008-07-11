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
// Module Name: Main.c                                                        //
//                                                                            //
// Abstract: Driver Entry routine                                             //
//                                                                            //
// Authors: Babu Pallithanam                                                  //
//          Jim Trethewey                                                     //
//                                                                            //
// Environment: kernel mode only                                              //
//                                                                            //
// Revision History:                                                          //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////


#include "Main.h"


#ifdef ALLOC_PRAGMA
    #pragma alloc_text( INIT,  DriverEntry )
#endif


/*++
NTSTATUS
DriverEntry (
    IN PDRIVER_OBJECT  DriverObject,
    IN PUNICODE_STRING RegistryPath
    )

Routine Description:
    DriverEntry is the first routine called after a driver is loaded,
	and is responsible for initializing the driver.

	Installable driver initialization entry point.
    This entry point is called directly by the I/O system.

Arguments:

    DriverObject - pointer to the driver object

    RegistryPath - pointer to a unicode string representing the path,
                   to driver-specific key in the registry.

Return Value:

    STATUS_SUCCESS if successful,
    STATUS_UNSUCCESSFUL otherwise.

--*/

NTSTATUS
DriverEntry (
    IN PDRIVER_OBJECT  DriverObject,
    IN PUNICODE_STRING RegistryPath
    )
{
    NTSTATUS                      ntStatus;
    HID_MINIDRIVER_REGISTRATION   hidMinidriverRegistration;

// TODO: print out registry path by doing rtlunicode to ansi string function which allocates
// and then calling rtlansi free string function
	DebugPrint(("[acpihid.sys] Enter DriverEntry( DriverObject=0x%lx, RegistryPath=0x%lx )\n", DriverObject, RegistryPath ));

// Set up entry points for the driver's standard routines
    DriverObject->MajorFunction[IRP_MJ_CREATE]					=
    DriverObject->MajorFunction[IRP_MJ_CLOSE]			        = AcpiHidCreateClose;

    DriverObject->MajorFunction[IRP_MJ_INTERNAL_DEVICE_CONTROL] = AcpiHidInternalIoctl;
    DriverObject->MajorFunction[IRP_MJ_PNP]	                    = AcpiHidDispatchPnP;
    DriverObject->MajorFunction[IRP_MJ_POWER]                   = AcpiHidDispatchPower ;
//	DriverObject->MajorFunction[IRP_MJ_CLEANUP]                 = IrpCleanUp;

    DriverObject->DriverUnload                                  = AcpiHidUnload;
    DriverObject->DriverExtension->AddDevice                    = AcpiHidAddDevice;

    RtlZeroMemory( &hidMinidriverRegistration, sizeof( hidMinidriverRegistration ));

//
// Revision must be set to HID_REVISION by the minidriver
//
    hidMinidriverRegistration.Revision            = HID_REVISION;
    hidMinidriverRegistration.DriverObject        = DriverObject;
    hidMinidriverRegistration.RegistryPath        = RegistryPath;
    hidMinidriverRegistration.DeviceExtensionSize = sizeof( DEVICE_EXTENSION );


// Disabled the poll option for  the hidclass driver
    hidMinidriverRegistration.DevicesArePolled    = FALSE;


//
// Register with hidclass
//

    ntStatus = HidRegisterMinidriver(&hidMinidriverRegistration);

    if( !NT_SUCCESS( ntStatus ))
    {
        DebugPrint(("[acpihid.sys] HidRegisterMinidriver FAILED, returnCode=%x\n",
                  ntStatus));
    }

    DebugPrint(("[acpihid.sys] Exit DriverEntry() status=0x%x\n", ntStatus));

    return ntStatus;
}
