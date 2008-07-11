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
// Module Name: PowerManage.c                                                 //
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
    #pragma alloc_text( NONPAGED, SS_DispatchPower )
#endif


NTSTATUS
SS_DispatchPower(
    IN PDEVICE_OBJECT DeviceObject,
    IN PIRP Irp
    )
{
    PDEVICE_EXTENSION  deviceExtension;

    //
    // initialize the variables
    //
    DebugPrint(("[acpihid.sys] SS_DispatchPower Entry\n"));

    deviceExtension = GET_MINIDRIVER_DEVICE_EXTENSION (DeviceObject);

    PoStartNextPowerIrp( Irp );
    IoSkipCurrentIrpStackLocation( Irp );
    return PoCallDriver(GET_NEXT_DEVICE_OBJECT (DeviceObject), Irp);
}
