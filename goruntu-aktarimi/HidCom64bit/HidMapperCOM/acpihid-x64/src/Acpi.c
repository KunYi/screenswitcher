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


#include "Main.h"
#include "Acpi.h"
#include "MapButtons.h"


#define  BUTTON_STATUS						'SNTB'   //BTNS
#define  BUTTON_CONTROL                     'CNTB'   //BTNC
#define  LED_STATUS							'SDEL' 	 //LEDS
#define  LED_CONTROL                        'CDEL'   //LEDC


#ifdef ALLOC_PRAGMA
    #pragma alloc_text( PAGE,  OnAcpiRequestComplete )
    #pragma alloc_text( PAGE,  GetAcpiInterfaces )
    #pragma alloc_text( PAGE,  ExecuteACPICommand )
    #pragma alloc_text( PAGE,  AcpiLedCtrl )
    #pragma alloc_text( PAGE,  AcpiLedStatus )
    #pragma alloc_text( PAGE,  PerformLedLightShow )
    #pragma alloc_text( PAGE,  AcpiButtonCtrl )
    #pragma alloc_text( PAGE,  AcpiButtonStatus )
#endif


PCHAR MethodNameAsString(
	ULONG ulMethodName
	)
{
	switch (ulMethodName)
	{
		case BUTTON_STATUS:
			return "BTNS";
		case BUTTON_CONTROL:
			return "BTNC";
		case LED_STATUS:
			return "LEDS";
		case LED_CONTROL:
			return "LEDC";
		default:
			return "";
	}
}

/********************************************************

*   Name                :OnRequestComplete()

*   Description         :This function is used to set the notification
*                        event.It is called by IoSetCompletionRoutine.
*
*   Input Parameters    :IN PDEVICE_OBJECT fdo
*                        IN PIRP Irp
*                        IN PKEVENT pev

*   Returns             :NTSTATUS
*********************************************************/

NTSTATUS OnAcpiRequestComplete(IN PDEVICE_OBJECT fdo,
						   IN PIRP Irp,
						   IN PKEVENT pev)
{
	PAGED_CODE();

    KeSetEvent(pev, 0, FALSE);
    return STATUS_MORE_PROCESSING_REQUIRED;
}


/********************************************************

*   Name                :GetAcpiInterfaces()

*   Description         :This is the helper function called
*						 in AddDevice which asks ACPI.SYS for
*						 its direct function call table.
*
*   Input Parameters    :IN PDEVICE_OBJECT Pdo
*                        PACPI_INTERFACE_STANDARD PAcpiStruct
*
*
*   Returns             :NTSTATUS
*********************************************************/


