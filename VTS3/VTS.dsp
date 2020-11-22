# Microsoft Developer Studio Project File - Name="VTS" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=VTS - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "VTS.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "VTS.mak" CFG="VTS - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "VTS - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "VTS - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""$/vts3", BAAAAAAA"
# PROP Scc_LocalPath "."
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "VTS - Win32 Release"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 5
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /MT /W3 /GX /O2 /I "." /I ".\BACnetLibrary" /I ".\Sniffer" /I ".\DUDAPI" /I ".\FileVersionInfo" /I ".\Statistics" /I ".\ptp" /I ".\winpcap" /I ".\GetMACAddress" /I ".\NBLink" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /FR /Yu"stdafx.h" /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG" /d "_AFXDLL"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /machine:I386
# ADD LINK32 version.lib macdll/ptp/release/ptp.lib /nologo /subsystem:windows /machine:I386
# SUBTRACT LINK32 /pdb:none /debug

!ELSEIF  "$(CFG)" == "VTS - Win32 Debug"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 5
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /Yu"stdafx.h" /FD /GZ /c
# ADD CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /I "." /I ".\BACnetLibrary" /I ".\Sniffer" /I ".\DUDAPI" /I ".\FileVersionInfo" /I ".\Statistics" /I ".\ptp" /I ".\winpcap" /I ".\GetMACAddress" /I ".\NBLink" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /FR /Yu"stdafx.h" /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG" /d "_AFXDLL"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept
# ADD LINK32 version.lib macdll/PTP/debug/PTP.lib /nologo /subsystem:windows /debug /machine:I386 /nodefaultlib:"LIBCMT" /pdbtype:sept
# SUBTRACT LINK32 /pdb:none

!ENDIF 

# Begin Target

# Name "VTS - Win32 Release"
# Name "VTS - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\BACnetLibrary\BACnet.cpp
# End Source File
# Begin Source File

SOURCE=.\BACnetLibrary\BACnetAPDU.cpp
# End Source File
# Begin Source File

SOURCE=.\BACnetLibrary\BACnetAPDUDecoder.cpp
# End Source File
# Begin Source File

SOURCE=.\BACnetLibrary\BACnetAPDUEncoder.cpp
# End Source File
# Begin Source File

SOURCE=.\BACnetLibrary\BACnetAPDUSegment.cpp
# End Source File
# Begin Source File

SOURCE=.\BACnetLibrary\BACnetBBMD.cpp
# End Source File
# Begin Source File

SOURCE=.\BACnetLibrary\BACnetBIPForeign.cpp
# End Source File
# Begin Source File

SOURCE=.\BACnetLibrary\BACnetBIPSimple.cpp
# End Source File
# Begin Source File

SOURCE=.\BACnetLibrary\BACnetBTR.cpp
# End Source File
# Begin Source File

SOURCE=.\BACnetLibrary\BACnetClient.cpp
# End Source File
# Begin Source File

SOURCE=.\BACnetLibrary\BACnetClientTSM.cpp
# End Source File
# Begin Source File

SOURCE=.\BACnetLibrary\BACnetDevice.cpp
# End Source File
# Begin Source File

SOURCE=.\BACnetLibrary\BACnetIP.cpp
# End Source File
# Begin Source File

SOURCE=.\BACnetLibrary\BACnetPDU.cpp
# End Source File
# Begin Source File

SOURCE=.\BACnetLibrary\BACnetRouter.cpp
# End Source File
# Begin Source File

SOURCE=.\BACnetLibrary\BACnetServer.cpp
# End Source File
# Begin Source File

SOURCE=.\BACnetLibrary\BACnetServerTSM.cpp
# End Source File
# Begin Source File

SOURCE=.\BACnetLibrary\BACnetTask.cpp
# End Source File
# Begin Source File

SOURCE=.\BACnetLibrary\BACnetTimer.cpp
# End Source File
# Begin Source File

SOURCE=.\BACnetLibrary\BACnetTSM.cpp
# End Source File
# Begin Source File

SOURCE=.\BACnetLibrary\BACnetVIPLAN.cpp
# End Source File
# Begin Source File

SOURCE=.\BACnetLibrary\BACnetVLAN.cpp
# End Source File
# Begin Source File

SOURCE=.\Sniffer\BACSN015.cpp
# End Source File
# Begin Source File

SOURCE=.\BakRestoreExecutor.cpp
# End Source File
# Begin Source File

SOURCE=.\ChildFrm.cpp
# End Source File
# Begin Source File

SOURCE=.\ColorSettingDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\DetailTreeCtrl.cpp
# End Source File
# Begin Source File

SOURCE=.\DetailView.cpp
# End Source File
# Begin Source File

SOURCE=.\DiscoveryExecutor.cpp
# End Source File
# Begin Source File

SOURCE=.\DockingDetailViewBar.cpp
# End Source File
# Begin Source File

SOURCE=.\DockingEPICSViewBar.cpp
# End Source File
# Begin Source File

SOURCE=.\DockingHexViewbar.cpp
# End Source File
# Begin Source File

SOURCE=.\DUDAPI\dudapi32.cpp
# End Source File
# Begin Source File

SOURCE=.\DUDAPI\DUDTOOL.CPP
# End Source File
# Begin Source File

SOURCE=.\EditResentPktDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\EPICSTreeView.cpp
# End Source File
# Begin Source File

SOURCE=.\EPICSViewInfoPanel.cpp
# End Source File
# Begin Source File

SOURCE=.\EPICSViewNode.cpp
# End Source File
# Begin Source File

SOURCE=.\EPICSViewNodeAppService.cpp
# End Source File
# Begin Source File

SOURCE=.\EPICSViewPropPanel.cpp
# End Source File
# Begin Source File

SOURCE=.\FileVersionInfo\file_ver.cpp
# End Source File
# Begin Source File

SOURCE=.\FrameContext.cpp
# End Source File
# Begin Source File

SOURCE=.\GoToLineDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\HexView.cpp
# End Source File
# Begin Source File

SOURCE=.\InconsistentParsExecutor.cpp
# End Source File
# Begin Source File

SOURCE=.\LineNumCtrl.cpp
# End Source File
# Begin Source File

SOURCE=.\ListSummaryCache.cpp
# End Source File
# Begin Source File

SOURCE=.\ListSummaryView.cpp
# End Source File
# Begin Source File

SOURCE=.\MainFrm.cpp
# End Source File
# Begin Source File

SOURCE=.\md5.cpp
# End Source File
# Begin Source File

SOURCE=.\MSTP_NBLinkDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\Sniffer\PI.cpp
# End Source File
# Begin Source File

SOURCE=.\ReadAllPropSettingsDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\scbarcf.cpp
# End Source File
# Begin Source File

SOURCE=.\scbarg.cpp
# End Source File
# Begin Source File

SOURCE=.\ScriptBase.cpp
# End Source File
# Begin Source File

