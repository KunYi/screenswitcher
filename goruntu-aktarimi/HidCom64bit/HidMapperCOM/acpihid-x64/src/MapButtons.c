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
// Module Name: MapButtons.c                                                  //
//                                                                            //
// Abstract: Handle interrupts from the Embedded Control in response to       //
//           button presses by the user.  Map those button presses into       //
//           configured HID codes and push up to the Windows HID stack.       //
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


#ifdef ALLOC_PRAGMA
    #pragma alloc_text( PAGE, StartButtonThread )
	#pragma alloc_text( NONPAGED, ButtonStatusThread )
	#pragma alloc_text( NONPAGED, CompleteButtonRequest )
#endif


//	DEFINITION OF WINDOWS CONSUMER-CONTROL [0C/01] INPUT REPORT and SiLabs LED OUTPUT REPORT
//          Total size (including ReportID): input=3, output=2, feature=0
//
//  05 0c	Usage Page (12, Consumer)
//	09 01	Usage (1, Consumer Control)
//	a1 01	Collection (1, Application)
//	85 01		Report ID (1)
//	75 10		Report Size (16 bits)
//	95 01		Report Count (1)
//	19 00		Usage Minimum (0)
//	2a ff ff	Usage Maximum (65535)
//	15 00		Logical Minimum (0)
//	26 ff ff	Logical Maximum (65535)
//	81 02	   INPUT (Data, Variable, Absolute)
//	75 07		Report Size (7 bits)
//	95 01		Report Count (1)
//	05 08		Usage Page (8, LEDs)
//	19 01		Usage Minimum (1)
//	29 80		Usage Maximum (128) maximum 128 LEDs
//	91 03	   OUTPUT (Constant, Variable, Absolute) for LED number
//	75 01		Report Size (1 bit)
//	95 01		Report Count (1)
//	91 03	   OUTPUT (Constant, Variable, Absolute) LED status bit
//	c0		End Collection
//
//  LAYOUT OF WINDOWS CONSUMER-CONTROL [0C/01] INPUT REPORT
//  Byte	Bit 7		Bit 6		Bit 5		Bit 4		Bit 3		Bit 2		Bit 1		Bit 0
//  ----	--------	--------	----------	---------	-------		-------		---------	--------
//  0		Report ID = 1 ..............................................................................
//  1		HID Button Usage (Least Significant 8 bits) ................................................
//  2       HID Button Usage (Most Significant 8 bits) .................................................
//
//  LAYOUT OF SiLabs CONSUMER-CONTROL [0C/01] OUTPUT REPORT (LEDs)
//  Byte	Bit 7		Bit 6		Bit 5		Bit 4		Bit 3		Bit 2		Bit 1		Bit 0
//  ----	--------	--------	----------	---------	-------		-------		---------	--------
//  0		Report ID = 1 ..............................................................................
//  1		LedStatus	LedNumber (7 bits) .............................................................


//	DEFINITION OF WINDOWS CONSUMER-PROGRAMMABLE-BUTTONS [0C/03] INPUT REPORT
//          Total size (including ReportID): input=3, output=0, feature=0
//
//  05 0c	Usage Page (12, Consumer)
//	09 03	Usage (3, Programmable Buttons)
//	a1 01	Collection (1, Application)
//	85 02		Report ID (2)
//	75 10		Report Size (16 bits)
//	95 01		Report Count (1)
//	19 00		Usage Minimum (0)
//	2a ff ff	Usage Maximum (65535)
//	15 00		Logical Minimum (0)
//	26 ff ff	Logical Maximum (65535)
//	81 02	   INPUT (Data, Variable, Absolute) for Programmable Buttons
//	c0		End Collection
//
//  LAYOUT OF WINDOWS CONSUMER-PROGRAMMABLE-BUTTONS [0C/03] INPUT REPORT
//  Byte	Bit 7		Bit 6		Bit 5		Bit 4		Bit 3		Bit 2		Bit 1		Bit 0
//  ----	--------	--------	----------	---------	-------		-------		---------	--------
//  0		Report ID = 2 ..............................................................................
//  1		HID Button Usage (Least Significant 8 bits) ................................................
//  2       HID Button Usage (Most Significant 8 bits) .................................................