NTSTATUS GetAcpiInterfaces( IN PDEVICE_OBJECT Pdo, PACPI_INTERFACE_STANDARD PAcpiStruct )
{
    NTSTATUS                status = STATUS_SUCCESS;
    PIRP                    Irp;
    PIO_STACK_LOCATION      IrpStack;
    PDEVICE_EXTENSION		pdx;
    KEVENT                  event;

	PAGED_CODE();

    DebugPrint(( "[acpihid.sys] Entering GetAcpiInterfaces( Pdo=0x%lx, PAcpiStruct=0x%lx\n", Pdo, PAcpiStruct ));
    pdx = ( PDEVICE_EXTENSION ) GET_MINIDRIVER_DEVICE_EXTENSION( Pdo );

    Irp = IoAllocateIrp( GET_NEXT_DEVICE_OBJECT(Pdo)->StackSize, FALSE );

    if ( ! Irp )
    {
        DebugPrint(("[acpihid.sys] IoAllocateIrp  STATUS_INSUFFICIENT_RESOURCES\n"));
        return STATUS_INSUFFICIENT_RESOURCES;
    }

    IrpStack = IoGetNextIrpStackLocation( Irp );

// set up irp
    IrpStack->MajorFunction = IRP_MJ_PNP;
    IrpStack->MinorFunction = IRP_MN_QUERY_INTERFACE;
    IrpStack->Parameters.QueryInterface.InterfaceType = &GUID_ACPI_INTERFACE_STANDARD;
    IrpStack->Parameters.QueryInterface.Version = 1;
    IrpStack->Parameters.QueryInterface.Size = sizeof(ACPI_INTERFACE_STANDARD);
    IrpStack->Parameters.QueryInterface.Interface = (PINTERFACE) PAcpiStruct;
    IrpStack->Parameters.QueryInterface.InterfaceSpecificData = NULL;

    KeInitializeEvent( &event, NotificationEvent, FALSE );
    IoSetCompletionRoutine(Irp, (PIO_COMPLETION_ROUTINE) OnAcpiRequestComplete, &event, TRUE, TRUE, TRUE);

    status = IoCallDriver(GET_NEXT_DEVICE_OBJECT(Pdo), Irp);

    if (status == STATUS_PENDING) {
        // wait for completion
		DebugPrint(("[acpihid.sys] wait for completion\n"));
        KeWaitForSingleObject(&event, Executive, KernelMode, FALSE, NULL);
        status = Irp->IoStatus.Status;
    }

   	DebugPrint(("[acpihid.sys] GetAcpiInterfaces:IoCallDriver Status = %x\n", status));

    IoFreeIrp(Irp);
    DebugPrint(("[acpihid.sys] GetAcpiInterfaces Exit: status = 0x%lx\n", status));

    return status;
}


/********************************************************

*   Name                :ExecuteACPICommand()

*   Description         :This is a General-purpose function
*						 called to send a request to the Pdo.
*						 The Ioctl argument accepts the control
*						 method being passed down by the calling function.
*
*   Input Parameters    :IN PDEVICE_OBJECT Pdo
*                        IN PVOID  InputBuffer
*                        IN ULONG  InputSize
*                        IN PACPI_EVAL_OUTPUT_BUFFER  OutputBuffer
*                        IN ULONG  OutputSize
*
*
*   Returns             :NTSTATUS
*********************************************************/

NTSTATUS ExecuteACPICommand(IN PDEVICE_OBJECT			 Pdo,
							IN PVOID					 InputBuffer,
							IN ULONG					 InputSize,
							IN PACPI_EVAL_OUTPUT_BUFFER  OutputBuffer,
							IN ULONG                     OutputSize)
{

    IO_STATUS_BLOCK                 ioBlock;
    KEVENT                          MyIoctlEvent;
    NTSTATUS                        status;
    PIRP                            Irp;
    PACPI_EVAL_INPUT_BUFFER_COMPLEX pInputBuffer = (PACPI_EVAL_INPUT_BUFFER_COMPLEX)InputBuffer;

#ifndef NDEBUG
	// to collect performance statistics
	LARGE_INTEGER			CurrentTimeAfter;
	TIME_FIELDS				StopTime;
	TIME_FIELDS				StartTime;
	LARGE_INTEGER			CurrentTimeBefore;
	CSHORT					cShort;
	////////////////////////////////////
#endif

	PAGED_CODE();

    DebugPrint(("[acpihid.sys] Entering ExecuteACPICommand %s\n",
    	MethodNameAsString(pInputBuffer->MethodNameAsUlong)
    	));

// Initialize an event to wait upon
    KeInitializeEvent ( &MyIoctlEvent, SynchronizationEvent, FALSE );

#ifndef NDEBUG
// get the time stamp
    KeQuerySystemTime( &CurrentTimeBefore );
    RtlTimeToTimeFields( &CurrentTimeBefore, &StartTime );
#endif

    // Build the request
    Irp = IoBuildDeviceIoControlRequest(IOCTL_ACPI_EVAL_METHOD,
										Pdo,
										InputBuffer,
										InputSize,
										OutputBuffer,
										OutputSize,
										FALSE,
										&MyIoctlEvent,
										&ioBlock
									   );

    if ( ! Irp )
    {
        DebugPrint(("[acpihid.sys] Insufficient resources\n"));
        return STATUS_INSUFFICIENT_RESOURCES;
    }

    // Pass the request to the Pdo, always wait for the completion routine
    status = IoCallDriver(Pdo, Irp);

    if (status == STATUS_PENDING) {
        // Wait for the IRP to be completed, then grab the real status code
		DebugPrint(("[acpihid.sys] Pending on IO request to Pdo: 0x%lx\n", Pdo));
        KeWaitForSingleObject(&MyIoctlEvent,
							  Executive,
							  KernelMode,
							  FALSE,
							  NULL
							 );

        status = ioBlock.Status;
    }

#ifndef NDEBUG
	KeQuerySystemTime(&CurrentTimeAfter);
	RtlTimeToTimeFields(&CurrentTimeAfter, &StopTime);

	cShort = StopTime.Milliseconds - StartTime.Milliseconds;
	if (cShort < 0)
		cShort = 1000 + cShort;
	DebugPrint(("[acpihid.sys] Took %d milliseconds\n", cShort));
#endif

	if (!NT_SUCCESS(status)) {
        DebugPrint(("[acpihid.sys] IoCallDriver Status = %x\n", status));
    }

    DebugPrint(("[acpihid.sys] Exiting ExecuteACPICommand, status: 0x%lx\n", status));

	return status;
}


