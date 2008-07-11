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
// Module Name: DispatchPnp.c                                                 //
//                                                                            //
// Abstract: Plug and Play routines, ACPI event callback registration         //
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
#include "Registry.h"
#include "Acpi.h"
#include "MapButtons.h"


extern HID_DESCRIPTOR DefaultHidDescriptor;
extern HID_REPORT_DESCRIPTOR  DefaultReportDescriptor[];


NTSTATUS
PnPComplete (
    IN PDEVICE_OBJECT   DeviceObject,
    IN PIRP             Irp,
    IN PVOID            Context
    );

VOID ECEventHandler(
    IN PVOID  Context,
    IN ULONG  NotifyValue
    );

NTSTATUS RegisterForButtonEvent(
    PDEVICE_EXTENSION pdx,
    PVOID buf,
    ULONG size,
    KPROCESSOR_MODE mode
    );

NTSTATUS UnregisterForButtonEvent(
    PDEVICE_EXTENSION pdx
    );

PCHAR
PnPMinorFunctionString (
	UCHAR MajorFunction,
    UCHAR MinorFunction
    );

PCHAR
PnPMajorFunctionString (
    UCHAR MajorFunction
    );


#ifdef ALLOC_PRAGMA
    #pragma alloc_text( PAGE,  PnPMinorFunctionString )
    #pragma alloc_text( PAGE,  PnPMajorFunctionString )
    #pragma alloc_text( NONPAGED,  PnPComplete )
    #pragma alloc_text( NONPAGED,  ECEventHandler )
    #pragma alloc_text( PAGE,  RegisterForButtonEvent )
    #pragma alloc_text( PAGE,  UnregisterForButtonEvent )
    #pragma alloc_text( PAGE,  AcpiHidDispatchPnP )
#endif


#if DBG

PCHAR
PnPMajorFunctionString (
    UCHAR MajorFunction
    )
{
	PAGED_CODE();

    switch (MajorFunction)
    {
    case IRP_MJ_CREATE:
        return "IRP_MJ_CREATE";
    case IRP_MJ_CREATE_NAMED_PIPE:
        return "IRP_MJ_CREATE_NAMED_PIPE";
    case IRP_MJ_CLOSE:
        return "IRP_MJ_CLOSE";
    case IRP_MJ_READ:
        return "IRP_MJ_READ";
    case IRP_MJ_WRITE:
        return "IRP_MJ_WRITE";
    case IRP_MJ_QUERY_INFORMATION:
        return "IRP_MJ_QUERY_INFORMATION";
    case IRP_MJ_SET_INFORMATION:
        return "IRP_MJ_SET_INFORMATION";
    case IRP_MJ_QUERY_EA:
        return "IRP_MJ_QUERY_EA";
    case IRP_MJ_SET_EA:
        return "IRP_MJ_SET_EA";
    case IRP_MJ_FLUSH_BUFFERS:
        return "IRP_MJ_FLUSH_BUFFERS";
    case IRP_MJ_QUERY_VOLUME_INFORMATION:
        return "IRP_MJ_QUERY_VOLUME_INFORMATION";
    case IRP_MJ_SET_VOLUME_INFORMATION:
        return "IRP_MJ_SET_VOLUME_INFORMATION";
    case IRP_MJ_DIRECTORY_CONTROL:
        return "IRP_MJ_DIRECTORY_CONTROL";
    case IRP_MJ_FILE_SYSTEM_CONTROL:
        return "IRP_MJ_FILE_SYSTEM_CONTROL";
    case IRP_MJ_DEVICE_CONTROL:
        return "IRP_MJ_DEVICE_CONTROL";
    case IRP_MJ_INTERNAL_DEVICE_CONTROL:
        return "IRP_MJ_INTERNAL_DEVICE_CONTROL";
    case IRP_MJ_SHUTDOWN:
        return "IRP_MJ_SHUTDOWN";
    case IRP_MJ_LOCK_CONTROL:
        return "IRP_MJ_LOCK_CONTROL";
    case IRP_MJ_CLEANUP:
        return "IRP_MJ_CLEANUP";
    case IRP_MJ_CREATE_MAILSLOT:
        return "IRP_MJ_CREATE_MAILSLOT";
    case IRP_MJ_QUERY_SECURITY:
        return "IRP_MJ_QUERY_SECURITY";
    case IRP_MJ_SET_SECURITY:
        return "IRP_MJ_SET_SECURITY";
    case IRP_MJ_POWER:
        return "IRP_MJ_POWER";
    case IRP_MJ_SYSTEM_CONTROL:
        return "IRP_MJ_SYSTEM_CONTROL";
    case IRP_MJ_DEVICE_CHANGE:
        return "IRP_MJ_DEVICE_CHANGE";
    case IRP_MJ_QUERY_QUOTA:
        return "IRP_MJ_QUERY_QUOTA";
    case IRP_MJ_SET_QUOTA:
        return "IRP_MJ_SET_QUOTA";
    case IRP_MJ_PNP:
        return "IRP_MJ_PNP";
    default:
        return "(Unknown PnP IRP Major)";
    }
}