//	DEFINITION OF WINDOWS DESKTOP/KEYBOARD [01/06] INPUT REPORT and LED OUTPUT REPORT (q.v. http://www.microsoft.com/whdc/device/input/w2kbd.mspx)
//          Total size (including ReportID): input=9, output=2, feature=0
//
//  05 01	Usage Page (1, Generic Desktop)
//	09 06	Usage (6, Keyboard)
//	a1 01	Collection (1, Application)
//	85 03		Report ID (3)
//	75 01		Report Size (1 bit)
//	95 08		Report Count (8)
//	05 01		Usage Page (1, Desktop:Keyboard)
//	19 e0		Usage Minimum (224, Keyboard LeftCtrl)
//	29 e7		Usage Maximum (231, Keyboard RightGUI)
//	15 00		Logical Minimum (0, bit off)
//	25 01		Logical Maximum (1, bit on)
//	81 02	   INPUT (Data, Variable, Absolute) for the modifier keys
//	75 08		Report Size (8 bits, i.e., 1 byte)
//	95 01		Report Count (1)
//	81 03	   INPUT (Constant, Variable, Absolute) for the "Reserved" byte
//	75 01		Report Size (1 bit)
//	95 05		Report Count (5)
//	05 08		Usage Page (8, LEDs)
//	19 01		Usage Minimum (1, LED NumLock)
//	29 05		Usage Maximum (5, LED Kana)
//	91 02	   OUTPUT (Data, Variable, Absolute) for the keyboard LEDs
//	75 03		Report Size (3 bits)
//	95 01		Report Count (1)
//	91 03	   OUTPUT (Constant, Variable, Absolute) for the "Reserved" LED bits
//	75 08		Report Size (8 bits, i.e., 1 byte)
//	95 06		Report Count (6)
//	15 00		Logical Minimum (0)
//	25 ff		Logical Maximum (255)
//	05 01		Usage Page (1, Desktop:Keyboard)
//	19 00		Usage Minimum (0, Keyboard NoEvent)
//	29 ff		Usage Maximum (255, Keyboard Reserved)
//	81 00	   INPUT (Data, Array, Absolute)
//	0c		End Collection
//
//	LAYOUT OF WINDOWS DESKTOP/KEYBOARD [01/06] INPUT REPORT
//
//  NOTE: no Report ID used!
//  Byte	Bit 7		Bit 6		Bit 5		Bit 4		Bit 3		Bit 2		Bit 1		Bit 0
//  ----	--------	--------	----------	---------	-------		-------		---------	--------
//  0		RightGUI	RightAlt	RightShift	RightCtrl	LeftGUI		LeftAlt		LeftShift	LeftCtrl
//  1		Reserved (set to 0x00) .....................................................................
//  2		Button Usage from keyboard page ............................................................
//  3		Button Usage from keyboard page ............................................................
//  4		Button Usage from keyboard page ............................................................
//  5		Button Usage from keyboard page ............................................................
//  6		Button Usage from keyboard page ............................................................
//  7		Button Usage from keyboard page ............................................................
//
//  Therefore, Ctrl+Alt+Del = { 0x05, 0x00, 0x4C, 0x00, 0x00, 0x00, 0x00, 0x00 }
//  (you could use 0x50 instead of 0x05, or even 0x14, or 0x41 as alternatives for the Ctrl+Alt part).
//
//  LAYOUT OF WINDOWS DESKTOP/KEYBOARD [01/06] OUTPUT REPORT (for turning on/off keyboard LEDs)
//  NOTE: no Report ID used!
//  Byte	Bit 7		Bit 6		Bit 5		Bit 4		Bit 3		Bit 2		Bit 1		Bit 0
//  ----	--------	--------	----------	---------	-------		-------		---------	--------
//  0		Reserved	Reserved	Reserved	Kana		Compose		ScrollLock	CapsLock	NumLock


//	DEFINITION OF MICROSOFT TABLET-PC-BUTTONS [01/09] INPUT REPORT
//          Total size (including ReportID): input=5, output=0, feature=0
//
//	05 01   Usage Page (1, Generic Desktop)
//	09 09   Usage (9, Tablet PC Buttons)
//	a1 01   Collection (1, Application)
//	85 04       Report ID (4)
//	05 09       Usage Page (9, Button Page)
//	19 01       Usage Minimum (Button 1)
//	29 20       Usage Maximum (Button 32)
//	15 00       Logical Minimum (0)
//	25 01       Logical Maximum (1)
//	75 01       Report Size (1 bit)
//	95 20       Report Count (32 bits)
//	81 02      INPUT (Data, Variable, Absolute) for the button number
//	c0		End Collection
//
//  LAYOUT OF MICROSOFT TABLET-PC-BUTTONS [01/09] INPUT REPORT
//  Byte	Bit 7		Bit 6		Bit 5		Bit 4		Bit 3		Bit 2		Bit 1		Bit 0
//  ----	--------	--------	----------	---------	-------		-------		---------	--------
//  0		Report ID = 4 ..............................................................................
//  1		Button 8    Button 7    Button 6    Button 5    Button 4    Button 3    Button 2    Button 1
//  2       Button 16   Button 15   Button 14   Button 13   Button 12   Button 11   Button 10   Button 9
//  3       Button 24   Button 23   Button 22   Button 21   Button 20   Button 19   Button 18   Button 17
//  4       Button 32   Button 31   Button 30   Button 29   Button 28   Button 27   Button 26   Button 25
//
//  IMPORTANT NOTE:
//     In the BUTTON_INFO struct (see below), buttons on [HID Page 1, HID Usage 9] will be defined using
//     sequential integers (e.g., 1, 2, 3, 4, 5, etc.), but when reported in the HID Input Report packet
//     diagrammed above, it will be converted into a 32-bit-mask (e.g., 0001, 0002, 0004, 0008, 0010, etc.).


