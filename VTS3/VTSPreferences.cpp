// VTSPreferences.cpp: implementation of the VTSPreferences class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "vts.h"
#include "VTSPreferences.h"
#include "VTSPreferencesDlg.h"

#include "VTSDoc.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

VTSPreferences::VTSPreferences()
{
   // We formerly initialized all fields on.
   // But "Port" is almost never useful (since VTS doesn't seem happy with multiple Ports),
   // and Source and Destination now show network portion if SADDR/DADDR are disabled.
   // So we turn off Port/SNET/SADDR/DNET/DADDR
   m_nSummaryFields = 0x21B;        // bit mask of display options

   m_nSummaryColumnWidth[0] = 60;   // 1 Number
   m_nSummaryColumnWidth[1] = 100;  // 1 timestamp
   m_nSummaryColumnWidth[2] = 30;   // 0 port
   m_nSummaryColumnWidth[3] = 100;  // 1 source
   m_nSummaryColumnWidth[4] = 100;  // 1 destination
   m_nSummaryColumnWidth[5] = 80;   // 0 snet
   m_nSummaryColumnWidth[6] = 80;   // 0 saddr
   m_nSummaryColumnWidth[7] = 80;   // 0 dnet
   m_nSummaryColumnWidth[8] = 80;   // 0 daddr
   m_nSummaryColumnWidth[9] = 360;  // 1 service type

   m_nInvokeID = 0;
   m_nTimeFormat = 0;
   m_nCachePacketCount = 10000;     // start with about 2M of memory
   m_nAutoscrollTimeout = 300;      // seconds
   m_fRelativePacketFile = true;
   m_fVerifyDelete = true;
   m_bAutoScroll = true;
   m_bRecvPkt = true;
   m_bSaveSentPkt = true;
   m_resendInterval = 0;
}


bool VTSPreferences::SView_IsColumnOn(int nColumn)
{
   ASSERT(nColumn < SUMMARY_VIEW_COLUMN_COUNT && nColumn >= 0);
   return (m_nSummaryFields & (1 << nColumn)) != 0;
}

void VTSPreferences::SView_SetColumnOn(int nColumn, bool fOn /* = true */ )
{
   ASSERT(nColumn < SUMMARY_VIEW_COLUMN_COUNT && nColumn >= 0);
   m_nSummaryFields = (m_nSummaryFields & ~(1<<nColumn)) | ((fOn ? 1 : 0) << nColumn);
}


int VTSPreferences::SView_GetColumnWidth(int nColumn)
{
   ASSERT(nColumn < SUMMARY_VIEW_COLUMN_COUNT && nColumn >= 0);
   return m_nSummaryColumnWidth[nColumn];
}


void VTSPreferences::SView_SetColumnWidth(int nColumn, int nWidth)
{
   ASSERT(nColumn < SUMMARY_VIEW_COLUMN_COUNT && nColumn >= 0);
   m_nSummaryColumnWidth[nColumn] = nWidth;
}


void VTSPreferences::Setting_SetLastEPICS(LPCSTR lpszEPICSFile)
{
   if ( lpszEPICSFile == NULL )
      m_strLastEPICS.Empty();
   else
      m_strLastEPICS = lpszEPICSFile;
}

void VTSPreferences::Setting_SetTextEditor(LPCSTR lpszTextEditor)
{
   if (lpszTextEditor == NULL )
      m_textEditor.Empty();
   else
      m_textEditor = lpszTextEditor;
}

LPCSTR VTSPreferences::Setting_GetTextEditor() const
{
   // Return a default editor if none has been set
   return (m_textEditor.IsEmpty()) ? "Notepad.exe" : m_textEditor;
}

//
// VTSPreferences::Load
//

void VTSPreferences::Load( void )
{
   CWinApp* pApp = AfxGetApp();

   m_nSummaryFields = pApp->GetProfileInt( "SummaryView", "Fields", m_nSummaryFields); // LJT 5/22/2006 changed default from 0xff to 0x3ff to include all columns

   char szBuffer[15];
   for ( int i = 0; i < SUMMARY_VIEW_COLUMN_COUNT; i++ )
   {
      sprintf(szBuffer, "CWidth%d", i);
      m_nSummaryColumnWidth[i] = pApp->GetProfileInt( "SummaryView", szBuffer, m_nSummaryColumnWidth[i] );
   }

   m_nTimeFormat = pApp->GetProfileInt( "SummaryView", "TimeFormat", m_nTimeFormat);
   m_nInvokeID = pApp->GetProfileInt( "Send", "InvokeID", m_nInvokeID);
   m_nCachePacketCount = pApp->GetProfileInt( "Settings", "CachePacketCount", m_nCachePacketCount);
   m_nAutoscrollTimeout = pApp->GetProfileInt( "Settings", "AutoscrollTimeout", m_nAutoscrollTimeout);

   int nRelative = pApp->GetProfileInt( "Settings", "RelativePacketFile", m_fRelativePacketFile ? 1 : 0);
   m_fRelativePacketFile = nRelative != 0;

   nRelative = pApp->GetProfileInt( "Settings", "VerifyDelete", m_fVerifyDelete ? 1 : 0);
   m_fVerifyDelete = nRelative != 0;

   nRelative = pApp->GetProfileInt( "Settings", "LoadEPICS", m_fLoadEPICS ? 1 : 0);
   m_fLoadEPICS = nRelative != 0;

   m_strLastEPICS = pApp->GetProfileString("Settings", "LastEPICS", NULL);

   nRelative = pApp->GetProfileInt( "Settings", "AutoScroll", m_bAutoScroll ? 1 : 0);
   m_bAutoScroll = nRelative != 0;

   nRelative = pApp->GetProfileInt( "Settings", "RecvPkt", m_bRecvPkt ? 1 : 0);
   m_bRecvPkt = nRelative != 0;

   nRelative = pApp->GetProfileInt( "Settings", "SaveSentPkt", m_bSaveSentPkt ? 1 : 0);
   m_bSaveSentPkt = nRelative != 0;

   m_resendInterval = pApp->GetProfileInt( "Settings", "ResendInterval", m_resendInterval);

   m_textEditor = pApp->GetProfileString("Settings", "TextEditor", NULL);
}

