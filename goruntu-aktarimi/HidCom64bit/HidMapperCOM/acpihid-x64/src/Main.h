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
// Module Name: Main.h                                                        //
//                                                                            //
// Abstract: Main include file                                                //
//                                                                            //
// Authors: Babu Pallithanam                                                  //
//          Jim Trethewey                                                     //
//                                                                            //
// Environment: kernel mode only                                              //
//                                                                            //
// Revision History:                                                          //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////


//
// Debugging Output Levels
//
#define DBG_STARTUP_SHUTDOWN_MASK  0x0000000F
#define DBG_SS_NOISE               0x00000001
#define DBG_SS_TRACE               0x00000002
#define DBG_SS_INFO                0x00000004
#define DBG_SS_ERROR               0x00000008

#define DBG_PNP_MASK               0x000000F0
#define DBG_PNP_NOISE              0x00000010
#define DBG_PNP_TRACE              0x00000020
#define DBG_PNP_INFO               0x00000040
#define DBG_PNP_ERROR              0x00000080

#define DEFAULT_DEBUG_OUTPUT_LEVEL 0xFFFFFFFF

#if DBG
#define DebugPrint(_x_)		\
               DbgPrint _x_;
#define TRAP() DbgBreakPoint()
#define MyKdPrint_Def(_l_, _x_) \
            if (DEFAULT_DEBUG_OUTPUT_LEVEL & (_l_)) { \
               DbgPrint _x_; \
            }
#else
#define DebugPrint(_x_)
#define TRAP()
#define MyKdPrint_Def(_l_, _x_)
#endif




#include <wdm.h>
#include <wdmguid.h>
#include <hidport.h>
#include <usbdi.h>
#include <usbdlib.h>
#include <usb100.h>
#include <initguid.h>
#include <acpiioct.h>

#include "common.h"   //for control code definitions


//
//These are the device attributes returned by the mini driver in response
// to IOCTL_HID_GET_DEVICE_ATTRIBUTES.
//


#define	REPORT_BUF_SIZE				9		// Should be MAX I/O/F buffer size; dangerous to change this (cause BSOD)
#define MAX_NO_BUTTONS       		64
#define MAX_NO_VIRTUAL_BUTTONS      64
#define MAX_NO_LEDS       			64


//NOT USED: ? #define VHID_POOL_TAGM (ULONG) 'diHM'

typedef UCHAR HID_REPORT_DESCRIPTOR, *PHID_REPORT_DESCRIPTOR;


typedef struct _BUTTON_INFO {
	UCHAR	ReportID;
	UCHAR   UsagePageMsb;
	UCHAR   UsagePageLsb;
	UCHAR	UsageMsb;
	UCHAR	UsageLsb;
	UCHAR   ButtonUsageMsb;
	UCHAR   ButtonUsageLsb;
	UCHAR	KeyStatus;
} BUTTON_INFO, *PBUTTON_INFO;

typedef struct _VIRTUAL_BUTTON_INFO {
	UCHAR	InitialValue;
	UCHAR	KeyStatus;
} VIRTUAL_BUTTON_INFO, *PVIRTUAL_BUTTON_INFO;


typedef struct _HID_TYPE3INPUTBUFFER {
	USHORT	hidStringId;
	USHORT	hidStringLanguage;
} HID_TYPE3INPUTBUFFER, *PHID_TYPE3INPUTBUFFER;


typedef UCHAR HID_REPORT_DESCRIPTOR, *PHID_REPORT_DESCRIPTOR;


typedef struct tagDrvEventMap {
	PKEVENT	PKEvent;
	ULONG	id;
} DrvEventMap;


typedef struct tagAppEventMap {
	HANDLE	hEventHandle;
	ULONG	id;
} AppEventMap;


typedef struct {
    LIST_ENTRY  List;
    union {
        PIRP    Irp;
    };
} NODE, *PNODE;

typedef enum _QUEUE_STATE {

    HoldRequests,       // device is not started yet
    AllowRequests,      // device is ready to process
    FailRequests        // fail both existing and queued up requests

} QUEUE_STATE;


typedef enum _DEVICE_PNP_STATE {

    NotStarted = 0,         // Not started yet
    Started,                // Device has received the START_DEVICE IRP
    StopPending,            // Device has received the QUERY_STOP IRP
    Stopped,                // Device has received the STOP_DEVICE IRP
    RemovePending,          // Device has received the QUERY_REMOVE IRP
    SurpriseRemovePending,  // Device has received the SURPRISE_REMOVE IRP
    Deleted                 // Device has received the REMOVE_DEVICE IRP

} DEVICE_PNP_STATE;


