// VTSNotifyPage.h: interface for the VTSNotifyPage class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_VTSNOTIFYPAGE_H__C16F918A_7D62_11D4_BEDA_00A0C95A9812__INCLUDED_)
#define AFX_VTSNOTIFYPAGE_H__C16F918A_7D62_11D4_BEDA_00A0C95A9812__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "BACnet.hpp"

class VTSNotificationParameters;

class VTSNotifyPage {
	public:
		VTSNotifyPage();
		virtual ~VTSNotifyPage();

		VTSNotificationParameters	*pageParent;
		virtual void Encode( BACnetAPDUEncoder& enc, int context = kAppContext ) = 0;	// encode
	};

typedef VTSNotifyPage *VTSNotifyPagePtr;

#endif // !defined(AFX_VTSNOTIFYPAGE_H__C16F918A_7D62_11D4_BEDA_00A0C95A9812__INCLUDED_)
