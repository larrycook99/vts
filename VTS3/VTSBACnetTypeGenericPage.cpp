// VTSBACnetTypeGenericPage.cpp : implementation file
//

#include "stdafx.h"
#include "vts.h"
#include "VTSCtrl.h"
#include "VTSPropValue.h"

#include "VTSBACnetTypeGenericPage.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// VTSBACnetTypeGenericPage property page

IMPLEMENT_DYNCREATE(VTSBACnetTypeGenericPage, VTSPropertyPage)

VTSBACnetTypeGenericPage::VTSBACnetTypeGenericPage( VTSPageCallbackInterface * pparent )
						 : VTSPropertyPage(VTSBACnetTypeGenericPage::IDD, pparent)
{
	//{{AFX_DATA_INIT(VTSBACnetTypeGenericPage)
		// NOTE: the ClassWizard will add member initialization here
	m_strValue = _T("");
	//}}AFX_DATA_INIT

	m_pdevvalue = NULL;
}


VTSBACnetTypeGenericPage::VTSBACnetTypeGenericPage() : VTSPropertyPage()
{
	ASSERT(0);
}

VTSBACnetTypeGenericPage::~VTSBACnetTypeGenericPage()
{
}


void VTSBACnetTypeGenericPage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(VTSBACnetTypeGenericPage)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	DDX_Text(pDX, IDC_VALUE, m_strValue);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(VTSBACnetTypeGenericPage, VTSPropertyPage)
	//{{AFX_MSG_MAP(VTSBACnetTypeGenericPage)
	ON_EN_CHANGE(IDC_VALUE, OnChangeValue)
	ON_BN_CLICKED(IDC_VALUEBOOL, OnChangeValue)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// VTSBACnetTypeGenericPage message handlers

BOOL VTSBACnetTypeGenericPage::OnKillActive() 
{
	VTSPropertyPage::OnKillActive();
	EncodeValue();
	EnableControls(false);
	return TRUE;
}


BOOL VTSBACnetTypeGenericPage::OnSetActive() 
{
	m_pdevvalue = (VTSDevValue *) RetrieveCurrentData();
	if ( m_pdevvalue != NULL &&  !m_pdevvalue->IsKindOf(RUNTIME_CLASS(VTSDevValue)) )
		m_pdevvalue = NULL;

	VTSPropertyPage::OnSetActive();

	// stuff value into control... should be AFTER base OnSetActive because that initiates DDX
	// and our DecodeValue() stuffs the control ourselves...
	DecodeValue();
	EnableControls( m_pdevvalue != NULL );	

	return m_pdevvalue != NULL;
}



void VTSBACnetTypeGenericPage::EnableControls( bool fEnable )
{
	if ( m_pdevvalue == NULL )
		return;

	if ( GetDlgItem(IDC_VALUE)->GetSafeHwnd() != NULL )
	{
		GetDlgItem(IDC_VALUE)->EnableWindow( fEnable && m_pdevvalue->m_nType != 0  &&  m_pdevvalue->m_nType != 13  &&  m_pdevvalue->m_nType != 14 );

		// change format help string according to type
		CString str;
		str.LoadString(IDS_BTYPE0 + m_pdevvalue->m_nType);
		GetDlgItem(IDC_BTYPEDESCRIPTOR)->SetWindowText(str);

		// show or hide the value and check part if boolean...
		GetDlgItem(IDC_VALUEBOOL)->ShowWindow( m_pdevvalue->m_nType == 1 ? SW_SHOW : SW_HIDE );
		GetDlgItem(IDC_VALUE)->ShowWindow( m_pdevvalue->m_nType == 1 ? SW_HIDE : SW_SHOW );
	}
}


void VTSBACnetTypeGenericPage::OnChangeValue() 
{
	UpdateData(TRUE);
	EncodeValue();
	NotifyOfDataChange();
}


//	Given the contents of the m_Context and m_Value strings, encode the 
//	value into the descriptor based on the current selected component.
//