NTSTATUS AcpiLedCtrl(
    PDEVICE_EXTENSION pdx,
    int   LedId,
    ULONG LedStatus
	)
{
    ULONG AcpiInBufferSize = 0;
    int i = 0;
    int j = 0;

    PACPI_EVAL_INPUT_BUFFER_COMPLEX pInputBuffer = NULL;
    NTSTATUS status = STATUS_SUCCESS;

	PAGED_CODE();

 	DebugPrint(("[acpihid.sys] Enter AcpiLedCtrl(%d,0x%lx) \n", LedId, LedStatus));

// allocate memory for input buffer (no output buffer used)
    AcpiInBufferSize = sizeof( ACPI_EVAL_INPUT_BUFFER_COMPLEX ) + 2 * sizeof( ACPI_METHOD_ARGUMENT ) + 1;
    pInputBuffer     = ExAllocatePool( NonPagedPool, AcpiInBufferSize );

    if ( NULL == pInputBuffer )
    {
        DebugPrint(("[acpihid.sys] Insufficient resources\n"));
        status = STATUS_INSUFFICIENT_RESOURCES;
        return status;
    }

// initialize buffers to all zeroes
    memset( pInputBuffer, 0, AcpiInBufferSize );

// fill in input buffer with our request to ACPI
    pInputBuffer->Signature = ACPI_EVAL_INPUT_BUFFER_COMPLEX_SIGNATURE;
    pInputBuffer->MethodNameAsUlong	= LED_CONTROL;
    pInputBuffer->Size = 2 * sizeof( ACPI_METHOD_ARGUMENT );
    pInputBuffer->ArgumentCount = 2;

    pInputBuffer->Argument[0].Type		 = ACPI_METHOD_ARGUMENT_INTEGER;
    pInputBuffer->Argument[0].DataLength = sizeof(int);
    pInputBuffer->Argument[0].Argument	 = (int) LedId;

    pInputBuffer->Argument[1].Type		 = ACPI_METHOD_ARGUMENT_INTEGER;
    pInputBuffer->Argument[1].DataLength = sizeof(int);
    pInputBuffer->Argument[1].Argument	 = (int) LedStatus;

    DebugPrint(("[acpihid.sys]   pInputBuffer->Signature: 0x%04lx  \n", pInputBuffer->Signature));
    DebugPrint(("[acpihid.sys]   pInputBuffer->MethodNameAsUlong: 0x%04lx (\"%s\") \n",
    	pInputBuffer->MethodNameAsUlong,
    	MethodNameAsString(pInputBuffer->MethodNameAsUlong)
    	));
    DebugPrint(("[acpihid.sys]   pInputBuffer->Size: 0x%lx  \n", pInputBuffer->Size));
    DebugPrint(("[acpihid.sys]   pInputBuffer->ArgumentCount: 0x%lx  \n", pInputBuffer->ArgumentCount));

    for ( j = 0; j < (int) pInputBuffer->ArgumentCount; ++j )
    {
        DebugPrint(("[acpihid.sys]   pInputBuffer->Argument[%d].Type: 0x%lx  \n", j, pInputBuffer->Argument[j].Type));
    	DebugPrint(("[acpihid.sys]   pInputBuffer->Argument[%d].DataLength: 0x%lx  \n", j, pInputBuffer->Argument[j].DataLength));
        for ( i = 0; i < pInputBuffer->Argument[j].DataLength; ++i )
        {
            DebugPrint(("[acpihid.sys]   pInputBuffer->Argument[%d].Data[%d]: 0x%x  \n", j, i, pInputBuffer->Argument[j].Data[i]));
        }
    }

// execute the ACPI request
    status = ExecuteACPICommand(pdx->LowerDevObj,
        pInputBuffer,
        AcpiInBufferSize,
        NULL,
        0
        );

    DebugPrint(("[acpihid.sys] ExecuteACPIcommand LEDC(%d,0x%04lx), call return %lx \n", LedId, LedStatus, status));

// free the allocated space
    if ( pInputBuffer )
    {
        ExFreePool(pInputBuffer);
    }

    DebugPrint(("[acpihid.sys] Exiting AcpiLedCtrl, status: 0x%lx\n", status));

    return status;
}



