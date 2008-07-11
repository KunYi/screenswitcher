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
// Module Name: InternalIoctl.c                                               //
//                                                                            //
// Abstract: IOCTL routines, Read Report, HID Descriptor functions            //
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
#include "HidDescriptor.h"
#include "Acpi.h"
#include "MapButtons.h"


PCHAR
IoctlString(
	ULONG ControlCode
	);

NTSTATUS
ReadReport(
    IN PDEVICE_OBJECT DeviceObject,
    IN PIRP              Irp
    );

VOID
ReadReportCanceled(
    IN PDEVICE_OBJECT DevObj,
    IN PIRP           Irp
    );

NTSTATUS
GetReportDescriptor(
    IN PDEVICE_OBJECT DeviceObject,
    IN PIRP Irp
    );

NTSTATUS
GetHidDescriptor(
    IN PDEVICE_OBJECT DeviceObject,
    IN PIRP Irp
    );

NTSTATUS
GetDeviceAttributes(
    IN PDEVICE_OBJECT DeviceObject,
    IN PIRP Irp
    );

NTSTATUS
GetStringDescriptor(
    IN PDEVICE_OBJECT DeviceObject,
    IN PIRP Irp
    );


#ifdef ALLOC_PRAGMA
	#pragma alloc_text( PAGE,  IoctlString )
	#pragma alloc_text( PAGE,  GetStringDescriptor )
    #pragma alloc_text( NONPAGED,  ReadReportCanceled )
    #pragma alloc_text( NONPAGED,  ReadReport )
    #pragma alloc_text( PAGE,  GetReportDescriptor )
	#pragma alloc_text( PAGE,  GetDeviceAttributes )
	#pragma alloc_text( PAGE,  GetHidDescriptor )
    #pragma alloc_text( NONPAGED,  AcpiHidInternalIoctl )
#endif


#if DBG

PCHAR
IoctlString(
	ULONG ControlCode
	)
{
	PAGED_CODE();

	switch (ControlCode)
	{
    case IOCTL_HID_GET_DEVICE_DESCRIPTOR:
		return "IOCTL_HID_GET_DEVICE_DESCRIPTOR";

	case IOCTL_HID_GET_REPORT_DESCRIPTOR:
		return "IOCTL_HID_GET_REPORT_DESCRIPTOR";

	case IOCTL_HID_READ_REPORT:
		return "IOCTL_HID_READ_REPORT";

    case IOCTL_HID_GET_DEVICE_ATTRIBUTES:
		return "IOCTL_HID_GET_DEVICE_ATTRIBUTES";

	case IOCTL_HID_WRITE_REPORT:
		return "IOCTL_HID_WRITE_REPORT";

	case IOCTL_HID_GET_INPUT_REPORT:
		return "IOCTL_HID_GET_INPUT_REPORT";

	case IOCTL_HID_SET_OUTPUT_REPORT:
		return "IOCTL_HID_SET_OUTPUT_REPORT";

	case IOCTL_HID_SET_FEATURE:
		return "IOCTL_HID_SET_FEATURE";

	case IOCTL_HID_GET_FEATURE:
		return "IOCTL_HID_GET_FEATURE";

	case IOCTL_HID_GET_STRING:
        return "IOCTL_HID_GET_STRING";

	case IOCTL_HID_ACTIVATE_DEVICE:
        return "IOCTL_HID_ACTIVATE_DEVICE";

    case IOCTL_HID_DEACTIVATE_DEVICE:
        return "IOCTL_HID_DEACTIVATE_DEVICE";

	default:
        return "Unknown or unsupported IOCTL";
	}
}


#endif




NTSTATUS
GetStringDescriptor(
    IN PDEVICE_OBJECT DeviceObject,
    IN PIRP Irp
    )
