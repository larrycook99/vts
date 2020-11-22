// ScriptParmList.cpp : implementation file
//

#include "stdafx.h"
#include "VTS.h"

#include "ScriptBase.h"
#include "ScriptParmList.h"
#include "ScriptParmUpdate.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

ScriptParmList		*gCurrentEnv;
ScriptParmListList	gScriptParmLists;

/////////////////////////////////////////////////////////////////////////////
// ScriptParmList

IMPLEMENT_DYNCREATE(ScriptParmList, CListView)

//
//	ScriptParmList::ScriptParmList
//

ScriptParmList::ScriptParmList()
	: m_iSelectedElem(-1)
{
	gScriptParmLists.Add( this );
}

//
//	ScriptParmList::~ScriptParmList
//

ScriptParmList::~ScriptParmList()
{
	// remove this from the list of parameter lists
	gScriptParmLists.Remove( this );

	// if this was the current environment, tell everyone (else) to unload
	if (this == gCurrentEnv) {
		for (int i = 0; i < gScriptParmLists.Length(); i++)
			gScriptParmLists[i]->UnloadEnv();
		gCurrentEnv = 0;
	}
}

BEGIN_MESSAGE_MAP(ScriptParmList, CListView)
	//{{AFX_MSG_MAP(ScriptParmList)
	ON_WM_CREATE()
	ON_NOTIFY_REFLECT(LVN_ITEMCHANGING, OnItemChanging)
	ON_NOTIFY_REFLECT(NM_DBLCLK, OnDoubleClick)
	ON_NOTIFY_REFLECT(LVN_COLUMNCLICK, OnColumnClick)
	ON_WM_DESTROY()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

//
//	ScriptParm::ScriptParm
//

ScriptParm::ScriptParm( const char *name )
	: parmName(name), parmValue(""), parmScriptValue(""), parmDesc("")
	, parmLine(-1)
{
	parmSymbol = ScriptToken::HashCode( name );
}

//
//	ScriptParm::~ScriptParm
//

ScriptParm::~ScriptParm( void )
{
}

//
//	ScriptParmList::Add
//

void ScriptParmList::Add( CString &name, CString &valu, CString &desc, int line, bool env )
{
	int				indx
	,				code = ScriptToken::HashCode( name )
	;
	ScriptParmPtr	pp
	;

	// look for the var
	indx = LookupIndex( code );

	// if found, update the script value and line
	if (indx >= 0) {
		// the data is tucked behind the item element
		pp = (ScriptParmPtr)m_pListCtrl->GetItemData( indx );

		// if the current value matches the script update it
		if (pp->parmValue == pp->parmScriptValue) {
			pp->parmValue = valu;
			m_pListCtrl->SetItemText( indx, 1, (LPCTSTR)pp->parmValue );
		}

		// set the script value, line, and description, leave the current value alone
		pp->parmScriptValue = valu;
		pp->parmLine = line;
		pp->parmDesc = desc;
		pp->parmEnv = env;

		// reset the mark
		pp->parmMark = false;

		// make sure the list gets the new description
		m_pListCtrl->SetItemText( indx, 2, (LPCTSTR)pp->parmDesc );

		// set the image status
		m_pListCtrl->SetItem( indx, 0, TVIF_IMAGE, 0
			, (pp->parmEnv ? 2 : 0) + ((pp->parmValue != pp->parmScriptValue) ? 1 : 0)
			, 0, 0, 0
			);
	} else {
		// create a new parameter record
		pp = new ScriptParm( name );

		// save the values and line number
		pp->parmValue = pp->parmScriptValue = valu;
		pp->parmLine = line;
		pp->parmDesc = desc;
		pp->parmEnv = env;

		// reset the mark
		pp->parmMark = false;

		// add the item to the list
		indx = m_pListCtrl->InsertItem( 0x7FFFFFFF, (LPCTSTR)pp->parmName );
		m_pListCtrl->SetItemText( indx, 1, (LPCTSTR)pp->parmValue );
		m_pListCtrl->SetItemText( indx, 2, (LPCTSTR)pp->parmDesc );
		m_pListCtrl->SetItemData( indx, (DWORD)pp );

		// set the image for the list to match the status of the parm
		m_pListCtrl->SetItem( indx, 0, TVIF_IMAGE, 0
			, (pp->parmEnv ? 2 : 0) + ((pp->parmValue != pp->parmScriptValue) ? 1 : 0)
			, 0, 0, 0
			);

		//Now add the item to the shadow array
		m_apParms.Add(pp);
	}
}

