// ScriptIfdefHandler.h: interface for the ScriptIfdefHandler class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SCRIPTIFDEFHANDLER_H__2423F5E1_196D_4954_9BAA_A60777FD9F9C__INCLUDED_)
#define AFX_SCRIPTIFDEFHANDLER_H__2423F5E1_196D_4954_9BAA_A60777FD9F9C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "ScriptDocument.h"
#include "ScriptPacket.h"
#include "ScriptKeywords.h"


class ScriptScanner;

// Define maximum left for ifdef nesting...
#define IFDEF_LEVEL_MAX			20


class ScriptIfdefHandler : public CObject
{
	private:
		int				 m_curIfdefLevel;
		unsigned char	 m_bIfdefFlags[IFDEF_LEVEL_MAX];

//madanner 6/03
//		ScriptScanner  & m_scanner;

		ScriptDocument * m_pDoc;

//madanner 6/03		bool EvaluateConditional(ScriptToken & tok );
		bool EvaluateConditional(ScriptToken & tok, ScriptScanner * pscan );

	public:
//madanner 6/03		ScriptIfdefHandler( ScriptDocument * pDoc, ScriptScanner & scan );
		ScriptIfdefHandler( ScriptDocument * pDoc );
		virtual ~ScriptIfdefHandler();

		bool IsIfdefExpression(ScriptToken &tok, ScriptScanner * pscan);
		bool IsSkipping(void);
		bool IsIfBlock(void);
};


class ScriptIfdefExpr : public CObject
{
	private:
		void GetEPICSProperty(LPCTSTR lpszValue, BACnetAnyValue * pbacnetAny);        			//GJB, 12/21/2003
		ScriptDocument * m_pdocument;

		void ParseForValue(ScriptToken &tok );
		void ResolveToValue( ScriptToken & tok );
		void ResolveToValue( LPCTSTR lpszValue, ScriptTokenList & tokenList );				//GJB, 12/21/2003
		BACnetEncodeable * CreateOperand( ScriptToken & token );

	public:
		ScriptIfdefExpr( ScriptDocument * pdoc );
		ScriptIfdefExpr( ScriptDocument * pdoc, ScriptScanner &scan, ScriptToken & tok );

		bool Evaluate( ScriptToken & tok);
		void Parse( ScriptScanner &scan, ScriptToken &tok );

		ScriptToken		m_tokLValue; 					// value on left of operator
		ScriptToken		m_tokRValue;					// value on right of operator
		int				m_nOp;							// operator
		int				m_nLine;						// line number in script
};



#endif // !defined(AFX_SCRIPTIFDEFHANDLER_H__2423F5E1_196D_4954_9BAA_A60777FD9F9C__INCLUDED_)