PCHAR
PnPMinorFunctionString (
	UCHAR MajorFunction,
    UCHAR MinorFunction
    )
{
	PAGED_CODE();

    switch (MajorFunction)
    {
    case IRP_MJ_POWER:
		switch (MinorFunction)
		{
		case IRP_MN_WAIT_WAKE:
			return "IRP_MN_WAIT_WAKE";
		case IRP_MN_POWER_SEQUENCE:
			return "IRP_MN_POWER_SEQUENCE";
		case IRP_MN_SET_POWER:
			return "IRP_MN_SET_POWER";
		case IRP_MN_QUERY_POWER:
			return "IRP_MN_QUERY_POWER";
		default:
			return "(Unknown PnP IRP Minor)";
		}
    case IRP_MJ_SYSTEM_CONTROL:
		switch (MinorFunction)
		{
		case IRP_MN_QUERY_ALL_DATA:
			return "IRP_MN_QUERY_ALL_DATA";
		case IRP_MN_QUERY_SINGLE_INSTANCE:
			return "IRP_MN_QUERY_SINGLE_INSTANCE";
		case IRP_MN_CHANGE_SINGLE_INSTANCE:
			return "IRP_MN_CHANGE_SINGLE_INSTANCE";
		case IRP_MN_CHANGE_SINGLE_ITEM:
			return "IRP_MN_CHANGE_SINGLE_ITEM";
		case IRP_MN_ENABLE_EVENTS:
			return "IRP_MN_ENABLE_EVENTS";
		case IRP_MN_DISABLE_EVENTS:
			return "IRP_MN_DISABLE_EVENTS";
		case IRP_MN_ENABLE_COLLECTION:
			return "IRP_MN_ENABLE_COLLECTION";
		case IRP_MN_DISABLE_COLLECTION:
			return "IRP_MN_DISABLE_COLLECTION";
		case IRP_MN_REGINFO:
			return "IRP_MN_REGINFO";
		case IRP_MN_EXECUTE_METHOD:
			return "IRP_MN_EXECUTE_METHOD";
		case IRP_MN_REGINFO_EX:
			return "IRP_MN_REGINFO_EX";
		default:
			return "(Unknown PnP IRP Minor)";
		}
	default:
		switch (MinorFunction)
		{
		case IRP_MN_START_DEVICE:
			return "IRP_MN_START_DEVICE";
		case IRP_MN_QUERY_REMOVE_DEVICE:
			return "IRP_MN_QUERY_REMOVE_DEVICE";
		case IRP_MN_REMOVE_DEVICE:
			return "IRP_MN_REMOVE_DEVICE";
		case IRP_MN_CANCEL_REMOVE_DEVICE:
			return "IRP_MN_CANCEL_REMOVE_DEVICE";
		case IRP_MN_STOP_DEVICE:
			return "IRP_MN_STOP_DEVICE";
		case IRP_MN_QUERY_STOP_DEVICE:
			return "IRP_MN_QUERY_STOP_DEVICE";
		case IRP_MN_CANCEL_STOP_DEVICE:
			return "IRP_MN_CANCEL_STOP_DEVICE";
		case IRP_MN_QUERY_DEVICE_RELATIONS:
			return "IRP_MN_QUERY_DEVICE_RELATIONS";
		case IRP_MN_QUERY_INTERFACE:
			return "IRP_MN_QUERY_INTERFACE";
		case IRP_MN_QUERY_CAPABILITIES:
			return "IRP_MN_QUERY_CAPABILITIES";
		case IRP_MN_QUERY_RESOURCES:
			return "IRP_MN_QUERY_RESOURCES";
		case IRP_MN_QUERY_RESOURCE_REQUIREMENTS:
			return "IRP_MN_QUERY_RESOURCE_REQUIREMENTS";
		case IRP_MN_QUERY_DEVICE_TEXT:
			return "IRP_MN_QUERY_DEVICE_TEXT";
		case IRP_MN_FILTER_RESOURCE_REQUIREMENTS:
			return "IRP_MN_FILTER_RESOURCE_REQUIREMENTS";
		case IRP_MN_READ_CONFIG:
			return "IRP_MN_READ_CONFIG";
		case IRP_MN_WRITE_CONFIG:
			return "IRP_MN_WRITE_CONFIG";
		case IRP_MN_EJECT:
			return "IRP_MN_EJECT";
		case IRP_MN_SET_LOCK:
			return "IRP_MN_SET_LOCK";
		case IRP_MN_QUERY_ID:
			return "IRP_MN_QUERY_ID";
		case IRP_MN_QUERY_PNP_DEVICE_STATE:
			return "IRP_MN_QUERY_PNP_DEVICE_STATE";
		case IRP_MN_QUERY_BUS_INFORMATION:
			return "IRP_MN_QUERY_BUS_INFORMATION";
		case IRP_MN_DEVICE_USAGE_NOTIFICATION:
			return "IRP_MN_DEVICE_USAGE_NOTIFICATION";
		case IRP_MN_SURPRISE_REMOVAL:
			return "IRP_MN_SURPRISE_REMOVAL";
		default:
			return "(Unknown PnP IRP Minor)";
		}
    }
}
#endif



