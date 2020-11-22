// ScriptParmUpdate.cpp : implementation file
//

#include "stdafx.h"
#include "VTS.h"

#include "ScriptBase.h"
#include "ScriptParmUpdate.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// ScriptParmUpdate dialog

ScriptParmUpdate::ScriptParmUpdate( ScriptParmPtr pp, CWnd * pparent /* = NULL */ )
	: CDialog(ScriptParmUpdate::IDD, pparent)	//madanner 5/03, needed for proper frame parent , NULL)
	, m_ParmValueOK(true)
{
	m_ParmDesc = pp->parmDesc;
	m_ParmName = pp->parmName;
	m_ParmScriptValue = pp->parmScriptValue;
	m_ParmValue = pp->parmValue;

	m_ParmValueNormalized = pp->parmValue;
}

ScriptParmUpdate::ScriptParmUpdate(CWnd* pParent /*=NULL*/)
	: CDialog(ScriptParmUpdate::IDD, pParent)
	, m_ParmValueOK(true)
{
	//{{AFX_DATA_INIT(ScriptParmUpdate)
	m_ParmDesc = _T("");
	m_ParmName = _T("");
	m_ParmScriptValue = _T("");
	m_ParmValue = _T("");
	//}}AFX_DATA_INIT
}

void ScriptParmUpdate::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(ScriptParmUpdate)
	DDX_Text(pDX, IDC_PARMDESC, m_ParmDesc);
	DDX_Text(pDX, IDC_PARMNAME, m_ParmName);
	DDX_Text(pDX, IDC_PARMSCRIPTVALUE, m_ParmScriptValue);
	DDX_Text(pDX, IDC_PARMVALUE, m_ParmValue);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(ScriptParmUpdate, CDialog)
	//{{AFX_MSG_MAP(ScriptParmUpdate)
	ON_EN_UPDATE(IDC_PARMVALUE, OnUpdateParmValue)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// ScriptParmUpdate message handlers

void ScriptParmUpdate::OnUpdateParmValue() 
{
	// sync the member variables with the dialog controls
	UpdateData( true );

	CString			valu
	;
	ScriptScanner	scan( m_ParmValue )
	;
	ScriptToken		tok
	;
	CEdit*			parmErrorCtrl = (CEdit *)GetDlgItem( IDC_PARMERRMSG )
	;

	try {
		valu = "";
		scan.Next( tok );

		for (;;) {
			// get the value and save it
			if ((tok.tokenType != scriptValue) && (tok.tokenType != scriptKeyword)
					&& (tok.tokenType != scriptReference))
				throw "Parameter value expected";

			// save the value
			valu += tok.tokenValue;
			scan.Next( tok );

			// check for EOL
			if (tok.tokenType == scriptEOL)
				break;

			// check for more
			if ((tok.tokenType != scriptSymbol) || (tok.tokenSymbol != ','))
				throw "End-of-line or comma expected";
			valu += ", ";

			// get ready for next value
			scan.Next( tok );
		}
	}
	catch (char *errMsg) {
		parmErrorCtrl->SetWindowText( errMsg );
		m_ParmValueOK = false;
		return;
	}
	if ((tok.tokenType != scriptValue) && (tok.tokenType != scriptKeyword)) {
		// What is the purpose of this?  Both values overwritten below
		parmErrorCtrl->SetWindowText( "Value or keyword expected" );
		m_ParmValueOK = false;
	}

	// success, update to normalized value
	parmErrorCtrl->SetWindowText( _T("") );
	m_ParmValueNormalized = valu;
	m_ParmValueOK = true;
}