NTSTATUS AcpiLedStatus(PDEVICE_EXTENSION pdx,
					   int LedId,
					   OUT ULONG *LedStatus)
{
    short AcpiInBufferSize		=  0;
	short AcpiOutBufferSize		=  0;
	int i = 0;
	int j = 0;

	PACPI_EVAL_INPUT_BUFFER_COMPLEX pInputBuffer = NULL;
    PACPI_EVAL_OUTPUT_BUFFER pOutputBuffer = NULL;
    NTSTATUS status	 = STATUS_SUCCESS;

	PAGED_CODE();

 	DebugPrint(("[acpihid.sys] Enter AcpiLedStatus(%d) \n", LedId));

	// allocate memory for input and output buffers

	AcpiInBufferSize  = sizeof(ACPI_EVAL_INPUT_BUFFER_COMPLEX) + sizeof(ACPI_METHOD_ARGUMENT) + 1;
	pInputBuffer      = ExAllocatePool(NonPagedPool, AcpiInBufferSize);
	AcpiOutBufferSize = sizeof(ACPI_EVAL_OUTPUT_BUFFER) +  2 * (sizeof(ACPI_METHOD_ARGUMENT)) + 1;
	pOutputBuffer     = ExAllocatePool(NonPagedPool, AcpiOutBufferSize);

	if ((pInputBuffer == NULL) || (pOutputBuffer == NULL)) {
		status = STATUS_INSUFFICIENT_RESOURCES;
		return status;
	}

	// initialize buffers to all zeroes

	memset(pInputBuffer,  0, AcpiInBufferSize);
	memset(pOutputBuffer, 0, AcpiOutBufferSize);

	// fill in input buffer with our request to ACPI

	pInputBuffer->Signature = ACPI_EVAL_INPUT_BUFFER_COMPLEX_SIGNATURE;
	pInputBuffer->MethodNameAsUlong = LED_STATUS;
	pInputBuffer->Size = sizeof(ACPI_METHOD_ARGUMENT);
	pInputBuffer->ArgumentCount = 1;
	pInputBuffer->Argument[0].Type		 = ACPI_METHOD_ARGUMENT_INTEGER;
	pInputBuffer->Argument[0].DataLength = sizeof(int);
	pInputBuffer->Argument[0].Argument	 = (int) LedId;

    DebugPrint(("[acpihid.sys]   pInputBuffer->Signature: 0x%04lx  \n", pInputBuffer->Signature));
    DebugPrint(("[acpihid.sys]   pInputBuffer->MethodNameAsUlong: 0x%04lx (\"%s\") \n",
    	pInputBuffer->MethodNameAsUlong,
    	MethodNameAsString(pInputBuffer->MethodNameAsUlong)
    	));
    DebugPrint(("[acpihid.sys]   pInputBuffer->Size: 0x%lx  \n", pInputBuffer->Size));
    DebugPrint(("[acpihid.sys]   pInputBuffer->ArgumentCount: 0x%lx  \n", pInputBuffer->ArgumentCount));
    for (j = 0; j < (int) pInputBuffer->ArgumentCount; j++) {
    	DebugPrint(("[acpihid.sys]   pInputBuffer->Argument[%d].Type: 0x%lx  \n", j, pInputBuffer->Argument[j].Type));
    	DebugPrint(("[acpihid.sys]   pInputBuffer->Argument[%d].DataLength: 0x%lx  \n", j, pInputBuffer->Argument[j].DataLength));
		for (i = 0; i < pInputBuffer->Argument[j].DataLength; i++) {
			DebugPrint(("[acpihid.sys]   pInputBuffer->Argument[%d].Data[%d]: 0x%x  \n", j, i, pInputBuffer->Argument[j].Data[i]));
		}
	}

	// execute the ACPI request

	status = ExecuteACPICommand(pdx->LowerDevObj,
							    pInputBuffer,
							    AcpiInBufferSize,
								pOutputBuffer,
								AcpiOutBufferSize
							   );
	DebugPrint(("[acpihid.sys] ExecuteACPIcommand LEDS(%d), call return %lx \n", LedId, status));

	// check the output result

	if (NT_SUCCESS(status)) {
	    DebugPrint(("[acpihid.sys]   pOutputBuffer->Signature: 0x%04lx  \n", pOutputBuffer->Signature));
        DebugPrint(("[acpihid.sys]   pOutputBuffer->Length: 0x%lx  \n",pOutputBuffer->Length));
        DebugPrint(("[acpihid.sys]   pOutputBuffer->Count: 0x%lx  \n",pOutputBuffer->Count));
        for (j = 0; j < (int) pInputBuffer->ArgumentCount; j++) {
        	DebugPrint(("[acpihid.sys]   pOutputBuffer->Argument[%d].Type: 0x%lx  \n", j, pOutputBuffer->Argument[j].Type));
        	DebugPrint(("[acpihid.sys]   pOutputBuffer->Argument[%d].DataLength: 0x%lx  \n", j, pOutputBuffer->Argument[j].DataLength));
			for ( i =0; i < pOutputBuffer->Argument[j].DataLength; i++) {
	        	DebugPrint(("[acpihid.sys]   pOutputBuffer->Argument[%d].Data[%d]: 0x%x  \n", j, i, pOutputBuffer->Argument[j].Data[i]));
			}
		}

		*LedStatus  = (ULONG) pOutputBuffer->Argument[0].Data[0];
 		DebugPrint(("[acpihid.sys] LEDS(%d) led status is 0x%lx \n", LedId, *LedStatus));
	}

	// free the allocated space
	if (pInputBuffer) {
		ExFreePool(pInputBuffer);
	}

	if (pOutputBuffer) {
		ExFreePool(pOutputBuffer);
    }

    DebugPrint(("[acpihid.sys] Exiting AcpiLedStatus, status: 0x%lx\n", status));

	return status;
}