NTSTATUS
PnPComplete (
    IN PDEVICE_OBJECT   DeviceObject,
    IN PIRP             Irp,
    IN PVOID            Context
    )
{

	NTSTATUS status;
    UNREFERENCED_PARAMETER (DeviceObject);

	DebugPrint(("[acpihid.sys] PnPComplete Entry: DeviceObject=0x%lx, Irp=0x%lx, Context=0x%lx\n", DeviceObject, Irp, Context));

    if ( Irp->PendingReturned )
	{
        KeSetEvent ((PKEVENT) Context, 0, FALSE);
    }

	status = STATUS_MORE_PROCESSING_REQUIRED;
	DebugPrint(("[acpihid.sys] PnPComplete Exit: 0x%lx\n", status));
	return status;
}



/********************************************************

*   Name                :ECEventHandler()

*   Description         :This function is used to handle the
*                        notify values for button presses.
*
*   Input Parameters    :IN PVOID	Context
*                        IN ULONG	NotifyValue

*   Returns             :VOID
*********************************************************/

VOID ECEventHandler(
    IN PVOID  Context,
    IN ULONG  NotifyValue
    )
{
    PDEVICE_EXTENSION pdx = (PDEVICE_EXTENSION) Context;
    ULONG i;
    DrvEventMap* pEventMap = pdx->pGPIOEventMap;

    if ( ! pEventMap )
    {
        return;
    }

    DebugPrint(("[acpihid.sys] Entering ECEventHandler: NotifyValue %d eventsize %d\n", NotifyValue, pdx->cbEventSize));
    StartButtonThread( pdx );

    for (i = 0; i < pdx->cbEventSize; i++) {
        if ( pEventMap->id == NotifyValue )
        {
            if ( pEventMap->PKEvent )
            {
                KeSetEvent(pEventMap->PKEvent, IO_NO_INCREMENT, FALSE);
            }
            return;
        }
        ++pEventMap;
    }
    return;
}
// */

