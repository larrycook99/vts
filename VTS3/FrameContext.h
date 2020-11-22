// FrameContext.h: interface for the CFrameContext class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_FRAMECONTEXT_H__369650F1_C25B_11D3_BE59_00A0C95A9812__INCLUDED_)
#define AFX_FRAMECONTEXT_H__369650F1_C25B_11D3_BE59_00A0C95A9812__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "VTSDoc.h"

#include "PI.h"

// forward declaration
class CFrameContextListener;
typedef CFrameContextListener *CFrameContextListenerPtr;

//
//	CFrameContext
//

class CFrameContext {
	public:
		CFrameContext();
		virtual ~CFrameContext();
		
		VTSDoc*		m_pDoc;				// associated document
		CFrameContext	*m_NextFrame;		// next frame context in document list

		int				m_PacketCount;		// tracks pDoc->m_PacketCount
		int				m_CurrentPacket;	// 0..m_PacketCount, or -1

//MAD_DB	VTSPacket		m_Packet;			// packet contents
		BACnetPIInfo	m_PacketInfo;		// decoded packet details

		int				m_DetailCount;		// number of detail lines in current packet
		int				m_CurrentDetail;	// 0..frame detail line count, or -1

		int				m_HexStart;			// 0..(m_PacketLen-1)
		int				m_HexLen;			// 0..(m_PacketLen-m_HexStart)

		enum Signal
			{ eNewPacketCount = 1
			, eNewCurrentPacket = 2
			, eNewDetailCount = 3
			, eNewCurrentDetail = 4
			, eNewCurrentRange = 5
			, eUpdatePrefs = 6
			};
		
		void SetPacketCount( int packetCnt );
		void SetCurrentPacket( int packetNum );
		void SetDetailCount( int detailCnt );
		void SetCurrentDetail( int detailNum );
		void SetCurrentRange( int start, int len );
		void UpdatePrefs();

		// MAD_DB
		VTSPacketPtr GetCurrentPacket(void);

		void AddListener( CFrameContextListener* pListener );
		void RemoveListener( CFrameContextListener * pListener );

	protected:
		CFrameContextListener*	m_Listeners;

		void NotifyListeners( Signal s );
	};

typedef CFrameContext *CFrameContextPtr;

//
//	CFrameContextListener
//

class CFrameContextListener {
		friend class CFrameContext;

	private:
		CFrameContextListenerPtr	m_NextListener;

	public:
		CFrameContextListener( void );

		CFrameContextPtr			m_FrameContext;

		void SetContext( CFrameContextPtr pContext );
		void RemoveContext();
		virtual void ContextChange( CFrameContext::Signal s ) = 0;
	};

extern CFrameContextPtr	gNewFrameContext;

#endif // !defined(AFX_FRAMECONTEXT_H__369650F1_C25B_11D3_BE59_00A0C95A9812__INCLUDED_)