//
//	ScriptParmList::Mark
//
//	First part of a mark/sweep method for cleaning up parameters.  The idea is to leave as 
//	many untouched as possible between versions of a script, so if a parameter is still 
//	"marked" after re-parsing the script then it's OK to delete the variable.
//
//	When env is true, this is being used during the process of synchronizing the environment
//	parameters, see ScriptParmList::MatchEnv().
//

void ScriptParmList::Mark( bool env )
{
	for (int i = 0; i < m_pListCtrl->GetItemCount(); i++) {
		ScriptParmPtr pp = (ScriptParmPtr)m_pListCtrl->GetItemData( i );
		if (pp->parmEnv == env)
			pp->parmMark = true;
	}
}

//
//	ScriptParmList::Release
//

void ScriptParmList::Release( void )
{
	for (int i = 0; i < m_pListCtrl->GetItemCount(); i++) {
		ScriptParmPtr pp = (ScriptParmPtr)m_pListCtrl->GetItemData( i );

		// if it's marked, delete it
		if (pp->parmMark) {
			// remove from list
			m_pListCtrl->DeleteItem( i );

			//now kill from shadow array
			m_apParms.RemoveAt(i);

			// delete the parameter
			delete pp;

			// reset to check this spot again
			i -= 1;
		}
	}
}

//
//	ScriptParmList::Length
//

int ScriptParmList::Length( void )
{
// Changed to shadow array for thread safety...
// madanner 5/03
//	return m_pListCtrl->GetItemCount();

	return m_apParms.GetSize();
}

//
//	ScriptParmList::operator []
//

ScriptParmPtr ScriptParmList::operator []( int indx )
{
	ASSERT( (indx >= 0) && (indx < Length()) );

// Changed to shadow array for thread safety...
// madanner 5/03
//	return (ScriptParmPtr)m_pListCtrl->GetItemData( indx );

	return (ScriptParmPtr) m_apParms[indx];
}

//
//	ScriptParmList::Reset
//

void ScriptParmList::Reset( void )
{
	bool	isEnv = (this == gCurrentEnv)
	;

	for (int i = 0; i < m_pListCtrl->GetItemCount(); i++) {
		ScriptParmPtr pp = (ScriptParmPtr)m_pListCtrl->GetItemData( i );

		// if this is an env parm and we are not resetting the current env, skip it
		if (pp->parmEnv && !isEnv)
			continue;

		// if they're the same, skip it
		if (pp->parmValue == pp->parmScriptValue)
			continue;

		// set to the script value
		pp->parmValue = pp->parmScriptValue;

		// clean up the display
		m_pListCtrl->SetItemText( i, 1, (LPCTSTR)pp->parmValue );
		m_pListCtrl->SetItem( i, 0, TVIF_IMAGE, 0, (pp->parmEnv ? 2 : 0), 0, 0, 0 );
	}

	// if this is the current environment, tell everyone else to match
	if (isEnv)
		for (int i = 0; i < gScriptParmLists.Length(); i++) {
			ScriptParmListPtr splp = gScriptParmLists[i];

			if (splp != gCurrentEnv)
				splp->MatchEnv();
		}
}

//
//	ScriptParmList::LookupIndex
//

