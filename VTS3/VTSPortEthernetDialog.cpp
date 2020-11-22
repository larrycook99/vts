// VTSPortEthernetDialog.cpp : implementation file
//

#include "stdafx.h"
#include "VTS.h"
#include "VTSPortEthernetDialog.h"

#include "WinWinPcap.hpp"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

short UnicodeToAscii( char *ap, wchar_t *up, short max );

/////////////////////////////////////////////////////////////////////////////
// VTSPortEthernetDialog dialog

IMPLEMENT_DYNCREATE(VTSPortEthernetDialog, VTSPropertyPage)


VTSPortEthernetDialog::VTSPortEthernetDialog( VTSPageCallbackInterface * pparent )
                      :VTSPropertyPage(VTSPortEthernetDialog::IDD, pparent)
{
	//{{AFX_DATA_INIT(VTSPortEthernetDialog)
	//}}AFX_DATA_INIT

	m_pstrConfigParms = NULL;
}

VTSPortEthernetDialog::VTSPortEthernetDialog(void) : VTSPropertyPage()
{
	ASSERT(0);
}


VTSPortEthernetDialog::~VTSPortEthernetDialog()
{
}



void VTSPortEthernetDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(VTSPortEthernetDialog)
	DDX_CBString(pDX, IDC_ADAPTERCOMBO, m_strAdapter);
	DDX_Check(pDX, IDC_PROMISCUOUS, m_fPromiscuous);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(VTSPortEthernetDialog, VTSPropertyPage)
	//{{AFX_MSG_MAP(VTSPortEthernetDialog)
	ON_BN_CLICKED(IDC_PROMISCUOUS, OnDataChange)
	ON_CBN_SELCHANGE(IDC_ADAPTERCOMBO, OnDataChange)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

//
//	VTSPortEthernetDialog::OnOK
//
//	When the user accepts the configuration information and clicks OK, 
//	this function is called which transfers the new configuration into 
//	the config data provided by the caller, VTSPort::Configuration.
//

BOOL VTSPortEthernetDialog::OnKillActive() 
{
	// calls DDX
	VTSPropertyPage::OnKillActive();
	CtrlToObj();

	return TRUE;
}



BOOL VTSPortEthernetDialog::OnSetActive() 
{
	m_pstrConfigParms = (CString * ) RetrieveCurrentData();

	// make sure the adapter list is initialized
	if (!gAdapterList)
		InitAdapterList();

	// look for ethernet ports
//	char nameBuff[64];

	// We'll keep adding and adding and adding the stuff since OnSetActive is called mutliple times
	// but the VTSPortEthernetDialog is not destroyed

	((CComboBox *) GetDlgItem(IDC_ADAPTERCOMBO))->ResetContent();

	for (int i = 0; i < gAdapterListLen; i++) {
//		if (gAdapterList[i].AdapterType == ETH_802_3_ADAPTER) {
			// convert the name
//			UnicodeToAscii( nameBuff, gAdapterList[i].MacDriverName, sizeof(nameBuff) );

			// add it to the combo
//			((CComboBox *) GetDlgItem(IDC_ADAPTERCOMBO))->AddString( nameBuff );
			((CComboBox *) GetDlgItem(IDC_ADAPTERCOMBO))->AddString( gAdapterList[i].AdapterDescription );
//			m_AdapterCombo.SetItemData( len, i );
		}

	// change dialog, produce message and hide other controls if no ethernet adapters present.
	// only need to do this once.

	if ( gAdapterListLen == 0 )
	{
		GetDlgItem(IDC_NOETHERNET)->ShowWindow(SW_SHOW);
		GetDlgItem(IDC_ADAPTERCOMBO)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_PROMISCUOUS)->ShowWindow(SW_HIDE);
	}

	ObjToCtrl();

	// calls DDX
	VTSPropertyPage::OnSetActive();
	NotifyOfDataChange();

	//m_AdapterCombo.SetCurSel( 0 );		 what if not found with string?

	// if we return FALSE here then this page will not be activated.  It will move on to the next. 
	// That's good because it's extremely painful to disable all of the stuff.

	return m_pstrConfigParms != NULL;
}
	

//
//	UnicodeToAscii
//

short UnicodeToAscii( char *ap, wchar_t *up, short max )
{
	short	n = 0
	;
	
	while ((*up != 0) && (n < max-1)) {
		*ap++ = (char)(*up++);
		n++;
	}
	*ap = 0;

	return n;
}


void VTSPortEthernetDialog::ObjToCtrl()
{
	if ( m_pstrConfigParms == NULL )
		return;

	// If there is already a configuration, set the combo box text to
	// it, otherwise, set it to the first one.  It would be nice if I went 
	// through all of the ports and picked one that wasn't already picked.
	// That will make a nice enhancement.

	if ( m_pstrConfigParms->GetLength() == 0 || m_pstrConfigParms->Find(',') == -1 )
	{
		if ( gAdapterListLen > 0 )
		{
			((CComboBox *) GetDlgItem(IDC_ADAPTERCOMBO))->GetLBText(0, *m_pstrConfigParms );
			*m_pstrConfigParms += ",0";
		}
	}

	m_fPromiscuous = FALSE;
	m_strAdapter.Empty();

	if ( m_pstrConfigParms->GetLength() != 0 )
	{
        char  * dst = m_strAdapter.GetBuffer(m_pstrConfigParms->GetLength());  // MAG 16JUN04 
		LPCSTR src = *m_pstrConfigParms;

		// copy the name over
		while (*src && (*src != ','))
			*dst++ = *src++;
		*dst++ = 0;

		// look for promiscuous mode
		if (*src == ',') {
			src += 1;
			m_fPromiscuous = (*src == '1');
		}

		m_strAdapter.ReleaseBuffer();
	}
}


void VTSPortEthernetDialog::CtrlToObj()
{
	if ( m_pstrConfigParms == NULL )
		return;

	*m_pstrConfigParms = m_strAdapter + "," + (m_fPromiscuous ? "1" : "0");
}


void VTSPortEthernetDialog::OnDataChange() 
{
	UpdateData( true );
	CtrlToObj();
	NotifyOfDataChange();
}

