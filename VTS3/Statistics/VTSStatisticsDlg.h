#if !defined(AFX_VTSSTATISTICSDLG_H__2F16E2CA_CD71_4445_A572_199FDB9712CF__INCLUDED_)
#define AFX_VTSSTATISTICSDLG_H__2F16E2CA_CD71_4445_A572_199FDB9712CF__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// VTSStatisticsDlg.h : header file
//

class VTSDoc;
typedef VTSDoc* VTSDocPtr;

//#include "VTSDoc.h"

/////////////////////////////////////////////////////////////////////////////
// VTSStatisticsDlg dialog
#include "TreeListCtrl.h"

class VTSStatisticsDlg : public CDialog
{
// Construction
public:
	void UpdateStatisticsView(CString type,int data[12]);
	void Update(char* service);
	VTSStatisticsDlg(CWnd* pParent = NULL);   // standard constructor

	VTSDocPtr m_pDoc;

// Dialog Data
	//{{AFX_DATA(VTSStatisticsDlg)
	enum { IDD = IDD_STATISTICS };
	CStatic	m_Frame;
	int		m_nArcnetR;
	int		m_nArcnetS;
	int		m_nEthernetR;
	int		m_nEthernetS;
	int		m_nIPR;
	int		m_nIPS;
	int		m_nMSTPR;
	CString	m_strPacketType;
	int		m_nMSTPS;
	int		m_nPTPR;
	int		m_nPTPS;
	int		m_nTotalR;
	int		m_nTotalS;
	//}}AFX_DATA
private:
	void GetServiceName(char *summary, char *service);

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(VTSStatisticsDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	CTreeListCtrl* m_pList;
	// Generated message map functions
	//{{AFX_MSG(VTSStatisticsDlg)
	virtual BOOL OnInitDialog();
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnOptions();
	afx_msg void OnDestroy();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_VTSSTATISTICSDLG_H__2F16E2CA_CD71_4445_A572_199FDB9712CF__INCLUDED_)