int ScriptParmList::LookupIndex( int code )
{
// Changed to shadow array for thread safety
// madanner 5/03

//	for (int i = 0; i < m_pListCtrl->GetItemCount(); i++) {
//		ScriptParmPtr pp = (ScriptParmPtr)m_pListCtrl->GetItemData( i );
	for (int i = 0; i < m_apParms.GetSize(); i++) {
		ScriptParmPtr pp = (ScriptParmPtr) m_apParms[i];
		if (pp->parmSymbol == code)
			return i;
	}

	// no success
	return -1;
}

//
//	ScriptParmList::LookupParm
//

ScriptParmPtr ScriptParmList::LookupParm( int code )
{
// Changed to shadow array for thread safety
// madanner 5/03
//	for (int i = 0; i < m_pListCtrl->GetItemCount(); i++) {
//		ScriptParmPtr pp = (ScriptParmPtr)m_pListCtrl->GetItemData( i );

	for (int i = 0; i < m_apParms.GetSize(); i++) {
		ScriptParmPtr pp = (ScriptParmPtr) m_apParms[i];
		if (pp->parmSymbol == code)
			return pp;
	}

	// no success
	return 0;
}

//
//	ScriptParmList::SetCurrentEnv
//

void ScriptParmList::SetCurrentEnv( void )
{
	for (int i = 0; i < m_pListCtrl->GetItemCount(); i++) {
		ScriptParmPtr pp = (ScriptParmPtr)m_pListCtrl->GetItemData( i );

		// tell it its an environment parameter
		pp->parmEnv = true;

		// set the image status
		m_pListCtrl->SetItem( i, 0, TVIF_IMAGE, 0
			, 2 + ((pp->parmValue != pp->parmScriptValue) ? 1 : 0)
			, 0, 0, 0
			);
	}
}

//
//	ScriptParmList::ResetCurrentEnv
//

void ScriptParmList::ResetCurrentEnv( void )
{
	for (int i = 0; i < m_pListCtrl->GetItemCount(); i++) {
		ScriptParmPtr pp = (ScriptParmPtr)m_pListCtrl->GetItemData( i );

		// tell it its not an environment parameter
		pp->parmEnv = false;

		// set the image status
		m_pListCtrl->SetItem( i, 0, TVIF_IMAGE, 0
			, ((pp->parmValue != pp->parmScriptValue) ? 1 : 0)
			, 0, 0, 0
			);
	}
}

//
//	ScriptParmList::LoadEnv
//

void ScriptParmList::LoadEnv( void )
{
	int				indx
	;
	ScriptParmPtr	pp
	;

	for (int i = 0; i < gCurrentEnv->Length(); i++) {
		ScriptParmPtr envpp = (*gCurrentEnv)[ i ];

		// look for the var
		indx = LookupIndex( envpp->parmSymbol );

		// if found, make sure it's locally defined.  If it is, leave it alone.
		if (indx >= 0) {
			// the data is tucked behind the item element
			pp = (ScriptParmPtr)m_pListCtrl->GetItemData( indx );

			// if it's a non-env parm, leave it alone
			if (!pp->parmEnv)
				continue;

			// should never get here, this wasn't properly unloaded
			ASSERT( 0 );
		}

		// create a new parameter record
		pp = new ScriptParm( envpp->parmName );

		// save the values and line number
		pp->parmValue = envpp->parmValue;
		pp->parmScriptValue = envpp->parmScriptValue;
		pp->parmLine = -1;
		pp->parmDesc = envpp->parmDesc;
		pp->parmEnv = true;

		// reset the mark
		pp->parmMark = false;

		// add the item to the list
		indx = m_pListCtrl->InsertItem( 0x7FFFFFFF, (LPCTSTR)pp->parmName );
		m_pListCtrl->SetItemText( indx, 1, (LPCTSTR)pp->parmValue );
		m_pListCtrl->SetItemText( indx, 2, (LPCTSTR)pp->parmDesc );
		m_pListCtrl->SetItemData( indx, (DWORD)pp );

		// set the image for the list to match the status of the parm
		m_pListCtrl->SetItem( indx, 0, TVIF_IMAGE, 0
			, 2 + ((pp->parmValue != pp->parmScriptValue) ? 1 : 0)
			, 0, 0, 0
			);

		//madanner 5/03
		//Now add to shadow array
		m_apParms.Add(pp);
	}
}