/*++

Routine Description:

    Finds the HID string descriptor and copies it into the buffer provided by the Irp.

Arguments:

    DeviceObject - pointer to a device object.

    Irp - Pointer to Interrupt Request Packet.

Return Value:

    NT status code.

typedef struct _HID_TYPE3INPUTBUFFER {
	USHORT	hidStringId;
	USHORT	hidStringLanguage;
} HID_TYPE3INPUTBUFFER, *PHID_TYPE3INPUTBUFFER;

--*/
{
    NTSTATUS            ntStatus = STATUS_SUCCESS;
    PDEVICE_EXTENSION   deviceInfo;
    PIO_STACK_LOCATION  IrpStack;
    PHID_TYPE3INPUTBUFFER	pType3Buf;
    ULONG               bytesToCopy;
    ULONG				sourceByteCount;
    const wchar_t *		sourceBytePointer;
    ULONG				hidStringId;

	PAGED_CODE();

    DebugPrint(("[acpihid.sys] GetStringDescriptor Entry\n"));

    //
    // Get a pointer to the current location in the Irp
    //
    deviceInfo = GET_MINIDRIVER_DEVICE_EXTENSION(DeviceObject);
    IrpStack = IoGetCurrentIrpStackLocation(Irp);

    //
    // Get a pointer to the device extension
    //
    deviceInfo = GET_MINIDRIVER_DEVICE_EXTENSION(DeviceObject);

	//
    // Copy string descriptor to HIDCLASS buffer
    //
	DebugPrint(("[acpihid.sys] GetStringDescriptor Type3InputBuffer = %p \n", IrpStack->Parameters.DeviceIoControl.Type3InputBuffer));
    pType3Buf = (PHID_TYPE3INPUTBUFFER) &IrpStack->Parameters.DeviceIoControl.Type3InputBuffer;

    hidStringId = pType3Buf->hidStringId;
	if (hidStringId == HID_STRING_ID_IMANUFACTURER) {
	    DebugPrint(("[acpihid.sys] GetStringDescriptor HID_STRING_ID_IMANUFACTURER\n"));
		sourceBytePointer = String1Desc;
	} else if (hidStringId == HID_STRING_ID_IPRODUCT) {
	    DebugPrint(("[acpihid.sys] GetStringDescriptor HID_STRING_ID_IPRODUCT\n"));
		sourceBytePointer = String2Desc;
	} else if (hidStringId == HID_STRING_ID_ISERIALNUMBER) {
	    DebugPrint(("[acpihid.sys] GetStringDescriptor HID_STRING_ID_ISERIALNUMBER\n"));
		sourceBytePointer = String3Desc;
	} else {
	    DebugPrint(("[acpihid.sys] GetStringDescriptor %d STATUS_NOT_SUPPORTED\n", hidStringId));
		return STATUS_NOT_SUPPORTED;
	}
	// figure out byte-length of the wide-character string, and don't forget to copy the NULL at the end too.
	sourceByteCount = (wcslen(sourceBytePointer) + 1) * sizeof(wchar_t);

    //
    // Copy MIN (OutputBufferLength, StringDescriptor->bLength)
    //
    bytesToCopy = IrpStack->Parameters.DeviceIoControl.OutputBufferLength;
    if(!bytesToCopy) {
	    DebugPrint(("[acpihid.sys] GetStringDescriptor buffer too small (len=%d, need=%d)\n",
	    	bytesToCopy,
	    	sourceByteCount
	    	));
        return STATUS_BUFFER_TOO_SMALL;
    }

    //
    // Since HidDescriptor.bLength could be >= sizeof(HID_DESCRIPTOR) we
    // just check for StringDescriptor.bLength and
    // copy MIN (OutputBufferLength, StringDescriptor->bLength)
    //

    if (bytesToCopy > sourceByteCount) {
	    DebugPrint(("[acpihid.sys] GetStringDescriptor buffer too big, truncating (len=%d, need=%d)\n",
	    	bytesToCopy,
	    	sourceByteCount
	    	));
        bytesToCopy = sourceByteCount;
    }

    RtlCopyMemory((PUCHAR) Irp->UserBuffer,
		(PUCHAR) sourceBytePointer,
		bytesToCopy);

    //
    // Report how many bytes were copied
    //
    Irp->IoStatus.Information = bytesToCopy;
    return ntStatus;
}