SOURCE=.\ScriptCase.cpp
# End Source File
# Begin Source File

SOURCE=.\ScriptCommand.cpp
# End Source File
# Begin Source File

SOURCE=.\ScriptContentTree.cpp
# End Source File
# Begin Source File

SOURCE=.\ScriptDependency.cpp
# End Source File
# Begin Source File

SOURCE=.\ScriptDocument.cpp
# End Source File
# Begin Source File

SOURCE=.\ScriptEdit.cpp
# End Source File
# Begin Source File

SOURCE=.\ScriptEditIncludeDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\ScriptExecMsg.cpp
# End Source File
# Begin Source File

SOURCE=.\ScriptExecutor.cpp

!IF  "$(CFG)" == "VTS - Win32 Release"

# ADD CPP /Ob0

!ELSEIF  "$(CFG)" == "VTS - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\ScriptFrame.cpp
# End Source File
# Begin Source File

SOURCE=.\ScriptIfdefHandler.cpp
# End Source File
# Begin Source File

SOURCE=.\ScriptKeywords.cpp
# End Source File
# Begin Source File

SOURCE=.\ScriptLoadResults.cpp
# End Source File
# Begin Source File

SOURCE=.\ScriptMakeDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\ScriptMsgMake.cpp
# End Source File
# Begin Source File

SOURCE=.\ScriptPacket.cpp
# End Source File
# Begin Source File

SOURCE=.\ScriptParmList.cpp
# End Source File
# Begin Source File

SOURCE=.\ScriptParmUpdate.cpp
# End Source File
# Begin Source File

SOURCE=.\ScriptReference.cpp
# End Source File
# Begin Source File

SOURCE=.\ScriptSection.cpp
# End Source File
# Begin Source File

SOURCE=.\ScriptTest.cpp
# End Source File
# Begin Source File

SOURCE=.\ScrollLineView.cpp
# End Source File
# Begin Source File

SOURCE=.\SelectTemplateDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\Send.cpp
# End Source File
# Begin Source File

SOURCE=.\SendAbort.cpp
# End Source File
# Begin Source File

SOURCE=.\SendAckAlarm.cpp
# End Source File
# Begin Source File

SOURCE=.\SendAddListElement.cpp
# End Source File
# Begin Source File

SOURCE=.\SendBVLCI.cpp
# End Source File
# Begin Source File

SOURCE=.\SendBVLCResult.cpp
# End Source File
# Begin Source File

SOURCE=.\SendChangeListError.cpp
# End Source File
# Begin Source File

SOURCE=.\SendComplexACK.cpp
# End Source File
# Begin Source File

SOURCE=.\SendConfCOVNotification.cpp
# End Source File
# Begin Source File

SOURCE=.\SendConfEventNotification.cpp
# End Source File
# Begin Source File

SOURCE=.\SendConfirmedRequest.cpp
# End Source File
# Begin Source File

SOURCE=.\SendConfTextMsg.cpp
# End Source File
# Begin Source File

SOURCE=.\SendCreateObject.cpp
# End Source File
# Begin Source File

SOURCE=.\SendCreateObjectACK.cpp
# End Source File
# Begin Source File

SOURCE=.\SendCreateObjectError.cpp
# End Source File
# Begin Source File

SOURCE=.\SendDeleteFDTEntry.cpp
# End Source File
# Begin Source File

SOURCE=.\SendDeleteObject.cpp
# End Source File
# Begin Source File

SOURCE=.\SendDevice.cpp
# End Source File
# Begin Source File

SOURCE=.\SendDeviceCommCtrl.cpp
# End Source File
# Begin Source File

SOURCE=.\SendDisconnectCTN.cpp
# End Source File
# Begin Source File

SOURCE=.\SendEnet.cpp
# End Source File
# Begin Source File

SOURCE=.\SendError.cpp
# End Source File
# Begin Source File

SOURCE=.\SendEstablishCTN.cpp
# End Source File
# Begin Source File

SOURCE=.\SendGetAlarmSummary.cpp
# End Source File
# Begin Source File

SOURCE=.\SendGetAlarmSummaryACK.cpp
# End Source File
# Begin Source File

SOURCE=.\SendGetEnrollmentSummary.cpp
# End Source File
# Begin Source File

SOURCE=.\SendGetEnrollmentSummaryACK.cpp
# End Source File
# Begin Source File

SOURCE=.\SendGetEventInfo.cpp
# End Source File
# Begin Source File

SOURCE=.\SendGetEventInfoACK.cpp
# End Source File
# Begin Source File

SOURCE=.\SendIAm.cpp
# End Source File
# Begin Source File

SOURCE=.\SendIAmRTN.cpp
# End Source File
# Begin Source File

SOURCE=.\SendICouldBeRTN.cpp
# End Source File
# Begin Source File

SOURCE=.\SendIHave.cpp
# End Source File
# Begin Source File

SOURCE=.\SendInitRT.cpp
# End Source File
# Begin Source File

SOURCE=.\SendInitRTAck.cpp
# End Source File
# Begin Source File

SOURCE=.\SendIP.cpp
# End Source File
# Begin Source File

SOURCE=.\SendMSTP.cpp
# End Source File
# Begin Source File

SOURCE=.\SendNetworkNumberIs.cpp
# End Source File
# Begin Source File

SOURCE=.\SendNPCI.cpp
# End Source File
# Begin Source File

SOURCE=.\SendNull.cpp
# End Source File
# Begin Source File

SOURCE=.\SendPage.cpp
# End Source File
# Begin Source File

SOURCE=.\SendRaw.cpp
# End Source File
# Begin Source File

SOURCE=.\SendReadBDT.cpp
# End Source File
# Begin Source File

SOURCE=.\SendReadBDTAck.cpp
# End Source File
# Begin Source File

SOURCE=.\SendReadFDT.cpp
# End Source File
# Begin Source File

SOURCE=.\SendReadFDTAck.cpp
# End Source File
# Begin Source File

SOURCE=.\SendReadFile.cpp
# End Source File
# Begin Source File

SOURCE=.\SendReadFileACK.cpp
# End Source File
# Begin Source File

SOURCE=.\SendReadProp.cpp
# End Source File
# Begin Source File

SOURCE=.\SendReadPropACK.cpp
# End Source File
# Begin Source File

SOURCE=.\SendReadPropMult.cpp
# End Source File
# Begin Source File

SOURCE=.\SendReadPropMultACK.cpp
# End Source File
# Begin Source File

SOURCE=.\SendReadRange.cpp
# End Source File
# Begin Source File

SOURCE=.\SendReceiveExecutor.cpp
# End Source File
# Begin Source File

SOURCE=.\SendReceiveExecutorProgressDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\SendRegisterFD.cpp
# End Source File
# Begin Source File

SOURCE=.\SendReinitDevice.cpp
# End Source File
# Begin Source File

SOURCE=.\SendReject.cpp
# End Source File
# Begin Source File