void VTSBACnetTypeGenericPage::EncodeValue( void )
{
	if ( m_pdevvalue == NULL )
		return;

	BACnetAPDUEncoder	compEncoder;

	switch (m_pdevvalue->m_nType)
	{
		case 0: {
					BACnetNull().Encode( compEncoder, m_pdevvalue->m_nContext );
					break;
				}
		case 1: {
					VTSBooleanCtrl m_Boolean( this, IDC_VALUEBOOL, true );

					m_Boolean.CtrlToObj();
					m_Boolean.Encode( compEncoder, m_pdevvalue->m_nContext );
					break;
				}
		case 2: {
					VTSUnsignedCtrl m_Unsigned( this, IDC_VALUE );

					m_Unsigned.CtrlToObj();
					m_Unsigned.Encode( compEncoder, m_pdevvalue->m_nContext );
					break;
				}
		case 3: {
					VTSIntegerCtrl m_Integer( this, IDC_VALUE );

					m_Integer.CtrlToObj();
					m_Integer.Encode( compEncoder, m_pdevvalue->m_nContext );
					break;
				}
		case 4: {
					VTSRealCtrl m_Real( this, IDC_VALUE );

					m_Real.CtrlToObj();
					m_Real.Encode( compEncoder, m_pdevvalue->m_nContext );
					break;
				}
		case 5: {
					VTSDoubleCtrl m_Double( this, IDC_VALUE );

					m_Double.CtrlToObj();
					m_Double.Encode( compEncoder, m_pdevvalue->m_nContext );
					break;
				}
		case 6: {
					VTSOctetStringCtrl m_OctetStr( this, IDC_VALUE );

					m_OctetStr.CtrlToObj();
					m_OctetStr.Encode( compEncoder, m_pdevvalue->m_nContext );
					break;
				}
		case 7: {
					VTSCharacterStringCtrl m_CharStr( this, IDC_VALUE );

					m_CharStr.CtrlToObj();
					m_CharStr.Encode( compEncoder, m_pdevvalue->m_nContext );
					break;
				}
		case 8: {
					VTSBitStringCtrl m_BitStr( this, IDC_VALUE );

					m_BitStr.CtrlToObj();
					m_BitStr.Encode( compEncoder, m_pdevvalue->m_nContext );
					break;
				}
		case 9: {
					VTSEnumeratedCtrl m_Enumerated( this, IDC_VALUE, 0, 0 );

					m_Enumerated.CtrlToObj();
					m_Enumerated.Encode( compEncoder, m_pdevvalue->m_nContext );
					break;
				}
		case 10: {
					VTSDateCtrl m_Date( this, IDC_VALUE );

					m_Date.CtrlToObj();
					m_Date.Encode( compEncoder, m_pdevvalue->m_nContext );
					break;
				}
		case 11: {
					VTSTimeCtrl m_Time( this, IDC_VALUE );

					m_Time.CtrlToObj();
					m_Time.Encode( compEncoder, m_pdevvalue->m_nContext );
					break;
				}
		case 13: {
					BACnetOpeningTag().Encode( compEncoder, m_pdevvalue->m_nContext );
					break;
				}
		case 14: {
					BACnetClosingTag().Encode( compEncoder, m_pdevvalue->m_nContext );
					break;
				}
		case 15: {
//					m_Unsigned.CtrlToObj();
//					m_Unsigned.Encode( compEncoder, m_pdevvalue->m_nContext );
					BACnetANY().Encode( compEncoder, m_pdevvalue->m_nContext );
					break;
				}
		default:	;
	}

	// save the encoding
	m_pdevvalue->m_nLength = compEncoder.pktLength;
	memcpy( m_pdevvalue->m_abContent, compEncoder.pktBuffer, compEncoder.pktLength );
}


//
//	Decode the contents of the current component into the m_Value control.
//

void VTSBACnetTypeGenericPage::DecodeValue( void )
{
	if ( m_pdevvalue == NULL )
		return;

	BACnetAPDUDecoder	dec( m_pdevvalue->m_abContent, m_pdevvalue->m_nLength );

	// depending on the type, decode the value
	try {
	switch ( m_pdevvalue->m_nType )
	{
		case 0: {
					GetDlgItem( IDC_VALUE )->SetWindowText( _T("") );
					break;
				}
		case 1: {
					VTSBooleanCtrl m_Boolean( this, IDC_VALUEBOOL, true );

					m_Boolean.Decode( dec );
					m_Boolean.ctrlNull = false;
					m_Boolean.ObjToCtrl();
					break;
				}
		case 2: {
					VTSUnsignedCtrl m_Unsigned( this, IDC_VALUE );

					m_Unsigned.Decode( dec );
					m_Unsigned.ctrlNull = false;
					m_Unsigned.ObjToCtrl();
					break;
				}
		case 3: {
					VTSIntegerCtrl m_Integer( this, IDC_VALUE );

					m_Integer.Decode( dec );
					m_Integer.ctrlNull = false;
					m_Integer.ObjToCtrl();
					break;
				}
		case 4: {
					VTSRealCtrl m_Real( this, IDC_VALUE );

					m_Real.Decode( dec );
					m_Real.ctrlNull = false;
					m_Real.ObjToCtrl();
					break;
				}
		case 5: {
					VTSDoubleCtrl m_Double( this, IDC_VALUE );

					m_Double.Decode( dec );
					m_Double.ctrlNull = false;
					m_Double.ObjToCtrl();
					break;
				}
		case 6: {
					VTSOctetStringCtrl m_OctetStr( this, IDC_VALUE );

					m_OctetStr.Decode( dec );
					m_OctetStr.ctrlNull = false;
					m_OctetStr.ObjToCtrl();
					break;
				}
		case 7: {
					VTSCharacterStringCtrl m_CharStr( this, IDC_VALUE );

					m_CharStr.Decode( dec );
					m_CharStr.ctrlNull = false;
					m_CharStr.ObjToCtrl();
					break;
				}
		case 8: {
					VTSBitStringCtrl m_BitStr( this, IDC_VALUE );

					m_BitStr.Decode( dec );
					m_BitStr.ctrlNull = false;
					m_BitStr.ObjToCtrl();
					break;
				}
		case 9: {
					VTSEnumeratedCtrl m_Enumerated( this, IDC_VALUE, 0, 0 );

					m_Enumerated.Decode( dec );
					m_Enumerated.ctrlNull = false;
					m_Enumerated.ObjToCtrl();
					break;
				}
		case 10: {
					VTSDateCtrl m_Date( this, IDC_VALUE );

					m_Date.Decode( dec );
					m_Date.ctrlNull = false;
					m_Date.ObjToCtrl();
					break;
				}
		case 11: {
					VTSTimeCtrl m_Time( this, IDC_VALUE );

					m_Time.Decode( dec );
					m_Time.ctrlNull = false;
					m_Time.ObjToCtrl();
					break;
				}
		case 13: {
					GetDlgItem( IDC_VALUE )->SetWindowText( _T("") );
					break;
				}
		case 14: {
					GetDlgItem( IDC_VALUE )->SetWindowText( _T("") );
					break;
				}
		default:	;
	}
	} catch(...) {}

	// synchronize the local m_Value
	GetDlgItem( IDC_VALUE )->GetWindowText( m_strValue );
}
