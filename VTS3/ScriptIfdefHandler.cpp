// ScriptIfdefHandler.cpp: implementation of the ScriptIfdefHandler class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ScriptIfdefHandler.h"

namespace PICS {
	
#include "db.h" 
#include "service.h"
#include "vtsapi.h"
#include "props.h"
#include "bacprim.h"
#include "dudapi.h"
#include "propid.h"
	
	//extern struct generic_object;
	//class BACnetAnyValue;
	extern "C" void CreatePropertyFromEPICS( PICS::generic_object * pObj, int PropId, BACnetAnyValue * pbacnetAnyValue );
	
}
extern PICS::PICSdb *gPICSdb;


// Flags for testing IFDEF stuff

#define IFDEF_INCLUDING		0x01			// we're currently including the stuff
#define IFDEF_SPENT			0x02			// we've already taken the true condition
#define IFDEF_ELSE			0x04			// we've encountered the final else (to prevent elseif following)


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

//madanner 6/03  ScriptIfdefHandler::ScriptIfdefHandler( ScriptDocument * pDoc, ScriptScanner & scan )
ScriptIfdefHandler::ScriptIfdefHandler( ScriptDocument * pDoc )
				   :m_pDoc(pDoc)
{
	m_curIfdefLevel = 0;

	// setup first condition for other ifdefs to build from...
	m_bIfdefFlags[0] = IFDEF_SPENT | IFDEF_INCLUDING;
}


ScriptIfdefHandler::~ScriptIfdefHandler()
{
}


bool ScriptIfdefHandler::IsIfBlock()
{
	return m_curIfdefLevel != 0;
}


// This method returns true if the token encountered is an IFDEF type thing...
// The caller, in the true instance, should abort searching through the next line...
// This class keeps track of whether or not we should be collecting stuff...

bool ScriptIfdefHandler::IsIfdefExpression(ScriptToken &tok, ScriptScanner * pscan)
{
	// we have to deal with conditional compilation.  We want to take care of it here
	// so we don't have to specially deal with any token if we're not collection.  Dig?

	switch (tok.tokenSymbol)
	{
		case kwELSEDEF:

			// see if we should skip or do this section...
			if ( m_curIfdefLevel <= 0  ||  (m_bIfdefFlags[m_curIfdefLevel] & IFDEF_ELSE)  )
				throw "ELSE encountered without matching IF";

			// set flag telling us we've just encounted the valid else
			m_bIfdefFlags[m_curIfdefLevel] |= IFDEF_ELSE;
			// drop through....

		case kwELSEIFDEF:

			if ( tok.tokenSymbol == kwELSEIFDEF  )
			{
				if ( m_curIfdefLevel <= 0 )
					throw "ELSEIF encountered without matching IF";
				if ( m_bIfdefFlags[m_curIfdefLevel] & IFDEF_ELSE )
					throw "ELSEIF encountered following ELSE clause";
			}

			// if we haven't had a valid true condition yet, make this on go...
			// otherwise... keep skipping until end

			if ( m_bIfdefFlags[m_curIfdefLevel] & IFDEF_SPENT )
				m_bIfdefFlags[m_curIfdefLevel] &= ~IFDEF_INCLUDING;
			else
			{
				// see if we should include this next section...
				if ( tok.tokenSymbol == kwELSEDEF || EvaluateConditional(tok, pscan) )
					m_bIfdefFlags[m_curIfdefLevel] |= (IFDEF_SPENT | IFDEF_INCLUDING);
				else
					// shut off including code
					m_bIfdefFlags[m_curIfdefLevel] &= ~IFDEF_INCLUDING;
			}
			return true;


		case kwENDIF:

			if ( m_curIfdefLevel <= 0 )
				throw "ENDIF encountered without matching IF";

			m_curIfdefLevel--;
			return true;


		case kwIFDEF:

			// they want to conditional compile this section...  test expression result
			// throw if an error

			if ( m_curIfdefLevel+1 >= IFDEF_LEVEL_MAX )
				throw "Exceeded maximum number of nested IFs";

			// check current level to see if we're including... if so.. just make this one.
			// otherwise, test this condition

			if ( (m_bIfdefFlags[m_curIfdefLevel++] & IFDEF_INCLUDING) == 0 )
				m_bIfdefFlags[m_curIfdefLevel] = IFDEF_SPENT;
			else
			{
				if ( EvaluateConditional(tok, pscan) )
					m_bIfdefFlags[m_curIfdefLevel] = (IFDEF_SPENT | IFDEF_INCLUDING);
				else
					m_bIfdefFlags[m_curIfdefLevel] = 0;		// not spent and not including, start fresh
			}
			return true;
	}

	return false;
}


