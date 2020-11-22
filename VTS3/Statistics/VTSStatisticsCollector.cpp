// VTSStatisticsCollector.cpp: implementation of the VTSStatisticsCollector class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "vts.h"
#include "VTSStatisticsCollector.h"

#include "ServiceList.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

VTSStatisticsCollector::VTSStatisticsCollector()
{
	int i;
	for(i=0;i<TOTAL_MSG_NUMBER;i++)
	{
		m_mapStringToCounter.SetAt(BACnet_msgs[i],new CPacketCounter());
	}
	m_nDistributionArray[0]=0;
	m_nDistributionArray[1]=10;
	m_nDistributionArray[2]=11;
	m_nDistributionArray[3]=20;
	m_nDistributionArray[4]=21;
	m_nDistributionArray[5]=0;
	for(i=0;i<12;i++)
	{
		if(i<10)
			m_bColumnStatus[i]=FALSE;
		else
			m_bColumnStatus[i]=TRUE;
	}
}

VTSStatisticsCollector::~VTSStatisticsCollector()
{
	POSITION pos;
	CPacketCounter* pa;
	CString key;
	WORD key1;
	for( pos = m_mapStringToCounter.GetStartPosition(); pos != NULL; )
	{
		m_mapStringToCounter.GetNextAssoc( pos, key, (CPacketCounter*&)pa );
		delete pa;
	}
	for( pos = m_mapSizeToCounter.GetStartPosition(); pos != NULL; )
	{
		m_mapSizeToCounter.GetNextAssoc( pos, key1, (CPacketCounter*&)pa );
		delete pa;
	}
}

/*****************************************************************************
called to update the statistics when one packet is coming

******************************************************************************/
bool VTSStatisticsCollector::Update(char* summary,int length,int dir,int protocol)
{	

    if (dir==2)
		return false;
	//if it is a vts generated msg.

	char pServiceName[100];
	GetServiceName(summary,pServiceName);

	CString ServiceName(pServiceName);
	CPacketCounter* counter=NULL;
	CPacketCounter* sizeCounter=NULL;
	
	m_mapStringToCounter.Lookup(ServiceName,(CPacketCounter*&)counter);

	if (counter==NULL)
		return false;
	//can not find this message in map.
	//maybe a vts created message
	
	//add to size map
	m_mapSizeToCounter.Lookup(length,(CPacketCounter*&)sizeCounter);
	
	if (sizeCounter==NULL)
	{
		sizeCounter=new CPacketCounter();
		m_mapSizeToCounter.SetAt(length,sizeCounter);
	}
	

	switch(protocol)
	{
		case ipProtocol:
			if(dir)
			{
				counter->IPReceive++;
				sizeCounter->IPReceive++;
			}
			else
			{	
				counter->IPSend++;
				sizeCounter->IPSend++;
			}
			break;
		case ethernetProtocol:
			if(dir)
			{	
				counter->EthernetReceive++;
				sizeCounter->EthernetReceive++;
			}
			else
			{	
				counter->EthernetSend++;
				sizeCounter->EthernetSend++;
			}
			break;
		case arcnetProtocol:
			if(dir)
			{	
				counter->ArcnetReceive++;
				sizeCounter->ArcnetReceive++;
			}
			else
			{	
				counter->ArcnetSend++;
				sizeCounter->ArcnetSend++;
			}
			break;
		case mstpProtocol:
			if(dir)
			{	
				counter->MSTPReceive++;
				sizeCounter->MSTPReceive++;
			}
			else
			{	
				counter->MSTPSend++;
				sizeCounter->MSTPSend++;
			}
			break;
		case ptpProtocol:
			if(dir)
			{
				counter->PTPReceive++;
				sizeCounter->PTPReceive++;
			}
			else
			{
				counter->PTPSend++;
				sizeCounter->PTPSend++;
			}
			break;
	}

	return true;
}

/*************************************************************************
get the service name from the summary line

*************************************************************************/
void VTSStatisticsCollector::GetServiceName(char *summary, char *service)
{
	char* temp1=summary;
	char* temp2=service;
	while(*(temp1)!=0&&*(temp1)!=',')
	{
		*temp2++=*temp1++;
	}
	*temp2=0;
}


int VTSStatisticsCollector::GetTotalReceive(CPacketCounter::ProtocolType protocol)
{
	POSITION pos;
	CPacketCounter* pa;
	CString key;
	int sum=0;
	for( pos = m_mapStringToCounter.GetStartPosition(); pos != NULL; )
	{
		m_mapStringToCounter.GetNextAssoc( pos, key, (CPacketCounter*&)pa );
		sum+=pa->GetPacketCount(protocol,CPacketCounter::receivePacket);
   }

	return sum;
}

