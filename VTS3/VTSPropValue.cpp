// VTSPropValue.cpp:
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "vts.h"
#include "VTSCtrl.h"
#include "VTSPropValue.h"

#include <memory.h>

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//=================================================================================

IMPLEMENT_VTSPTRARRAY(VTSDevObjects, VTSDevObject);

IMPLEMENT_VTSPTRARRAY(VTSDevProperties, VTSDevProperty);

IMPLEMENT_VTSPTRARRAY(VTSDevValues, VTSDevValue);

//=================================================================================

/////////////////////////////////////////////////////////////////////////////

IMPLEMENT_SERIAL(VTSDevValue, CObject, 1);

VTSDevValue::VTSDevValue()
{
	m_nType = PRIM_NULL;
	m_nContext = -1;
	m_nLength = kVTSValueElemContentLength;
	memset(m_abContent, 0, m_nLength);
	m_pdevvalues = NULL;
}


VTSDevValue::~VTSDevValue()
{
	KillValueList();
}


void VTSDevValue::Serialize(CArchive& ar, UINT version)
{
	if (ar.IsStoring())
	{
		// Decode for version 1.  If changes are made, switch on schema and decode properly
		ar << m_nType;
		ar << m_nContext;
		ar << m_nLength;
		ar.Write(m_abContent, m_nLength);
		if ( m_pdevvalues == NULL )
			ar << 0;
		else
		{
			ar << 1;
			m_pdevvalues->Serialize(ar, version);
		}
	}
	else
	{
		ar >> m_nType;
		ar >> m_nContext;
		ar >> m_nLength;
		ar.Read(m_abContent, m_nLength);

		int nValueArray;
		ar >> nValueArray;
		if ( nValueArray  &&  AllocateNewValueList() )
			m_pdevvalues->Serialize(ar, version);
	}
}


void VTSDevValue::KillValueList( void )
{
	if ( m_pdevvalues != NULL )
		delete m_pdevvalues;
	m_pdevvalues = NULL;
}


VTSDevValues * VTSDevValue::AllocateNewValueList( void )
{
	KillValueList();
	return (m_pdevvalues = new VTSDevValues());
}


const VTSDevValue& VTSDevValue::operator=(const VTSDevValue& rdevvalueSrc)
{
	m_nType = rdevvalueSrc.m_nType;
	m_nContext = rdevvalueSrc.m_nContext;
	m_nLength = rdevvalueSrc.m_nLength;
	memcpy(m_abContent, rdevvalueSrc.m_abContent, rdevvalueSrc.m_nLength);

	// deep copy value component list if present... kill ours if not...
	KillValueList();

	if ( rdevvalueSrc.m_pdevvalues != NULL  &&  AllocateNewValueList() )
		m_pdevvalues->DeepCopy(rdevvalueSrc.m_pdevvalues);

	return *this;
}

// Short type names to be shown on value lines
static LPCTSTR s_typeNames[] =
{
	_T(""),			// "Null" is shown by Encode()
	_T("Boolean"),
	_T("Unsigned"),
	_T("Signed"),
	_T("Real"),
	_T("Double"),
	_T("Octet string"),
	_T("Character string"),
	_T("Bit string"),
	_T("Enumerated"),
	_T("Date"),
	_T("Time"),
	_T("Object Identifier"),
	_T(""),		// Opening Tag
	_T("")		// Closing Tag
};

CString VTSDevValue::GetDescription( void )
{
	CString str = _T("<Undefined>");
	LPCTSTR type = _T("");

	BACnetEncodeable * pbacnet = CreateBACnetObject();
	if (pbacnet != NULL)
	{
		type = s_typeNames[m_nType];
		try {
			switch(m_nType)
			{
			case 13: str = _T("Opening Tag");	break;
			case 14: str = _T("Closing Tag");	break;

			default:	
				pbacnet->Encode(str, BACnetEncodeable::FMT_SCRIPT);
				break;
			}
		}
		catch(...) {
			str = _T("<Error during conversion to text>");
		}

		delete pbacnet;
	}

	CString retval;
	if (m_nContext >= 0)
	{
		retval.Format( "[%d] %s  %s", m_nContext, (LPCTSTR)str, type );
	}
	else
	{
		retval.Format( "%s  %s", (LPCTSTR)str, type );
	}
	return retval;
}