bool ScriptIfdefHandler::IsSkipping()
{
	// OK... if we made it this far then what we have at least isn't an IFDEF type deal...
	// just toss it if we're not including, eh?

	return ( m_curIfdefLevel  &&  (m_bIfdefFlags[m_curIfdefLevel] & IFDEF_INCLUDING) == 0 );
}


bool ScriptIfdefHandler::EvaluateConditional(ScriptToken & tok, ScriptScanner * pscan )
{
	pscan->Next( tok );
	if ( tok.tokenType != scriptSymbol  &&  tok.tokenSymbol != '(' )
		throw "'(' expected for IF conditional expression";

	bool fResult;
	bool fFinished = false;

	do
	{
		// suck expression into parts and evalute.  will throw exception if problem
		ScriptIfdefExpr		expression(m_pDoc, *pscan, tok);

		if ( !fFinished )
			fResult = expression.Evaluate(tok);

		// expression is finished... now look for either AND OR or )
		pscan->Next( tok );

		// we could just bail here if final condition is known... but continue on so we look for closing parens
		// this is to add some measure of completeness here...

		if ( tok.tokenType == scriptKeyword &&  tok.tokenSymbol == kwAND )
		{
			// we've just read the AND...  if the previous expression is false, they want to AND that with the
			// coming expression... but it will be flase because of the last expresson... just kill it.

			if ( !fResult )
				fFinished = true;
		}
		else if ( tok.tokenType == scriptKeyword  && tok.tokenSymbol == kwOR )
		{
			// next thing is an OR.  We can stop if the expression just evaluated to true

			if ( fResult )
				fFinished = true;
		}

		// keep going if we've hit the AND or OR... all others will stop this
	}
	while ( tok.tokenType == scriptKeyword && (tok.tokenSymbol == kwAND || tok.tokenSymbol == kwOR) );

	if ( tok.tokenType != scriptSymbol  &&  tok.tokenSymbol != ')' )
		throw "Closing parenthesis ')', OR or AND expected";

	return fResult;
}



//===========================================================
//===========================================================
//===========================================================


ScriptIfdefExpr::ScriptIfdefExpr( ScriptDocument * pdoc )
	: m_pdocument(pdoc), m_tokLValue(), m_tokRValue(), m_nOp(0), m_nLine(0)
{
}


ScriptIfdefExpr::ScriptIfdefExpr( ScriptDocument * pdoc, ScriptScanner & scan, ScriptToken &tok )
	: m_pdocument(pdoc), m_tokLValue(), m_tokRValue(), m_nOp(0), m_nLine(0)
{
	Parse(scan, tok);
}



// We need to pass tok here so error reported shows proper stuff in editor on throw