//
//	ScriptParmList::UnloadEnv
//

void ScriptParmList::UnloadEnv( void )
{
	for (int i = 0; i < m_pListCtrl->GetItemCount(); i++) {
		ScriptParmPtr pp = (ScriptParmPtr)m_pListCtrl->GetItemData( i );

		// if it's an env parm, delete it
		if (pp->parmEnv) {
			// remove from list
			m_pListCtrl->DeleteItem( i );

			// Now kill from shadow array
			m_apParms.RemoveAt(i);

			// delete the parameter
			delete pp;

			// reset to check this spot again
			i -= 1;
		}
	}
}

//
//	ScriptParmList::MatchEnv
//

void ScriptParmList::MatchEnv( void )
{
	ASSERT( gCurrentEnv );

	// mark current env parms
	Mark( true );

	// go through the parms in the current env
	for (int i = 0; i < gCurrentEnv->Length(); i++) {
		ScriptParmPtr envpp = (*gCurrentEnv)[ i ];

		// match it
		MatchEnvParm( envpp );
	}

	// delete obsolete env parms
	Release();
}

//
//	ScriptParmList::MatchEnvParm
//

void ScriptParmList::MatchEnvParm( ScriptParmPtr envpp )
{
	int				indx
	;
	ScriptParmPtr	pp
	;

	// look for the var
	indx = LookupIndex( envpp->parmSymbol );

	// if found, update the script value and line
	if (indx >= 0) {
		// the data is tucked behind the item element
		pp = (ScriptParmPtr)m_pListCtrl->GetItemData( indx );

		// if there's already a non-env parm defined, leave it alone
		if (pp->parmEnv) {
			// update all of the information
			pp->parmValue = envpp->parmValue;
			pp->parmScriptValue = envpp->parmScriptValue;
			pp->parmLine = -1;
			pp->parmDesc = envpp->parmDesc;
			pp->parmEnv = true;

			// reset the mark
			pp->parmMark = false;

			// make sure the list gets the new information
			m_pListCtrl->SetItemText( indx, 1, (LPCTSTR)pp->parmValue );
			m_pListCtrl->SetItemText( indx, 2, (LPCTSTR)pp->parmDesc );

			// set the image status
			m_pListCtrl->SetItem( indx, 0, TVIF_IMAGE, 0
				, 2 + ((pp->parmValue != pp->parmScriptValue) ? 1 : 0)
				, 0, 0, 0
				);
		}
	} else {
		// create a new parameter record
		pp = new ScriptParm( envpp->parmName );

		// save the values and line number
		pp->parmValue = envpp->parmValue;
		pp->parmScriptValue = envpp->parmScriptValue;
		pp->parmLine = -1;
		pp->parmDesc = envpp->parmDesc;
		pp->parmEnv = true;

		// reset the mark
		pp->parmMark = false;

		// add the item to the list
		indx = m_pListCtrl->InsertItem( 0x7FFFFFFF, (LPCTSTR)pp->parmName );
		m_pListCtrl->SetItemText( indx, 1, (LPCTSTR)pp->parmValue );
		m_pListCtrl->SetItemText( indx, 2, (LPCTSTR)pp->parmDesc );
		m_pListCtrl->SetItemData( indx, (DWORD)pp );

		// set the image for the list to match the status of the parm
		m_pListCtrl->SetItem( indx, 0, TVIF_IMAGE, 0
			, 2 + ((pp->parmValue != pp->parmScriptValue) ? 1 : 0)
			, 0, 0, 0
			);

		//madanner 5/03
		//Now add to shadow array
		m_apParms.Add(pp);
	}
}