/*++
    @doc    EXTERNAL

    @func   VOID | ReadReportCanceled |
            ReadReport IRP has been canceled, so do the clean up.

    @parm   IN PDEVICE_OBJECT | DevObj | Points to the device object.
    @parm   IN PIRP | Irp | Points to an I/O Request Packet.

    @rvalue None.
--*/

VOID
ReadReportCanceled(
    IN PDEVICE_OBJECT DevObj,
    IN PIRP           Irp
    )
{
    PDEVICE_EXTENSION devext;
    KIRQL OldIrql;

    DebugPrint(("[acpihid.sys] Entering ReadReportCanceled"));

    DebugPrint(("[acpihid.sys] (DevObj=%p, Irp=%p)\n", DevObj, Irp));

    devext = GET_MINIDRIVER_DEVICE_EXTENSION(DevObj);
    KeAcquireSpinLock(&devext->SpinLock, &OldIrql);
    RemoveEntryList(&Irp->Tail.Overlay.ListEntry);
    KeReleaseSpinLock(&devext->SpinLock, OldIrql);
    IoReleaseCancelSpinLock(Irp->CancelIrql);

    Irp->IoStatus.Status = STATUS_CANCELLED;
    IoReleaseRemoveLock(&devext->RemoveLock, Irp);
    IoCompleteRequest(Irp, IO_NO_INCREMENT);
    DebugPrint(("[acpihid.sys] ReadReport IRP was canceled.\n"));

    DebugPrint(("[acpihid.sys] Exiting  ReadReportCanceled\n"));
    return;
}       //ReadReportCanceled



NTSTATUS
ReadReport(
    IN PDEVICE_OBJECT DeviceObject,
    IN PIRP              Irp
    )
{
    PIO_STACK_LOCATION irpsp;
    NTSTATUS status		= STATUS_SUCCESS;
    PDEVICE_EXTENSION		DevExt;


    DevExt = GET_MINIDRIVER_DEVICE_EXTENSION(DeviceObject);
    irpsp = IoGetCurrentIrpStackLocation(Irp);

    DebugPrint(("[acpihid.sys] Entering ReadReport\n"));
    DebugPrint(("[acpihid.sys] ReadReport: DevExt=%p, Irp=%p, IrpSp=%p\n", DevExt, Irp, irpsp));

    if ( REPORT_BUF_SIZE != irpsp->Parameters.DeviceIoControl.OutputBufferLength )
    {
        status = STATUS_INVALID_BUFFER_SIZE;
        DebugPrint(("[acpihid.sys] ReadReport: invalid input report size (len=%d, expect=%d)\n",
                   irpsp->Parameters.DeviceIoControl.OutputBufferLength, REPORT_BUF_SIZE));
    }
    else
    {
        KIRQL OldIrql;
        PDRIVER_CANCEL OldCancelRoutine;

        KeAcquireSpinLock(&DevExt->SpinLock, &OldIrql);
        OldCancelRoutine = IoSetCancelRoutine(Irp, ReadReportCanceled);

//
// This IRP was canceled.  Do not queue it.
//
        if ( Irp->Cancel )
        {
            OldCancelRoutine = IoSetCancelRoutine(Irp, NULL);
//
// Cancel routine was not called if OldCancelRoutine is not NULL.
// Otherwise, cancel routine was called and it will complete this IRP
// as soon as we drop the spinlock.  Return PENDING so the
// caller doesn't touch this IRP.
//
            if ( NULL != OldCancelRoutine )
            {
                status = STATUS_CANCELLED;
            }
            else
            {
                InitializeListHead( &Irp->Tail.Overlay.ListEntry );
                IoMarkIrpPending( Irp );
                status = STATUS_PENDING;
            }
        }
        else
        {
            InsertTailList( &DevExt->PendingIrpList, &Irp->Tail.Overlay.ListEntry );
            IoMarkIrpPending( Irp );
            status = STATUS_PENDING;
            DevExt->ReadReportIrp = Irp;
        }

        KeReleaseSpinLock(&DevExt->SpinLock, OldIrql);
    }

    DebugPrint(("[acpihid.sys] Exiting ReadReport Status = %x\n", status));
    return status;
}       //ReadReport