bool ScriptIfdefExpr::Evaluate(ScriptToken & tok)
{
	CString				strError;

	// Need to contain dynamically allocated objects so throws don't, well, throw off the delete :)

	// Set assignment so editor reports proper section on throw
	tok = m_tokLValue;
	BACnetAnyValue		bacnet1(CreateOperand(m_tokLValue));

	// Set assignment so editor reports proper section on throw
	tok = m_tokRValue;
	BACnetAnyValue		bacnet2(CreateOperand(m_tokRValue));

	// if we're using any don't cares...  we should just say "fine!"
	if ( bacnet1.GetObject()->IsKindOf(RUNTIME_CLASS(BACnetNull)) || bacnet2.GetObject()->IsKindOf(RUNTIME_CLASS(BACnetNull)) )
		return true;

	// types can be either char, int, bool or unsigned (means keyword) or undefined var
	// cannot mix and match types...

	if ( strcmp(bacnet1.GetObjectType()->m_lpszClassName, bacnet2.GetObjectType()->m_lpszClassName) != 0 )
		throw "Unable to compare different data types in IF expression";

	// remember we're using unsigned values to compare enums like (I-AM-ROUTER-TO-NETWORK) so we have to
	// report an error if the script is attempting <, >, <= or >=

	if ( (bacnet1.GetObject()->IsKindOf(RUNTIME_CLASS(BACnetUnsigned)) || bacnet1.GetObject()->IsKindOf(RUNTIME_CLASS(BACnetBoolean))) &&  m_nOp != '=' && m_nOp != '!=' )
		throw "Operators <, <=, >, >= are unsupported for keyword and boolean comparisons";

	return bacnet1.GetObject()->Match(*(bacnet2.GetObject()), m_nOp, &strError);
}



BACnetEncodeable * ScriptIfdefExpr::CreateOperand( ScriptToken & token )
{
	CString	strError;
	int		nValue;			

	ResolveToValue(token);

	if ( token.IsDontCare() )			// deal with don't cares... caller must know BACnetNull == don't care
		return new BACnetNull();

	if ( token.tokenEnc == scriptASCIIEnc  &&  token.tokenType == scriptValue )
		return new BACnetCharacterString(token.RemoveQuotes());

//	if ( token.tokenEnc == scriptIntegerEnc )
//	{
		if ( token.tokenType == scriptKeyword )
		{
			// Value after resolving through parameters is still a keyword... this means it's either
			// a boolean, intended enumeration or an undefined variable.

			if ( (nValue = token.Lookup(token.tokenSymbol, ScriptBooleanMap)) >= 0 )
				return new BACnetBoolean(nValue);

			return new BACnetUnsigned(token.tokenSymbol);
		}

		if ( token.tokenType == scriptValue && token.IsInteger(nValue) )
			return new BACnetInteger(nValue);
//	}
		//GJB, 12/21/2003, in case of using an EPICS reference in an IF expression 
		if ( token.tokenType = scriptReference ) {
			BACnetAnyValue		bacnetEPICSProperty;
			GetEPICSProperty( token.tokenValue, &bacnetEPICSProperty);
			int nType = bacnetEPICSProperty.GetType();
			switch( nType ) {
				case u127:		// 1..127 ---------------------------------
				case u16:		// 1..16 ----------------------------------
				case ud:		// unsigned dword -------------------------
				case uw:		// unsigned word --------------------------
					return new BACnetUnsigned( ((BACnetUnsigned *) bacnetEPICSProperty.GetObject())->uintValue );
					break;
				case ssint:		// short signed int -----------------------		// actually the same type
				case sw:		// signed word ----------------------------
				case ptInt32:
					return new BACnetInteger( ((BACnetInteger *) bacnetEPICSProperty.GetObject())->intValue );
					break;
				case ebool:
					return new BACnetBoolean( ((BACnetBoolean *) bacnetEPICSProperty.GetObject())->boolValue );
				case ch_string:		// char [MAX_TEXT_STRING]
					return new BACnetCharacterString( *((BACnetCharacterString *) bacnetEPICSProperty.GetObject()) );
				case 0:
					break;		// not EPICS property reference
				default:
					strError.Format("Unsupported EPICS property reference for conditional expression", token.tokenValue );
					throw CString(strError);
			}
			
			//if this reference is not a EPICS property reference, then maybe it is a EPICS service support reference
			// make sure there's something to search
			if (!gPICSdb)
				throw "No EPICS information loaded";
			CString strValue(token.tokenValue);
			ScriptTokenList tokenList;
			ResolveToValue(strValue, tokenList);
			int nLength = tokenList.Length();
			if ( (nLength!=2) && (nLength!=3) ) {
				throw "BACnetStandardServices requirement: \'INITIATE| EXECUTE\' expected";
			}
			
			// get BACnet Standard Service type
			int nServiceType = ScriptToken::Lookup( tokenList[0].tokenSymbol, scriptStandardServicesMap);
			if ( nServiceType > 0 ) {
				unsigned char  service = ssNotSupported;
				// so, it is a EPICS BACnetStandardServices reference
				int nLoop = (nLength==2) ? 1 : 2;
				for( int i=0; i<nLoop; i++){
					// get requirement of BACnetStandardServices
					int nRequirement = ScriptToken::Lookup( tokenList[i+1].tokenSymbol, scriptStandardServicesRequirementMap);
					if ( nRequirement < 0 ) {
						throw "requirement: \'INITIATE| EXECUTE\' expected";
					}
					switch(nRequirement) {
					case 1:
						service |= ssInitiate;
						break;
					case 2:
						service |= ssExecute;
						break;
					default:
						;
					}
				}
				BOOL bFlag = FALSE;
				if ( gPICSdb->BACnetStandardServices[nServiceType] == service ) {
					bFlag = TRUE;
				}
			return new BACnetBoolean(bFlag);
			}
		}

	strError.Format("Unsupported operand type (%s) for conditional expression", token.tokenValue );
	throw CString(strError);
	return NULL;
}



