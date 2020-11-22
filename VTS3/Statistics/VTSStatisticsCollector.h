// VTSStatisticsCollector.h: interface for the VTSStatisticsCollector class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_VTSSTATISTICSCOLLECTOR_H__70834B1D_3AFA_4782_9F1E_7FB1E8615764__INCLUDED_)
#define AFX_VTSSTATISTICSCOLLECTOR_H__70834B1D_3AFA_4782_9F1E_7FB1E8615764__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CPacketCounter;

typedef CTypedPtrMap<CMapStringToOb,CString,CPacketCounter*> CMapStringToPacketCounter;
typedef CTypedPtrMap<CMapWordToOb,WORD,CPacketCounter*> CMapWordToPacketCounter;

class CMapSizeToPacketCounter:public CMapWordToPacketCounter
{
public:
	//GetTota

};

class CPacketCounter: public CObject
{
public:
	CPacketCounter();

// Attributes
public:
	int IPSend;
	int IPReceive;
	int EthernetSend;
	int EthernetReceive;
	int ArcnetSend;
	int ArcnetReceive;
	int MSTPSend;
	int MSTPReceive;
	int PTPSend;
	int PTPReceive;

	enum ProtocolType
		{ ipProtocol = 0
		, ethernetProtocol
		, arcnetProtocol
		, mstpProtocol
		, ptpProtocol
		, allProtocol
		};
	enum directionType
		{ receivePacket =0
		, sendPacket
		, allDirection
		};

//	CMapWordToPacketCounter m_mapSizeToCounter;	
	
// Operations
public:
	int GetPacketCount(ProtocolType protocol,directionType dir);
// Implementation
public:
	virtual ~CPacketCounter();
};




class VTSStatisticsCollector  
{
public:
	int GetSizeCounter(int min,int max,CPacketCounter::ProtocolType protocol,CPacketCounter::directionType dir);
	int GetPacketCounter(CString service, CPacketCounter::ProtocolType protocol,CPacketCounter::directionType dir);
	int GetTotalSend(CPacketCounter::ProtocolType protocol);
	int GetTotalReceive(CPacketCounter::ProtocolType protocol);
	bool Update(char* summary,int length,int dir,int protocol);
	VTSStatisticsCollector();
	virtual ~VTSStatisticsCollector();
	
	CMapStringToPacketCounter m_mapStringToCounter;
	CMapWordToPacketCounter m_mapSizeToCounter;
	int m_nDistributionArray[6];//define the size distribution scale
	BOOL m_bColumnStatus[12];//define which column are watched
	CList<CString,CString&> m_WatchGroupList;
	CList<CString,CString&> m_WatchServiceList;

private:
		void GetServiceName(char *summary,char* service);
	enum ProtocolType
		{ ipProtocol = 0
		, ethernetProtocol
		, arcnetProtocol
		, mstpProtocol
		, ptpProtocol
		};
	
};

#endif // !defined(AFX_VTSSTATISTICSCOLLECTOR_H__70834B1D_3AFA_4782_9F1E_7FB1E8615764__INCLUDED_)
