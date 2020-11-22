// VTSWinMSTPPort.h: interface for the VTSWinMSTPPort class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_VTSWINMSTPPORT_H__922C8251_208F_46C1_BC38_BDF36EA038B1__INCLUDED_)
#define AFX_VTSWINMSTPPORT_H__922C8251_208F_46C1_BC38_BDF36EA038B1__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "VTSDoc.h"
#include "WinMSTP.h"


class VTSWinMSTPPort : public WinMSTP
{
protected:
	VTSPortPtr			m_pPort;


public:
	VTSWinMSTPPort( VTSPortPtr pp );
	virtual ~VTSWinMSTPPort();

	virtual void SetPortStatus( int nStatus, char * pszStatusMsg );

	void FilterData( BACnetOctet *, int len, BACnetPortDirection dir );
	void PortStatusChange( void );

	virtual void Msg( const char * msg );
};


#endif // !defined(AFX_VTSWINMSTPPORT_H__922C8251_208F_46C1_BC38_BDF36EA038B1__INCLUDED_)