void ScriptIfdefExpr::ResolveToValue( ScriptToken & tok )
{
	if ( tok.tokenType == scriptKeyword )
	{
		ScriptParm *  pp;
	
		if ( (pp = m_pdocument->m_pParmList->LookupParm( tok.tokenSymbol )) != 0 )
		{
			ScriptScanner	scanner(pp->parmValue);
			ScriptToken		token;

			scanner.Next(token);
			ResolveToValue(token);
			tok = token;
		}
	}
}

void ScriptIfdefExpr::ResolveToValue( LPCTSTR lpszValue, ScriptTokenList & tokenList )
{
	ScriptScanner	scan( lpszValue );
	ScriptToken		tok;
	ScriptParmPtr	pParm;

	// start at the beginning
	scan.Next( tok );
	if (tok.tokenType == scriptEOL)
		return;
	
	for (;;) {
		// check the value
		switch (tok.tokenType) {
		case scriptKeyword:
			// resolve the keyword as a parameter?
			if ((pParm = m_pdocument->m_pParmList->LookupParm( tok.tokenSymbol )) != 0) {
				ResolveToValue(pParm->parmValue, tokenList);
				break;
			}
			
		case scriptValue:
			tokenList.Append( tok );
			break;
			
		case scriptReference:
			tokenList.Append( tok );
			break;
			
		default:;
			throw "Object Type, instance number or property type should be here";
		}
		
		// check for EOL
		scan.Next( tok );
		if (tok.tokenType == scriptEOL)
			break;
		
		// check for more
		if ( ((tok.tokenType != scriptSymbol) || (tok.tokenSymbol != ','))
		  && ((tok.tokenType != scriptSymbol) || (tok.tokenSymbol != '|')) )
			throw  "End-of-line or comma expected in value list";
		
		// get ready for next value
		scan.Next( tok );
	}

/*
	// allow cases of {property[int]}, {property[var]}, {property[{property}]}, {property[{property[int]}]}, etc.
	// Don't allow cases (yet) of var[var], var[int], var[etc.]
	for ( int i = 0; i < tokenList.GetCount(); i++ )
		tokenList[i].ResolveIndex(m_pdocument->m_pParmList);
*/

}



