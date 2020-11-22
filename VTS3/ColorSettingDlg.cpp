// ColorSettingDlg.cpp : implementation file
//

#include "stdafx.h"
#include "vts.h"
#include "ColorSettingDlg.h"
#include "resource.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

const COLORREF COLOR_DEFINITION[COLOR_PDU_COUNT] = 
	{
		RGB(64, 0, 255), 
		RGB(0, 128, 0),
		RGB(255, 0, 255),
		RGB(128, 0, 0),
		RGB(0, 128, 128),
		RGB(255, 0, 0)
	};

COLORREF SUMMARY_PACKET_COLOR[COLOR_PDU_COUNT] = 
	{
		COLOR_DEFINITION[0], 
		COLOR_DEFINITION[1],
		COLOR_DEFINITION[2],
		COLOR_DEFINITION[3],
		COLOR_DEFINITION[4],
		COLOR_DEFINITION[5]
	};
		
/////////////////////////////////////////////////////////////////////////////
// CColorSettingDlg dialog


CColorSettingDlg::CColorSettingDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CColorSettingDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CColorSettingDlg)
	//}}AFX_DATA_INIT
}


void CColorSettingDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CColorSettingDlg)
	DDX_Control(pDX, IDC_COLORCOMBO6, m_ctrl[5]);
	DDX_Control(pDX, IDC_COLORCOMBO5, m_ctrl[4]);
	DDX_Control(pDX, IDC_COLORCOMBO3, m_ctrl[3]);
	DDX_Control(pDX, IDC_COLORCOMBO4, m_ctrl[2]);
	DDX_Control(pDX, IDC_COLORCOMBO2, m_ctrl[1]);
	DDX_Control(pDX, IDC_COLORCOMBO1, m_ctrl[0]);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CColorSettingDlg, CDialog)
	//{{AFX_MSG_MAP(CColorSettingDlg)
	ON_BN_CLICKED(IDC_RESET, OnReset)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CColorSettingDlg message handlers

BOOL CColorSettingDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	int i;
	
	// TODO: Why not use CColorDialog to select ARBITRARY colors?
	// Replace the Combo boxes with buttons, each showing the current color.
	// Pressing the button invokes CColorDialog.

	m_imageList.Create(IDB_COLOR, 16, 1, RGB(255, 255, 255));

	COMBOBOXEXITEM     cbi;
	memset( &cbi, 0, sizeof(cbi) );
	cbi.mask = CBEIF_IMAGE | CBEIF_SELECTEDIMAGE;		

	// Fill the combo boxes with our color choices
	for(i = 0; i < COLOR_PDU_COUNT; i++)
	{
		m_ctrl[i].SetImageList(&m_imageList);

		cbi.iItem = i;
		cbi.iImage = i;
		cbi.iSelectedImage = i;
		
		for(int j = 0; j < COLOR_PDU_COUNT; j++)
		{
			int at = m_ctrl[j].InsertItem(&cbi);
		}
	}	

	// Show the current color selection
	for(i = 0; i < COLOR_PDU_COUNT; i++)
	{
		for(int j = 0; j < COLOR_PDU_COUNT; j++)
		{
			if(SUMMARY_PACKET_COLOR[i] == COLOR_DEFINITION[j])
			{
				m_ctrl[i].SetCurSel(j);
				break;
			}
		}
	}	

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CColorSettingDlg::OnOK() 
{
	// TODO: Add extra validation here
	for(int index = 0; index < COLOR_PDU_COUNT; index++)
	{
		SUMMARY_PACKET_COLOR[index] = COLOR_DEFINITION[m_ctrl[index].GetCurSel()];		
	}
	
	CDialog::OnOK();
}

void CColorSettingDlg::OnReset() 
{
	// TODO: Add your control notification handler code here
	for(int i = 0; i < COLOR_PDU_COUNT; i++)
	{		
		m_ctrl[i].SetCurSel(i);
	}	

	UpdateData();
}
