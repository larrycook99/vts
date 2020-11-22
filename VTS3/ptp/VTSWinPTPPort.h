// VTSWinPTPPort.h: interface for the VTSWinPTPPort class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_VTSWINPTPPORT_H__3B582353_75E9_47D6_BFE3_BC3241FEFF5D__INCLUDED_)
#define AFX_VTSWINPTPPORT_H__3B582353_75E9_47D6_BFE3_BC3241FEFF5D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "WinPTP.h"
#include "VTSDoc.h"

// supported by Xiao Shiyuan 2002-4-22

class VTSWinPTPPort;
typedef VTSWinPTPPort *VTSWinPTPPortPtr;

class VTSWinPTPPort : public WinPTP  
{
protected:
		VTSPortPtr			m_pPort;

public:
	VTSWinPTPPort( VTSPortPtr pp );
	virtual ~VTSWinPTPPort( void );
	void FilterData( BACnetOctet *, int len, BACnetPortDirection dir );
	void PortStatusChange( void );
};

#endif // !defined(AFX_VTSWINPTPPORT_H__3B582353_75E9_47D6_BFE3_BC3241FEFF5D__INCLUDED_)