//	DEFINITION OF INTEL VIRTUAL BUTTON FEATURE/OUTPUT REPORT
//          Total size (including ReportID): input=0, output=0, feature=3
//
//  05 0c	Usage Page (12, Consumer)
//	09 03	Usage (3, Programmable Buttons)
//	a1 01	Collection (1, Application)
//	85 05		Report ID (5)
//	75 10		Report Size (16 bits)
//	95 01		Report Count (1)
//	19 00		Usage Minimum (0)
//	2a ff ff	Usage Maximum (65535)
//	15 00		Logical Minimum (0)
//	26 ff ff	Logical Maximum (65535)
//	b1 03	   FEATURE (Constant, Variable, Absolute) for Virtual Buttons
//	c0		End Collection
//
//  LAYOUT OF INTEL VIRTUAL BUTTON FEATURE/OUTPUT REPORT
//  Byte	Bit 7		Bit 6		Bit 5		Bit 4		Bit 3		Bit 2		Bit 1		Bit 0
//  ----	--------	--------	----------	---------	-------		-------		---------	--------
//  0		Report ID = 5 ..............................................................................
//  1		HID Button Usage (Least Significant 8 bits) ................................................
//  2       HID Button Usage (Most Significant 8 bits) .................................................


//	DEFINITION OF INTEL BLUETOOTH-STETHOSCOPE-BUTTONS [40/02] INPUT/OUTPUT REPORT
//          Total size (including ReportID): input=3, output=3, feature=0
//
//  05 40	Usage Page (64, Medical Instrument Page)
//	09 02	Usage (2, Stethoscope)
//	a1 01	Collection (1, Application)
//	85 06		Report ID (6)
//	75 10		Report Size (16 bits)
//	95 01		Report Count (1)
//	19 00		Usage Minimum (0)
//	2a ff ff	Usage Maximum (65535)
//	15 00		Logical Minimum (0)
//	26 ff ff	Logical Maximum (65535)
//	81 02	   INPUT (Data, Variable, Absolute) for Stethoscope Buttons
//	75 10		Report Size (16 bits)
//	95 01		Report Count (1)
//	19 00		Usage Minimum (0)
//	2a ff ff	Usage Maximum (65535)
//	15 00		Logical Minimum (0)
//	26 ff ff	Logical Maximum (65535)
//	91 02	   OUTPUT (Data, Variable, Absolute) for Stethoscope Buttons
//	c0		End Collection
//
//  LAYOUT OF WINDOWS CONSUMER-PROGRAMMABLE-BUTTONS [0C/03] INPUT REPORT
//  Byte	Bit 7		Bit 6		Bit 5		Bit 4		Bit 3		Bit 2		Bit 1		Bit 0
//  ----	--------	--------	----------	---------	-------		-------		---------	--------
//  0		Report ID = 6 ..............................................................................
//  1		HID Button Usage (Least Significant 8 bits) ................................................
//  2       HID Button Usage (Most Significant 8 bits) .................................................


//	DEFINITION OF WINDOWS MEDIA-CENTER REMOTE [FFBC/88] INPUT REPORT
//          Total size (including ReportID): input=3, output=0, feature=0
//
//  06 bc ff Usage Page (0xffbc, Windows Media Center) -- N.B. byte reversal!
//	09 88	Usage (0x88, MCE Buttons)
//	a1 01	Collection (1, Application)
//	85 07		Report ID (7)
//	75 10		Report Size (16 bits)
//	95 01		Report Count (1)
//	19 00		Usage Minimum (0)
//	2a ff ff	Usage Maximum (65535)
//	15 00		Logical Minimum (0)
//	26 ff ff	Logical Maximum (65535)
//	81 02	   INPUT (Data, Variable, Absolute)
//	c0		End Collection
//
//  LAYOUT OF WINDOWS MEDIA-CENTER REMOTE [FFBC/88] INPUT REPORT
//  Byte	Bit 7		Bit 6		Bit 5		Bit 4		Bit 3		Bit 2		Bit 1		Bit 0
//  ----	--------	--------	----------	---------	-------		-------		---------	--------
//  0		Report ID = 7 ..............................................................................
//  1		HID Button Usage (Least Significant 8 bits) ................................................
//  2       HID Button Usage (Most Significant 8 bits) .................................................
//