NTSTATUS
GetReportDescriptor(
    IN PDEVICE_OBJECT DeviceObject,
    IN PIRP Irp
    )
/*++

Routine Description:

    Finds the Report descriptor and copies it into the buffer provided by the
    Irp.

Arguments:

    DeviceObject - pointer to a device object.

    Irp - Pointer to Interrupt Request Packet.

Return Value:

    NT status code.

--*/
{
    NTSTATUS            ntStatus = STATUS_SUCCESS;
    PDEVICE_EXTENSION   deviceInfo;
    PIO_STACK_LOCATION  IrpStack;
    ULONG               bytesToCopy;

	PAGED_CODE();

	DebugPrint(("[acpihid.sys] Enter GetReportDescriptor\n"));

    //
    // Get a pointer to the current location in the Irp
    //

	IrpStack = IoGetCurrentIrpStackLocation(Irp);

    //
    // Get a pointer to the device extension
    //

	deviceInfo = GET_MINIDRIVER_DEVICE_EXTENSION(DeviceObject);

    //
    // Copy device descriptor to HIDCLASS buffer
    //

	DebugPrint(("[acpihid.sys] GetReportDescriptor HIDCLASS Buffer = 0x%x, Buffer length = 0x%x\n",
        Irp->UserBuffer,
	    IrpStack->Parameters.DeviceIoControl.OutputBufferLength));


	bytesToCopy = IrpStack->Parameters.DeviceIoControl.OutputBufferLength;

	if (bytesToCopy >= deviceInfo->HidDescriptor.DescriptorList[0].wReportLength)
	{
		bytesToCopy = deviceInfo->HidDescriptor.DescriptorList[0].wReportLength;

		DebugPrint(("[acpihid.sys] GetReportDescriptor Copying 0x%x bytes to HIDCLASS buffer\n", bytesToCopy));

		RtlCopyMemory((PUCHAR) Irp->UserBuffer,
		    (PUCHAR) deviceInfo->ReportDescriptor,
			bytesToCopy
			);

	}
	else
	{
		bytesToCopy = 0;
		ntStatus = STATUS_BUFFER_TOO_SMALL;
	}

	Irp->IoStatus.Information = bytesToCopy;
	DebugPrint(("[acpihid.sys] GetReportDescriptor Exit = 0x%x\n", ntStatus));

	return ntStatus;
}



NTSTATUS
GetDeviceAttributes(
    IN PDEVICE_OBJECT DeviceObject,
    IN PIRP Irp)
/*++

Routine Description:

    Fill in the given struct _HID_DEVICE_ATTRIBUTES

Arguments:

    DeviceObject - pointer to a device object.

    Irp - Pointer to Interrupt Request Packet.

Return Value:

    NT status code.

--*/
{
    NTSTATUS                 ntStatus = STATUS_SUCCESS;
    PIO_STACK_LOCATION       irpStack;
    PHID_DEVICE_ATTRIBUTES   deviceAttributes;

	PAGED_CODE();

    DebugPrint(("[acpihid.sys] Enter GetDeviceAttributes\n"));

    //
    // Get a pointer to the current location in the Irp
    //

    irpStack = IoGetCurrentIrpStackLocation(Irp);

    //
    // Get a pointer to the device extension
    //
    deviceAttributes = (PHID_DEVICE_ATTRIBUTES) Irp->UserBuffer;

    if ( sizeof(HID_DEVICE_ATTRIBUTES) >
        irpStack->Parameters.DeviceIoControl.OutputBufferLength)
	{
        ntStatus = STATUS_BUFFER_TOO_SMALL;
    }
	else
	{
    //
    // Report how many bytes were copied
    //
    Irp->IoStatus.Information		=	sizeof (HID_DEVICE_ATTRIBUTES);
    deviceAttributes->Size			=	sizeof (HID_DEVICE_ATTRIBUTES);
    deviceAttributes->VendorID		=	HIDMINI_VID;
    deviceAttributes->ProductID		=	HIDMINI_PID;
    deviceAttributes->VersionNumber =	HIDMINI_VERSION;
	}

	DebugPrint(("[acpihid.sys] Exit GetDeviceAttributes:  ntStatus = 0x%x\n", ntStatus));

    return ntStatus;
}