SOURCE=.\SendRejectMTN.cpp
# End Source File
# Begin Source File

SOURCE=.\SendRemoveListElement.cpp
# End Source File
# Begin Source File

SOURCE=.\SendRouterAvailable.cpp
# End Source File
# Begin Source File

SOURCE=.\SendRouterBusy.cpp
# End Source File
# Begin Source File

SOURCE=.\SendSegmentACK.cpp
# End Source File
# Begin Source File

SOURCE=.\SendSimpleACK.cpp
# End Source File
# Begin Source File

SOURCE=.\SendSubscribeCOV.cpp
# End Source File
# Begin Source File

SOURCE=.\SendSubscribeCOVProperty.cpp
# End Source File
# Begin Source File

SOURCE=.\SendTest.cpp
# End Source File
# Begin Source File

SOURCE=.\SendTimeSync.cpp
# End Source File
# Begin Source File

SOURCE=.\SendUnconfCOVNotification.cpp
# End Source File
# Begin Source File

SOURCE=.\SendUnconfEventNotification.cpp
# End Source File
# Begin Source File

SOURCE=.\SendUnconfTextMsg.cpp
# End Source File
# Begin Source File

SOURCE=.\SendUTCTimeSync.cpp
# End Source File
# Begin Source File

SOURCE=.\SendVendorNPDU.cpp
# End Source File
# Begin Source File

SOURCE=.\SendVTClose.cpp
# End Source File
# Begin Source File

SOURCE=.\SendVTCloseError.cpp
# End Source File
# Begin Source File

SOURCE=.\SendVTData.cpp
# End Source File
# Begin Source File

SOURCE=.\SendVTDataACK.cpp
# End Source File
# Begin Source File

SOURCE=.\SendVTOpen.cpp
# End Source File
# Begin Source File

SOURCE=.\SendVTOpenACK.cpp
# End Source File
# Begin Source File

SOURCE=.\SendWhatIsNetworkNumber.cpp
# End Source File
# Begin Source File

SOURCE=.\SendWhoHas.cpp
# End Source File
# Begin Source File

SOURCE=.\SendWhoIs.cpp
# End Source File
# Begin Source File

SOURCE=.\SendWhoIsRTN.cpp
# End Source File
# Begin Source File

SOURCE=.\SendWriteBDT.cpp
# End Source File
# Begin Source File

SOURCE=.\SendWriteFile.cpp
# End Source File
# Begin Source File

SOURCE=.\SendWriteFileACK.cpp
# End Source File
# Begin Source File

SOURCE=.\SendWriteProp.cpp
# End Source File
# Begin Source File

SOURCE=.\SendWritePropMult.cpp
# End Source File
# Begin Source File

SOURCE=.\SendWritePropMultError.cpp
# End Source File
# Begin Source File

SOURCE=.\sizecbar.cpp
# End Source File
# Begin Source File

SOURCE=.\StdAfx.cpp
# ADD CPP /Yc"stdafx.h"
# End Source File
# Begin Source File

SOURCE=.\Sniffer\StringTables.cpp
# End Source File
# Begin Source File

SOURCE=.\Statistics\supergridctrl.cpp
# End Source File
# Begin Source File

SOURCE=.\Statistics\TreeListCtrl.cpp
# End Source File
# Begin Source File

SOURCE=.\VdbPrint.cpp
# End Source File
# Begin Source File

SOURCE=.\VTS.cpp
# End Source File
# Begin Source File

SOURCE=.\VTS.rc
# End Source File
# Begin Source File

SOURCE=.\VTSActionCommandDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\VTSActionDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\VTSActionListDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\VTSAddressBindingDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\VTSAny.cpp
# End Source File
# Begin Source File

SOURCE=.\DUDAPI\Vtsapi32.cpp
# End Source File
# Begin Source File

SOURCE=.\VTSBackupRestoreDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\VTSBACnetTypeGenericPage.cpp
# End Source File
# Begin Source File

SOURCE=.\VTSBinaryPVDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\VTSCalendarEntryDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\VTSClientCOVDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\VTSCOVSubscriptionDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\VTSCtrl.cpp
# End Source File
# Begin Source File

SOURCE=.\VTSDailyScheduleDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\VTSDateRangeDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\VTSDateTimeDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\VTSDaysOfWeekDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\VTSDestinationDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\VTSDeviceObjectPropertyReferenceDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\VTSDeviceObjectReferenceDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\VTSDeviceStatusDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\VTSDevicesTreeDevPage.cpp
# End Source File
# Begin Source File

SOURCE=.\VTSDevicesTreeDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\VTSDevicesTreeObjPage.cpp
# End Source File
# Begin Source File

SOURCE=.\VTSDevicesTreeOptPage.cpp
# End Source File
# Begin Source File

SOURCE=.\VTSDevicesTreePropPage.cpp
# End Source File
# Begin Source File

SOURCE=.\VTSDevicesTreeValuePage.cpp
# End Source File
# Begin Source File

SOURCE=.\VTSDiscoveryDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\VTSDoc.cpp
# End Source File
# Begin Source File

SOURCE=.\VTSEngineeringUnitsDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\VTSEventBitstringDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\VTSEventBufferReadyDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\VTSEventChangeOfStateDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\VTSEventChangeOfValueDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\VTSEventCommandFailureDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\VTSEventFloatLimitDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\VTSEventLifeSafetyDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\VTSEventOutOfRangeDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\VTSEventParameterDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\VTSEventStateDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\VTSEventTransitionBitsDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\VTSEventTypeDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\VTSFileAccessMethodDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\VTSFiltersDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\VTSInconsistentParsDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\VTSLifeSafetyModeDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\VTSLifeSafetyOperationDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\VTSLifeSafetyStateDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\VTSLimitEnable.cpp
# End Source File
# Begin Source File

SOURCE=.\VTSLimitEnableDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\VTSListOfBitstringDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\VTSListOfDestinationDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\VTSListOfLifeSafetyStateDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\VTSListOfPropertyStatesDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\VTSListOfReadAccessResultDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\VTSListOfReadAccessSpecDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\VTSListOfTimeStampDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\VTSListOfUnsignedDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\VTSLogRecordDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\VTSLogStatusDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\VTSMaintenanceDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\VTSNamesDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\VTSNotificationParameters.cpp
# End Source File
# Begin Source File

SOURCE=.\VTSNotifyBitstring.cpp
# End Source File
# Begin Source File

SOURCE=.\VTSNotifyBufferReady.cpp
# End Source File
# Begin Source File

SOURCE=.\VTSNotifyChgLifeSafety.cpp
# End Source File
# Begin Source File

SOURCE=.\VTSNotifyCmdFailure.cpp
# End Source File
# Begin Source File

SOURCE=.\VTSNotifyComplex.cpp
# End Source File
# Begin Source File

SOURCE=.\VTSNotifyExtended.cpp
# End Source File
# Begin Source File