//	DEFINITION OF WINDOWS MEDIA-CENTER REMOTE OEM [FFBC/89] INPUT REPORT
//          Total size (including ReportID): input=3, output=0, feature=0
//
//  06 bc ff Usage Page (0xffbc, Windows Media Center) -- N.B. byte reversal!
//	09 89	Usage (0x89, MCE OEM Buttons)
//	a1 01	Collection (1, Application)
//	85 08		Report ID (8)
//	75 10		Report Size (16 bits)
//	95 01		Report Count (1)
//	19 00		Usage Minimum (0)
//	2a ff ff	Usage Maximum (65535)
//	15 00		Logical Minimum (0)
//	26 ff ff	Logical Maximum (65535)
//	81 02	   INPUT (Data, Variable, Absolute)
//	c0		End Collection
//
//  LAYOUT OF WINDOWS MEDIA-CENTER REMOTE OEM [FFBC/89] INPUT REPORT
//  Byte	Bit 7		Bit 6		Bit 5		Bit 4		Bit 3		Bit 2		Bit 1		Bit 0
//  ----	--------	--------	----------	---------	-------		-------		---------	--------
//  0		Report ID = 8 ..............................................................................
//  1		HID Button Usage (Least Significant 8 bits) ................................................
//  2       HID Button Usage (Most Significant 8 bits) .................................................
//


//  LOOK-UP TABLE FOR SOFT BUTTONS
//  Table structure member order
//  ReportID, UsagePage, Usage, ButtonUsageIdMsb, ButtonUsageIdLsb, KeyStatus/Flags
//  REPORT ID KEY (Maps to HID devices):
//    Report ID 1: Consumer Control Page, Function buttons 0c/01
//    Report ID 2: Consumer Control Page, Generic buttons 0c/03
//    Report ID 3: Desktop Keyboard Page, 01/06
//    Report ID 4: Tablet PC Buttons Page, 01/09
//    Report ID 5: Virtual Buttons (we use 0c/03)
//    Report ID 6: Medical Device Page, Stethoscope usage 40/02
//    Report ID 7: MCE Buttons, FFBC/88
//    Report ID 8: MCE OEM Buttons, FFBC/89
//  KEY-STATUS/FLAGS KEY:
//    0x01: Use Registry value instead of built-in values from table below.
//    0x02: This button also sends Ctrl+Alt+Del HID = [01/07/e2,e4,4c]

static BUTTON_INFO  Buttons [] = {			// Consumer multimedia soft buttons
	{ 0x1, 0x00, 0x0C, 0x00, 0x01, 0x00, 0x38, 0x0 },	//  0: Button Hold Notice [0c/01/38]
	{ 0x1, 0x00, 0x0C, 0x00, 0x01, 0x00, 0x37, 0x0 },	//  1: Screen Image Rotation [0c/01/37]
	{ 0x1, 0x00, 0x0C, 0x00, 0x01, 0x01, 0xAD, 0x0 },	//  2: Wireless Activation (RF Kill) [0c/01/1ad]
	{ 0x1, 0x00, 0x0C, 0x00, 0x01, 0x01, 0x9C, 0x2 },	//  3: Logoff [0c/01/19c] + SAS
	{ 0x1, 0x00, 0x0C, 0x00, 0x01, 0x01, 0xBB, 0x0 },	//  4: Bluetooth Pairing [0c/01/1bb]
	{ 0x1, 0x00, 0x0C, 0x00, 0x01, 0x01, 0x0D, 0x0 },	//  5: Patient Alert [0c/01/10d]
	{ 0x1, 0x00, 0x0C, 0x00, 0x01, 0x00, 0xB2, 0x0 },	//  6: Voice Anotation recording [0c/01/b2]
	{ 0x4, 0x00, 0x01, 0x00, 0x09, 0x00, 0x01, 0x0 },	//  7: Medical Application #1 [01/09/01]
	{ 0x8, 0xff, 0xbc, 0x00, 0x89, 0x00, 0x80, 0x0 },	//  8: MCE OEM1 [ffbc/89/80]
	{ 0x7, 0xff, 0xbc, 0x00, 0x88, 0x00, 0x47, 0x0 },	//  9: My Music [ffbc/88/47]
	{ 0x1, 0x00, 0x0C, 0x00, 0x01, 0x01, 0x0E, 0x0 },	// 10: Scan Barcode/RFID [0c/01/10e]
	{ 0x1, 0x00, 0x0C, 0x00, 0x01, 0x00, 0x67, 0x0 },	// 11: Photo Shutter 1/2-way [0c/01/67]
	{ 0x1, 0x00, 0x0C, 0x00, 0x01, 0x00, 0x65, 0x0 }	// 12: Photo Shutter full-way [0c/01/65]
};

//#pragma LOCKEDCODE