NTSTATUS PerformLedLightShow( PDEVICE_EXTENSION pdx, int showStyle )
{
    NTSTATUS ntStatus = STATUS_SUCCESS;
    ULONG savedLedStatus[ MAX_NO_LEDS ];
    int i;

	PAGED_CODE();

 	DebugPrint(("[acpihid.sys] Enter PerformLedLightShow \n"));

// do IRP_MN_STOP_DEVICE light-show (turn every thing off during system shutdown)
    if ( 0 == showStyle )
    {
        for (i = 1; i <= (int) pdx->NumberOfLeds; ++i )
        {
            ntStatus = AcpiLedCtrl( pdx, i, 0 );
        }
    }
    else
    {
// do IRP_MN_START_DEVICE light-show (test all LEDs on and off)
// start by determining current settings and saving them for later restore.
        for ( i = 1; i <= (int) pdx->NumberOfLeds; ++i )
        {
            ntStatus = AcpiLedStatus( pdx, i, &savedLedStatus[i - 1] );
        }

// turn each LED on one at a time
        for ( i = 1; i <= (int) pdx->NumberOfLeds; ++i )
        {
            ntStatus = AcpiLedCtrl( pdx, i, 1 );
            ntStatus = AcpiLedCtrl( pdx, i, 0 );
        }

// turn all LEDs on and off at once, and do it two times.
        for ( i = 1; i <= (int) pdx->NumberOfLeds; ++i )
        {
            ntStatus = AcpiLedCtrl( pdx, i, 1 );
        }

        for ( i = 1; i <= (int) pdx->NumberOfLeds; ++i )
        {
            ntStatus = AcpiLedCtrl( pdx, i, 0 );
        }

        for ( i = 1; i <= (int) pdx->NumberOfLeds; ++i )
        {
            ntStatus = AcpiLedCtrl( pdx, i, 1 );
        }

        for ( i = 1; i <= (int) pdx->NumberOfLeds; ++i )
        {
            ntStatus = AcpiLedCtrl( pdx, i, 0 );
        }

// restore LED settings back to the way they started.
        for ( i = 1; i <= (int) pdx->NumberOfLeds; ++i )
        {
            ntStatus = AcpiLedCtrl( pdx, i, savedLedStatus[i - 1] );
        }
    }

	DebugPrint(( "[acpihid.sys] PerformLedLightShow Exiting: status = 0x%x\n", ntStatus ));
    return ntStatus;
}