SOURCE=.\VTSNotifyFloatLimit.cpp
# End Source File
# Begin Source File

SOURCE=.\VTSNotifyOutOfRange.cpp
# End Source File
# Begin Source File

SOURCE=.\VTSNotifyPage.cpp
# End Source File
# Begin Source File

SOURCE=.\VTSNotifyState.cpp
# End Source File
# Begin Source File

SOURCE=.\VTSNotifyTypeDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\VTSNotifyUnsignedRange.cpp
# End Source File
# Begin Source File

SOURCE=.\VTSNotifyValue.cpp
# End Source File
# Begin Source File

SOURCE=.\VTSObjectIdentifierDialog.cpp
# End Source File
# Begin Source File

SOURCE=.\VTSObjectPropertyReferenceDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\VTSObjectPropertyValueDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\VTSObjectTypeDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\VTSObjectTypesSupportedDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\VTSPacket.cpp
# End Source File
# Begin Source File

SOURCE=.\VTSPacketDB.cpp
# End Source File
# Begin Source File

SOURCE=.\VTSPolarityDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\VTSPortARCNETDialog.cpp
# End Source File
# Begin Source File

SOURCE=.\VTSPortDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\VTSPortEthernetDialog.cpp
# End Source File
# Begin Source File

SOURCE=.\VTSPortIPDialog.cpp
# End Source File
# Begin Source File

SOURCE=.\VTSPortMSTPDialog.cpp
# End Source File
# Begin Source File

SOURCE=.\ptp\VTSPortPTPDialog.cpp
# End Source File
# Begin Source File

SOURCE=.\VTSPreferences.cpp
# End Source File
# Begin Source File

SOURCE=.\VTSPreferencesDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\VTSPriorityArrayDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\VTSPriorityValueDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\VTSProgramErrorDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\VTSProgramRequestDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\VTSProgramStateDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\VTSPropertyIdentifierDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\VTSPropertyPage.cpp
# End Source File
# Begin Source File

SOURCE=.\VTSPropertyReferenceDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\VTSPropertyStatesDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\VTSPropertyValueDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\VTSPropValue.cpp
# End Source File
# Begin Source File

SOURCE=.\VTSRecipientDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\VTSRecipientProcess.cpp
# End Source File
# Begin Source File

SOURCE=.\VTSReliabilityDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\VTSResultFlasDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\VTSSegmentationDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\VTSServicesSupportedDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\VTSSessionKeyDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\VTSSetPointReferenceDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\VTSSilenceStateDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\VTSSpecialEventDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\Statistics\VTSStatisticsCollector.cpp
# End Source File
# Begin Source File

SOURCE=.\Statistics\VTSStatisticsDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\Statistics\VTSStatisticsOptionsDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\VTSStatusFlagsDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\VTSTimeStamp.cpp
# End Source File
# Begin Source File

SOURCE=.\VTSTimeStampDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\VTSTimeValueDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\VTSVendorPropIDDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\VTSVTClassDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\VTSVTSessionDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\VTSWeekNDayDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\VTSWinMSTPPort.cpp
# End Source File
# Begin Source File

SOURCE=.\ptp\VTSWinPTPPort.cpp
# End Source File
# Begin Source File

SOURCE=.\BACnetLibrary\WinBACnetTaskManager.cpp
# End Source File
# Begin Source File

SOURCE=.\BACnetLibrary\WinIP.cpp
# End Source File
# Begin Source File

SOURCE=.\WinMSTP.cpp
# End Source File
# Begin Source File

SOURCE=.\ptp\WinPTP.cpp
# End Source File
# Begin Source File

SOURCE=.\BACnetLibrary\WinWinPcap.cpp
# End Source File
# Begin Source File

SOURCE=.\WPRPList.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\Sniffer\Bacfuncs.h
# End Source File
# Begin Source File

SOURCE=.\BACnetLibrary\BACnet.hpp
# End Source File
# Begin Source File

SOURCE=.\BACnetLibrary\BACnetBBMD.hpp
# End Source File
# Begin Source File

SOURCE=.\BACnetLibrary\BACnetBIPForeign.hpp
# End Source File
# Begin Source File

SOURCE=.\BACnetLibrary\BACnetBIPSimple.hpp
# End Source File
# Begin Source File

SOURCE=.\BACnetLibrary\BACnetBTR.hpp
# End Source File
# Begin Source File

SOURCE=.\BACnetLibrary\BACnetIP.hpp
# End Source File
# Begin Source File

SOURCE=.\BACnetLibrary\BACnetRouter.hpp
# End Source File
# Begin Source File

SOURCE=.\BACnetLibrary\BACnetTimer.hpp
# End Source File
# Begin Source File

SOURCE=.\BACnetLibrary\BACnetVIPLAN.hpp
# End Source File
# Begin Source File

SOURCE=.\BACnetLibrary\BACnetVLAN.hpp
# End Source File
# Begin Source File

SOURCE=.\DUDAPI\Bacprim.h
# End Source File
# Begin Source File

SOURCE=.\Sniffer\Bacproto.h
# End Source File
# Begin Source File

SOURCE=.\BakRestoreExecutor.h
# End Source File
# Begin Source File

SOURCE=.\ChildFrm.h
# End Source File
# Begin Source File

SOURCE=.\ColorSettingDlg.h
# End Source File
# Begin Source File

SOURCE=.\DUDAPI\Db.h
# End Source File
# Begin Source File

SOURCE=.\DetailTreeCtrl.h
# End Source File
# Begin Source File

SOURCE=.\DetailView.h
# End Source File
# Begin Source File

SOURCE=.\DiscoveryExecutor.h
# End Source File
# Begin Source File

SOURCE=.\DockingDetailViewBar.h
# End Source File
# Begin Source File

SOURCE=.\DockingEPICSViewBar.h
# End Source File
# Begin Source File

SOURCE=.\DockingHexViewbar.h
# End Source File
# Begin Source File

SOURCE=.\DUDAPI\Dudapi.h
# End Source File
# Begin Source File

SOURCE=.\DUDAPI\DUDTOOL.H
# End Source File
# Begin Source File

SOURCE=.\EditResentPktDlg.h
# End Source File
# Begin Source File

SOURCE=.\DUDAPI\EPICSConsCheck.h
# End Source File
# Begin Source File

SOURCE=.\EPICSTreeView.h
# End Source File
# Begin Source File

SOURCE=.\EPICSViewInfoPanel.h
# End Source File
# Begin Source File

SOURCE=.\EPICSViewNode.h
# End Source File
# Begin Source File

SOURCE=.\EPICSViewPropPanel.h
# End Source File
# Begin Source File

SOURCE=.\FileVersionInfo\file_ver.h
# End Source File
# Begin Source File

SOURCE=.\FileVersionInfo\file_ver.inl
# End Source File
# Begin Source File

SOURCE=.\FrameContext.h
# End Source File
# Begin Source File