BACnetEncodeable * VTSDevValue::CreateBACnetObject( void )
{
	BACnetAPDUDecoder	dec( m_abContent, m_nLength );
	BACnetEncodeable * pbacnet = NULL;

	// depending on the type, decode the value
	try {
	switch ( m_nType )
	{
		case PRIM_NULL:			pbacnet = new BACnetNull(dec);	break;
		case PRIM_BOOLEAN:		pbacnet = new BACnetBoolean(dec);	break;
		case PRIM_UNSIGNED:		pbacnet = new BACnetUnsigned(dec);	break;
		case PRIM_SIGNED:		pbacnet = new BACnetInteger(dec);	break;
		case PRIM_REAL:			pbacnet = new BACnetReal(dec);	break;
		case PRIM_DOUBLE:		pbacnet = new BACnetDouble(dec);	break;
		case PRIM_OCTET_STRING:	pbacnet = new BACnetOctetString(dec);	break;
		case PRIM_CHARACTER_STRING:	pbacnet = new BACnetCharacterString(dec);	break;
		case PRIM_BIT_STRING:	pbacnet = new BACnetBitString(dec);	break;
		case PRIM_ENUMERATED:	pbacnet = new BACnetEnumerated(dec);	break;
		case PRIM_DATE:			pbacnet = new BACnetDate(dec); break;
		case PRIM_TIME:			pbacnet = new BACnetTime(dec); break;
		case PRIM_OBJECT_IDENTIFIER:	pbacnet = new BACnetObjectIdentifier(dec); break;
		case 13:				pbacnet = new BACnetOpeningTag();	break;
		case 14:				pbacnet = new BACnetClosingTag();	break;

		// Let the user type in whatever they want as hex
		case 15:				pbacnet = new BACnetANY();	break;
	}
	}
	catch(...)
	{
		pbacnet = NULL;
	}

	return pbacnet;
}


/////////////////////////////////////////////////////////////////////////////

IMPLEMENT_SERIAL(VTSDevProperty, CObject, 1);

VTSDevProperty::VTSDevProperty()
				: m_devvalues()
{
	m_nPropID = 0;
	m_fIsArray = false;
}


VTSDevProperty::~VTSDevProperty()
{
}


// Encode for version 1

void VTSDevProperty::Serialize(CArchive& ar, UINT version)
{
	m_devvalues.Serialize(ar, version);
	if (ar.IsStoring())
	{
		// Decode for version 1.  If changes are made, switch on schema and decode properly
		ar << m_nPropID;
		ar << (m_fIsArray ? 1 : 0);
	}
	else
	{
		ar >> m_nPropID;
		int n;
		ar >> n;
		m_fIsArray = n != 0;
	}
}


const VTSDevProperty& VTSDevProperty::operator=(const VTSDevProperty& rdevpropertySrc)
{
	m_nPropID = rdevpropertySrc.m_nPropID;
	m_fIsArray = rdevpropertySrc.m_fIsArray;
	m_devvalues.DeepCopy(&rdevpropertySrc.m_devvalues);

	return *this;
}


CString VTSDevProperty::GetDescription( void )
{
	return CString(GetDescription(GetID()));
}


LPCSTR VTSDevProperty::GetDescription( unsigned int nPropID )
{
	return NetworkSniffer::BAC_STRTAB_BACnetPropertyIdentifier.EnumString( nPropID );
}


/////////////////////////////////////////////////////////////////////////////

IMPLEMENT_SERIAL(VTSDevObject, CObject, 1);

VTSDevObject::VTSDevObject()
{
	m_nbacnetObjID = 0;
}


VTSDevObject::~VTSDevObject()
{
}


CString VTSDevObject::GetDescription( void )
{
	CString buff;
    BACnetObjectIdentifier(GetID()).Encode( buff, BACnetEncodeable::FMT_SCRIPT );

	return buff;
}

// Encode for version 1

void VTSDevObject::Serialize(CArchive& ar, UINT version)
{
	m_devproperties.Serialize(ar, version);

	if (ar.IsStoring())
	{
		// Decode for version 1.  If changes are made, switch on schema and decode properly
		ar << m_nbacnetObjID;
	}
	else
	{
		ar >> m_nbacnetObjID;
	}
}


const VTSDevObject& VTSDevObject::operator=(const VTSDevObject& rdevobjectSrc)
{
	m_nbacnetObjID = rdevobjectSrc.m_nbacnetObjID;
	m_devproperties.DeepCopy(&rdevobjectSrc.m_devproperties);

	return *this;
}

//=================================================================================
//=================================================================================