//*
NTSTATUS  StartButtonThread(PDEVICE_EXTENSION pdx)
    /*++

Routine Description:





Arguments:



Return Value:

    NT status code.

--*/
{
    NTSTATUS ntStatus = STATUS_SUCCESS;
    HANDLE          ThreadHandle;
    OBJECT_ATTRIBUTES objAttrib;

	PAGED_CODE();

    DebugPrint(("[acpihid.sys] Found  soft button, Starting the Button Thread\n"));

// Start the thread - save referenced pointer to thread in our extension
    InitializeObjectAttributes( &objAttrib, NULL, OBJ_KERNEL_HANDLE, NULL, NULL );

    ntStatus  = PsCreateSystemThread(
        &ThreadHandle,
        THREAD_ALL_ACCESS,
        &objAttrib,
        NULL,
        NULL,
        ButtonStatusThread,
        pdx );

    if ( ! NT_ERROR( ntStatus ))
    {
        DebugPrint(( "[acpihid.sys] PsCreateSystemThread success, ntStatus: 0x%lx \n", ntStatus));
    }
    else
    {
        DebugPrint(( "[acpihid.sys] PsCreateSystemThread -  FAIL, ntStatus: 0x%lx\n", ntStatus ));
    }

    return ntStatus;
}




VOID
ButtonStatusThread(IN PVOID Context)
{
    NTSTATUS				ntStatus = STATUS_SUCCESS;
    PDEVICE_EXTENSION		deviceInfo;
    PIRP					Irp;
    PIO_STACK_LOCATION		IrpStack;
    ULONG					ButtonNo = 0;
    int						i;
    ULONG					btnBitmask = 0;

	PAGED_CODE();

    DebugPrint(("[acpihid.sys] ButtonStatusThread Entry\n"));

    // Get the Irp from context

    deviceInfo = (PDEVICE_EXTENSION) Context;
    if (NULL == deviceInfo) return;  // protect from faults if we get events before we're ready.
    Irp = deviceInfo->ReadReportIrp;
    if (NULL == Irp) return;  // protect from faults if we get events before we're ready.
    IrpStack = IoGetCurrentIrpStackLocation(Irp);
    if (NULL == IrpStack) return;  // protect from faults if we get events before we're ready.

	// Figure out which button was pressed

    ntStatus = AcpiButtonStatus(deviceInfo,
		0,  // when we ask for BTNS(0), that's asking for the number of the button most recently pressed
		&ButtonNo);

	// Do the mapping and send keypress

	if (!NT_ERROR(ntStatus)) {

		ButtonNo = ButtonNo & 0xff;
        DebugPrint(("[acpihid.sys] AcpiButtonStatus successful detection, ButtonNo: 0x%04lx \n", ButtonNo));

        // check to see if we are in "button hold" or not.
		if (deviceInfo->DeviceActivatedFlag == FALSE) {
	        DebugPrint(("[acpihid.sys] Button report suppressed due to 'button hold'. \n"));
	        // substitute with "button 0", the "hold button code".
	        ButtonNo = 0;
		}

		// Map buttons from BIOS button number to HID code
		if (ButtonNo <= deviceInfo->NumberOfButtons) {

			// zero out report buffer
			for (i = 0;  i < (int) IrpStack->Parameters.DeviceIoControl.OutputBufferLength; i++) {
				deviceInfo->ReportBuffer[i] = 0x00;
			}

			// Check to see whether to source HID codes from Registry or use built-in table
			if ((deviceInfo->Buttons[ButtonNo].KeyStatus & 0x01) == 0x1) {  // Use Registry

				if ((((deviceInfo->Buttons[ButtonNo].UsagePageMsb << 8) | deviceInfo->Buttons[ButtonNo].UsagePageLsb) == 0x01) &&
				    (((deviceInfo->Buttons[ButtonNo].UsageMsb << 8) | deviceInfo->Buttons[ButtonNo].UsageLsb) == 0x09)) {
					// handle special input report formatting for Tablet PC Buttons
					// convert button number to single bit in 32-bit mask, e.g., button 4 -> 00000000 00000000 00000000 00001000
					btnBitmask = 1 << (((deviceInfo->Buttons[ButtonNo].ButtonUsageMsb << 8) | deviceInfo->Buttons[ButtonNo].ButtonUsageLsb) - 1);
					// Sending Make
					deviceInfo->ReportBuffer[0] = deviceInfo->Buttons[ButtonNo].ReportID;
					deviceInfo->ReportBuffer[1] = (UCHAR)(btnBitmask & 0x0f);
				    deviceInfo->ReportBuffer[2] = (UCHAR)((btnBitmask >> 8) & 0x0f);
				    deviceInfo->ReportBuffer[3] = (UCHAR)((btnBitmask >> 16) & 0x0f);
				    deviceInfo->ReportBuffer[4] = (UCHAR)((btnBitmask >> 24) & 0x0f);
			        DebugPrint(("[acpihid.sys] AcpiButtonStatus Mapped HID via Registry, ReportID: 0x%x, HID: 0x%04lx,0x%04lx,0x%04lx MAKE \n",
						   deviceInfo->Buttons[ButtonNo].ReportID,
						   (deviceInfo->Buttons[ButtonNo].UsagePageMsb << 8) | deviceInfo->Buttons[ButtonNo].UsagePageLsb,
						   (deviceInfo->Buttons[ButtonNo].UsageMsb << 8) | deviceInfo->Buttons[ButtonNo].UsageLsb,
						   (deviceInfo->Buttons[ButtonNo].ButtonUsageMsb << 8) | deviceInfo->Buttons[ButtonNo].ButtonUsageLsb));
					CompleteButtonRequest(deviceInfo);

					// Sending Break
					deviceInfo->ReportBuffer[0] = deviceInfo->Buttons[ButtonNo].ReportID;
					deviceInfo->ReportBuffer[1] = 0x00;
					deviceInfo->ReportBuffer[2] = 0x00;
					deviceInfo->ReportBuffer[3] = 0x00;
					deviceInfo->ReportBuffer[4] = 0x00;
			        DebugPrint(("[acpihid.sys] AcpiButtonStatus Mapped HID via Registry, ReportID: 0x%x, HID: 0x%04lx,0x%04lx,0x%04lx BREAK \n",
						   deviceInfo->Buttons[ButtonNo].ReportID,
						   (deviceInfo->Buttons[ButtonNo].UsagePageMsb << 8) | deviceInfo->Buttons[ButtonNo].UsagePageLsb,
						   (deviceInfo->Buttons[ButtonNo].UsageMsb << 8) | deviceInfo->Buttons[ButtonNo].UsageLsb,
						   (deviceInfo->Buttons[ButtonNo].ButtonUsageMsb << 8) | deviceInfo->Buttons[ButtonNo].ButtonUsageLsb));
					CompleteButtonRequest(deviceInfo);
				} else {
					// handle normal buttons
					// Sending Make
					deviceInfo->ReportBuffer[0] = deviceInfo->Buttons[ButtonNo].ReportID;
					deviceInfo->ReportBuffer[1] = deviceInfo->Buttons[ButtonNo].ButtonUsageLsb;
				    deviceInfo->ReportBuffer[2] = deviceInfo->Buttons[ButtonNo].ButtonUsageMsb;
			        DebugPrint(("[acpihid.sys] AcpiButtonStatus Mapped HID via Registry, ReportID: 0x%x, HID: 0x%04lx,0x%04lx,0x%04lx MAKE \n",
						   deviceInfo->Buttons[ButtonNo].ReportID,
						   (deviceInfo->Buttons[ButtonNo].UsagePageMsb << 8) | deviceInfo->Buttons[ButtonNo].UsagePageLsb,
						   (deviceInfo->Buttons[ButtonNo].UsageMsb << 8) | deviceInfo->Buttons[ButtonNo].UsageLsb,
						   (deviceInfo->Buttons[ButtonNo].ButtonUsageMsb << 8) | deviceInfo->Buttons[ButtonNo].ButtonUsageLsb));
					CompleteButtonRequest(deviceInfo);

					// Sending Break
					deviceInfo->ReportBuffer[0] = deviceInfo->Buttons[ButtonNo].ReportID;
					deviceInfo->ReportBuffer[1] = 0x00;
					deviceInfo->ReportBuffer[2] = 0x00;
			        DebugPrint(("[acpihid.sys] AcpiButtonStatus Mapped HID via Registry, ReportID: 0x%x, HID: 0x%04lx,0x%04lx,0x%04lx BREAK \n",
						   deviceInfo->Buttons[ButtonNo].ReportID,
						   (deviceInfo->Buttons[ButtonNo].UsagePageMsb << 8) | deviceInfo->Buttons[ButtonNo].UsagePageLsb,
						   (deviceInfo->Buttons[ButtonNo].UsageMsb << 8) | deviceInfo->Buttons[ButtonNo].UsageLsb,
						   (deviceInfo->Buttons[ButtonNo].ButtonUsageMsb << 8) | deviceInfo->Buttons[ButtonNo].ButtonUsageLsb));
					CompleteButtonRequest(deviceInfo);
				}

			} else {  // Use Built-In Table

				if ((((Buttons[ButtonNo].UsagePageMsb << 8) | Buttons[ButtonNo].UsagePageLsb) == 0x01) &&
				    (((Buttons[ButtonNo].UsageMsb << 8) | Buttons[ButtonNo].UsageLsb) == 0x09)) {
					// handle special input report formatting for Tablet PC Buttons
					// convert button number to single bit in 32-bit mask, e.g., button 4 -> 00000000 00000000 00000000 00001000
					btnBitmask = 1 << (((Buttons[ButtonNo].ButtonUsageMsb << 8) | Buttons[ButtonNo].ButtonUsageLsb) - 1);
					// Sending Make
					deviceInfo->ReportBuffer[0] = Buttons[ButtonNo].ReportID;
					deviceInfo->ReportBuffer[1] = (UCHAR)(btnBitmask & 0x0f);
				    deviceInfo->ReportBuffer[2] = (UCHAR)((btnBitmask >> 8) & 0x0f);
				    deviceInfo->ReportBuffer[3] = (UCHAR)((btnBitmask >> 16) & 0x0f);
				    deviceInfo->ReportBuffer[4] = (UCHAR)((btnBitmask >> 24) & 0x0f);
		        	DebugPrint(("[acpihid.sys] AcpiButtonStatus Mapped HID via Hard-coding, ReportID: 0x%x, HID: 0x%04lx,0x%04lx,0x%04lx MAKE \n",
						   Buttons[ButtonNo].ReportID,
						   (Buttons[ButtonNo].UsagePageMsb << 8) | Buttons[ButtonNo].UsagePageLsb,
						   (Buttons[ButtonNo].UsageMsb << 8) | Buttons[ButtonNo].UsageLsb,
						   (Buttons[ButtonNo].ButtonUsageMsb << 8) | Buttons[ButtonNo].ButtonUsageLsb));
					CompleteButtonRequest(deviceInfo);

					// Sending Break
					deviceInfo->ReportBuffer[0] = Buttons[ButtonNo].ReportID;
					deviceInfo->ReportBuffer[1] = 0x00;
					deviceInfo->ReportBuffer[2] = 0x00;
					deviceInfo->ReportBuffer[3] = 0x00;
					deviceInfo->ReportBuffer[4] = 0x00;
		        	DebugPrint(("[acpihid.sys] AcpiButtonStatus Mapped HID via Hard-coding, ReportID: 0x%x, HID: 0x%04lx,0x%04lx,0x%04lx BREAK \n",
						   Buttons[ButtonNo].ReportID,
						   (Buttons[ButtonNo].UsagePageMsb << 8) | Buttons[ButtonNo].UsagePageLsb,
						   (Buttons[ButtonNo].UsageMsb << 8) | Buttons[ButtonNo].UsageLsb,
						   (Buttons[ButtonNo].ButtonUsageMsb << 8) | Buttons[ButtonNo].ButtonUsageLsb));
					CompleteButtonRequest(deviceInfo);
				} else {
					// handle normal buttons
					// Sending Make
					deviceInfo->ReportBuffer[0] = Buttons[ButtonNo].ReportID;
					deviceInfo->ReportBuffer[1] = Buttons[ButtonNo].ButtonUsageLsb;
			    	deviceInfo->ReportBuffer[2] = Buttons[ButtonNo].ButtonUsageMsb;
		        	DebugPrint(("[acpihid.sys] AcpiButtonStatus Mapped HID via Hard-coding, ReportID: 0x%x, HID: 0x%04lx,0x%04lx,0x%04lx MAKE \n",
						   Buttons[ButtonNo].ReportID,
						   (Buttons[ButtonNo].UsagePageMsb << 8) | Buttons[ButtonNo].UsagePageLsb,
						   (Buttons[ButtonNo].UsageMsb << 8) | Buttons[ButtonNo].UsageLsb,
						   (Buttons[ButtonNo].ButtonUsageMsb << 8) | Buttons[ButtonNo].ButtonUsageLsb));
					CompleteButtonRequest(deviceInfo);

					// Sending Break
					deviceInfo->ReportBuffer[0] = Buttons[ButtonNo].ReportID;
					deviceInfo->ReportBuffer[1] = 0x00;
					deviceInfo->ReportBuffer[2] = 0x00;
		        	DebugPrint(("[acpihid.sys] AcpiButtonStatus Mapped HID via Hard-coding, ReportID: 0x%x, HID: 0x%04lx,0x%04lx,0x%04lx BREAK \n",
						   Buttons[ButtonNo].ReportID,
						   (Buttons[ButtonNo].UsagePageMsb << 8) | Buttons[ButtonNo].UsagePageLsb,
						   (Buttons[ButtonNo].UsageMsb << 8) | Buttons[ButtonNo].UsageLsb,
						   (Buttons[ButtonNo].ButtonUsageMsb << 8) | Buttons[ButtonNo].ButtonUsageLsb));
					CompleteButtonRequest(deviceInfo);
				}
			}

			// zero out report buffer
			for (i = 0;  i < (int) IrpStack->Parameters.DeviceIoControl.OutputBufferLength; i++) {
				deviceInfo->ReportBuffer[i] = 0x00;
			}

			// Check to see whether to emit the Ctrl+Alt+Del security action sequence
			if ((deviceInfo->Buttons[ButtonNo].KeyStatus & 0x02) == 0x02) {  // Do the SAS
				// N.B.: it is critical, for this to work at all, that the HID Report Descriptor be correct!!!!
				// Sending Make
				deviceInfo->ReportBuffer[0] = 0x03; // Report ID 0x03 for Page 0x01 / Usage 0x06.
				deviceInfo->ReportBuffer[1] = 0x05; // "Modifier Flags" for LeftCtrl+LeftAlt
				deviceInfo->ReportBuffer[2] = 0x00; // "Reserved"
				deviceInfo->ReportBuffer[3] = 0x4C; // Delete key (see "USB HID Usage Tables 1.12" document from http://www.usb.org)
				deviceInfo->ReportBuffer[4] = 0x00;
				deviceInfo->ReportBuffer[5] = 0x00;
				deviceInfo->ReportBuffer[6] = 0x00;
				deviceInfo->ReportBuffer[7] = 0x00;
				deviceInfo->ReportBuffer[8] = 0x00;
			    DebugPrint(("[acpihid.sys] AcpiButtonStatus Sending Ctrl+Alt+Delete, ReportID: 0x%x, HID: 0x%04lx,0x%04lx,0x%02lx+0x%02lx MAKE \n",
			    	deviceInfo->ReportBuffer[0], 1, 6, deviceInfo->ReportBuffer[1], deviceInfo->ReportBuffer[3]));
				CompleteButtonRequest(deviceInfo);
				// Sending Break
				deviceInfo->ReportBuffer[0] = 0x03; // Report ID 0x03 for Page 0x01 / Usage 0x07.
				deviceInfo->ReportBuffer[1] = 0x00;
				deviceInfo->ReportBuffer[2] = 0x00;
				deviceInfo->ReportBuffer[3] = 0x00;
				deviceInfo->ReportBuffer[4] = 0x00;
				deviceInfo->ReportBuffer[5] = 0x00;
				deviceInfo->ReportBuffer[6] = 0x00;
				deviceInfo->ReportBuffer[7] = 0x00;
				deviceInfo->ReportBuffer[8] = 0x00;
			    DebugPrint(("[acpihid.sys] AcpiButtonStatus Sending Ctrl+Alt+Delete, ReportID: 0x%x, HID: 0x%04lx,0x%04lx,0x%02lx+0x%02lx BREAK \n",
			    	deviceInfo->ReportBuffer[0], 1, 6, deviceInfo->ReportBuffer[1], deviceInfo->ReportBuffer[3]));
				CompleteButtonRequest(deviceInfo);
			}

		} else {
		     DebugPrint(("[acpihid.sys] AcpiButtonStatus returned Invalid Button No: 0x%x\n", ButtonNo));
		}
	} else {

		DebugPrint(("[acpihid.sys] AcpiButtonStatus failed, ntStatus: 0x%lx \n", ntStatus));

	}
}