SOURCE=.\GoToLineDlg.h
# End Source File
# Begin Source File

SOURCE=.\HexView.h
# End Source File
# Begin Source File

SOURCE=.\InconsistentParsExecutor.h
# End Source File
# Begin Source File

SOURCE=.\GetMACAddress\IPExport.h
# End Source File
# Begin Source File

SOURCE=.\GetMACAddress\IPHlpApi.h
# End Source File
# Begin Source File

SOURCE=.\GetMACAddress\IPTypes.h
# End Source File
# Begin Source File

SOURCE=.\LineNumCtrl.h
# End Source File
# Begin Source File

SOURCE=.\ListSummaryCache.h
# End Source File
# Begin Source File

SOURCE=.\ListSummaryView.h
# End Source File
# Begin Source File

SOURCE=.\MainFrm.h
# End Source File
# Begin Source File

SOURCE=.\md5.h
# End Source File
# Begin Source File

SOURCE=.\MSTP_NBLinkDlg.h
# End Source File
# Begin Source File

SOURCE=.\NBLink\nbsettings.h
# End Source File
# Begin Source File

SOURCE=.\BACnetLibrary\NTDDNDIS.H
# End Source File
# Begin Source File

SOURCE=.\WinPcap\PCAP.H
# End Source File
# Begin Source File

SOURCE=.\Sniffer\PI.h
# End Source File
# Begin Source File

SOURCE=.\DUDAPI\Propid.h
# End Source File
# Begin Source File

SOURCE=.\DUDAPI\PROPS.H
# End Source File
# Begin Source File

SOURCE=.\ptp\ptp.h
# End Source File
# Begin Source File

SOURCE=.\ReadAllPropSettingsDlg.h
# End Source File
# Begin Source File

SOURCE=.\Resource.h
# End Source File
# Begin Source File

SOURCE=.\scbarcf.h
# End Source File
# Begin Source File

SOURCE=.\scbarg.h
# End Source File
# Begin Source File

SOURCE=.\ScriptBase.h
# End Source File
# Begin Source File

SOURCE=.\ScriptCommand.h
# End Source File
# Begin Source File

SOURCE=.\ScriptContentTree.h
# End Source File
# Begin Source File

SOURCE=.\ScriptDocument.h
# End Source File
# Begin Source File

SOURCE=.\ScriptEdit.h
# End Source File
# Begin Source File

SOURCE=.\ScriptEditIncludeDlg.h
# End Source File
# Begin Source File

SOURCE=.\ScriptExecMsg.h
# End Source File
# Begin Source File

SOURCE=.\ScriptExecutor.h
# End Source File
# Begin Source File

SOURCE=.\ScriptFrame.h
# End Source File
# Begin Source File

SOURCE=.\ScriptIfdefHandler.h
# End Source File
# Begin Source File

SOURCE=.\ScriptKeywords.h
# End Source File
# Begin Source File

SOURCE=.\ScriptLoadResults.h
# End Source File
# Begin Source File

SOURCE=.\ScriptMakeDlg.h
# End Source File
# Begin Source File

SOURCE=.\ScriptMsgMake.h
# End Source File
# Begin Source File

SOURCE=.\ScriptPacket.h
# End Source File
# Begin Source File

SOURCE=.\ScriptParmList.h
# End Source File
# Begin Source File

SOURCE=.\ScriptParmUpdate.h
# End Source File
# Begin Source File

SOURCE=.\ScrollLineView.h
# End Source File
# Begin Source File

SOURCE=.\SelectTemplateDlg.h
# End Source File
# Begin Source File

SOURCE=.\Send.h
# End Source File
# Begin Source File

SOURCE=.\SendAbort.h
# End Source File
# Begin Source File

SOURCE=.\SendAckAlarm.h
# End Source File
# Begin Source File

SOURCE=.\SendAddListElement.h
# End Source File
# Begin Source File

SOURCE=.\SendBVLCI.h
# End Source File
# Begin Source File

SOURCE=.\SendBVLCResult.h
# End Source File
# Begin Source File

SOURCE=.\SendChangeListError.h
# End Source File
# Begin Source File

SOURCE=.\SendComplexACK.h
# End Source File
# Begin Source File

SOURCE=.\SendConfCOVNotification.h
# End Source File
# Begin Source File

SOURCE=.\SendConfEventNotification.h
# End Source File
# Begin Source File

SOURCE=.\SendConfirmedRequest.h
# End Source File
# Begin Source File

SOURCE=.\SendConfTextMsg.h
# End Source File
# Begin Source File

SOURCE=.\SendCreateObject.h
# End Source File
# Begin Source File

SOURCE=.\SendCreateObjectACK.h
# End Source File
# Begin Source File

SOURCE=.\SendCreateObjectError.h
# End Source File
# Begin Source File

SOURCE=.\SendDeleteFDTEntry.h
# End Source File
# Begin Source File

SOURCE=.\SendDeleteObject.h
# End Source File
# Begin Source File

SOURCE=.\SendDevice.h
# End Source File
# Begin Source File

SOURCE=.\SendDeviceCommCtrl.h
# End Source File
# Begin Source File

SOURCE=.\SendDisconnectCTN.h
# End Source File
# Begin Source File

SOURCE=.\SendEnet.h
# End Source File
# Begin Source File

SOURCE=.\SendError.h
# End Source File
# Begin Source File

SOURCE=.\SendEstablishCTN.h
# End Source File
# Begin Source File

SOURCE=.\SendGetAlarmSummary.h
# End Source File
# Begin Source File

SOURCE=.\SendGetAlarmSummaryACK.h
# End Source File
# Begin Source File

SOURCE=.\SendGetEnrollmentSummary.h
# End Source File
# Begin Source File

SOURCE=.\SendGetEnrollmentSummaryACK.h
# End Source File
# Begin Source File

SOURCE=.\SendGetEventInfo.h
# End Source File
# Begin Source File

SOURCE=.\SendGetEventInfoACK.h
# End Source File
# Begin Source File

SOURCE=.\SendIAm.h
# End Source File
# Begin Source File

SOURCE=.\SendIAmRTN.h
# End Source File
# Begin Source File

SOURCE=.\SendICouldBeRTN.h
# End Source File
# Begin Source File

SOURCE=.\SendIHave.h
# End Source File
# Begin Source File

SOURCE=.\SendInitRT.h
# End Source File
# Begin Source File

SOURCE=.\SendInitRTAck.h
# End Source File
# Begin Source File

SOURCE=.\SendIP.h
# End Source File
# Begin Source File

SOURCE=.\SendMSTP.h
# End Source File
# Begin Source File

SOURCE=.\SendNPCI.h
# End Source File
# Begin Source File

SOURCE=.\SendNull.h
# End Source File
# Begin Source File

SOURCE=.\SendPage.h
# End Source File
# Begin Source File

SOURCE=.\SendRaw.h
# End Source File
# Begin Source File