NTSTATUS AcpiButtonCtrl(PDEVICE_EXTENSION pdx,
					 int   ButtonId,
					 ULONG ButtonStatus)
{
    ULONG AcpiInBufferSize = 0;
	int i = 0;
	int j = 0;

	PACPI_EVAL_INPUT_BUFFER_COMPLEX pInputBuffer = NULL;
    NTSTATUS status = STATUS_SUCCESS;

	PAGED_CODE();

 	DebugPrint(("[acpihid.sys] Enter AcpiButtonCtrl(%d,0x%lx) \n", ButtonId, ButtonStatus));

	// allocate memory for input buffer (no output buffer used)

	AcpiInBufferSize = sizeof(ACPI_EVAL_INPUT_BUFFER_COMPLEX) + 2 * sizeof(ACPI_METHOD_ARGUMENT) + 1;
	pInputBuffer     = ExAllocatePool(NonPagedPool, AcpiInBufferSize);

	if (pInputBuffer == NULL) {
        DebugPrint(("[acpihid.sys] Insufficient resources\n"));
		status = STATUS_INSUFFICIENT_RESOURCES;
		return status;
	}

	// initialize buffers to all zeroes

	memset(pInputBuffer, 0, AcpiInBufferSize);

	// fill in input buffer with our request to ACPI

    pInputBuffer->Signature = ACPI_EVAL_INPUT_BUFFER_COMPLEX_SIGNATURE;
	pInputBuffer->MethodNameAsUlong	= BUTTON_CONTROL;
	pInputBuffer->Size = 2 * sizeof (ACPI_METHOD_ARGUMENT);
	pInputBuffer->ArgumentCount = 2;

	pInputBuffer->Argument[0].Type		 = ACPI_METHOD_ARGUMENT_INTEGER;
	pInputBuffer->Argument[0].DataLength = sizeof(int);
	pInputBuffer->Argument[0].Argument	 = (int) ButtonId;

	pInputBuffer->Argument[1].Type		 = ACPI_METHOD_ARGUMENT_INTEGER;
	pInputBuffer->Argument[1].DataLength = sizeof(int);
	pInputBuffer->Argument[1].Argument	 = (int) ButtonStatus;

    DebugPrint(("[acpihid.sys]   pInputBuffer->Signature: 0x%04lx  \n", pInputBuffer->Signature));
    DebugPrint(("[acpihid.sys]   pInputBuffer->MethodNameAsUlong: 0x%04lx (\"%s\") \n",
    	pInputBuffer->MethodNameAsUlong,
    	MethodNameAsString(pInputBuffer->MethodNameAsUlong)
    	));
    DebugPrint(("[acpihid.sys]   pInputBuffer->Size: 0x%lx  \n", pInputBuffer->Size));
    DebugPrint(("[acpihid.sys]   pInputBuffer->ArgumentCount: 0x%lx  \n", pInputBuffer->ArgumentCount));
    for (j = 0; j < (int) pInputBuffer->ArgumentCount; j++) {
    	DebugPrint(("[acpihid.sys]   pInputBuffer->Argument[%d].Type: 0x%lx  \n", j, pInputBuffer->Argument[j].Type));
    	DebugPrint(("[acpihid.sys]   pInputBuffer->Argument[%d].DataLength: 0x%lx  \n", j, pInputBuffer->Argument[j].DataLength));
		for (i = 0; i < pInputBuffer->Argument[j].DataLength; i++) {
			DebugPrint(("[acpihid.sys]   pInputBuffer->Argument[%d].Data[%d]: 0x%x  \n", j, i, pInputBuffer->Argument[j].Data[i]));
		}
	}

	// execute the ACPI request

	status = ExecuteACPICommand(pdx->LowerDevObj,
								pInputBuffer,
							    AcpiInBufferSize,
								NULL,
								0);


	DebugPrint(("[acpihid.sys] ExecuteACPIcommand BTNC(%d,0x%04lx), call return %lx \n", ButtonId, ButtonStatus, status));

	// free the allocated space

	if(pInputBuffer) {
		ExFreePool(pInputBuffer);
	}

    DebugPrint(("[acpihid.sys] Exiting AcpiButtonCtrl, status: 0x%lx\n", status));

	return status;
}




