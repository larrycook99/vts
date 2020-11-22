// EPICSViewNodeAppService.cpp: implementation of the CEPICSViewNodeAppService class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"

#include "vts.h"
#include "EPICSTreeView.h"
#include "EPICSViewNode.h"
#include "EPICSViewInfoPanel.h"
#include "EPICSViewPropPanel.h"
#include "StringTables.h"

///////////////////////////////
namespace PICS {
#include "db.h" 
#include "service.h"
#include "vtsapi.h"
#include "stdobj.h"
#include "props.h"
#include "bacprim.h"
#include "dudapi.h"
#include "propid.h"

extern "C" void CreatePropertyFromEPICS( PICS::generic_object * pObj, int PropId, BACnetAnyValue * pbacnetAnyValue );

}

extern PICS::PICSdb * gPICSdb;

namespace NetworkSniffer {
	extern char *BACnetPropertyIdentifier[];
}

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////


CEPICSViewNodeAppService::CEPICSViewNodeAppService( CEPICSTreeView * ptreeview )
				   :CEPICSViewNode(ptreeview)
{
}


CEPICSViewNodeAppService::~CEPICSViewNodeAppService()
{
}


void CEPICSViewNodeAppService::LoadInfoPanel()
{
	CEPICSViewListPanel * ppanel = (CEPICSViewListPanel *) m_ptreeview->GetInfoPanel();

	if (  ppanel == NULL || !m_ptreeview->GetInfoPanel()->IsKindOf(RUNTIME_CLASS(CEPICSViewListPanel)) )
		ppanel = (CEPICSViewListPanel *) m_ptreeview->CreateInfoView(RUNTIME_CLASS(CEPICSViewListPanel), TRUE);


	ppanel->Reset();
	CListCtrl & listCtrl = ppanel->GetListCtrl();

	listCtrl.InsertColumn( 0, _T("Service"), LVCFMT_LEFT, 200 );
	listCtrl.InsertColumn( 1, _T("Initiate"), LVCFMT_LEFT, 75 );
	listCtrl.InsertColumn( 2, _T("Execute"), LVCFMT_LEFT, 75 );


	if ( gPICSdb )
	{
		int x = 0;
		for ( int i=0; i < PICS::GetStandardServicesSize(); i++ )
			if ( gPICSdb->BACnetStandardServices[i] != ssNotSupported )
			{
				listCtrl.InsertItem(x, PICS::GetStandardServicesName(i) );
				listCtrl.SetItemText(x, 1, (gPICSdb->BACnetStandardServices[i] & ssInitiate) != 0 ? "True" : "False");
				listCtrl.SetItemText(x, 2, (gPICSdb->BACnetStandardServices[i] & ssExecute) != 0 ? "True" : "False");
				x++;
			}
	}
}


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////



CEPICSViewNodeBIBB::CEPICSViewNodeBIBB( CEPICSTreeView * ptreeview )
				   :CEPICSViewNode(ptreeview)
{
}


CEPICSViewNodeBIBB::~CEPICSViewNodeBIBB()
{
}


void CEPICSViewNodeBIBB::LoadInfoPanel()
{
	CEPICSViewListPanel * ppanel = (CEPICSViewListPanel *) m_ptreeview->GetInfoPanel();

	if (  ppanel == NULL || !m_ptreeview->GetInfoPanel()->IsKindOf(RUNTIME_CLASS(CEPICSViewListPanel)) )
		ppanel = (CEPICSViewListPanel *) m_ptreeview->CreateInfoView(RUNTIME_CLASS(CEPICSViewListPanel), TRUE);

	ppanel->Reset();
	CListCtrl & listCtrl = ppanel->GetListCtrl();

	listCtrl.InsertColumn( 0, _T("Supported BIBBs"), LVCFMT_LEFT, 200 );


	if ( gPICSdb )
	{
		int x = 0;
		for ( int i=0; i < PICS::GetBIBBSize(); i++ )
			if ( gPICSdb->BIBBSupported[i] != 0 )
			{
				listCtrl.InsertItem(x, PICS::GetBIBBName(i) );
				x++;
			}
	}
}


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////



CEPICSViewNodeObjTypes::CEPICSViewNodeObjTypes( CEPICSTreeView * ptreeview )
				   :CEPICSViewNode(ptreeview)
{
}


CEPICSViewNodeObjTypes::~CEPICSViewNodeObjTypes()
{
}


