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
// Module Name: Acpi.c                                                        //
//                                                                            //
// Abstract: Implementation of the ACPI functions.                            //
//           Key among these are the routines which interface to the ACPI     //
//           BIOS Control Methods for LED and Button manipulation:            //
//              1. AcpiLedStatus() -> LEDS control method                     //
//              2. AcpiLedCtrl() -> LEDC control method                       //
//              3. AcpiButtonStatus() -> BTNS control method                  //
//              4. AcpiButtonCtrl() -> BTNC control method                    //
//                                                                            //
// Authors: Babu Pallithanam                                                  //
//          Jim Trethewey                                                     //
//                                                                            //
// Environment: kernel mode only                                              //
//                                                                            //
// Revision History:                                                          //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////
#ifndef ACPI_H
#define ACPI_H


#ifndef ACPIHID_TAG

// Tag for memory block allocation
#define	ACPIHID_TAG		'HID'

// force to use tag to avoid obsolete call warning
#undef  ExAllocatePool
#define ExAllocatePool(a,b) ExAllocatePoolWithTag(a, b, ACPIHID_TAG)

#endif

NTSTATUS
OnAcpiRequestComplete(
	IN PDEVICE_OBJECT fdo,
	IN PIRP Irp,
	IN PKEVENT pev
	);


NTSTATUS ExecuteACPICommand(
                 IN PDEVICE_OBJECT           Pdo,
                 IN PVOID                    InputBuffer,
                 IN ULONG                    InputSize,
                 IN PACPI_EVAL_OUTPUT_BUFFER OutputBuffer,
                 IN ULONG                    OutputSize
                 );

NTSTATUS AcpiLedCtrl(PDEVICE_EXTENSION pdx,
					 int   LedId,
					 ULONG LedStatus);

NTSTATUS AcpiLedStatus(PDEVICE_EXTENSION pdx,
					   int LedId,
					   OUT ULONG *LedStatus);

NTSTATUS PerformLedLightShow(PDEVICE_EXTENSION pdx,
							 int showStyle);

NTSTATUS AcpiButtonCtrl(PDEVICE_EXTENSION pdx,
						int   ButtonId,
						ULONG ButtonStatus);

NTSTATUS AcpiButtonStatus(PDEVICE_EXTENSION pdx,
						  int ButtonId,
					      OUT ULONG *ButtonStatus);

NTSTATUS GetAcpiInterfaces(
    IN PDEVICE_OBJECT Pdo,
    PACPI_INTERFACE_STANDARD PAcpiStruct
	);


#endif // ACPI_H