NTSTATUS
GetHidDescriptor(
    IN PDEVICE_OBJECT DeviceObject,
    IN PIRP Irp
    )
/*++

Routine Description:

    Finds the HID descriptor and copies it into the buffer provided by the Irp.

Arguments:

    DeviceObject - pointer to a device object.

    Irp - Pointer to Interrupt Request Packet.

Return Value:

    NT status code.

--*/
{
    NTSTATUS            ntStatus = STATUS_SUCCESS;
    PDEVICE_EXTENSION   deviceInfo;
    PIO_STACK_LOCATION  IrpStack;
    ULONG               bytesToCopy;

	PAGED_CODE();

    DebugPrint(("[acpihid.sys] Enter GetHIDDescriptor\n"));

    //
    // Get a pointer to the current location in the Irp
    //
    deviceInfo = GET_MINIDRIVER_DEVICE_EXTENSION(DeviceObject);
    IrpStack = IoGetCurrentIrpStackLocation(Irp);

    //
    // Get a pointer to the device extension
    //
    deviceInfo = GET_MINIDRIVER_DEVICE_EXTENSION(DeviceObject);

    //
    // Copy device descriptor to HIDCLASS buffer
    //
    bytesToCopy = IrpStack->Parameters.DeviceIoControl.OutputBufferLength;

    //
    // Since HidDescriptor.bLength could be >= sizeof(HID_DESCRIPTOR) we
    // just check for HidDescriptor.bLength and
    // copy MIN (OutputBufferLength, DeviceExtension->HidDescriptor->bLength)
    //

    if ( bytesToCopy >= deviceInfo->HidDescriptor.bLength )
	{
        bytesToCopy = deviceInfo->HidDescriptor.bLength;
        RtlCopyMemory((PUCHAR) Irp->UserBuffer,
	                  (PUCHAR) &deviceInfo->HidDescriptor,
		              bytesToCopy);
    }
	else
	{
        DebugPrint(("[acpihid.sys] GetHIDDescriptor error: STATUS_BUFFER_TOO_SMALL\n"));
		bytesToCopy = 0;
        ntStatus = STATUS_BUFFER_TOO_SMALL;
	}

    //
    // Report how many bytes were copied
    //
    Irp->IoStatus.Information = bytesToCopy;

	DebugPrint(("[acpihid.sys] Exit GetHIDDescriptor: ntStatus = 0x%x\n", ntStatus));

	return ntStatus;
}





NTSTATUS
AcpiHidInternalIoctl
    (
    IN PDEVICE_OBJECT DeviceObject,
    IN PIRP Irp
    )