void CEPICSViewNodeObjTypes::LoadInfoPanel()
{
	CEPICSViewListPanel * ppanel = (CEPICSViewListPanel *) m_ptreeview->GetInfoPanel();

	if (  ppanel == NULL || !m_ptreeview->GetInfoPanel()->IsKindOf(RUNTIME_CLASS(CEPICSViewListPanel)) )
		ppanel = (CEPICSViewListPanel *) m_ptreeview->CreateInfoView(RUNTIME_CLASS(CEPICSViewListPanel), TRUE);

	ppanel->Reset();
	CListCtrl & listCtrl = ppanel->GetListCtrl();

	listCtrl.InsertColumn( 0, _T("Supported Object Type"), LVCFMT_LEFT, 200 );
	listCtrl.InsertColumn( 1, _T("Createable"), LVCFMT_LEFT, 60 );
	listCtrl.InsertColumn( 2, _T("Deleteable"), LVCFMT_LEFT, 60 );


	if ( gPICSdb )
	{
		int x = 0;
		for ( int i=0; i < PICS::GetObjectTypeSize(); i++ )
			if ( gPICSdb->BACnetStandardObjects[i] != 0 )
			{
				listCtrl.InsertItem(x, PICS::GetObjectTypeName(i) );
				listCtrl.SetItemText(x, 1, (gPICSdb->BACnetStandardObjects[i] & soCreateable) != 0 ? "True" : "False");
				listCtrl.SetItemText(x, 2, (gPICSdb->BACnetStandardObjects[i] & soDeleteable) != 0 ? "True" : "False");
				x++;
			}
	}
}


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////



CEPICSViewNodeDataLink::CEPICSViewNodeDataLink( CEPICSTreeView * ptreeview )
				   :CEPICSViewNode(ptreeview)
{
}


CEPICSViewNodeDataLink::~CEPICSViewNodeDataLink()
{
}


void CEPICSViewNodeDataLink::LoadInfoPanel()
{
	CEPICSViewListPanel * ppanel = (CEPICSViewListPanel *) m_ptreeview->GetInfoPanel();

	if (  ppanel == NULL || !m_ptreeview->GetInfoPanel()->IsKindOf(RUNTIME_CLASS(CEPICSViewListPanel)) )
		ppanel = (CEPICSViewListPanel *) m_ptreeview->CreateInfoView(RUNTIME_CLASS(CEPICSViewListPanel), TRUE);

	ppanel->Reset();
	CListCtrl & listCtrl = ppanel->GetListCtrl();

	listCtrl.InsertColumn( 0, _T("Data Link Layer Option"), LVCFMT_LEFT, 200 );


	if ( gPICSdb )
	{
        char achBuffer[300];
		int x = 0;
		achBuffer[0] = '\0';
        for ( int i=0; i < PICS::GetDataLinkSize() ; i++)
		{
			if ( gPICSdb->DataLinkLayerOptions[i] != 0 )
			{
				PICS::GetDataLinkString(i, gPICSdb, achBuffer);
				listCtrl.InsertItem(x, achBuffer );
				x++;
			}
        }

	}
}


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

IMPLEMENT_DYNCREATE(CEPICSViewNodeObject, CEPICSViewNode)


CEPICSViewNodeObject::CEPICSViewNodeObject( CEPICSTreeView * ptreeview, void far * pObj )
				   :CEPICSViewNode(ptreeview)
{
	m_pObj = pObj;
	m_astrDevValues.SetSize(0);
}


CEPICSViewNodeObject::~CEPICSViewNodeObject()
{
}


unsigned long CEPICSViewNodeObject::GetObjectID()
{
	PICS::generic_object * pObj = (PICS::generic_object *) m_pObj;

	return ( gPICSdb && pObj  ? pObj->object_id : 0);
}


int CEPICSViewNodeObject::GetPropertyIndex( DWORD dwPropID, unsigned short * pParseType )
{
	PICS::generic_object * pObj = (PICS::generic_object *) m_pObj;

	DWORD dwPropIDSearch = 0;
	char szPropName[100];
	int x = 0;

	for (int i = 0; i < sizeof(pObj->propflags); i++ )
		if ( PICS::GetPropNameSupported(szPropName, i, pObj->object_type, pObj->propflags, &dwPropIDSearch, pParseType) > 0 )
		{
			if ( dwPropIDSearch == dwPropID )
				return x;
			else
				x++;
		}

	return -1;
}



// Local index here is the nth SUPPORTED property... not just a blind index into the array
// of propflags

unsigned long CEPICSViewNodeObject::GetPropertyID( int nLocalIndex )
{
	PICS::generic_object * pObj = (PICS::generic_object *) m_pObj;

	DWORD dwPropID = 0;
	char szPropName[100];
	int x = 0;

	for (int i = 0; i < sizeof(pObj->propflags); i++ )
		if ( PICS::GetPropNameSupported(szPropName, i, pObj->object_type, pObj->propflags, &dwPropID, NULL) > 0 )
			if ( x++ == nLocalIndex )
				return (unsigned long) dwPropID;

	return 0;
}