/********************************************************

*   Name               : RegisterForButtonEvent()
*
*   Description        : This function is used to register for
*                        button events.
*
*   Input Parameters   : PFDO_DATA pdx
*                        PVOID buf
*                        ULONG size
*                        KPROCESSOR_MODE mode

*   Returns             :NTSTATUS
*********************************************************/


NTSTATUS RegisterForButtonEvent(
    PDEVICE_EXTENSION pdx,
    PVOID buf,
    ULONG size,
    KPROCESSOR_MODE mode
    )
{
	NTSTATUS status = STATUS_SUCCESS;
	ULONG eventsize = 0;
	AppEventMap  *pAppEvent;
	DrvEventMap  *pDrvEventMap;
	ULONG i;

	PAGED_CODE();

    DebugPrint(("[acpihid.sys] RegisterForButtonEvent Entry: pdx = 0x%lx\n", pdx));

	eventsize = size / sizeof(AppEventMap);
	pAppEvent = (AppEventMap*) buf;

    if ( size - eventsize * sizeof(AppEventMap))
    {
        status = STATUS_INVALID_DEVICE_REQUEST;
        DebugPrint(("[acpihid.sys] RegisterForButtonEvent: invalid map size (%x)\n", size));
		return STATUS_INVALID_PARAMETER;
	}

    pdx->pGPIOEventMap = (DrvEventMap*) ExAllocatePool( PagedPool, (eventsize + 1) * sizeof( DrvEventMap ));
    pdx->cbEventSize = eventsize;
    pDrvEventMap = pdx->pGPIOEventMap;

	for ( i = 0; i < eventsize; ++i )
    {
        pDrvEventMap->id = pAppEvent->id;
        status = ObReferenceObjectByHandle(
            pAppEvent->hEventHandle,
            EVENT_MODIFY_STATE,
            *ExEventObjectType,
            mode,
            (PVOID*) &pDrvEventMap->PKEvent,
            NULL
            );

        if ( STATUS_SUCCESS != status )
        {
            DebugPrint(("[acpihid.sys] RegisterForButtonEvent: ObReferenceObjectByHandle failed (%x)\n", status));
            ExFreePool(pdx->pGPIOEventMap);
            pdx->cbEventSize = 0;
            return status;
        }

        if ( 0 == pDrvEventMap->id )
        {
            pdx->sleepevent = pDrvEventMap->PKEvent;
        }

        ++pDrvEventMap;
        ++pAppEvent;
    }

    status = pdx->AcpiInterface.RegisterForDeviceNotifications(
        pdx->LowerDevObj,
        ECEventHandler,
        pdx
        );

    if ( ! NT_SUCCESS( status ))
    {
        DebugPrint(("[acpihid.sys] RegisterForButtonEvent: RegisterForDeviceNotifications failure (%lx)\n", status));
        ExFreePool(pdx->pGPIOEventMap);
        pdx->cbEventSize = 0;
        return status;
    }

    DebugPrint(("[acpihid.sys] RegisterForButtonEvent: success\n"));
    return status;
}
// */


/********************************************************

*   Name                :UnregisterEvent()
*
*   Description         :This function is used to unregister
*                        button events.
*
*   Input Parameters    :PFDO_DATA pdx
*
*   Returns             :NTSTATUS
*********************************************************/

