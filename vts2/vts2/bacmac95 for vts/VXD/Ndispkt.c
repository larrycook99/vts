/*---------------------------------------------------------------------------
	©1997, PolarSoft Inc., All Rights Reserved  (taken from:)
    ©1997  Microsoft Corporation
	
module:		NDISPKT.C
desc:		VxD Standard Procedures for BACnet Packet Driver
authors:	William Ingle, Jack Neyer
last edit:	11-Dec-97 [001] JN  first cut
------------------------------------------------------------------------------*/
//Environment:

//	Ndis 3.1+ Windows 95 VxD

// This module provides the main VxD Message processor (BACMAC95_Control).
// In addition the BACMAC95_DDB for the VxD is defined here.

// The major changes are:
//
//	rename VPACKET to BACMAC
//	rename PACKET to BACMAC95

// Also, where possible comments are added for clarification.

// This module hase no direct counterpart in NT, although it does register/
// deregister the protocol with NDIS.

#include <basedef.h>
#include <vmm.h>
#include <ndis.h>
#include <vwin32.h>
#include "debug.h"
#include "packet.h"							//leave as is

#pragma VxD_LOCKED_CODE_SEG
#pragma VxD_LOCKED_DATA_SEG

// The VxD Device Descriptor Block (DDB).  This is exported by the VxD
// and used by the system to obtain the pointer to the VxD Control proc.
//
struct VxD_Desc_Block BACMAC95_DDB =					//	***001
{
    0,                                 // DDB_Next
    DDK_VERSION,                       // DDK_SDK_Version
    UNDEFINED_DEVICE_ID,               // DDB_Req_Device_Number
    3,                                 // DDB_Dev_Major_Version
    10,                                // DDB_Dev_Minor_Version
    0,                                 // DDB_Flags
    {'B','A','C','P','A','C','9','5'}, // DDB_Name		//	***001
    PROTOCOL_INIT_ORDER,               // DDB_Init_Order
    (ULONG)BACMAC95_Control,           // DDB_Control_Proc	***001
    0,                                 // DDB_V86_API_Proc
    0,                                 // DDB_PM_API_Proc
    0,                                 // DDB_V86_API_CSIP
    0,                                 // DDB_PM_API_CSIP
    0,                                 // DDB_Reference_Data
    0,                                 // DDB_Service_Table_Ptr
    0,                                 // DDB_Service_Table_Size
    0,                                 // DDB_Win32_Service_Table
    'Prev',                            // DDB_Prev
    0,                                 // DDB_Reserved0
    'Rsv1',                            // DDB_Reserved1
    'Rsv2',                            // DDB_Reserved2
    'Rsv3'                             // DDB_Reserved3
};

//********************************************************************
// PACKET_Control()
//
// Main entry point into our VxD
//
// Entry:
//      EAX = System Control message
//
//********************************************************************

VOID _declspec(naked) BACMAC95_Control (VOID)		//					***001
{
	int msg;

   _asm	mov	msg, eax
 
//	Manage SYS_DYNAMIC_DEVICE_INIT, DEVICE_INIT, SYS_DYNAMIC_DEVICE_EXIT, W32_DEVICEIOCONTROL
	switch ( msg )
	{    
	case SYS_DYNAMIC_DEVICE_INIT:
	case DEVICE_INIT:
        jnz     VXDSAMP_Control_clc_exit				//looks like clc=success
        jmp     VXDSAMP_Control_stc_exit				//looks like stc=failure
    
	not_SYS_DYNAMIC_DEVICE_INIT:
        cmp     eax, SYS_DYNAMIC_DEVICE_EXIT
        jnz     not_SYS_DYNAMIC_DEVICE_EXIT
        call    SysDynamicDeviceExit
        jmp     VXDSAMP_Control_clc_exit
 
	not_SYS_DYNAMIC_DEVICE_EXIT:
        cmp     eax, W32_DEVICEIOCONTROL
        jnz     not_W32_DEVICEIOCONTROL
        //      EAX = W32_DEVICEIOCONTROL
        //      EBX = DDB ptr
        //      ECX = dwIoControlCode
        //      EDX = hDevice
        //      ESI = pointer to a DIOCPARAMETERS struct
        push    esi
        push    ecx
        call    DeviceIOControl

    not_W32_DEVICEIOCONTROL:

    VXDSAMP_Control_clc_exit:
        clc
        ret

    VXDSAMP_Control_stc_exit:
        stc
        ret
    }
}
 