// Local index here is the nth SUPPORTED property... not just a blind index into the array
// of propflags

unsigned long CEPICSViewNodeObject::GetPropertyIDAndValue( int nLocalIndex, BACnetAnyValue * pany )
{
	PICS::generic_object * pObj = (PICS::generic_object *) m_pObj;

	DWORD dwPropID = 0;
	char szPropName[100];
	int x = 0;

	for (int i = 0; i < sizeof(pObj->propflags); i++ )
		if ( PICS::GetPropNameSupported(szPropName, i, pObj->object_type, pObj->propflags, &dwPropID, NULL) > 0 )
			if ( x++ == nLocalIndex )
			{
				try
				{
					PICS::CreatePropertyFromEPICS( pObj, (PICS::BACnetPropertyIdentifier) dwPropID, pany );
				}
				catch(...)
				{
					pany->SetObject(NULL);
				}
				return (unsigned long) dwPropID;
			}

	return 0;
}




void CEPICSViewNodeObject::LoadInfoPanel()
{
	CEPICSViewPropPanel * ppanel = (CEPICSViewPropPanel *) m_ptreeview->GetInfoPanel();

	if (  ppanel == NULL || !m_ptreeview->GetInfoPanel()->IsKindOf(RUNTIME_CLASS(CEPICSViewPropPanel)) )
		ppanel = (CEPICSViewPropPanel *) m_ptreeview->CreateInfoView(RUNTIME_CLASS(CEPICSViewPropPanel), TRUE);

	char szPropHeader[120];
	PICS::generic_object * pObj = (PICS::generic_object *) m_pObj;

	strcpy(szPropHeader, "Property Name");

	if ( gPICSdb && pObj )
	{
		strcat(szPropHeader, " - ");

		// 5-23-2005 Shiyuan Xiao
		if(pObj->object_type < 128)
			strcat(szPropHeader, PICS::GetObjectTypeName(pObj->object_type));
		else
			strcat(szPropHeader, "Proprietary Object");
	}

	ppanel->Reset(this, szPropHeader);
	CListCtrl & listCtrl = ppanel->GetListCtrl();


	if ( gPICSdb )
	{
		PICS::generic_object * pObj = (PICS::generic_object *) m_pObj;
		int x = 0;

		DWORD dwPropID;
		char szPropName[100];
		BACnetAnyValue bacnetAnyValue;

		for (int i = 0; i < sizeof(pObj->propflags); i++ )
		{
			if ( PICS::GetPropNameSupported(szPropName, i, pObj->object_type, pObj->propflags, &dwPropID, NULL) > 0 )
			{
				listCtrl.InsertItem(x, NetworkSniffer::BAC_STRTAB_BACnetPropertyIdentifier.EnumString(dwPropID) );

				if ( (pObj->propflags[i] & PropIsWritable) != 0 )
					listCtrl.SetItemText(x, 1, "True");

				if ( (pObj->propflags[i] & ValueUnknown) != 0 )
					listCtrl.SetItemText(x, 2, "?");
				else
				{
					try
					{
						PICS::CreatePropertyFromEPICS( pObj, (PICS::BACnetPropertyIdentifier) dwPropID, &bacnetAnyValue );
					}
					catch(...)
					{
						bacnetAnyValue.SetObject(NULL);
					}
					listCtrl.SetItemText(x, 2, bacnetAnyValue.ToString());
				}

				x++;
			}
		}

		// Now that all the properties are loaded into the list we can add our stored device values
		// if we have some... Size of array must match our property list... so we can count on the list size

		for (int j = 0; j < m_astrDevValues.GetSize(); j++ )
			listCtrl.SetItemText(j, 3, m_astrDevValues[j]);
	}
}


// Call this guy to set a new device value and turn the text red...
// If the text equals the value, turn it green, otherwise, turn it red

void CEPICSViewNodeObject::SetDeviceValue( int iIndex, LPCSTR lpszValue )
{
	CEPICSViewPropPanel * ppanel = (CEPICSViewPropPanel *) m_ptreeview->GetInfoPanel();

	m_astrDevValues.SetAtGrow(iIndex, lpszValue);

	if ( ppanel->m_pObjectNode == this )
		ppanel->GetListCtrl().SetItemText(iIndex, 3, lpszValue);
}


