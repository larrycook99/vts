// VTSBackupRestoreDlg.cpp: implementation of the VTSBackupRestoreDlg class.
// Jingbo Gao, Sep 20 2004
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "vts.h"
#include "VTSBackupRestoreDlg.h"
#include "VTSDoc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// VTSBackupRestoreDlg dialog


VTSBackupRestoreDlg::VTSBackupRestoreDlg(const VTSNames& names, const VTSPorts& ports, 
										 CWnd* pParent /*=NULL*/)
	: CDialog(VTSBackupRestoreDlg::IDD, pParent),
	  m_names(names),
	  m_ports(ports)
    , Delay(0)
	, timeout(60)
{
	//{{AFX_DATA_INIT(VTSBackupRestoreDlg)
	m_nFunction = -1;
	m_strBackupFileName = _T("");
	m_strDevice = _T("");
	m_strPassword = _T("");
	m_strPort = _T("");
	m_strDevObjInst = _T("");
	//}}AFX_DATA_INIT
}


void VTSBackupRestoreDlg::DoDataExchange(CDataExchange* pDX)
{
  CDialog::DoDataExchange(pDX);
  //{{AFX_DATA_MAP(VTSBackupRestoreDlg)
  DDX_Control(pDX, IDC_ALLBACKUPRESTORE, m_funCtrl);
  DDX_Control(pDX, IDC_PORTCOMBO, m_portCtrl);
  DDX_Control(pDX, IDC_DEVICECOMBO, m_deviceCtrl);
  DDX_Radio(pDX, IDC_ALLBACKUPRESTORE, m_nFunction);
  DDX_Text(pDX, IDC_BACKUPFILENAME, m_strBackupFileName);
  DDX_CBString(pDX, IDC_DEVICECOMBO, m_strDevice);
  DDX_Text(pDX, IDC_PASSWORD, m_strPassword);
  DDX_CBString(pDX, IDC_PORTCOMBO, m_strPort);
  DDX_Text(pDX, IDC_DEVICEOBJINST, m_strDevObjInst);
  DDV_MaxChars(pDX, m_strDevObjInst, 7);
  DDX_Control(pDX, IDC_DELAY, m_Delay);
  DDX_Control(pDX, IDC_BACKUPTIMEOUT, m_backup_timeout);
  //}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(VTSBackupRestoreDlg, CDialog)
	//{{AFX_MSG_MAP(VTSBackupRestoreDlg)
	ON_BN_CLICKED(IDC_BROWSE, OnBrowse)
	ON_CBN_SELENDOK(IDC_DEVICECOMBO, OnSelendokDevicecombo)
	ON_CBN_SELENDOK(IDC_PORTCOMBO, OnSelendokPortcombo)
	//}}AFX_MSG_MAP
  ON_EN_CHANGE(IDC_DELAY, OnEnChangeDelay)
  ON_EN_CHANGE(IDC_BACKUPTIMEOUT, OnEnChangeTimeout)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// VTSBackupRestoreDlg message handlers

BOOL VTSBackupRestoreDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
	int i = 0;
	for ( ; i < m_names.GetSize(); i++ )
	{
		m_deviceCtrl.AddString( ((VTSName *) m_names.GetAt(i))->m_strName );
	}
	m_deviceCtrl.SelectString(-1, "IUT");

	VTSPortPtr pPort;
	for ( i = 0; i < m_ports.GetSize(); i++ )
	{
		pPort = (VTSPort *) m_ports.GetAt(i);
		if (pPort->IsEnabled())
		{
			m_portCtrl.AddString( pPort->m_strName );
			m_portCtrl.SelectString(-1, pPort->m_strName);
		}
	}

	CString str;
	str.Format( "%d", Delay );
	m_Delay.SetWindowText( (LPCTSTR)str );

	str.Format( "%d", timeout );
	m_backup_timeout.SetWindowText( (LPCTSTR)str );

	m_funCtrl.SetCheck(2);
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}