NTSTATUS  AcpiButtonStatus(PDEVICE_EXTENSION pdx,
						   int   ButtonId,
					       OUT ULONG *ButtonStatus
						   )
{
    short AcpiInBufferSize		=  0;
	short AcpiOutBufferSize		=  0;
	int i = 0;
	int j = 0;

	PACPI_EVAL_INPUT_BUFFER_COMPLEX pInputBuffer = NULL;
    PACPI_EVAL_OUTPUT_BUFFER pOutputBuffer = NULL;
    NTSTATUS status	 = STATUS_SUCCESS;

	PAGED_CODE();

 	DebugPrint(("[acpihid.sys] Enter AcpiButtonStatus(%d) \n", ButtonId));

	// allocate memory for input and output buffers

	AcpiInBufferSize  = sizeof(ACPI_EVAL_INPUT_BUFFER_COMPLEX) + sizeof(ACPI_METHOD_ARGUMENT) + 1;
	pInputBuffer      = ExAllocatePool(NonPagedPool, AcpiInBufferSize);
	AcpiOutBufferSize = sizeof(ACPI_EVAL_OUTPUT_BUFFER) +  2 * (sizeof(ACPI_METHOD_ARGUMENT)) + 1;
	pOutputBuffer     = ExAllocatePool(NonPagedPool, AcpiOutBufferSize);

	if ((pInputBuffer == NULL) || (pOutputBuffer == NULL)) {
		status = STATUS_INSUFFICIENT_RESOURCES;
		return status;
	}

	// initialize buffers to all zeroes

	memset(pInputBuffer,  0, AcpiInBufferSize);
	memset(pOutputBuffer, 0, AcpiOutBufferSize);

	// fill in input buffer with our request to ACPI

	pInputBuffer->Signature = ACPI_EVAL_INPUT_BUFFER_COMPLEX_SIGNATURE;
	pInputBuffer->MethodNameAsUlong = BUTTON_STATUS;
	pInputBuffer->Size = sizeof(ACPI_METHOD_ARGUMENT);
	pInputBuffer->ArgumentCount = 1;
	pInputBuffer->Argument[0].Type		 = ACPI_METHOD_ARGUMENT_INTEGER;
	pInputBuffer->Argument[0].DataLength = sizeof(int);
	pInputBuffer->Argument[0].Argument	 = (int) ButtonId;

    DebugPrint(("[acpihid.sys]   pInputBuffer->Signature: 0x%04lx  \n", pInputBuffer->Signature));
    DebugPrint(("[acpihid.sys]   pInputBuffer->MethodNameAsUlong: 0x%04lx (\"%s\") \n",
    	pInputBuffer->MethodNameAsUlong,
    	MethodNameAsString(pInputBuffer->MethodNameAsUlong)
    	));
    DebugPrint(("[acpihid.sys]   pInputBuffer->Size: 0x%lx  \n", pInputBuffer->Size));
    DebugPrint(("[acpihid.sys]   pInputBuffer->ArgumentCount: 0x%lx  \n", pInputBuffer->ArgumentCount));
    for (j = 0; j < (int) pInputBuffer->ArgumentCount; j++) {
    	DebugPrint(("[acpihid.sys]   pInputBuffer->Argument[%d].Type: 0x%lx  \n", j, pInputBuffer->Argument[j].Type));
    	DebugPrint(("[acpihid.sys]   pInputBuffer->Argument[%d].DataLength: 0x%lx  \n", j, pInputBuffer->Argument[j].DataLength));
		for (i = 0; i < pInputBuffer->Argument[j].DataLength; i++) {
			DebugPrint(("[acpihid.sys]   pInputBuffer->Argument[%d].Data[%d]: 0x%x  \n", j, i, pInputBuffer->Argument[j].Data[i]));
		}
	}

// execute the ACPI request
    status = ExecuteACPICommand(pdx->LowerDevObj,
							    pInputBuffer,
							    AcpiInBufferSize,
								pOutputBuffer,
								AcpiOutBufferSize
							   );
    DebugPrint(("[acpihid.sys] ExecuteACPIcommand BTNS(%d), call return %lx \n", ButtonId, status));

// check the output result
    if ( NT_SUCCESS( status ))
    {
        DebugPrint(("[acpihid.sys]   pOutputBuffer->Signature: 0x%04lx  \n", pOutputBuffer->Signature));
        DebugPrint(("[acpihid.sys]   pOutputBuffer->Length: 0x%lx  \n",pOutputBuffer->Length));
        DebugPrint(("[acpihid.sys]   pOutputBuffer->Count: 0x%lx  \n",pOutputBuffer->Count));
        for ( j = 0; j < (int) pInputBuffer->ArgumentCount; ++j )
        {
            DebugPrint(("[acpihid.sys]   pOutputBuffer->Argument[%d].Type: 0x%lx  \n", j, pOutputBuffer->Argument[j].Type));
            DebugPrint(("[acpihid.sys]   pOutputBuffer->Argument[%d].DataLength: 0x%lx  \n", j, pOutputBuffer->Argument[j].DataLength));
			for ( i = 0; i < pOutputBuffer->Argument[j].DataLength; ++i )
            {
                DebugPrint(("[acpihid.sys]   pOutputBuffer->Argument[%d].Data[%d]: 0x%x  \n", j, i, pOutputBuffer->Argument[j].Data[i]));
            }
        }

        *ButtonStatus  = (ULONG) pOutputBuffer->Argument[0].Data[0];
        DebugPrint(("[acpihid.sys] BTNS(%d) button status is 0x%lx \n", ButtonId, *ButtonStatus));
    }

    if ( NULL != pInputBuffer )
    {
        ExFreePool( pInputBuffer );
    }

    if ( NULL != pOutputBuffer )
    {
        ExFreePool( pOutputBuffer );
    }

    DebugPrint(("[acpihid.sys] Exiting AcpiButtonStatus, status: 0x%lx\n", status));
    return status;
}