void CEPICSViewNodeObject::ClearDeviceValues()
{
	CEPICSViewPropPanel * ppanel = (CEPICSViewPropPanel *) m_ptreeview->GetInfoPanel();

	if ( ppanel->m_pObjectNode == this )
	{
		for ( int i = 0; i < ppanel->GetListCtrl().GetItemCount(); i++ )
			ppanel->GetListCtrl().SetItemText(i, 3, "");
	}

	m_astrDevValues.RemoveAll();
	m_astrDevValues.SetSize(0);
}


BOOL CEPICSViewNodeObject::HasDeviceValues()
{
	return m_astrDevValues.GetSize() != 0;
}


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////



CEPICSViewNodeCharsets::CEPICSViewNodeCharsets( CEPICSTreeView * ptreeview )
				   :CEPICSViewNode(ptreeview)
{
}


CEPICSViewNodeCharsets::~CEPICSViewNodeCharsets()
{
}


void CEPICSViewNodeCharsets::LoadInfoPanel()
{
	CEPICSViewListPanel * ppanel = (CEPICSViewListPanel *) m_ptreeview->GetInfoPanel();

	if (  ppanel == NULL || !m_ptreeview->GetInfoPanel()->IsKindOf(RUNTIME_CLASS(CEPICSViewListPanel)) )
		ppanel = (CEPICSViewListPanel *) m_ptreeview->CreateInfoView(RUNTIME_CLASS(CEPICSViewListPanel), TRUE);

	ppanel->Reset();
	CListCtrl & listCtrl = ppanel->GetListCtrl();

	listCtrl.InsertColumn( 0, _T("Character Sets Supported"), LVCFMT_LEFT, 200 );


	if ( gPICSdb )
	{
		int x = 0;
        unsigned char csTag;
        for ( int i=0; i < PICS::GetCharsetSize() ; i++)
		{
			csTag = 1 << i;
			if ( gPICSdb->BACnetCharsets & csTag )
			{
				listCtrl.InsertItem(x, PICS::GetCharsetName(csTag) );
				x++;
			}
        }

	}
}


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////



CEPICSViewNodeSpecialFunctionality::CEPICSViewNodeSpecialFunctionality( CEPICSTreeView * ptreeview )
				   :CEPICSViewNode(ptreeview)
{
}


CEPICSViewNodeSpecialFunctionality::~CEPICSViewNodeSpecialFunctionality()
{
}


void CEPICSViewNodeSpecialFunctionality::LoadInfoPanel()
{
	CEPICSViewListPanel * ppanel = (CEPICSViewListPanel *) m_ptreeview->GetInfoPanel();

	if (  ppanel == NULL || !m_ptreeview->GetInfoPanel()->IsKindOf(RUNTIME_CLASS(CEPICSViewListPanel)) )
		ppanel = (CEPICSViewListPanel *) m_ptreeview->CreateInfoView(RUNTIME_CLASS(CEPICSViewListPanel), TRUE);

	ppanel->Reset();
	CListCtrl & listCtrl = ppanel->GetListCtrl();

	listCtrl.InsertColumn( 0, _T("Special Functionality"), LVCFMT_LEFT, 200 );
	listCtrl.InsertColumn( 1, _T("Value"), LVCFMT_LEFT, 100 );


	if ( gPICSdb )
	{
		int x = 0;
        CString str;

		// Max APDU
		listCtrl.InsertItem(x, PICS::GetSpecialFunctionalityName(0) ); 
		str.Format("%d",(gPICSdb->MaxAPDUSize));
	    listCtrl.SetItemText(x, 1, str );
		x++;

		 // Segmented requests
		if (gPICSdb->SegmentedRequestWindow)
		{
			listCtrl.InsertItem(x, PICS::GetSpecialFunctionalityName(1) ); 
			str.Format("%d",(gPICSdb->SegmentedRequestWindow));
 		    listCtrl.SetItemText(x, 1, str );
			x++;
		}

		 // Segmented responses
		if (gPICSdb->SegmentedResponseWindow)
		{
			listCtrl.InsertItem(x, PICS::GetSpecialFunctionalityName(2) ); 
			str.Format("%d",(gPICSdb->SegmentedResponseWindow));
 		    listCtrl.SetItemText(x, 1, str );
			x++;
		}

        // Router functionality
		if (gPICSdb->RouterFunctions)
		{
			listCtrl.InsertItem(x, PICS::GetSpecialFunctionalityName(3) ); 
			x++;
		}

        // BBMD functionality
		if (gPICSdb->BBMD)
		{
			listCtrl.InsertItem(x, PICS::GetSpecialFunctionalityName(4) ); 
			x++;
		}

	}
}
