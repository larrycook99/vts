// VTSPreferencesDlg.cpp : implementation file
//

#include "stdafx.h"
#include "vts.h"
#include "VTSPreferences.h"				// only for some defines
#include "VTSPreferencesDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// VTSPreferencesDlg dialog


VTSPreferencesDlg::VTSPreferencesDlg(CWnd* pParent /*=NULL*/)
	: CDialog(VTSPreferencesDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(VTSPreferencesDlg)
	m_nAutoscrollTimeout = 0;
	m_nPacketCount = 0;
	m_nRelative = -1;
	m_fVerify = FALSE;
	m_fLoadEPICS = FALSE;
	m_bAutoScroll = FALSE;
	m_bRecvPkt = FALSE;
	m_bSaveSentPkt = FALSE;
	m_resendInterval = 0;
	m_textEditor = "Notepad.exe";
	//}}AFX_DATA_INIT
}


void VTSPreferencesDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(VTSPreferencesDlg)
	DDX_Text(pDX, IDC_PREF_ASTIMEOUT, m_nAutoscrollTimeout);
	DDV_MinMaxInt(pDX, m_nAutoscrollTimeout, 0, SUMMARY_VIEW_MAXCACHESLOTS);
	DDX_Text(pDX, IDC_PREF_PACKETCOUNT, m_nPacketCount);
	DDV_MinMaxInt(pDX, m_nPacketCount, 1, 32000);
	DDX_Radio(pDX, IDC_PREF_RELATIVE, m_nRelative);
	DDX_Check(pDX, IDC_PREF_DELETEVERIFY, m_fVerify);
	DDX_Check(pDX, IDC_PREF_LOADEPICS, m_fLoadEPICS);
	DDX_Check(pDX, IDC_AUTOSCROLL, m_bAutoScroll);
	DDX_Check(pDX, IDC_RECVPKT, m_bRecvPkt);
	DDX_Check(pDX, IDC_SAVESENTPKT, m_bSaveSentPkt);
	DDX_Text(pDX, IDC_RESENDINTERVAL, m_resendInterval);
	DDX_Text(pDX, IDC_TEXT_EDITOR, m_textEditor);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(VTSPreferencesDlg, CDialog)
	//{{AFX_MSG_MAP(VTSPreferencesDlg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// VTSPreferencesDlg message handlers