NTSTATUS UnregisterForButtonEvent( PDEVICE_EXTENSION pdx )
{
    NTSTATUS     status = STATUS_SUCCESS;
    ULONG        i      = 0;
    DrvEventMap* pDrvEventMap;

	PAGED_CODE();

    DebugPrint(("[acpihid.sys] UnregisterForButtonEvent Entry: pdx = 0x%lx\n", pdx));

    pdx->AcpiInterface.UnregisterForDeviceNotifications(
        pdx->LowerDevObj,
        ECEventHandler
        );

    if ( ! NT_SUCCESS( status ))
    {
        DebugPrint(("[acpihid.sys] UnregisterForButtonEvent failed (0x%x)\n", status));
    }

    pDrvEventMap = pdx->pGPIOEventMap;

    for ( i = 0; i < pdx->cbEventSize; ++i )
    {
        ObDereferenceObject( pDrvEventMap->PKEvent );
		++pDrvEventMap;
    }

    if ( NULL != pdx->pGPIOEventMap )
    {
        ExFreePool( pdx->pGPIOEventMap );
    }

	pdx->pGPIOEventMap = NULL;
	pdx->cbEventSize = 0;
	pdx->sleepevent = NULL;

	DebugPrint(("[acpihid.sys] UnregisterForButtonEvent success\n"));
	return 0;
}
// */


NTSTATUS
AcpiHidDispatchPnP (
    IN PDEVICE_OBJECT DeviceObject,
    IN PIRP Irp
    )