/*++

Routine Description:

    Handles all the internal ioctls

Arguments:

    DeviceObject - Pointer to the device object.

    Irp - Pointer to the request packet.

Return Value:

    NT Status code

--*/
{
    NTSTATUS            ntStatus = STATUS_SUCCESS;
    PIO_STACK_LOCATION  IrpStack;
    PDEVICE_EXTENSION   deviceInfo;
    PHID_XFER_PACKET    xPacket;
    ULONG               ReportBufferLength = 0;
    int                 ButtonStatus = 0;
    int                 LedState = 0;

    deviceInfo = GET_MINIDRIVER_DEVICE_EXTENSION( DeviceObject );

    ntStatus = IoAcquireRemoveLock( &deviceInfo->RemoveLock, Irp );

    if ( ! NT_SUCCESS( ntStatus ))
    {
        Irp->IoStatus.Status = ntStatus;
        Irp->IoStatus.Information =  0;
        IoCompleteRequest(Irp, IO_NO_INCREMENT);
        return ntStatus;
    }

    IrpStack = IoGetCurrentIrpStackLocation(Irp);

    switch( IrpStack->Parameters.DeviceIoControl.IoControlCode )
    {
//
// Retrieves the device's HID descriptor.
//
    case IOCTL_HID_GET_DEVICE_DESCRIPTOR:
        DebugPrint(("[acpihid.sys] AcpiHidInternalIoctl IOCTL_HID_GET_DEVICE_DESCRIPTOR\n"));
        ntStatus = GetHidDescriptor(DeviceObject, Irp);
        break;

//
//Obtains the report descriptor for the HID device.
//
    case IOCTL_HID_GET_REPORT_DESCRIPTOR:
        DebugPrint(("[acpihid.sys] AcpiHidInternalIoctl IOCTL_HID_GET_REPORT_DESCRIPTOR\n"));
        ntStatus = GetReportDescriptor(DeviceObject, Irp);
        break;

//
//  Return a report from the device into a class driver-supplied buffer (GET HID BUTTON PRESSES).
//
    case IOCTL_HID_READ_REPORT:
        DebugPrint(("[acpihid.sys] AcpiHidInternalIoctl IOCTL_HID_READ_REPORT (GET HID BUTTON PRESSES)\n"));
        if (deviceInfo->RemovedDevice == 0x0)
        {
            ntStatus = ReadReport( DeviceObject, Irp );

// CRITICAL: don't remove next "return" or change to "break" ... an infinite loop will ensue!
            return ntStatus;
        }
        break;

//
//Retrieves a device's attributes in a HID_DEVICE_ATTRIBUTES structure.
//
    case IOCTL_HID_GET_DEVICE_ATTRIBUTES:
        DebugPrint(("[acpihid.sys] AcpiHidInternalIoctl IOCTL_HID_GET_DEVICE_ATTRIBUTES\n"));
        ntStatus = GetDeviceAttributes( DeviceObject, Irp );
        break;

//
//  Transmits a class driver-supplied report to the device (SET LED STATE).
//
    case IOCTL_HID_WRITE_REPORT:
        DebugPrint(("[acpihid.sys] AcpiHidInternalIoctl IOCTL_HID_WRITE_REPORT (SET LED STATE)\n"));
        xPacket =  Irp->UserBuffer;
        DebugPrint(("[acpihid.sys]    reportId: 0x%lx \n", xPacket->reportId));
        DebugPrint(("[acpihid.sys]    reportBufferLen: 0x%lx \n", xPacket->reportBufferLen));
        DebugPrint(("[acpihid.sys]    reportBuffer[1]: 0x%lx \n", xPacket->reportBuffer[1]));
        DebugPrint(("[acpihid.sys]    AcpiLedCtrl\n"));
        ntStatus = AcpiLedCtrl (deviceInfo,
								xPacket->reportBuffer[1] & 0x7f,    //LED Number,
								(xPacket->reportBuffer[1] >> 7) & 0x1  //ON or OFF,
								);
		break;

//
//  Return a report from the device into a class driver-supplied buffer (GET LED STATE).
//
	case IOCTL_HID_GET_INPUT_REPORT:
		DebugPrint(("[acpihid.sys] AcpiHidInternalIoctl IOCTL_HID_GET_INPUT_REPORT (GET LED STATE)\n"));
		xPacket = Irp->UserBuffer;
		DebugPrint(("[acpihid.sys]    reportId: 0x%lx \n", xPacket->reportId));
		DebugPrint(("[acpihid.sys]    reportBufferLen: 0x%lx \n", xPacket->reportBufferLen));
		for (ReportBufferLength = 0;  ReportBufferLength < xPacket->reportBufferLen; ReportBufferLength++) {
			DebugPrint(("[acpihid.sys]    reportBuffer[%d]: 0x%lx \n",  ReportBufferLength, xPacket->reportBuffer[ReportBufferLength]));
		}
		ntStatus = AcpiLedStatus(deviceInfo,
								(xPacket->reportBuffer[1] & 0x7f),
								&LedState
								);
		xPacket->reportBuffer[2] = (UCHAR)LedState;
		DebugPrint(("[acpihid.sys]    LedState: 0x%lx, reportBuffer[2]: 0x%lx \n", LedState, xPacket->reportBuffer[2] ));
		Irp->IoStatus.Information = REPORT_BUF_SIZE;
		break;

//
//  Transmits a class driver-supplied report to the device (HID BUTTON PRESS LOOPBACK).
//
	case IOCTL_HID_SET_OUTPUT_REPORT:
        DebugPrint(("[acpihid.sys] AcpiHidInternalIoctl IOCTL_HID_SET_OUTPUT_REPORT (HID BUTTON PRESS LOOPBACK)\n"));
        xPacket = Irp->UserBuffer;
        DebugPrint(("[acpihid.sys]    reportId: 0x%lx \n", xPacket->reportId));
        DebugPrint(("[acpihid.sys]    reportBufferLen: 0x%lx \n", xPacket->reportBufferLen));
        for ( ReportBufferLength = 0;  ReportBufferLength < xPacket->reportBufferLen; ++ReportBufferLength )
        {
            DebugPrint(("[acpihid.sys]    reportBuffer[%d]: 0x%lx \n",  ReportBufferLength, xPacket->reportBuffer[ReportBufferLength]));
        }

// copy input UserBuffer to output ReportBuffer
        for ( ReportBufferLength = 0;  ReportBufferLength < xPacket->reportBufferLen; ++ReportBufferLength )
        {
            deviceInfo->ReportBuffer[ReportBufferLength] = xPacket->reportBuffer[ReportBufferLength];
        }
        CompleteButtonRequest(deviceInfo);

// we did the key "make", now we need to do the key "break": keep report ID, zero out everything else
        for ( ReportBufferLength = 1;  ReportBufferLength < xPacket->reportBufferLen; ++ReportBufferLength )
        {
            deviceInfo->ReportBuffer[ReportBufferLength] = 0x00;
        }

        CompleteButtonRequest(deviceInfo);
        Irp->IoStatus.Information = REPORT_BUF_SIZE;
        break;

//
//  This sends a HID class feature report to a top-level collection of a HID class device (SET VIRTUAL BUTTON STATE).
//
	case IOCTL_HID_SET_FEATURE:
        DebugPrint(("[acpihid.sys] AcpiHidInternalIoctl IOCTL_HID_SET_FEATURE (SET VIRTUAL BUTTON STATE)\n"));
        xPacket =  Irp->UserBuffer;
	    DebugPrint(("[acpihid.sys]    reportId: 0x%lx \n", xPacket->reportId));
	    DebugPrint(("[acpihid.sys]    reportBufferLen: 0x%lx \n", xPacket->reportBufferLen));
	    for (ReportBufferLength = 0;  ReportBufferLength < xPacket->reportBufferLen; ReportBufferLength++) {
			DebugPrint(("[acpihid.sys]    reportBuffer[%d]: 0x%lx \n",  ReportBufferLength, xPacket->reportBuffer[ReportBufferLength]));
		}
		if ((xPacket->reportBuffer[1] & 0x7f) == 0) {
			// set "button 0", which is the "hold button".
			if (((xPacket->reportBuffer[1] >> 7) & 0x01) == 0x01) { // to "on" means "engage hold".
				DebugPrint(("[acpihid.sys] AcpiHidInternalIoctl ENGAGE BUTTON HOLD \n"));
        		deviceInfo->DeviceActivatedFlag = FALSE;
			} else { // to "off" means "release hold".
				DebugPrint(("[acpihid.sys] AcpiHidInternalIoctl RELEASE BUTTON HOLD \n"));
        		deviceInfo->DeviceActivatedFlag = TRUE;
			}
		} else {
        	ntStatus = AcpiButtonCtrl(deviceInfo,
        						  xPacket->reportBuffer[1] &0x7f,
								  ((xPacket->reportBuffer[1] >> 7) & 0x01)
								 );
		}
        break;
// */


//
//  Returns a feature report associated with a top-level collection (GET VIRTUAL BUTTON STATE).
//
    case IOCTL_HID_GET_FEATURE:
        DebugPrint(("[acpihid.sys] AcpiHidInternalIoctl IOCTL_HID_GET_FEATURE (GET VIRTUAL BUTTON STATE)\n"));
	    xPacket =  Irp->UserBuffer;
		DebugPrint(("[acpihid.sys]    reportId: 0x%lx \n", xPacket->reportId));
		DebugPrint(("[acpihid.sys]    reportBufferLen: 0x%lx \n", xPacket->reportBufferLen));
		for (ReportBufferLength = 0;  ReportBufferLength < xPacket->reportBufferLen; ReportBufferLength++) {
			DebugPrint(("[acpihid.sys]    reportBuffer[%d]: 0x%lx \n",  ReportBufferLength, xPacket->reportBuffer[ReportBufferLength]));
		}
		if ((xPacket->reportBuffer[1] &0x7f) == 0) {
			// get "button 0", which is the "hold button".
			ButtonStatus = !deviceInfo->DeviceActivatedFlag;
		} else {
	        ntStatus = AcpiButtonStatus(deviceInfo,
        						  xPacket->reportBuffer[1],
								  &ButtonStatus
								 );
		}
		xPacket->reportBuffer[xPacket->reportBufferLen - 1] = (UCHAR)ButtonStatus;
	    DebugPrint(("[acpihid.sys]    ButtonStatus: 0x%lx, reportBuffer[2]: 0x%lx \n", ButtonStatus, xPacket->reportBuffer[2] ));
        Irp->IoStatus.Information = REPORT_BUF_SIZE;
        break;
// */

//
// Requests that the HID minidriver retrieve a human-readable string
// for either the manufacturer ID, the product ID, or the serial number
// from the string descriptor of the device. The minidriver must send
// a Get String Descriptor request to the device, in order to retrieve
// the string descriptor, then it must extract the string at the
// appropriate index from the string descriptor and return it in the
// output buffer indicated by the IRP. Before sending the Get String
// Descriptor request, the minidriver must retrieve the appropriate
// index for the manufacturer ID, the product ID or the serial number
// from the device extension of a top level collection associated with
// the device.
//
	case IOCTL_HID_GET_STRING:
        DebugPrint(("[acpihid.sys] AcpiHidInternalIoctl IOCTL_HID_GET_STRING\n"));
        ntStatus = GetStringDescriptor(DeviceObject, Irp);
        break;

//
// Makes the device ready for I/O operations.
//
    case IOCTL_HID_ACTIVATE_DEVICE:
        DebugPrint(("[acpihid.sys] AcpiHidInternalIoctl IOCTL_HID_ACTIVATE_DEVICE\n"));
        ntStatus = STATUS_NOT_SUPPORTED;
        break;

//
//Causes the device to cease operations and terminate all outstanding
//I/O requests.
//
    case IOCTL_HID_DEACTIVATE_DEVICE:
        DebugPrint(("[acpihid.sys] AcpiHidInternalIoctl IOCTL_HID_DEACTIVATE_DEVICE\n"));
        ntStatus = STATUS_NOT_SUPPORTED;
        break;

    default:
        DebugPrint(("[acpihid.sys] AcpiHidInternalIoctl Unknown or unsupported IOCTL (%x)\n", IrpStack->Parameters.DeviceIoControl.IoControlCode));
        ntStatus = STATUS_NOT_SUPPORTED;
        break;
    }

    //
    //Set real return status in Irp
    //
    Irp->IoStatus.Status = ntStatus;

    IoReleaseRemoveLock(&deviceInfo->RemoveLock, Irp);

    IoCompleteRequest(Irp, IO_NO_INCREMENT);

    ntStatus = STATUS_SUCCESS;
    return ntStatus;
}
