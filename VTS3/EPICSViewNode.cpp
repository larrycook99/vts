// EPICSViewNode.cpp: implementation of the CEPICSViewNode class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "vts.h"
#include "EPICSTreeView.h"
#include "EPICSViewNode.h"
#include "EPICSViewInfoPanel.h"

///////////////////////////////
namespace PICS {
#include "db.h" 
#include "service.h"
#include "vtsapi.h"
#include "props.h"
#include "bacprim.h"
#include "dudapi.h"
#include "propid.h"
}

extern PICS::PICSdb * gPICSdb;



#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////


IMPLEMENT_DYNCREATE(CEPICSViewNode, CObject)


CEPICSViewNode::CEPICSViewNode( CEPICSTreeView * ptreeview )
{
	m_ptreeview = ptreeview;
}

CEPICSViewNode::~CEPICSViewNode()
{

}

void CEPICSViewNode::LoadInfoPanel()
{
}


//==============================================================================


CEPICSViewNodeRoot::CEPICSViewNodeRoot( CEPICSTreeView * ptreeview, int nSyntaxErrors, int nConErrors )
				   :CEPICSViewNode(ptreeview)
{
	m_nSyntaxE = nSyntaxErrors;
	m_nConE = nConErrors;
}


CEPICSViewNodeRoot::~CEPICSViewNodeRoot()
{
}


void CEPICSViewNodeRoot::LoadInfoPanel()
{
	CEPICSViewInfoPanel * ppanel;
	CString str;

	if ( m_ptreeview->GetInfoPanel() == NULL || !m_ptreeview->GetInfoPanel()->IsKindOf(RUNTIME_CLASS(CEPICSViewInfoPanel)) )
		ppanel = (CEPICSViewInfoPanel *) m_ptreeview->CreateInfoView(RUNTIME_CLASS(CEPICSViewInfoPanel), TRUE);
	else
		ppanel = (CEPICSViewInfoPanel *) m_ptreeview->GetInfoPanel();

	ppanel->ResetPanel();

	if ( gPICSdb )
	{
		str.Format("Vendor Name: %s\nProduct Name: %s\nProduct Model Number: %s\nProduct Description: %s\n",
			gPICSdb->VendorName, gPICSdb->ProductName, gPICSdb->ProductModelNumber, gPICSdb->ProductDescription);

		ppanel->AddLine("EPICS Loaded:");
		ppanel->SetParagraphStyle(0, 180, 0, PFA_LEFT);
		ppanel->SetCharStyle(8);
		ppanel->AddText(str);

		if ( m_nConE != 0 )
		{
			ppanel->SetParagraphStyle(0, 0, 0, PFA_LEFT);
			ppanel->SetCharStyle(10, RGB(255, 0, 0));
			ppanel->AddLine("EPICS Consistency Errors:");
			LoadErrorFile(ppanel);
		}
	}
	else
	{
		// NO DB loaded... is it due to syntax errors?
		if ( m_nSyntaxE != 0 )
		{
			ppanel->SetCharStyle(10, RGB(255, 0, 0));
			ppanel->AddLine("Syntax Errors Loading File:");
			LoadErrorFile(ppanel);
		}
		else
		{
			// No DB due to simple empty EPICS situation... (cancel, etc.)
			ppanel->EnableWindow(FALSE);
		}
	}
}



void CEPICSViewNodeRoot::LoadErrorFile( CEPICSViewInfoPanel * ppanel )
{
	ppanel->SetParagraphStyle(PFN_BULLET, 200, 160, PFA_LEFT);		//  1/4 inch offset
	ppanel->SetCharStyle(8);

	CStdioFile fileErr;
	CString str;

	try
	{
		CString fileName;
		GetTempPath( MAX_PATH, fileName.GetBuffer( MAX_PATH ) );
		fileName.ReleaseBuffer();
		fileName += FILE_CHECK_EPICS_CONS;
		
		fileErr.Open( fileName, CFile::modeRead | CFile::shareDenyNone);
		int n = 0;
		while ( fileErr.ReadString(str) )
		{
			str.TrimLeft();
			if ( str.GetLength() > 0 )
			{
				if ( n++ > 0 )
					ppanel->AddLine("");
				ppanel->AddText(str);
			}
		}
	}
	catch(...)
	{}

	fileErr.Close();

	// Scroll the window to the top
	ppanel->GetRichEditCtrl().SetSel(0,0);
	// TODO: scroll bar is visible, but isn't enabled until you switch to another view
	// and back again.  No idea WHY
}
