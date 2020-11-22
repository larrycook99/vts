// VTSPacketDB.h: interface for the VTSPacketDB class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_VTSPACKETDB_H__BA0A2217_5F60_494F_A4BC_0B5BE5432A8D__INCLUDED_)
#define AFX_VTSPACKETDB_H__BA0A2217_5F60_494F_A4BC_0B5BE5432A8D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

//#include <Afxmt.h>

#include "BACnet.hpp"
#include "VTSPacket.h"


class VTSPacketDB : public CObject
{
	public:
		VTSPacketDB(void);
		virtual ~VTSPacketDB( void );

		virtual void Close( void );				// clean up operations

		bool Open( LPCTSTR lpszFileName );
		void DeletePackets( void );						// clear out the packets

		ULONGLONG WritePacket(VTSPacket& pkt);				// write a packet
		ULONGLONG ReadNextPacket(VTSPacket& pkt, ULONGLONG lPosition );		// MAD_DB

		CCriticalSection	writeLock;			// locked when transaction in progress

		CString			m_strPacketFileName;
		CFile *			m_pfilePackets;			// pointer to packet file dump
};

typedef VTSPacketDB * VTSPacketDBPtr;


#endif // !defined(AFX_VTSPACKETDB_H__BA0A2217_5F60_494F_A4BC_0B5BE5432A8D__INCLUDED_)