//********************************************************************
// SysDynamicDeviceInit()
//
// VxD is being loaded.  Register our protocol with NDIS wrapper.
//
//********************************************************************

BOOL _stdcall SysDynamicDeviceInit (VOID)
{	DWORD       dwVersion;
    NDIS_STATUS Status;

    DbgPrintf("VXDSAMP: Dynamic Init\n");

    dwVersion = NdisGetVersion();
    if (0 == dwVersion)
    {	DbgPrintf("VXDSAMP: NDIS not loaded\n");
        return FALSE;
    }
    else if (0x030A > dwVersion)
    {	DbgPrintf("VXDSAMP: bad NDIS version\n");
        return FALSE;
    }
// Register our protocol with NDIS
    NdisRegisterProtocol(&Status,&ProtocolHandle,&ProtocolCharacteristics,
							sizeof(ProtocolCharacteristics));
    if (NDIS_STATUS_SUCCESS != Status)
    {	DbgPrintf("VXDSAMP: failure NdisRegisterProtocol %08X\n", Status);
        return FALSE;
    }

// Hook system broadcast messages so we know about DevNodes which
// are removed.  If this was a real PnP protocol, NDIS wrapper would
// take care of this by calling our UnbindAdapterHandler when an
// adapter goes away (e.g. popping out a PCMCIA net adatper).
//
    hSysBroadcastHook = SHELL_HookSystemBroadcast(SystemBroadcastHook,0,0);
    return TRUE;
}

//********************************************************************
// SysDynamicDeviceExit()
//
// VxD is being unloaded.  Deregister our protocol with NDIS wrapper.
//
//********************************************************************

VOID _stdcall SysDynamicDeviceExit (VOID)
{
    PPROTOCOL_ADAPTER_BLOCK pProtocolAdapterBlock;
    NDIS_STATUS             Status;

    DbgPrintf("VXDSAMP: Dynamic Exit\n");

    SHELL_UnhookSystemBroadcast(hSysBroadcastHook);
// Make sure there are no open adapters which the app did not close
// as the protocol deregister will fail if it is still bound to any
// adapters.
    pProtocolAdapterBlock = ProtocolAdapterBlockHead.pNext;
    while (pProtocolAdapterBlock)
    {
        DbgPrintf("VXDSAMP: adapter still open, closing it due to exit\n");
// Attempt to close the adapter
        NdisCloseAdapter(&Status,pProtocolAdapterBlock->NdisBindingHandle);
        if (NDIS_STATUS_PENDING == Status)
        {
// XXXXX This code path not traversed yet
            Wait_Semaphore(pProtocolAdapterBlock->vmmSemComplete,BLOCK_SVC_INTS);
            if (NDIS_STATUS_SUCCESS != pProtocolAdapterBlock->Status)
            {	 DbgPrintf("VXDSAMP: failure NdisCloseAdapter %08X\n",
                          pProtocolAdapterBlock->Status);
            }
        }
        else if (NDIS_STATUS_SUCCESS != Status)
        {	DbgPrintf("VXDSAMP: failure NdisCloseAdapter %08X\n",
                      Status);
        }
        FreeProtocolAdapterBlock(pProtocolAdapterBlock);
        pProtocolAdapterBlock = ProtocolAdapterBlockHead.pNext;
    }
// Deregister our protocol with NDIS
    NdisDeregisterProtocol(&Status,ProtocolHandle);

    if (NDIS_STATUS_SUCCESS != Status)
    {	DbgPrintf("VXDSAMP: failure NdisDeregisterProtocol %08X\n", Status);
    }
}
