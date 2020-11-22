//ERROR.H
//Errors for BACnet
#ifndef __ERR_H_INCLUDED
#define __ERR_H_INCLUDED

/* ----- Error Productions ----- */
/* ----- Reject Reasons ----- */
enum BACnetRejectReason
	{	REJECT_Other,							//0
		REJECT_BufferOverflow,					//1
		REJECT_InconsistentParameters,			//2
		REJECT_InvalidParameterDataType,		//3
		REJECT_InvalidTag,						//4
		REJECT_MissingRequiredParameter,		//5
		REJECT_ParameterOutOfRange,				//6
		REJECT_TooManyArguments,				//7
		REJECT_UndefinedEnumeration,			//8
		REJECT_UnrecognizedService				//9
	};
	
/* ----- Error Classes ----- */
enum error_class
	{	ECLASS_Device,							//0
		ECLASS_Object,							//1
		ECLASS_Property,						//2
		ECLASS_Resources,						//3
		ECLASS_Security,						//4
		ECLASS_Services,						//5
		ECLASS_VT								//6
	};

/* ----- Error Codes ------- */
enum error_code
	{	ECODE_Other,							//0
		ECODE_AuthenticationFailed,				//1
		ECODE_ConfigurationInProgress,			//2
		ECODE_DeviceBusy,						//3
		ECODE_DynamicCreationNotSupported,		//4
		ECODE_FileAccessDenied,					//5
		ECODE_IncompatibilitySecurityLevels,	//6
		ECODE_InconsistentParameters,			//7
		ECODE_InconsistentSelectionCriterion,	//8
		ECODE_InvalidDataType,					//9
		ECODE_InvalidAccessMethod,				//10
		ECODE_InvalidFileSstartPosition,		//11
		ECODE_InvalidOperatorName,				//12
		ECODE_InvalidParameterDataType,			//13
		ECODE_InvalidTimeStamp,					//14
		ECODE_KeyGenerationError,				//15
		ECODE_MissingRequiredParameter,			//16
		ECODE_NoObjectsOfSpecifiedType,			//17
		ECODE_NoSpaceForObject,					//18
		ECODE_NoSpaceToAddListElement,			//19
		ECODE_NoSpaceToWriteProperty,			//20
		ECODE_NoVTSessionsAvailable,			//21
		ECODE_PropertyIsNotAList,				//22
		ECODE_ObjectDefinitionNotPermitted,		//23
		ECODE_ObjectIdentifierAlreadyExists,	//24
		ECODE_OperationalProblem,				//25
		ECODE_PasswordFailure,					//26
		ECODE_ReadAccessDenied,					//27
		ECODE_SecurityNotSupported,				//28
		ECODE_ServiceRequestDenied,				//29
		ECODE_Timeout,							//30
		ECODE_UnknownObject,					//31
		ECODE_UnknownProperty,					//32
		ECODE_ecode33,							//33
		ECODE_UnknownVTClass,					//34
		ECODE_UnknownVTSession,					//35
		ECODE_UnsupportedObjectType,			//36
		ECODE_ValueOutOfRange,					//37
		ECODE_VTSessionAlreadyClosed,			//38
		ECODE_VTSessionTerminationFailure,		//39
		ECODE_WriteAccessDenied,				//40
		ECODE_CharacterSetNotConfigured,		//41
		ECODE_InvalidArrayIndex					//42
	};
	
#endif //__ERR_H_INCLUDED