//
// VTSPreferences::Save
//

void VTSPreferences::Save( void )
{
   CWinApp* pApp = AfxGetApp();

   pApp->WriteProfileInt( "SummaryView", "Fields", m_nSummaryFields );

   char szBuffer[15];
   for ( int i = 0; i < SUMMARY_VIEW_COLUMN_COUNT; i++ )
   {
      sprintf(szBuffer, "CWidth%d", i);
      pApp->WriteProfileInt( "SummaryView", szBuffer, m_nSummaryColumnWidth[i] );
   }

   pApp->WriteProfileInt( "SummaryView", "TimeFormat", m_nTimeFormat );
   pApp->WriteProfileInt( "Send", "InvokeID", m_nInvokeID );

   pApp->WriteProfileInt( "Settings", "CachePacketCount", m_nCachePacketCount);
   pApp->WriteProfileInt( "Settings", "AutoscrollTimeout", m_nAutoscrollTimeout);
   pApp->WriteProfileInt( "Settings", "RelativePacketFile", m_fRelativePacketFile ? 1 : 0);
   pApp->WriteProfileInt( "Settings", "VerifyDelete", m_fVerifyDelete ? 1 : 0);
   pApp->WriteProfileInt( "Settings", "LoadEPICS", m_fLoadEPICS ? 1 : 0);
   pApp->WriteProfileString( "Settings", "LastEPICS", m_strLastEPICS);

   pApp->WriteProfileInt( "Settings", "AutoScroll", m_bAutoScroll ? 1 : 0);
   pApp->WriteProfileInt( "Settings", "RecvPkt", m_bRecvPkt ? 1 : 0);
   pApp->WriteProfileInt( "Settings", "SaveSentPkt", m_bSaveSentPkt ? 1 : 0);
   pApp->WriteProfileInt( "Settings", "ResendInterval", m_resendInterval);
   pApp->WriteProfileString( "Settings", "TextEditor", m_textEditor);
}


void VTSPreferences::DoPrefsDlg()
{
   VTSPreferencesDlg dlg(AfxGetMainWnd());

   dlg.m_nAutoscrollTimeout = Setting_GetAutoscrollTimeout();
   dlg.m_nPacketCount = Setting_GetCachePacketCount();
   dlg.m_nRelative = Setting_IsPacketFileRelative() ? 0 : 1;
   dlg.m_fVerify = Setting_IsVerifyDelete() ? TRUE : FALSE;
   dlg.m_fLoadEPICS = Setting_IsLoadEPICS() ? TRUE : FALSE;
   dlg.m_bAutoScroll = Setting_IsAutoScroll() ? TRUE : FALSE;
   dlg.m_bRecvPkt = Setting_IsRecvPkt() ? TRUE : FALSE;
   dlg.m_bSaveSentPkt = Setting_IsSaveSentPkt() ? TRUE : FALSE;
   dlg.m_resendInterval = Setting_GetResendInterval();
   dlg.m_textEditor = Setting_GetTextEditor();

   if ( dlg.DoModal() == IDOK )
   {
      Setting_SetAutoscrollTimeout(dlg.m_nAutoscrollTimeout);
      Setting_SetPacketFileRelative(dlg.m_nRelative != 1);

      if ( dlg.m_nPacketCount != Setting_GetCachePacketCount() )
      {
         Setting_SetCachePacketCount(dlg.m_nPacketCount);
         ((VTSDoc *) ((VTSApp *) AfxGetApp())->GetWorkspace())->SetNewCacheSize();
      }

      Setting_SetVerifyDelete(dlg.m_fVerify != 0);
      Setting_SetLoadEPICS(dlg.m_fLoadEPICS != 0);

      Setting_SetAutoScroll(dlg.m_bAutoScroll ? true : false);
      Setting_SetRecvPkt(dlg.m_bRecvPkt ? true : false);
      Setting_SetSaveSentPkt(dlg.m_bSaveSentPkt ? true : false);
      Setting_SetResendInterval(dlg.m_resendInterval);
      Setting_SetTextEditor(dlg.m_textEditor);
   }
}