/////////////////////////////////////////////////////////////////////////////
// ScriptParmList drawing

void ScriptParmList::OnDraw(CDC* pDC)
{
	CDocument* pDoc = GetDocument();
	// TODO: add draw code here
}

/////////////////////////////////////////////////////////////////////////////
// ScriptParmList diagnostics

#ifdef _DEBUG
void ScriptParmList::AssertValid() const
{
	CListView::AssertValid();
}

void ScriptParmList::Dump(CDumpContext& dc) const
{
	CListView::Dump(dc);
}
#endif //_DEBUG

//
//	ScriptParmList::PreCreateWindow
//

BOOL ScriptParmList::PreCreateWindow(CREATESTRUCT& cs) 
{
	// change to a report list
	cs.style &= ~LVS_TYPEMASK;
	cs.style |= LVS_REPORT;

	return CListView::PreCreateWindow(cs);
}

//
//	ScriptParmList::OnCreate
//

int ScriptParmList::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	// let the list view create the control
	if (CListView::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	// get a pointer to the control
	m_pListCtrl = &GetListCtrl();

	// add the columns
	m_pListCtrl->InsertColumn( 0, _T("Name"), LVCFMT_LEFT, 100 );
	m_pListCtrl->InsertColumn( 1, _T("Value"), LVCFMT_LEFT, 100 );
	m_pListCtrl->InsertColumn( 2, _T("Description"), LVCFMT_LEFT, 200 );

	// initialize the status image list
	m_ilStatus.Create( IDB_PARMSTATUS, 12, 1, RGB(255,0,255) );
	m_pListCtrl->SetImageList( &m_ilStatus, LVSIL_SMALL );

	// success
	return 0;
}

//
//	ScriptParmList::OnItemChanging
//

void ScriptParmList::OnItemChanging(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;

	// forget messages that dont change the state
	if (pNMListView->uOldState == pNMListView->uNewState)
		return;

	if ((pNMListView->uNewState & LVIS_SELECTED) != 0) {
		TRACE1( "New selection %d\n", pNMListView->iItem );
		m_iSelectedElem = pNMListView->iItem;
	} else
	if (pNMListView->iItem == m_iSelectedElem) {
		TRACE0( "No more selection\n" );
		m_iSelectedElem = -1;
	}

	*pResult = 0;
}

//
//	ScriptParmList::OnDoubleClick
//

void ScriptParmList::OnDoubleClick(NMHDR* pNMHDR, LRESULT* pResult) 
{
	// tell the caller we handled it
	*pResult = 0;

	// toss if nothing selected
	if (m_iSelectedElem < 0)
		return;

	// get a pointer to the element
	ScriptParmPtr pp = (ScriptParmPtr)m_pListCtrl->GetItemData( m_iSelectedElem );

	// make sure it's valid
	ASSERT( pp );

	// construct the dialog box
	// madanner, 5/03 necessary for new frame window parent
	ScriptParmUpdate	dlg( pp, AfxGetMainWnd()->GetActiveWindow());

	// execute the modal
	if (dlg.DoModal() && dlg.m_ParmValueOK) {
		// update the parameter value from the scanned results
		pp->parmValue = dlg.m_ParmValueNormalized;

		// update the list to the new value and status
		// madanner 11/5/02

		UpdateItemVisual(m_iSelectedElem, pp);
//		m_pListCtrl->SetItemText( m_iSelectedElem, 1, (LPCTSTR)pp->parmValue );
//		m_pListCtrl->SetItem( m_iSelectedElem, 0, TVIF_IMAGE, 0
//			, (pp->parmEnv ? 2 : 0) + ((pp->parmValue != pp->parmScriptValue) ? 1 : 0)
//			, 0, 0, 0
//			);

		// if this is an env parm, update everybody else
		if (pp->parmEnv)
			for (int i = 0; i < gScriptParmLists.Length(); i++) {
				ScriptParmListPtr splp = gScriptParmLists[i];

				if (splp != this)
					splp->MatchEnvParm( pp );
			}
	}
}


