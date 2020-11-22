//SERVICE.H
//BACnet Standard Object Structures
#ifndef __SERVICE_H_INCLUDED
#define __SERVICE_H_INCLUDED 

enum BACnetPDUTypes
	{	CONF_REQ_PDU,						//0
		UNCONF_REQ_PDU,						//1
		SIMPLE_ACK_PDU,						//2
		COMPLEX_ACK_PDU,					//3
		SEGMENT_ACK_PDU,					//4
		ERROR_PDU,							//5
		REJECT_PDU,							//6
		ABORT_PDU							//7
	};

//Confirmed Services
//-------------------------------------------------------
enum BACnetConfirmedService				//also BACnetConfirmedServiceACK
	{
	//Alarm and Event Services
		acknowledgeAlarm,					//0
		confirmedCOVNotification,			//1
		confirmedEventNotification,			//2
		getAlarmSummary,					//3
		getEnrollmentSummary,				//4
		subscribeCOV,						//5
	//File Access Services
		atomicFileRead,						//6
		atomicFileWrite,					//7
	//Object Access Services
		addListElement,						//8
		removeListElement,					//9
		createObject,						//10
		deleteObject,						//11
		readProperty,						//12
		readPropertyConditional,			//13
		readPropertyMultiple,				//14
		writeProperty,						//15
		writePropertyMultiple,				//16
	//Remote Device Management Services
		deviceCommunicationControl,			//17
		confirmedPrivateTransfer,			//18
		confirmedTextMessage,				//19
		reinitializeDevice,					//20
	//Virtual Terminal Services
		vtOpen,								//21
		vtClose,							//22
		vtData,								//23
	//Security Services
		authenticate,						//24
		requestKey							//25
	};

enum BACnetUnconfirmedService
	{
		I_Am,								//0
		I_Have,								//1
		unconfirmedCOVNotification,			//2
		unconfirmedEventNotification,		//3
		unconfirmedPrivateTransfer,			//4
		unconfirmedTextMessage,				//5
		timeSynchronization,				//6
		who_Has,							//7
		who_Is								//8		
	};
#endif //__SERVICE_H_INCLUDED
	