SOURCE=.\SendReadBDT.h
# End Source File
# Begin Source File

SOURCE=.\SendReadBDTAck.h
# End Source File
# Begin Source File

SOURCE=.\SendReadFDT.h
# End Source File
# Begin Source File

SOURCE=.\SendReadFDTAck.h
# End Source File
# Begin Source File

SOURCE=.\SendReadFile.h
# End Source File
# Begin Source File

SOURCE=.\SendReadFileACK.h
# End Source File
# Begin Source File

SOURCE=.\SendReadProp.h
# End Source File
# Begin Source File

SOURCE=.\SendReadPropACK.h
# End Source File
# Begin Source File

SOURCE=.\SendReadPropMult.h
# End Source File
# Begin Source File

SOURCE=.\SendReadPropMultACK.h
# End Source File
# Begin Source File

SOURCE=.\SendReadRange.h
# End Source File
# Begin Source File

SOURCE=.\SendRegisterFD.h
# End Source File
# Begin Source File

SOURCE=.\SendReinitDevice.h
# End Source File
# Begin Source File

SOURCE=.\SendReject.h
# End Source File
# Begin Source File

SOURCE=.\SendRejectMTN.h
# End Source File
# Begin Source File

SOURCE=.\SendRemoveListElement.h
# End Source File
# Begin Source File

SOURCE=.\SendRouterAvailable.h
# End Source File
# Begin Source File

SOURCE=.\SendRouterBusy.h
# End Source File
# Begin Source File

SOURCE=.\SendSegmentACK.h
# End Source File
# Begin Source File

SOURCE=.\SendSimpleACK.h
# End Source File
# Begin Source File

SOURCE=.\SendSubscribeCOV.h
# End Source File
# Begin Source File

SOURCE=.\SendSubscribeCOVProperty.h
# End Source File
# Begin Source File

SOURCE=.\SendTest.h
# End Source File
# Begin Source File

SOURCE=.\SendTimeSync.h
# End Source File
# Begin Source File

SOURCE=.\SendUnconfCOVNotification.h
# End Source File
# Begin Source File

SOURCE=.\SendUnconfEventNotification.h
# End Source File
# Begin Source File

SOURCE=.\SendUnconfTextMsg.h
# End Source File
# Begin Source File

SOURCE=.\SendUTCTimeSync.h
# End Source File
# Begin Source File

SOURCE=.\SendVendorNPDU.h
# End Source File
# Begin Source File

SOURCE=.\SendVTClose.h
# End Source File
# Begin Source File

SOURCE=.\SendVTCloseError.h
# End Source File
# Begin Source File

SOURCE=.\SendVTData.h
# End Source File
# Begin Source File

SOURCE=.\SendVTDataACK.h
# End Source File
# Begin Source File

SOURCE=.\SendVTOpen.h
# End Source File
# Begin Source File

SOURCE=.\SendVTOpenACK.h
# End Source File
# Begin Source File

SOURCE=.\SendWhoHas.h
# End Source File
# Begin Source File

SOURCE=.\SendWhoIs.h
# End Source File
# Begin Source File

SOURCE=.\SendWhoIsRTN.h
# End Source File
# Begin Source File

SOURCE=.\SendWriteBDT.h
# End Source File
# Begin Source File

SOURCE=.\SendWriteFile.h
# End Source File
# Begin Source File

SOURCE=.\SendWriteFileACK.h
# End Source File
# Begin Source File

SOURCE=.\SendWriteProp.h
# End Source File
# Begin Source File

SOURCE=.\SendWritePropMult.h
# End Source File
# Begin Source File

SOURCE=.\SendWritePropMultError.h
# End Source File
# Begin Source File

SOURCE=.\DUDAPI\Service.h
# End Source File
# Begin Source File

SOURCE=.\Statistics\ServiceList.h
# End Source File
# Begin Source File

SOURCE=.\sizecbar.h
# End Source File
# Begin Source File

SOURCE=.\StdAfx.h
# End Source File
# Begin Source File

SOURCE=.\DUDAPI\Stdobj.h
# End Source File
# Begin Source File

SOURCE=.\DUDAPI\Stdobjpr.h
# End Source File
# Begin Source File

SOURCE=.\Sniffer\StringTables.h
# End Source File
# Begin Source File

SOURCE=.\Statistics\supergridctrl.h
# End Source File
# Begin Source File

SOURCE=.\Statistics\TreeListCtrl.h
# End Source File
# Begin Source File

SOURCE=.\VdbPrint.h
# End Source File
# Begin Source File

SOURCE=.\VTS.h
# End Source File
# Begin Source File

SOURCE=.\VTSActionCommandDlg.h
# End Source File
# Begin Source File

SOURCE=.\VTSActionDlg.h
# End Source File
# Begin Source File

SOURCE=.\VTSActionListDlg.h
# End Source File
# Begin Source File

SOURCE=.\VTSAddressBindingDlg.h
# End Source File
# Begin Source File

SOURCE=.\VTSAny.h
# End Source File
# Begin Source File

SOURCE=.\DUDAPI\Vtsapi.h
# End Source File
# Begin Source File

SOURCE=.\VTSBackupRestoreDlg.h
# End Source File
# Begin Source File

SOURCE=.\VTSBackupRestoreProgressDlg.h
# End Source File
# Begin Source File

SOURCE=.\VTSBACnetTypeGenericPage.h
# End Source File
# Begin Source File

SOURCE=.\VTSBinaryPVDlg.h
# End Source File
# Begin Source File

SOURCE=.\VTSCalendarEntryDlg.h
# End Source File
# Begin Source File

SOURCE=.\VTSClientCOVDlg.h
# End Source File
# Begin Source File

SOURCE=.\VTSCOVSubscriptionDlg.h
# End Source File
# Begin Source File

SOURCE=.\VTSCtrl.h
# End Source File
# Begin Source File

SOURCE=.\VTSDailyScheduleDlg.h
# End Source File
# Begin Source File

SOURCE=.\VTSDateRangeDlg.h
# End Source File
# Begin Source File

SOURCE=.\VTSDateTimeDlg.h
# End Source File
# Begin Source File

SOURCE=.\VTSDaysOfWeekDlg.h
# End Source File
# Begin Source File

SOURCE=.\VTSDestinationDlg.h
# End Source File
# Begin Source File

SOURCE=.\VTSDeviceObjectPropertyReferenceDlg.h
# End Source File
# Begin Source File

SOURCE=.\VTSDeviceObjectReferenceDlg.h
# End Source File
# Begin Source File

SOURCE=.\VTSDeviceStatusDlg.h
# End Source File
# Begin Source File

SOURCE=.\VTSDevicesTreeDevPage.h
# End Source File
# Begin Source File

SOURCE=.\VTSDevicesTreeDlg.h
# End Source File
# Begin Source File

SOURCE=.\VTSDevicesTreeObjPage.h
# End Source File
# Begin Source File