/*++
Routine Description:

    Handles PnP Irps sent to FDO .

Arguments:

    DeviceObject - Pointer to deviceobject
    Irp          - Pointer to a PnP Irp.

Return Value:

    NT Status is returned.
--*/
{
    NTSTATUS              ntStatus = STATUS_SUCCESS;
	NTSTATUS              queryStatus;
    PDEVICE_EXTENSION     deviceInfo;
    KEVENT                startEvent;
    PIO_STACK_LOCATION    IrpStack;
    PIO_STACK_LOCATION    previousSp;
    PDEVICE_CAPABILITIES  deviceCapabilities;
    PWCHAR                buffer;


	PAGED_CODE();

//
// Get a pointer to the device extension
//
    deviceInfo = GET_MINIDRIVER_DEVICE_EXTENSION( DeviceObject );

//
// Get a pointer to the current location in the Irp
//
    IrpStack = IoGetCurrentIrpStackLocation (Irp);

	DebugPrint(("[acpihid.sys] Entering AcpiHidDispatchPnP: Major(0x%x)=%s Minor(0x%x)=%s, DeviceObject=0x%lx, Irp=0x%x\n",
        IrpStack->MajorFunction,
        PnPMajorFunctionString(IrpStack->MajorFunction),
        IrpStack->MinorFunction,
        PnPMinorFunctionString(IrpStack->MajorFunction, IrpStack->MinorFunction),
        DeviceObject,
        Irp ));

    switch(IrpStack->MinorFunction)
    {
    case IRP_MN_START_DEVICE:

        KeInitializeEvent(&startEvent, NotificationEvent, FALSE);

        IoCopyCurrentIrpStackLocationToNext (Irp);

        IoSetCompletionRoutine(
            Irp,
            PnPComplete,
            &startEvent,
            TRUE,
            TRUE,
            TRUE
            );

        ntStatus = IoCallDriver( GET_NEXT_DEVICE_OBJECT( DeviceObject ), Irp );

        if ( STATUS_PENDING == ntStatus )
        {
            KeWaitForSingleObject(
                &startEvent,
                Executive,
                KernelMode,
                FALSE, // No alert
                NULL ); // No timeout
            ntStatus = Irp->IoStatus.Status;
        }


//
// Use default "HID Descriptor" (hardcoded). We will set the
// wReportLength member of HID descriptor when we read the
// the report descriptor either from registry or the hard-coded
// one.
//
//
// We need to read read descriptor from registry
//
        if ( NT_SUCCESS( ntStatus ))
        {
            deviceInfo->HidDescriptor = DefaultHidDescriptor;
            queryStatus = CheckRegistryForDescriptor(DeviceObject);

            if ( NT_SUCCESS( queryStatus ))
            {
                queryStatus = ReadDescriptorFromRegistry(DeviceObject);

                if ( ! NT_SUCCESS( queryStatus ))
                {
                    DebugPrint(("[acpihid.sys] AcpiHidDispatchPnP IRP_MN_START_DEVICE Failed to read descriptor from registry\n"));
                    ntStatus = STATUS_UNSUCCESSFUL;
                }
                else
                {
                    DebugPrint(("[acpihid.sys] AcpiHidDispatchPnP IRP_MN_START_DEVICE Read descriptor from registry success\n"));
                }
            }
            else
            {
                deviceInfo->ReportDescriptor = DefaultReportDescriptor;
                DebugPrint(("[acpihid.sys] AcpiHidDispatchPnP IRP_MN_START_DEVICE Using Hard-coded Report descriptor\n"));
            }

//
// Reading number of buttons, virtual buttons, and LEDs from registry
// We need to read button HID mapping infromation from registry or
// We will use hard-coded Button count, if registry access fails.
//

// Buttons
            queryStatus = CheckNumberOfButtonsFromRegistry(DeviceObject);
            DebugPrint(("[acpihid.sys] AcpiHidDispatchPnP IRP_MN_START_DEVICE CheckNumberOfButtonsFromRegistry: deviceInfo->NumberOfButtons: %d\n",deviceInfo->NumberOfButtons));

            if ( NT_SUCCESS( queryStatus ))
            {
                queryStatus = ReadButtonInfoFromRegistry(DeviceObject);
                if(!NT_SUCCESS(queryStatus)){
                    DebugPrint(("[acpihid.sys] AcpiHidDispatchPnP IRP_MN_START_DEVICE ReadButtonInfoFromRegistry Failed to read \n"));
                    ntStatus = STATUS_UNSUCCESSFUL;
                }
                else
                {
                    DebugPrint(("[acpihid.sys] AcpiHidDispatchPnP IRP_MN_START_DEVICE ReadButtonInfoFromRegistry success\n"));
                }
            }
            else
            {
                deviceInfo->NumberOfButtons = MAX_NO_BUTTONS;
                DebugPrint(("[acpihid.sys] AcpiHidDispatchPnP IRP_MN_START_DEVICE CheckNumberOfButtonsFromRegistry failed \n"));
            }

// Virtual Buttons
            queryStatus = CheckNumberOfVirtualButtonsFromRegistry(DeviceObject);
            DebugPrint(("[acpihid.sys] AcpiHidDispatchPnP IRP_MN_START_DEVICE CheckNumberOfVirtualButtonsFromRegistry: deviceInfo->NumberOfVirtualButtons: %d\n",deviceInfo->NumberOfVirtualButtons));

            if ( NT_SUCCESS( queryStatus ))
            {
                queryStatus = ReadVirtualButtonInfoFromRegistry(DeviceObject);
                if(!NT_SUCCESS(queryStatus)){
                    DebugPrint(("[acpihid.sys] AcpiHidDispatchPnP IRP_MN_START_DEVICE ReadVirtualButtonInfoFromRegistry Failed to read \n"));
                    ntStatus = STATUS_UNSUCCESSFUL;
                }
                else
                {
                    DebugPrint(("[acpihid.sys] AcpiHidDispatchPnP IRP_MN_START_DEVICE ReadVirtualButtonInfoFromRegistry success\n"));
                    queryStatus = InitializeVirtualButtonsFromRegistry(DeviceObject);
                }
            }
            else
            {
                deviceInfo->NumberOfVirtualButtons = MAX_NO_VIRTUAL_BUTTONS;
                DebugPrint(("[acpihid.sys] AcpiHidDispatchPnP IRP_MN_START_DEVICE CheckNumberOfVirtualButtonsFromRegistry failed \n"));
            }

// LEDs
	        queryStatus = CheckNumberOfLedsFromRegistry(DeviceObject);
            DebugPrint(("[acpihid.sys] AcpiHidDispatchPnP IRP_MN_START_DEVICE CheckNumberOfLedsFromRegistry: deviceInfo->NumberOfLeds: %d\n",deviceInfo->NumberOfLeds));

//
// We will use hard-coded LED count.
//
            if ( ! NT_SUCCESS( queryStatus ))
            {
                deviceInfo->NumberOfButtons = MAX_NO_LEDS;
                DebugPrint(("[acpihid.sys] AcpiHidDispatchPnP IRP_MN_START_DEVICE CheckNumberOfLedsFromRegistry failed \n"));
            }

            queryStatus = PerformLedLightShow( deviceInfo, 1 );
            if ( ! NT_SUCCESS( queryStatus ))
            {
                DebugPrint(("[acpihid.sys] AcpiHidDispatchPnP IRP_MN_START_DEVICE PerformLedLightShow failed with status = 0x%lx\n", queryStatus));
            }

//
// Set new PnP state
//
            if ( NT_SUCCESS( ntStatus ))
			{
                DebugPrint(("[acpihid.sys] AcpiHidDispatchPnP IRP_MN_START_DEVICE PnP Device started successfully\n"));
                SET_NEW_PNP_STATE(deviceInfo, Started);
            }
        }


// Initialize common varaibles
        deviceInfo->ButtonID            = 0x0;
        deviceInfo->ButtonDetectFlag    = 0x0;
        deviceInfo->TestButtonNo        = 0x0;
        deviceInfo->HidReadFlag         = 0x0;
        deviceInfo->PendingIrpCount     = 0x0;
        deviceInfo->RemovedDevice       = 0x0;
		deviceInfo->DeviceActivatedFlag = TRUE;  // Release "button hold".


// Registering the Button Event interface with the ACPI.

        ntStatus = RegisterForButtonEvent(
            deviceInfo,
            Irp->AssociatedIrp.SystemBuffer,
            IrpStack->Parameters.DeviceIoControl.InputBufferLength,
			Irp->RequestorMode );


        Irp->IoStatus.Status = ntStatus;
        IoCompleteRequest (Irp, IO_NO_INCREMENT);
        DebugPrint(("[acpihid.sys] AcpiHidDispatchPnP Exiting: ntStatus = %lx\n", ntStatus ));
        return ntStatus;

//
// Get the packet.
// Set the capabilities.
//
	case IRP_MN_QUERY_CAPABILITIES:
        deviceCapabilities=IrpStack->Parameters.DeviceCapabilities.Capabilities;

        deviceCapabilities->Version = 1;
        deviceCapabilities->Size = sizeof( DEVICE_CAPABILITIES );

// We cannot wake the system.
        deviceCapabilities->SystemWake = PowerSystemUnspecified;
        deviceCapabilities->DeviceWake = PowerDeviceUnspecified;

// We have no latencies
        deviceCapabilities->D1Latency = 0;
        deviceCapabilities->D2Latency = 0;
        deviceCapabilities->D3Latency = 0;

// No locking or ejection
        deviceCapabilities->LockSupported = FALSE;
        deviceCapabilities->EjectSupported = FALSE;

// Device can be physically removed.
// Technically there is no physical device to remove, but this bus
// driver can yank the PDO from the PlugPlay system, when ever it
// receives an IOCTL_GAMEENUM_REMOVE_PORT device control command.
        deviceCapabilities->Removable = FALSE;
        deviceCapabilities->SurpriseRemovalOK = TRUE;

// not Docking device
        deviceCapabilities->DockDevice = FALSE;

        deviceCapabilities->UniqueID = FALSE;
        ntStatus = STATUS_SUCCESS;

        Irp->IoStatus.Status = ntStatus;
        IoCompleteRequest( Irp, IO_NO_INCREMENT );
// Note: Irp cannot and should not be accessed after this point

        DebugPrint(("[acpihid.sys] AcpiHidDispatchPnP IRP_MN_QUERY_CAPABILITIES Exiting: ntStatus = 0x%lx\n", ntStatus));
        return ntStatus;

//
// Mark the device as stopped.
//
    case IRP_MN_STOP_DEVICE:
        SET_NEW_PNP_STATE(deviceInfo, Stopped);
        ntStatus = STATUS_SUCCESS;
        break;

    case IRP_MN_CANCEL_STOP_DEVICE:
        RESTORE_PREVIOUS_PNP_STATE(deviceInfo);
        ntStatus = STATUS_SUCCESS;
        break;

    case IRP_MN_QUERY_STOP_DEVICE:
        SET_NEW_PNP_STATE(deviceInfo, StopPending);
        ntStatus = STATUS_SUCCESS;
        break;


     case IRP_MN_QUERY_DEVICE_RELATIONS:
        DebugPrint(("[acpihid.sys] AcpiHidDispatchPnP IRP_MN_QUERY_DEVICE_RELATIONS IrpStack->Parameters.QueryDeviceRelations.Type: 0x%lx\n",
			IrpStack->Parameters.QueryDeviceRelations.Type));
        ntStatus = STATUS_SUCCESS;
        break;

     case IRP_MN_QUERY_REMOVE_DEVICE:
        deviceInfo->RemovedDevice = 1;
        SET_NEW_PNP_STATE(deviceInfo, RemovePending);

//        ntStatus = HidMiniAbortPendingRequests(DeviceObject);

// Registering the Button Event interface with the ACPI.
        ntStatus = UnregisterForButtonEvent( deviceInfo );

        ntStatus = STATUS_SUCCESS;

        break;

    case IRP_MN_CANCEL_REMOVE_DEVICE:
        RESTORE_PREVIOUS_PNP_STATE(deviceInfo);
        ntStatus = STATUS_SUCCESS;
        break;

    case IRP_MN_SURPRISE_REMOVAL:
        SET_NEW_PNP_STATE(deviceInfo, SurpriseRemovePending);
        ntStatus = STATUS_SUCCESS;
        break;

//
// DRIVER UNLOAD / CLEANUP ACTIVITIES NEED TO GO HERE
//
    case IRP_MN_REMOVE_DEVICE:
        if ( deviceInfo->ReadReportDescFromRegistry )
        {
            ExFreePool(deviceInfo->ReportDescriptor);
        }

        SET_NEW_PNP_STATE( deviceInfo, Deleted );
        ntStatus = STATUS_SUCCESS;
        break;

    case IRP_MN_QUERY_ID:
        DebugPrint(("[acpihid.sys] AcpiHidDispatchPnP IRP_MN_QUERY_ID: %lx \n",
            IrpStack->Parameters.QueryId.IdType));
        ntStatus = STATUS_SUCCESS;
        break;

    case IRP_MN_QUERY_PNP_DEVICE_STATE:
    default:
        ntStatus = Irp->IoStatus.Status;
        break;
    }

    Irp->IoStatus.Status = ntStatus;
    IoSkipCurrentIrpStackLocation( Irp );
    ntStatus =  IoCallDriver( GET_NEXT_DEVICE_OBJECT(DeviceObject), Irp );
// Note: Irp cannot and should not be accessed after this point

    DebugPrint(("[acpihid.sys] Exiting AcpiHidDispatchPnP: ntStatus = 0x%lx\n", ntStatus));
    return ntStatus;
}