int VTSStatisticsCollector::GetTotalSend(CPacketCounter::ProtocolType protocol)
{
	POSITION pos;
	CPacketCounter* pa;
	CString key;
	int sum=0;
	for( pos = m_mapStringToCounter.GetStartPosition(); pos != NULL; )
	{
		m_mapStringToCounter.GetNextAssoc( pos, key, (CPacketCounter*&)pa );
		sum+=pa->GetPacketCount(protocol,CPacketCounter::sendPacket);
   }
	return sum;

}

int VTSStatisticsCollector::GetPacketCounter(CString service, CPacketCounter::ProtocolType protocol,CPacketCounter::directionType dir)
{
	CPacketCounter* pa;
	m_mapStringToCounter.Lookup(service,(CPacketCounter *&)pa);
	return pa->GetPacketCount(protocol,dir);
}

int VTSStatisticsCollector::GetSizeCounter(int min, int max,CPacketCounter::ProtocolType protocol,CPacketCounter::directionType dir)
{
	CPacketCounter* sizeCounter=NULL;
	WORD key=0;
	int sum=0;

	if(max!=0)
	{
		for(int i=min;i<=max;i++)
		{
			m_mapSizeToCounter.Lookup(i,(CPacketCounter*&)sizeCounter);
			if(sizeCounter!=NULL)
				sum+=sizeCounter->GetPacketCount(protocol,dir);
			sizeCounter=NULL;
		}
	}
	else
	{
		POSITION pos=m_mapSizeToCounter.GetStartPosition( );
		while(pos)
		{
			m_mapSizeToCounter.GetNextAssoc(pos,key,(CPacketCounter*&)sizeCounter);
			if(sizeCounter!=NULL&&(int)key>=min)
				sum+=sizeCounter->GetPacketCount(protocol,dir);
			sizeCounter=NULL;
		}
	}
	return sum;
}

/********************************
CPacketCounter class
********************************/

CPacketCounter::CPacketCounter()
{
	IPSend=0;
	IPReceive=0;
	EthernetSend=0;
	EthernetReceive=0;
	ArcnetSend=0;
	ArcnetReceive=0;
	MSTPSend=0;
	MSTPReceive=0;
	PTPSend=0;
	PTPReceive=0;
}

CPacketCounter::~CPacketCounter()
{
}

int CPacketCounter::GetPacketCount(ProtocolType protocol,directionType dir)
{
	int counter;
	switch(protocol){
		case ipProtocol:
			if (dir==sendPacket)
				counter= IPSend;
			if (dir==receivePacket)
				counter= IPReceive;
			if (dir==allDirection)
				counter= IPSend+IPReceive;
			break;
		case ethernetProtocol:
			if (dir==sendPacket)
				counter= EthernetSend;
			if (dir==receivePacket)
				counter= EthernetReceive;
			if (dir==allDirection)
				counter= EthernetSend+EthernetReceive;
			break;
		case arcnetProtocol:
			if (dir==sendPacket)
				counter= ArcnetSend;
			if (dir==receivePacket)
				counter= ArcnetReceive;
			if (dir==allDirection)
				counter= ArcnetSend+ArcnetReceive;
			break;
		case mstpProtocol:
			if (dir==sendPacket)
				counter= MSTPSend;
			if (dir==receivePacket)
				counter= MSTPReceive;
			if (dir==allDirection)
				counter= MSTPSend+MSTPReceive;
			break;
		case ptpProtocol:
			if (dir==sendPacket)
				counter= PTPSend;
			if (dir==receivePacket)
				counter= PTPReceive;
			if (dir==allDirection)
				counter= PTPSend+PTPReceive;
			break;
		case allProtocol:
			if (dir==sendPacket)
				counter= IPSend+EthernetSend+ArcnetSend+MSTPSend+PTPSend;
			if (dir==receivePacket)
				counter= IPReceive+EthernetReceive+ArcnetReceive+MSTPReceive+PTPReceive;
			if (dir==allDirection)
				counter= IPSend+IPReceive+EthernetSend+
						 EthernetReceive+ArcnetSend+ArcnetReceive+
						 MSTPSend+MSTPReceive+PTPSend+PTPReceive;
			break;
		}
	return counter;
}