void ScriptIfdefExpr::ParseForValue(ScriptToken &tok )
{
	if (tok.tokenType == scriptEOF || tok.tokenType == scriptEOL )
		throw "Unexpected end of line, IF expression expected";

	// should be a keyword (var yet to be resolved) or an actual value
//	if ( tok.tokenType != scriptKeyword && tok.tokenType != scriptValue )
	if ( tok.tokenType != scriptKeyword && tok.tokenType != scriptValue 
		  && tok.tokenType != scriptReference )
		throw "Keyword, script variable or literal value expected";
}



void ScriptIfdefExpr::Parse( ScriptScanner & scan, ScriptToken & tok )
{
	// We're scanning ' VALUE = VALUE ' so first parse the first VAR/Keyword/Value
	scan.Next(tok);

	//GJB, 12/21/2003  if the tok is '{', then combine the whole words in the braces to a single token.
	if ( tok.tokenType == scriptSymbol  &&  tok.tokenSymbol == '{' ) {
		//must be EPICS property reference
		m_tokLValue.tokenType = scriptReference;
		scan.Next(tok);
		while ( !(tok.tokenType == scriptSymbol  &&  tok.tokenSymbol == '}') ) {
			m_tokLValue.tokenValue += tok.tokenValue;
			scan.Next(tok);
			if (tok.tokenType == scriptEOL) {
				throw "Close brace '}' expected";
			}
		}
	}else{
		m_tokLValue = tok;
	}

	//	m_tokLValue = tok;
	ParseForValue(m_tokLValue);

	// OK.. got the first value, next check for operator...
	// use RValue as temp

	scan.Next(tok);
	switch(tok.tokenSymbol)
	{
		case '<':	case'<=':  case '>':  case '>=':  case '=':  case '!=':
			break;
		case '?=':	case '>>':
			throw "Assignement or \"don't care\" operator not supported in IF expression";
		default:
			throw "Expression operator expected (=, !=, <, <=, >, >=)";
	}

	m_nOp = tok.tokenSymbol;

	// now get the right side of the equation...
	scan.Next(tok);

	m_tokRValue = tok;
	ParseForValue(m_tokRValue);
}

//GJB, 12/21/2003, in case of using an EPICS reference in an IF expression 
void ScriptIfdefExpr::GetEPICSProperty(LPCTSTR lpszValue, BACnetAnyValue * pbacnetAny)
{
	
	const int whitespace = 0x20;
	CString strValue(lpszValue);
	strValue.Remove( whitespace );
	ScriptTokenList tokenList;
	ResolveToValue(strValue, tokenList);

	if ( tokenList.Length() != 3 ) {
		return;			//not EPICS property reference
//		throw "Object Type, instance number and property type should be here";
	}
	// get object ID
	int nObjType = ScriptToken::Lookup( tokenList[0].tokenSymbol, ScriptObjectTypeMap);
	if ( nObjType < 0 ) {
		return;			//not EPICS property reference
//		throw "Object identifier type expected";
	}
	CString strObjInstanceNum(tokenList[1].tokenValue);
	int nInstanceNum = atoi(strObjInstanceNum.GetBuffer(0));
	int nObjID = (nObjType << 22) + nInstanceNum;
	// get the property ID
	int nPropID = ScriptToken::Lookup( tokenList[2].tokenSymbol, ScriptPropertyMap );
	if (nPropID < 0)
		throw "No such property";
	// verify the context
	if (nObjID == 0xFFFFFFFF)
		throw "No object reference";
	
	// make sure there's something to search
	if (!gPICSdb)
		throw "No EPICS information loaded";
	
	// get a pointer to the object database record
	PICS::generic_object	*pobj;
	pobj = PICS::GetpObj( gPICSdb->Database, nObjID );
	if (!pobj)
		throw "Object not defined in EPICS";
	
	// get the index of the property flags
	int nPropIndex = PICS::GetPropIndex( pobj->object_type, nPropID );
	if (nPropIndex < 0)
		throw "Property not defined for this object type";

	PICS::CreatePropertyFromEPICS( pobj, nPropID, pbacnetAny );

	// deal with the array property

}