//*
NTSTATUS CompleteButtonRequest(PDEVICE_EXTENSION pdx)
/*++

Routine Description:

     Complete the IO request.

Arguments:

    DEVICE_EXTENSION.


Return Value:

    NT status code.

--*/

{

    NTSTATUS  status      =  STATUS_SUCCESS;
    ULONG     bytesToCopy =  REPORT_BUF_SIZE;
    PIO_STACK_LOCATION	IrpStack;
    KIRQL				OldIrql;
    PDRIVER_CANCEL		OldCancelRoutine;

  	IrpStack = IoGetCurrentIrpStackLocation(pdx->ReadReportIrp);

	DebugPrint(("[acpihid.sys] CompleteRequest: IrpStack->Parameters.DeviceIoControl.OutputBufferLength = 0x%x, need = 0x%x\n",
		IrpStack->Parameters.DeviceIoControl.OutputBufferLength,
		bytesToCopy
		));

	if (IrpStack->Parameters.DeviceIoControl.OutputBufferLength < bytesToCopy) {
		DebugPrint(("[acpihid.sys] CompleteRequest: Buffer too small, output = 0x%x, need = 0x%x\n",
			IrpStack->Parameters.DeviceIoControl.OutputBufferLength,
			bytesToCopy
			));

		status = STATUS_BUFFER_TOO_SMALL;
		return status;
	}

	RtlCopyMemory(pdx->ReadReportIrp->UserBuffer, pdx->ReportBuffer, bytesToCopy);

    //
    // Report how many bytes were copied
    //
	pdx->ReadReportIrp->IoStatus.Information = bytesToCopy;

	pdx->ReadReportIrp->IoStatus.Status = status;

    KeAcquireSpinLock(&pdx->SpinLock, &OldIrql);
    OldCancelRoutine = IoSetCancelRoutine(pdx->ReadReportIrp, NULL);
    KeReleaseSpinLock(&pdx->SpinLock, OldIrql);

	IoCompleteRequest(pdx->ReadReportIrp, IO_NO_INCREMENT);

    pdx->PendingIrpCount  -=1;
	return status;
}							// CompleteRequest
// */