void VTSBackupRestoreDlg::OnBrowse() 
{
	// TODO: Add your control notification handler code here
	CFileDialog	fd( TRUE, NULL, NULL, NULL, "BackupIndex (*.backupindex)|*.backupindex|All Files (*.*)|*.*||" );
	
	if (fd.DoModal() == IDOK)
	{
		m_strBackupFileName = fd.GetPathName();
		m_strBackupFileName.Replace(".backupindex", "");	
		CWnd* pWnd = GetDlgItem(IDC_BACKUPFILENAME);
		pWnd->SetWindowText(m_strBackupFileName);
	}

}

void VTSBackupRestoreDlg::OnOK() 
{
	// TODO: Add extra validation here
	UpdateData();
	if (m_strPort.IsEmpty())
	{
		AfxMessageBox("VTS port must be specified!");
		m_portCtrl.SetFocus();
		return;
	}
	if (m_strDevice.IsEmpty())
	{
		AfxMessageBox("Device must be specified!");
		m_portCtrl.SetFocus();
		return;
	}
	if (!m_strDevObjInst.IsEmpty())
	{
		int num = atoi(m_strDevObjInst);
		if (num < 0 || num > 4194303) 
		{
			AfxMessageBox("Device Obj Instance number should be in the"
				" rang from 0 to 4194303!");
			m_portCtrl.SetFocus();
			GetDlgItem(IDC_DEVICEOBJINST)->SetFocus();			
			return;
		}
	}
	if (m_strBackupFileName.IsEmpty())
	{
		AfxMessageBox("Backup filename must be specified!");
		GetDlgItem(IDC_BACKUPFILENAME)->SetFocus();	
		return;
	}
	
	CDialog::OnOK();
}

void VTSBackupRestoreDlg::OnSelendokDevicecombo() 
{
	// TODO: Add your control notification handler code here
	CString strDevice;
	m_deviceCtrl.GetWindowText(strDevice);
	CString strPort;
	m_portCtrl.GetWindowText(strPort);

	// consisteny check between Device and Port
	VTSPortPtr pPort;
	int index = const_cast<VTSNames&>(m_names).FindIndex(strDevice);
	ASSERT(index != -1);
	VTSName* pName = m_names.GetAt(index);
	if (pName->m_pportLink != NULL)
	{
		pPort = pName->m_pportLink;
		if (pPort->m_strName.CompareNoCase(strPort))
		{
			m_portCtrl.SetCurSel(-1);
		}
	}
}

void VTSBackupRestoreDlg::OnSelendokPortcombo() 
{
	// TODO: Add your control notification handler code here
	CString strPort;
	m_portCtrl.GetWindowText(strPort);
	CString strDevice;
	m_deviceCtrl.GetWindowText(strDevice);

	// consisteny check between Device and Port
	if (!strDevice.IsEmpty())
	{
		VTSPortPtr pPort;
		int index = const_cast<VTSNames&>(m_names).FindIndex(strDevice);
		ASSERT(index != -1);
		VTSName* pName = m_names.GetAt(index);
		if (pName->m_pportLink != NULL)
		{
			pPort = pName->m_pportLink;
			if (pPort->m_strName.CompareNoCase(strPort)) 
			{
				m_deviceCtrl.SetCurSel(-1);
			}
		}
	}	

}

void VTSBackupRestoreDlg::OnEnChangeDelay()
{
  CString delaystr;
  m_Delay.GetWindowText(delaystr);
  if(delaystr.GetLength())
    Delay = atoi((LPCTSTR)delaystr);
  else
    Delay = 0;
}

void VTSBackupRestoreDlg::OnEnChangeTimeout()
{
  CString timeoutstr;
  m_backup_timeout.GetWindowText(timeoutstr);
  if(timeoutstr.GetLength())
    timeout = atoi((LPCTSTR)timeoutstr);
  else
    timeout = 60;
}