// ScriptPacket.cpp: implementation of the ScriptPacket class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "VTS.h"
#include "ScriptDocument.h"
#include "ScriptParmList.h"
#include "ScriptPacket.h"
#include "ScriptKeywords.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//
//	ScriptPacketExpr::ScriptPacketExpr
//

ScriptPacketExpr::ScriptPacketExpr( void )
	: exprKeyword(0)
	, exprIsData(false)
	, exprOp(0)
	, exprValue("")
{
	m_fNoneValue = false;
}


bool ScriptPacketExpr::IsAssignment()
{
	return exprOp == '>>';
}


bool ScriptPacketExpr::IsDontCare()
{
	return exprOp == '?=';
}


//
//	ScriptPacketExpr::~ScriptPacketExpr
//

ScriptPacketExpr::~ScriptPacketExpr( void )
{
}



//
//	ScriptPacketExprList::ScriptPacketExprList
//

ScriptPacketExprList::ScriptPacketExprList( ScriptPacketExprListPtr prev )
	: listPrevious(prev)
{
}

//
//	ScriptPacketExprList::~ScriptPacketExprList
//

ScriptPacketExprList::~ScriptPacketExprList()
{
	for (POSITION pos = GetHeadPosition(); pos; )
		delete (ScriptPacketExprPtr)GetNext( pos );
}

//
//	ScriptPacketExprList::Append
//

void ScriptPacketExprList::Append( ScriptPacketExprPtr spep )
{
	// add to the end of the list
	AddTail( spep );
}

//
//	ScriptPacketExprList::Remove
//

void ScriptPacketExprList::Remove( int indx )
{
	POSITION	pos = FindIndex( indx )
	;

	ASSERT( pos != NULL );
	delete (ScriptPacketExprPtr)GetAt( pos );
	RemoveAt( pos );
}

//
//	ScriptPacketExprList::Length
//

int ScriptPacketExprList::Length( void )
{
	return CList<ScriptPacketExprPtr,ScriptPacketExprPtr>::GetCount();
}

//
//	ScriptPacketExprList::Child
//

ScriptPacketExprPtr ScriptPacketExprList::Child( int indx )
{
	POSITION	pos = FindIndex( indx )
	;

	if (pos != NULL )
		return (ScriptPacketExprPtr)GetAt( pos );
	else
		return 0;
}

//
//	ScriptPacketExprList::Find
//

ScriptPacketExprPtr ScriptPacketExprList::Find( int keyword )
{
	for (POSITION pos = GetHeadPosition(); pos; ) {
		ScriptPacketExprPtr spep = (ScriptPacketExprPtr)GetNext( pos );

		if (spep->exprKeyword == keyword)
			return spep;
	}

	// if there is a previous packet, continue search
	return (listPrevious ? listPrevious->Find( keyword ) : 0);
}

//
//	ScriptPacketExprList::FirstData
//

int ScriptPacketExprList::FirstData( void )
{
	int		len = Length()
	;

	for (int i = 0; i < len; i++) {
		ScriptPacketExprPtr spep = (ScriptPacketExprPtr)GetAt( FindIndex( i ) );

		if (spep->exprIsData)
			return i;
	}

	// failed
	return -1;
}


ScriptCommand::ScriptCommand( ScriptBaseType type )
	: m_pcmdNext(0), m_pcmdPass(0), m_pcmdFail(0), m_nCaseLevel(0)
{
	baseType = type;
	baseStatus = 0;
}


ScriptCommand::~ScriptCommand( void )
{
}


//
//	ScriptPacket::ScriptPacket
//

//ScriptPacket::ScriptPacket( ScriptPacketType type, ScriptPacketSubtype subtype, ScriptPacketPtr prev )
//	: packetType(type)
//	, packetSubtype(subtype)
//	, packetExprList( prev ? &prev->packetExprList : 0 )
//	, packetNext(0), packetDelay( kMaxPacketDelay )
//	, packetPass(0), packetFail(0)
ScriptPacket::ScriptPacket( ScriptPacketType type, ScriptPacketSubtype subtype, ScriptPacketPtr prev )
	: ScriptCommand( ScriptBase::scriptPacket)
	, packetType(type)
	, packetSubtype(subtype)
	, packetExprList( prev ? &prev->packetExprList : 0 )
	, bpacketNotExpect(false)		//Added by Zhu Zhenhua, 2003-11-25
//	, packetDelay( kMaxPacketDelay )    madanner 6/03
	, packetDelay( kDefaultPacketDelay )
{
	// tell the base class this is a packet
//	baseType = scriptPacket;

	CString	str;
	if ( prev != NULL && prev->baseType == ScriptBase::scriptPacket )
	{
		// have to do this... otherwise multiple ors and ands look like:  Or Expect, Or Or Expect, And Or Or Expect
		str = prev->baseLabel.Find("Expect", 0) == -1 ? "Send" : "Expect";
		m_pcmdMake = prev->m_pcmdMake;
	}
	else
	{
		m_pcmdMake = NULL;
		str = "";
	}

	// set the label based on the type
	switch (packetSubtype) {
		case rootPacket:
			switch (packetType) {
				case sendPacket:
					baseLabel = "Send";
					break;
				case expectPacket:
					baseLabel = "Expect";
					break;
			}
			break;
		case andPacket:
			baseLabel = "And " + str;
			break;
		case orPacket:
			baseLabel = "Or " + str;
			break;
	}

	// set the image, clear the status
	baseImage = 13;
	baseStatus = 0;
}

//
//	ScriptPacket::~ScriptPacket
//

ScriptPacket::~ScriptPacket( void )
{
}


bool ScriptPacket::Execute(CString * pstrError)
{
	return false;
}