#define INITIALIZE_PNP_STATE(_Data_)    \
        (_Data_)->DevicePnPState =  NotStarted;\
        (_Data_)->PreviousPnPState = NotStarted;

#define SET_NEW_PNP_STATE(_Data_, _state_) \
        (_Data_)->PreviousPnPState =  (_Data_)->DevicePnPState;\
        (_Data_)->DevicePnPState = (_state_);

#define RESTORE_PREVIOUS_PNP_STATE(_Data_)   \
        (_Data_)->DevicePnPState =   (_Data_)->PreviousPnPState;

#define GET_MINIDRIVER_DEVICE_EXTENSION(DO) \
    ((PDEVICE_EXTENSION) \
    (((PHID_DEVICE_EXTENSION)(DO)->DeviceExtension)->MiniDeviceExtension))

#define GET_NEXT_DEVICE_OBJECT(DO) \
    (((PHID_DEVICE_EXTENSION)(DO)->DeviceExtension)->NextDeviceObject)





typedef struct _DEVICE_EXTENSION {

    PDEVICE_OBJECT			 PhysicalDeviceObject;
	PDEVICE_OBJECT			 LowerDevObj;
	DEVICE_CAPABILITIES		 DeviceCapabilities;
	PDEVICE_RELATIONS        relations;

    DEVICE_PNP_STATE		 DevicePnPState;   // Track the state of the device
    DEVICE_PNP_STATE		 PreviousPnPState; // Remembers the previous pnp state
	LIST_ENTRY               PendingIrpList;

	IO_REMOVE_LOCK			 RemoveLock;	//to protect IRP_MN_REMOVE_DEVICE
  	PIRP					 ReadReportIrp;
	int                      PendingIrpCount;

    KSPIN_LOCK               SpinLock;
    int                      RemovedDevice;

 	ACPI_INTERFACE_STANDARD  AcpiInterface;
	DrvEventMap *            pGPIOEventMap;
	ULONG                    cbEventSize;
	PKEVENT                  sleepevent;

    HID_DESCRIPTOR           HidDescriptor;
    PHID_REPORT_DESCRIPTOR   ReportDescriptor;
    BOOLEAN                  ReadReportDescFromRegistry;
 	UCHAR					 ReportBuffer[REPORT_BUF_SIZE];

	BUTTON_INFO              Buttons[MAX_NO_BUTTONS + 1];
    int                      ButtonID;
	ULONG					 NumberOfButtons;
  	int                      ButtonDetectFlag;
	BOOLEAN		             DeviceActivatedFlag;   // "button hold" released (1) or engaged (0)
    int                      HidReadFlag;
	int                      TestButtonNo;

	VIRTUAL_BUTTON_INFO      VirtualButtons[MAX_NO_VIRTUAL_BUTTONS + 1];
	ULONG					 NumberOfVirtualButtons;

	ULONG					 NumberOfLeds;
	UCHAR                    LedStatus[MAX_NO_LEDS + 1];

} DEVICE_EXTENSION, * PDEVICE_EXTENSION;




//
// driver routines
//

NTSTATUS
DriverEntry(
    IN PDRIVER_OBJECT  DriverObject,
    IN PUNICODE_STRING registryPath
    );


// DispatchPnP.c

NTSTATUS
AcpiHidDispatchPnP(
    IN PDEVICE_OBJECT DeviceObject,
    IN PIRP Irp
    );


// AddDevice.c

NTSTATUS
AcpiHidAddDevice(
    IN PDRIVER_OBJECT DriverObject,
    IN PDEVICE_OBJECT FunctionalDeviceObject
    );

// Unload.c

VOID
AcpiHidUnload(
    IN PDRIVER_OBJECT DriverObject
    );


// InternalIoctl.c

NTSTATUS
AcpiHidInternalIoctl(
    IN PDEVICE_OBJECT DeviceObject,
    IN PIRP Irp
    );


// DispatchPower.c

NTSTATUS
AcpiHidDispatchPower(
    IN PDEVICE_OBJECT DeviceObject,
    IN PIRP           Irp
    );


// CreateClose.c

NTSTATUS
AcpiHidCreateClose(
    IN PDEVICE_OBJECT DevObj,
    IN PIRP Irp
    );