SOURCE=.\VTSDevicesTreeOptPage.h
# End Source File
# Begin Source File

SOURCE=.\VTSDevicesTreePropPage.h
# End Source File
# Begin Source File

SOURCE=.\VTSDevicesTreeValuePage.h
# End Source File
# Begin Source File

SOURCE=.\VTSDiscoveryDlg.h
# End Source File
# Begin Source File

SOURCE=.\VTSDoc.h
# End Source File
# Begin Source File

SOURCE=.\VTSEngineeringUnitsDlg.h
# End Source File
# Begin Source File

SOURCE=.\VTSEventBitstringDlg.h
# End Source File
# Begin Source File

SOURCE=.\VTSEventBufferReadyDlg.h
# End Source File
# Begin Source File

SOURCE=.\VTSEventChangeOfStateDlg.h
# End Source File
# Begin Source File

SOURCE=.\VTSEventChangeOfValueDlg.h
# End Source File
# Begin Source File

SOURCE=.\VTSEventCommandFailureDlg.h
# End Source File
# Begin Source File

SOURCE=.\VTSEventFloatLimitDlg.h
# End Source File
# Begin Source File

SOURCE=.\VTSEventLifeSafetyDlg.h
# End Source File
# Begin Source File

SOURCE=.\VTSEventOutOfRangeDlg.h
# End Source File
# Begin Source File

SOURCE=.\VTSEventParameterDlg.h
# End Source File
# Begin Source File

SOURCE=.\VTSEventStateDlg.h
# End Source File
# Begin Source File

SOURCE=.\VTSEventTransitionBitsDlg.h
# End Source File
# Begin Source File

SOURCE=.\VTSEventTypeDlg.h
# End Source File
# Begin Source File

SOURCE=.\VTSFileAccessMethodDlg.h
# End Source File
# Begin Source File

SOURCE=.\VTSFiltersDlg.h
# End Source File
# Begin Source File

SOURCE=.\VTSInconsistentParsDlg.h
# End Source File
# Begin Source File

SOURCE=.\VTSInconsistentProgressDlg.h
# End Source File
# Begin Source File

SOURCE=.\VTSLifeSafetyModeDlg.h
# End Source File
# Begin Source File

SOURCE=.\VTSLifeSafetyOperationDlg.h
# End Source File
# Begin Source File

SOURCE=.\VTSLifeSafetyStateDlg.h
# End Source File
# Begin Source File

SOURCE=.\VTSLimitEnable.h
# End Source File
# Begin Source File

SOURCE=.\VTSLimitEnableDlg.h
# End Source File
# Begin Source File

SOURCE=.\VTSListOfBitstringDlg.h
# End Source File
# Begin Source File

SOURCE=.\VTSListOfDestinationDlg.h
# End Source File
# Begin Source File

SOURCE=.\VTSListOfLifeSafetyStateDlg.h
# End Source File
# Begin Source File

SOURCE=.\VTSListOfPropertyStatesDlg.h
# End Source File
# Begin Source File

SOURCE=.\VTSListOfReadAccessResultDlg.h
# End Source File
# Begin Source File

SOURCE=.\VTSListOfReadAccessSpecDlg.h
# End Source File
# Begin Source File

SOURCE=.\VTSListOfTimeStampDlg.h
# End Source File
# Begin Source File

SOURCE=.\VTSListOfUnsignedDlg.h
# End Source File
# Begin Source File

SOURCE=.\VTSLogRecordDlg.h
# End Source File
# Begin Source File

SOURCE=.\VTSLogStatusDlg.h
# End Source File
# Begin Source File

SOURCE=.\vtsmaintenancedlg.h
# End Source File
# Begin Source File

SOURCE=.\VTSNamesDlg.h
# End Source File
# Begin Source File

SOURCE=.\VTSNotificationParameters.h
# End Source File
# Begin Source File

SOURCE=.\VTSNotifyBitstring.h
# End Source File
# Begin Source File

SOURCE=.\VTSNotifyBufferReady.h
# End Source File
# Begin Source File

SOURCE=.\VTSNotifyCmdFailure.h
# End Source File
# Begin Source File

SOURCE=.\VTSNotifyComplex.h
# End Source File
# Begin Source File

SOURCE=.\VTSNotifyFloatLimit.h
# End Source File
# Begin Source File

SOURCE=.\VTSNotifyOutOfRange.h
# End Source File
# Begin Source File

SOURCE=.\VTSNotifyPage.h
# End Source File
# Begin Source File

SOURCE=.\VTSNotifyState.h
# End Source File
# Begin Source File

SOURCE=.\VTSNotifyTypeDlg.h
# End Source File
# Begin Source File

SOURCE=.\VTSNotifyValue.h
# End Source File
# Begin Source File

SOURCE=.\VTSObjectIdentifierDialog.h
# End Source File
# Begin Source File

SOURCE=.\VTSObjectPropertyReferenceDlg.h
# End Source File
# Begin Source File

SOURCE=.\VTSObjectPropertyValueDlg.h
# End Source File
# Begin Source File

SOURCE=.\VTSObjectTypeDlg.h
# End Source File
# Begin Source File

SOURCE=.\VTSObjectTypesSupportedDlg.h
# End Source File
# Begin Source File

SOURCE=.\VTSPacket.h
# End Source File
# Begin Source File

SOURCE=.\VTSPacketDB.h
# End Source File
# Begin Source File

SOURCE=.\VTSPolarityDlg.h
# End Source File
# Begin Source File

SOURCE=.\VTSPortARCNETDialog.h
# End Source File
# Begin Source File

SOURCE=.\VTSPortDlg.h
# End Source File
# Begin Source File

SOURCE=.\VTSPortEthernetDialog.h
# End Source File
# Begin Source File

SOURCE=.\VTSPortIPDialog.h
# End Source File
# Begin Source File

SOURCE=.\VTSPortMSTPDialog.h
# End Source File
# Begin Source File

SOURCE=.\ptp\VTSPortPTPDialog.h
# End Source File
# Begin Source File

SOURCE=.\VTSPreferences.h
# End Source File
# Begin Source File

SOURCE=.\VTSPreferencesDlg.h
# End Source File
# Begin Source File

SOURCE=.\VTSPriorityArrayDlg.h
# End Source File
# Begin Source File

SOURCE=.\VTSPriorityValueDlg.h
# End Source File
# Begin Source File

SOURCE=.\VTSProgramErrorDlg.h
# End Source File
# Begin Source File

SOURCE=.\VTSProgramRequestDlg.h
# End Source File
# Begin Source File

SOURCE=.\VTSProgramStateDlg.h
# End Source File
# Begin Source File

SOURCE=.\VTSPropertyIdentifierDlg.h
# End Source File
# Begin Source File

SOURCE=.\VTSPropertyPage.h
# End Source File
# Begin Source File

SOURCE=.\VTSPropertyReferenceDlg.h
# End Source File
# Begin Source File

