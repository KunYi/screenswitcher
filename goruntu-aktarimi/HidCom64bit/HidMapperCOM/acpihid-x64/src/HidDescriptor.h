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
// Module Name: HidDescriptor.h                                               //
//                                                                            //
// Abstract: Definition of HID Report Descriptor                              //
//                                                                            //
// Authors: Babu Pallithanam                                                  //
//          Jim Trethewey                                                     //
//                                                                            //
// Environment: kernel mode only                                              //
//                                                                            //
// Revision History:                                                          //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////


#define HIDMINI_VID             0x8086	// 8086 = "Intel"
#define HIDMINI_PID             0x1018	// 1018 = "ACPI-to-HID Mapper"
#define HIDMINI_VERSION         0x0010


// Driver Identification Strings
const wchar_t *String1Desc = L"Intel";
const wchar_t *String2Desc = L"Intel ACPI-to-HID Mapper";
const wchar_t *String3Desc = L"8086.1018";


HID_REPORT_DESCRIPTOR  DefaultReportDescriptor[] = {

        0x05,  0x0c,		            //  Global, Usage Page (12, Consumer)
		0x09,  0x01,					//  Local,  Usage  (1, Consumer Controls)
		0xa1,  0x01,			        //  Main,   Collection (1, Application)
		0x85,  0x01,                    //  Global,    Report ID (1)
		0x19,  0x00,				    //  Local,     Usage Minimum (0)
		0x2a,  0xff,  0xff,				//  Local      Usage Maximum (65535)
		0x15,  0x00,					//  Global,    Logical Minimum (0)
		0x26,  0xff,  0xff,             //  Global,    Logical Maximum (65535)
		0x95,  0x01,					//  Local,     Report Count (1)
		0x75,  0x10,					//  Global,    Report Size (16)
		0x81,  0x00,					//  Main,     INPUT (Data, Variable, Absolute)
		0x95,  0x01,					//  Global,    Report count (1)
		0x75,  0x07,                    //  Global,    Report size  7 bits
		0x05,  0x08,					//  Global,    Usage Page (8, LEDs)
		0x19,  0x01,                    //  Local,     Usage Minimum (1)
		0x29,  0x7F,                    //  Local,     Usage Maximum (128)   // 128 LEDs
		0x91,  0x02,                    //  Main,     OUTPUT (Data, Variable, Absolute)
		0x95,  0x01,                    //  Global,    report count (1)
		0x75,  0x01,                    //  Global,    report size (1)   //Led turn ON and OFF
		0x91,  0x01,                    //  Main,     OUTPUT constant  // 1  bit  padding  to show the status of LEDs
		0x75,  0x08,					//  Global,    Report Size (8 bits)
		0x95,  0x07,					//  Local,     Report Count (7)
		0x19,  0x00,					//  Local,     Usage Minimum (0)
		0x29,  0xff,					//  Local,     Usage Maximum (255)
		0x15,  0x00,					//  Global,    Logical Minimum (0)
		0x25,  0xff,					//  Global,    Logical Maximum (255)
		0x91,  0x02,					//  Main,     OUTPUT (Data, Variable, Absolute) padding out to 9 bytes total
		0xc0,							//          End  collection

        0x05,  0x0c,					//  Global, Usage Page (12, Consumer)
		0x09,  0x03,					//  Local,  Usage (3, Programmable Buttons)
		0xa1,  0x01,					//  Main,   Collection (1, Application)
		0xa1,  0x02,					//  Main,      Collection (2, Logical)
		0x85,  0x02,					//  Global,       Report ID (2)
		0x19,  0x00,				    //  Local,        Usage Minimum (0)
		0x2a,  0xff,  0xff,				//  Local         Usage Maximum (65535)
		0x15,  0x00,					//  Global,       Logical Minimum (0)
		0x26,  0xff,  0xff,             //  Global,       Logical Maximum (65535)
		0x95,  0x01,					//  Local,        Report Count (1)
		0x75,  0x10,					//  Global,       Report Size (16)
		0x81,  0x00,					//  Main,        INPUT (Data, Array, Absolute)
		0x75,  0x08,					//  Global,       Report Size (8 bits)
		0x95,  0x08,					//  Local,        Report Count (8)
		0x19,  0x00,					//  Local,        Usage Minimum (0)
		0x29,  0xff,					//  Local,        Usage Maximum (255)
		0x15,  0x00,					//  Global,       Logical Minimum (0)
		0x25,  0xff,					//  Global,       Logical Maximum (255)
		0x91,  0x02,					//  Main,        OUTPUT (Data, Variable, Absolute) padding out to 9 bytes total
		0xc0,							//             End  collection
		0xa1,  0x02,					//  Main,      Collection (2, Logical)
		0x85,  0x05,					//  Global,       Report ID (5)
		0x05,  0x09,					//  Global,       Usage Page  (9, Button)
        0x09,  0x03,					//  Local,        Usage (1, Button)
        0x26,  0xff,  0xff,				//  Global,       Logical Maximum (65535)
        0x75,  0x10,					//  Global,       Report Size (16)
        0xb1,  0x02,					//  Main,        FEATURE (Data, Variable, Absolute)
        0x75,  0x10,					//  Global,       Report Size (16)
        0xb1,  0x03,					//  Main,        FEATURE (Constant, Variable, Absolute)
        0xc0,							//             End  collection
        0xc0,							//          End  collection

		0x05,  0x01,					//  Global, Usage Page (1, Desktop)
		0x09,  0x06,					//  Local,  Usage (6, Keyboard)
		0xa1,  0x01,					//  Local,  Collection (1, Application)
		0x85,  0x03,					//  Global,    Report ID (3)
		0x05,  0x07,					//  Global,    Usage Page (7, Keyboard/Keypad)
		0x19,  0xe0,					//  Local,     Usage Minimum (224, Keyboard LeftCtrl)
		0x29,  0xe7,					//  Local,     Usage Maximum (231, Keyboard RightGUI)
		0x75,  0x01,					//  Global,    Report Size (1 bit)
		0x95,  0x08,					//  Local,     Report Count (8, i.e., 8 bits)
		0x81,  0x02,					//  Main,     INPUT (Data, Variable, Absolute) for the Modifier Key bits
		0x75,  0x08,					//  Global,    Report Size (8 bits, i.e., 1 byte)
		0x95,  0x01,					//  Local,     Report Count (1)
		0x81,  0x03,					//  Main,     INPUT (Constant, Variable, Absolute) for the "Reserved" byte
		0x19,  0x00,					//  Local,     Usage Minimum (0)
		0x29,  0xdd,					//  Local,     Usage Maximum (221, Keypad Hexadecimal)
		0x25,  0xff,					//  Global,    Logical Maximum (255)
		0x75,  0x08,					//  Global,    Report Size (8 bits)
		0x95,  0x06,					//  Local,     Report Count (1 byte)
		0x81,  0x00,					//  Main,     INPUT (Data, Array, Absolute)
		0x75,  0x08,					//  Global,    Report Size (8 bits)
		0x95,  0x08,					//  Local,     Report Count (8)
		0x19,  0x00,					//  Local,     Usage Minimum (0)
		0x29,  0xff,					//  Local,     Usage Maximum (255)
		0x15,  0x00,					//  Global,    Logical Minimum (0)
		0x25,  0xff,					//  Global,    Logical Maximum (255)
		0x91,  0x02,					//  Main,     OUTPUT (Data, Variable, Absolute) padding out to 9 bytes total
		0xc0,							//  		End collection

		0x05,  0x40,					//  Global, Usage Page (64, Medical Instrument Page)
		0x09,  0x02,					//  Local,  Usage (2, Stethoscope)
		0xa1,  0x01,					//  Local,  Collection (1, Application)
		0x85,  0x06,					//  Global,    Report ID (6)
		0x75,  0x10,					//  Global,    Report Size (16 bits)
		0x95,  0x01,					//  Local,     Report Count (1)
		0x19,  0x00,					//  Local,     Usage Minimum (0)
		0x2a,  0xff,  0xff,				//  Local,     Usage Maximum (65535)
		0x15,  0x00,					//  Global,    Logical Minimum (0)
		0x26,  0xff,  0xff,				//  Global,    Logical Maximum (65535)
		0x81,  0x02,					//  Main,     INPUT (Data, Variable, Absolute) for Stethoscope Buttons
		0x75,  0x08,					//  Global,    Report Size (8 bits)
		0x95,  0x06,					//  Local,     Report Count (6)
		0x19,  0x00,					//  Local,     Usage Minimum (0)
		0x29,  0xff,					//  Local,     Usage Maximum (255)
		0x15,  0x00,					//  Global,    Logical Minimum (0)
		0x25,  0xff,					//  Global,    Logical Maximum (255)
		0x81,  0x02,					//  Main,     INPUT (Data, Variable, Absolute) padding out to 9 bytes total
		0x75,  0x10,					//  Global,    Report Size (16 bits)
		0x95,  0x01,					//  Local,     Report Count (1)
		0x19,  0x00,					//  Local,     Usage Minimum (0)
		0x2a,  0xff,  0xff,				//  Local,     Usage Maximum (65535)
		0x15,  0x00,					//  Global,    Logical Minimum (0)
		0x26,  0xff,  0xff,				//  Global,    Logical Maximum (65535)
		0x91,  0x02,					//  Main,     OUTPUT (Data, Variable, Absolute) for Stethoscope Buttons
		0x75,  0x08,					//  Global,    Report Size (8 bits)
		0x95,  0x06,					//  Local,     Report Count (6)
		0x19,  0x00,					//  Local,     Usage Minimum (0)
		0x29,  0xff,					//  Local,     Usage Maximum (255)
		0x15,  0x00,					//  Global,    Logical Minimum (0)
		0x25,  0xff,					//  Global,    Logical Maximum (255)
		0x91,  0x02,					//  Main,     OUTPUT (Data, Variable, Absolute) padding out to 9 bytes total
		0xc0,							//          End Collection

		0x05,  0x01,					//  Global, Usage Page (1, Generic Desktop)
		0x09,  0x09,					//  Local,  Usage (9, Tablet PC Buttons)
		0xa1,  0x01,					//  Local,  Collection (1, Application)
		0x85,  0x04,					//  Global,    Report ID (4)
		0x05,  0x09,					//  Global,    Usage Page (9, Button Page)
		0x19,  0x01,					//  Local,     Usage Minimum (Button 1)
		0x29,  0x20,					//  Local      Usage Maximum (Button 32)
		0x15,  0x00,					//  Global,    Logical Minimum (0)
		0x25,  0x01,					//  Global,    Logical Maximum (1)
		0x75,  0x01,					//  Global,    Report Size (1 bit)
		0x95,  0x20,					//  Local,     Report Count (32 bits)
		0x81,  0x02,					//  Main,     INPUT (Data, Variable, Absolute) for the button number
		0xc0,							//          End Collection

		0x06,  0xbc,  0xff,				//  Global, Usage Page (0xffbc, Windows MCE Buttons) -- N.B. byte reversal!
		0x09,  0x88,					//  Local,  Usage (0x88, MCE Buttons)
		0xa1,  0x01,					//  Local,  Collection (1, Application)
		0x85,  0x07,					//  Global,    Report ID (7)
		0x19,  0x00,				    //  Local,     Usage Minimum (0)
		0x2a,  0xff,  0xff,				//  Local      Usage Maximum (65535)
		0x15,  0x00,					//  Global,    Logical Minimum (0)
		0x26,  0xff,  0xff,             //  Global,    Logical Maximum (65535)
		0x95,  0x01,					//  Local,     Report Count (1)
		0x75,  0x10,					//  Global,    Report Size (16)
		0x81,  0x00,					//  Main,     INPUT (Data, Variable, Absolute)
		0xc0,							//          End Collection

		0x06,  0xbc,  0xff,				//  Global, Usage Page (0xffbc, Windows MCE Buttons) -- N.B. byte reversal!
		0x09,  0x89,					//  Local,  Usage (0x89, MCE OEM Buttons)
		0xa1,  0x01,					//  Local,  Collection (1, Application)
		0x85,  0x08,					//  Global,    Report ID (8)
		0x19,  0x00,				    //  Local,     Usage Minimum (0)
		0x2a,  0xff,  0xff,				//  Local      Usage Maximum (65535)
		0x15,  0x00,					//  Global,    Logical Minimum (0)
		0x26,  0xff,  0xff,             //  Global,    Logical Maximum (65535)
		0x95,  0x01,					//  Local,     Report Count (1)
		0x75,  0x10,					//  Global,    Report Size (16)
		0x81,  0x00,					//  Main,     INPUT (Data, Variable, Absolute)
		0xc0							//          End Collection

};


//
// This is the default HID descriptor returned by the mini driver
// in response to IOCTL_HID_GET_DEVICE_DESCRIPTOR. The size
// of report descriptor is currently the size of DefaultReportDescriptor.
// If the report descriptor is successfully read from the registry, it is
// modified to corresponding size of report descriptor.
//

HID_DESCRIPTOR DefaultHidDescriptor = {
    sizeof(DefaultHidDescriptor),		// 0x09 == length of HID descriptor
    HID_HID_DESCRIPTOR_TYPE,			// descriptor type == HID  0x21
    0x0101,								// hid spec release
    0x00,								// country code == Not Specified
    0x01,								// number of HID class descriptors
    {
		HID_REPORT_DESCRIPTOR_TYPE,		// report descriptor type 0x22
    	sizeof(DefaultReportDescriptor)	// total length of report descriptor
    }
};



