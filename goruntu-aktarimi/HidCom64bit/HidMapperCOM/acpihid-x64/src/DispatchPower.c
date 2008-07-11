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
// Module Name: DispatchPower.c                                               //
//                                                                            //
// Abstract: The power management related processing.                         //
//                                                                            //
//           The Power Manager uses IRPs to direct drivers to change system   //
//           and device power levels, to respond to system wake-up events,    //
//           and to query drivers about their devices. All power IRPs have    //
//           the major function code IRP_MJ_POWER.                            //
//                                                                            //
//           Most function and filter drivers perform some processing for     //
//           each power IRP, then pass the IRP down to the next lower driver  //
//           without completing it. Eventually the IRP reaches the bus        //
//           driver, which physically changes the power state of the device   //
//           and completes the IRP.                                           //
//                                                                            //
//           When the IRP has been completed, the I/O Manager calls any       //
//           IoCompletion routines set by drivers as the IRP traveled         //
//           down the device stack. Whether a driver needs to set a           //
//           completion routine depends upon the type of IRP and the          //
//           driver's individual requirements.                                //
//                                                                            //
//           This code is not USB specific. It is essential for every WDM     //
//           driver to handle power IRPs.                                     //
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
    #pragma alloc_text( NONPAGED, AcpiHidDispatchPower )
#endif


PCHAR
PnPMinorFunctionString (
	UCHAR MajorFunction,
    UCHAR MinorFunction
    );

PCHAR
PnPMajorFunctionString (
    UCHAR MajorFunction
    );

PCHAR
PowerActionString (
	PIO_STACK_LOCATION pIrpStack
	)
{
	switch (pIrpStack->Parameters.Power.ShutdownType)
	{
	case PowerActionNone:
		return "NO_ACTION";
	case PowerActionReserved:
		return "RESERVED_ACTION";
	case PowerActionSleep:
		return "SLEEP_ACTION";
	case PowerActionHibernate:
		return "HIBERNATE_ACTION";
	case PowerActionShutdown:
		return "SHUTDOWN_ACTION";
	case PowerActionShutdownReset:
		return "SHUTDOWN+RESET";
	case PowerActionShutdownOff:
		return "SHUTDOWN+OFF (G3)";
	case PowerActionWarmEject:
		return "WARM_EJECT";
	default:
		return "UNDEFINED_ACTION";
	}
}

PCHAR
PowerTypeString (
	PIO_STACK_LOCATION pIrpStack
	)
{
	switch (pIrpStack->Parameters.Power.Type)
	{
	case SystemPowerState:
		switch (pIrpStack->Parameters.Power.State.SystemState)
		{
		case PowerSystemUnspecified:
			return "S(unspecified)";
		case PowerSystemWorking:
			return "S0";
		case PowerSystemSleeping1:
			return "S1";
		case PowerSystemSleeping2:
			return "S2";
		case PowerSystemSleeping3:
			return "S3";
		case PowerSystemHibernate:
			return "S4";
		case PowerSystemShutdown:
			return "S5";
		default:
			return "S(undefined)";
		}
	case DevicePowerState:
		switch (pIrpStack->Parameters.Power.State.DeviceState)
		{
		case PowerDeviceUnspecified:
			return "D(unspecified)";
		case PowerDeviceD0:
			return "D0";
		case PowerDeviceD1:
			return "D1";
		case PowerDeviceD2:
			return "D2";
		case PowerDeviceD3:
			return "D3";
		default:
			return "D(undefined)";
		}
	default:
		return "(undefined)";
	}
}


NTSTATUS
AcpiHidDispatchPower(
    IN PDEVICE_OBJECT DeviceObject,
    IN PIRP Irp
    )
{
	NTSTATUS			status;
    PDEVICE_EXTENSION	deviceExtension;
    PIO_STACK_LOCATION  IrpStack;
    CHAR      			szPowerType[32];

    deviceExtension = GET_MINIDRIVER_DEVICE_EXTENSION( DeviceObject );

//
// Get a pointer to the current location in the Irp
//
    IrpStack = IoGetCurrentIrpStackLocation (Irp);

	strcpy(szPowerType, PowerTypeString(IrpStack));
	DebugPrint(("[acpihid.sys] ##### %s begin ######################################################################## %s #####",
			szPowerType,
			szPowerType
		));
	DebugPrint(("[acpihid.sys] Entering AcpiHidDispatchPower: Major(0x%x)=%s Minor(0x%x)=%s, DeviceObject=0x%lx, Irp=0x%x\n",
        IrpStack->MajorFunction,
        PnPMajorFunctionString(IrpStack->MajorFunction),
        IrpStack->MinorFunction,
        PnPMinorFunctionString(IrpStack->MajorFunction, IrpStack->MinorFunction),
        DeviceObject,
        Irp ));
    if ((IRP_MJ_POWER == IrpStack->MajorFunction) && (IRP_MN_SET_POWER == IrpStack->MinorFunction))
    {
		DebugPrint(("[acpihid.sys]    %s, to power state: %s\n",
			PowerActionString(IrpStack),
			szPowerType
			));
	}

    PoStartNextPowerIrp( Irp );
    IoSkipCurrentIrpStackLocation( Irp );
	DebugPrint(("[acpihid.sys]    Forwarding (PoCallDriver) Power Irp to ACPI.SYS driver now...\n" ));
    status = PoCallDriver( GET_NEXT_DEVICE_OBJECT( DeviceObject ), Irp );
	DebugPrint(("[acpihid.sys]    Return status from ACPI.SYS driver = 0x%lx\n", status ));
	DebugPrint(("[acpihid.sys] Exiting AcpiHidDispatchPower\n" ));
	DebugPrint(("[acpihid.sys] ##### %s end ########################################################################## %s #####",
			szPowerType,
			szPowerType
		));
	return status;
}