SOURCE=.\VTSPropertyStatesDlg.h
# End Source File
# Begin Source File

SOURCE=.\VTSPropertyValueDlg.h
# End Source File
# Begin Source File

SOURCE=.\VTSPropValue.h
# End Source File
# Begin Source File

SOURCE=.\VTSQueue.h
# End Source File
# Begin Source File

SOURCE=.\VTSRecipientDlg.h
# End Source File
# Begin Source File

SOURCE=.\VTSRecipientProcess.h
# End Source File
# Begin Source File

SOURCE=.\VTSReliabilityDlg.h
# End Source File
# Begin Source File

SOURCE=.\VTSResultFlasDlg.h
# End Source File
# Begin Source File

SOURCE=.\VTSSegmentationDlg.h
# End Source File
# Begin Source File

SOURCE=.\VTSServicesSupportedDlg.h
# End Source File
# Begin Source File

SOURCE=.\VTSSessionKeyDlg.h
# End Source File
# Begin Source File

SOURCE=.\VTSSetPointReferenceDlg.h
# End Source File
# Begin Source File

SOURCE=.\VTSSilenceStateDlg.h
# End Source File
# Begin Source File

SOURCE=.\VTSSpecialEventDlg.h
# End Source File
# Begin Source File

SOURCE=.\Statistics\VTSStatisticsCollector.h
# End Source File
# Begin Source File

SOURCE=.\Statistics\VTSStatisticsDlg.h
# End Source File
# Begin Source File

SOURCE=.\Statistics\VTSStatisticsOptionsDlg.h
# End Source File
# Begin Source File

SOURCE=.\VTSStatusFlagsDlg.h
# End Source File
# Begin Source File

SOURCE=.\VTSTimeStamp.h
# End Source File
# Begin Source File

SOURCE=.\VTSTimeStampDlg.h
# End Source File
# Begin Source File

SOURCE=.\VTSTimeValueDlg.h
# End Source File
# Begin Source File

SOURCE=.\VTSVendorPropIDDlg.h
# End Source File
# Begin Source File

SOURCE=.\VTSVTClassDlg.h
# End Source File
# Begin Source File

SOURCE=.\VTSVTSessionDlg.h
# End Source File
# Begin Source File

SOURCE=.\VTSWeekNDayDlg.h
# End Source File
# Begin Source File

SOURCE=.\VTSWinMSTPPort.h
# End Source File
# Begin Source File

SOURCE=.\ptp\VTSWinPTPPort.h
# End Source File
# Begin Source File

SOURCE=.\BACnetLibrary\WinBACnetTaskManager.hpp
# End Source File
# Begin Source File

SOURCE=.\BACnetLibrary\WinIP.hpp
# End Source File
# Begin Source File

SOURCE=.\WinMSTP.h
# End Source File
# Begin Source File

SOURCE=.\ptp\WinPTP.h
# End Source File
# Begin Source File

SOURCE=.\BACnetLibrary\WinWinPcap.hpp
# End Source File
# Begin Source File

SOURCE=.\WPRPList.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=.\res\AboutBox.bmp
# End Source File
# Begin Source File

SOURCE=.\res\BACnetCheck.bmp
# End Source File
# Begin Source File

SOURCE=.\res\BACnetCheckT.bmp
# End Source File
# Begin Source File

SOURCE=.\res\BACnetMake.bmp
# End Source File
# Begin Source File

SOURCE=.\res\BACnetMakeT.bmp
# End Source File
# Begin Source File

SOURCE=.\res\bitmap1.bmp
# End Source File
# Begin Source File

SOURCE=.\res\BMALogo.bmp
# End Source File
# Begin Source File

SOURCE=.\res\bmp00001.bmp
# End Source File
# Begin Source File

SOURCE=.\res\colors.bmp
# End Source File
# Begin Source File

SOURCE=.\res\contents.bmp
# End Source File
# Begin Source File

SOURCE=.\res\cursor1.cur
# End Source File
# Begin Source File

SOURCE=.\res\cursor2.cur
# End Source File
# Begin Source File

SOURCE=.\res\devtree.bmp
# End Source File
# Begin Source File

SOURCE=.\res\epicstree.bmp
# End Source File
# Begin Source File

SOURCE=.\res\globalba.bmp
# End Source File
# Begin Source File

SOURCE=.\res\globalbal.bmp
# End Source File
# Begin Source File

SOURCE=.\res\ico00001.ico
# End Source File
# Begin Source File

SOURCE=.\res\idr_epic.ico
# End Source File
# Begin Source File

SOURCE=.\res\idr_vtst.ico
# End Source File
# Begin Source File

SOURCE=.\mdi.ico
# End Source File
# Begin Source File

SOURCE=.\res\nblink.bmp
# End Source File
# Begin Source File

SOURCE=.\res\packettr.bmp
# End Source File
# Begin Source File

SOURCE=.\res\RECVPKT.ico
# End Source File
# Begin Source File

SOURCE=.\res\Scripttool.bmp
# End Source File
# Begin Source File

SOURCE=.\res\SENDPKT.ico
# End Source File
# Begin Source File

SOURCE=.\res\Toolbar.bmp
# End Source File
# Begin Source File

SOURCE=.\res\toolbar1.bmp
# End Source File
# Begin Source File

SOURCE=.\res\VTS.ico
# End Source File
# Begin Source File

SOURCE=.\res\VTS.rc2
# End Source File
# Begin Source File

SOURCE=.\res\VTSDoc.ico
# End Source File
# Begin Source File

SOURCE=.\res\WinPCapLogo.bmp
# End Source File
# Begin Source File

SOURCE=.\res\WorkSpcTool.bmp
# End Source File
# End Group
# Begin Source File

SOURCE=.\NBLink\nb_link_settings.dll

!IF  "$(CFG)" == "VTS - Win32 Release"

# Begin Custom Build
OutDir=.\Release
InputPath=.\NBLink\nb_link_settings.dll

"$(OutDir)\nb_link_settings.dll" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	copy                                                           $(InputPath)                                                           $(OutDir)\ 

# End Custom Build

!ELSEIF  "$(CFG)" == "VTS - Win32 Debug"

# Begin Custom Build - Performing Custom Build Step on $(InputPath)
OutDir=.\Debug
InputPath=.\NBLink\nb_link_settings.dll

"$(OutDir)\nb_link_settings.dll" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	copy                                                           $(InputPath)                                                           $(OutDir)\ 

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\ReadMe.txt
# End Source File
# Begin Source File

SOURCE=.\VTS.reg
# End Source File
# Begin Source File

SOURCE=.\WinPcap\wpcap.lib
# End Source File
# Begin Source File

SOURCE=.\NBLink\nb_link_settings.lib
# End Source File
# Begin Source File

SOURCE=.\GetMACAddress\IPHlpApi.Lib
# End Source File
# End Target
# End Project