void ScriptParmList::UpdateItemVisual( int nIndex, ScriptParmPtr pp )
{
//	change to thread safe posts...

	if ( nIndex >= 0 )
	{
		m_pListCtrl->SetItemText( nIndex, 1, (LPCTSTR)pp->parmValue );
		m_pListCtrl->SetItem( nIndex, 0, TVIF_IMAGE, 0, (pp->parmEnv ? 2 : 0) + ((pp->parmValue != pp->parmScriptValue) ? 1 : 0), 0, 0, 0);
	}
}

// This method differs from the UpdateItemVisual only in that it POSTS update messages instead of 
// sends them.  This is necessary because this is called only from the executor thread...
// Window access in another thread hangs the system

void ScriptParmList::UpdateParameterVisual( ScriptParmPtr pp )
{
	ASSERT(pp != NULL);

	if ( m_pListCtrl == NULL  ||  pp == NULL )
		return;

	// look for the var
//	m_pListCtrl->LockWindowUpdate();
	UpdateItemVisual(LookupIndex( pp->parmSymbol ), pp);
//	m_pListCtrl->UnlockWindowUpdate();
}

/*
BOOL CListCtrl::SetItemText(int nItem, int nSubItem, LPCTSTR lpszText)
{
	ASSERT(::IsWindow(m_hWnd));
	ASSERT((GetStyle() & LVS_OWNERDATA)==0);
	LVITEM lvi;
	lvi.iSubItem = nSubItem;
	lvi.pszText = (LPTSTR) lpszText;
	return (BOOL) ::SendMessage(m_hWnd, LVM_SETITEMTEXT, nItem, (LPARAM)&lvi);
}
*/


//
//	ScriptParmList::OnColumnClick
//

void ScriptParmList::OnColumnClick(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;
	
	TRACE1( "Column %d clicked\n", pNMListView->iSubItem );

	*pResult = 0;
}

//
//	ScriptParmList::OnDestroy
//

void ScriptParmList::OnDestroy() 
{
	// delete the parameters
	for (int i = 0; i < m_pListCtrl->GetItemCount(); i++)
		delete (ScriptParmPtr)m_pListCtrl->GetItemData( i );

	// kill values in shadow array
	m_apParms.RemoveAll();

	// continue with destruction
	CListView::OnDestroy();
}

//
//	ScriptParmListList::ScriptParmListList
//

ScriptParmListList::ScriptParmListList( void )
{
}

//
//	ScriptParmListList::~ScriptParmListList
//

ScriptParmListList::~ScriptParmListList( void )
{
}

//
//	ScriptParmListList::Add
//

void ScriptParmListList::Add( ScriptParmListPtr splp )
{
	// add it to our list of parameter lists
	AddTail( splp );
}

//
//	ScriptParmListList::Remove
//

void ScriptParmListList::Remove( ScriptParmListPtr splp )
{
	for (POSITION xpos = GetHeadPosition(); xpos; ) {
		POSITION		cur = xpos;
		ScriptParmListPtr cursplp = (ScriptParmListPtr)GetNext( xpos );

		if (cursplp == splp) {
			RemoveAt( cur );
			return;
		}
	}
}

//
//	ScriptParmListList::Length
//

int ScriptParmListList::Length( void )
{
	return CList<ScriptParmListPtr,ScriptParmListPtr>::GetCount();
}

//
//	ScriptParmListList::operator []
//

ScriptParmListPtr ScriptParmListList::operator []( int i )
{
	POSITION	pos = FindIndex( i )
	;

	ASSERT( pos != NULL );
	return GetAt( pos );
}
