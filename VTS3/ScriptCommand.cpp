// ScriptCommand.cpp: implementation of the ScriptCHECKCommand class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ScriptDocument.h"
#include "ScriptPacket.h"
#include "ScriptIfdefHandler.h"
#include "ScriptCommand.h"

#include "ScriptMakeDlg.h"
#include "ScriptMsgMake.h"

//Added by Zhu Zhenhua, 2003-12-31, to load PICS fail time
namespace PICS {
	
#include "db.h" 
#include "service.h"
#include "vtsapi.h"
#include "props.h"
#include "bacprim.h"
#include "dudapi.h"
#include "propid.h"
}
extern PICS::PICSdb *gPICSdb;
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////


ScriptMessage::ScriptMessage( ScriptIfdefHandler & ifdefHandler, ScriptScanner & scan, ScriptToken & tok, CString * pstrTestTitle, CWnd * pparent /* = NULL */ )
			  : ScriptCommand(ScriptBase::scriptCheck)
			  , m_pstrTestTitle(pstrTestTitle)
{	
	baseStatus = 0;
	m_pParent = pparent;

	baseLineStart = tok.tokenLine;
	scan.Next( tok );
	if ( tok.tokenType != scriptValue || tok.tokenEnc != scriptASCIIEnc )
		throw "CHECK/MAKE statement requires \"Title Text\" following keyword";
	
	baseLabel = tok.RemoveQuotes();
	Parse(ifdefHandler, scan, tok);
	baseLineCount = baseLineStart - tok.tokenLine;
}


void ScriptMessage::Parse( ScriptIfdefHandler & ifdefHandler, ScriptScanner & scan, ScriptToken & tok )
{
	bool fOpen = false;

	// start looking first for ')'... it might be where we left off or the next line.
	// either way... look for it first.

	scan.Next(tok);

	for(;;)
	{
		if ( tok.tokenType == scriptEOF )
			throw "Unexpected end of file";

		if ( tok.tokenType != scriptEOL )
		{
			if ( !fOpen )
			{
				// we need to check for the first thing...
				if ( tok.tokenType != scriptSymbol ||  tok.tokenSymbol != '(' )
					throw "Open parenthesis '(' expected after CHECK/MAKE title";

				fOpen = true;			// make sure we don't do this again
				scan.Next(tok);			// could look for next string right after (...
				continue;
			}

			if ( tok.tokenEnc == scriptASCIIEnc && tok.tokenType == scriptValue )
			{
				if ( !tok.IsDontCare() )
					m_strText = m_strText + (m_strText.IsEmpty() ? "" : "\n") + tok.RemoveQuotes();
				scan.Next(tok);
			}

			// know when to quit?
			if ( tok.tokenType != scriptEOL )
			{
				if ( tok.tokenType != scriptSymbol ||  tok.tokenSymbol != ')' )
					throw "Closing parenthesis ')' expected after CHECK/MAKE text";

				// found closing deal... move on from here
				break;
			}
		}

		do
			scan.NextLine(tok);
		while ( ifdefHandler.IsIfdefExpression(tok, &scan) || ifdefHandler.IsSkipping() );
	}
}


ScriptCHECKCommand::ScriptCHECKCommand( ScriptIfdefHandler & ifdefHandler, ScriptScanner & scan, ScriptToken & tok, CString * pstrTestTitle, CWnd * pparent /* = NULL */ )
				   :ScriptMessage(ifdefHandler, scan, tok, pstrTestTitle, pparent)
{
	baseType = scriptCheck;

	// set the image list offset and status
	baseImage = 21;
}


ScriptCHECKCommand::~ScriptCHECKCommand()
{
}


bool ScriptCHECKCommand::Execute(CString * pstrError)
{
	CString strFullText;

	strFullText.Format("TEST:  %s\nCHECK: %s\n_____________________________________________\n\n%s\n_____________________________________________\nPass %s?",
						(LPCSTR) *m_pstrTestTitle, baseLabel, m_strText, baseLabel);

	CScriptMakeDlg	dlg(false, strFullText, m_pParent);
	if ( dlg.DoModal() != IDOK )
//	if ( AfxMessageBox(strFullText, MB_YESNO | MB_ICONQUESTION) != IDYES )
	{
		ASSERT(pstrError != NULL);
		if ( pstrError != NULL )
			pstrError->Format("CHECK \"%s\" failed", baseLabel);

		return false;
	}

	return true;
}

//Class ScriptAssignCommand Added by Zhu Zhenhua, 2003-12-24, to ASSIGN statement

ScriptAssignCommand::ScriptAssignCommand( ScriptIfdefHandler & ifdefHandler, ScriptScanner & scan, ScriptToken & tok, CString * pstrTestTitle, CWnd * pparent /* = NULL */ )
			  : ScriptCommand(ScriptBase::scriptAssign)
{	
	baseStatus = 0;
	baseLineStart = tok.tokenLine;
	scan.Next( tok );
	if ( tok.tokenType != scriptValue || tok.tokenEnc != scriptASCIIEnc )
		throw "ASSIGN statement requires \"Title Text\" following keyword";
	
	baseLabel = tok.RemoveQuotes();
	Parse(ifdefHandler, scan, tok);
	baseLineCount = baseLineStart - tok.tokenLine;

	CString str = "Need 'PASSED' or 'FAILED' following ASSIGN !";
	m_assigntext = baseLabel;
	if(m_assigntext != "PASSED" && m_assigntext != "FAILED")
		AfxMessageBox(str);
	// set the image list offset and status
	baseImage = 22;
}

ScriptAssignCommand::~ScriptAssignCommand()
{
}
void ScriptAssignCommand::Parse( ScriptIfdefHandler & ifdefHandler, ScriptScanner & scan, ScriptToken & tok )
{
	bool fOpen = false;

	// start looking first for ')'... it might be where we left off or the next line.
	// either way... look for it first.

	scan.Next(tok);

	for(;;)
	{

		if (tok.tokenType != scriptEOL)
		 throw "End of line After ASSIGN text";
		 else
		 break;
		do
			scan.NextLine(tok);
		while ( ifdefHandler.IsIfdefExpression(tok, &scan) || ifdefHandler.IsSkipping() );
	}
}


bool ScriptAssignCommand::Execute(CString * pstrError)
{
	if (m_assigntext == "PASSED")
	{
		return true;
	}

	*pstrError =  "FAIL by ASSIGN statement";
	return false;
}

//Class ScriptWaitCommand Added by Zhu Zhenhua, 2003-12-31, to WAIT statement

ScriptWaitCommand::ScriptWaitCommand( ScriptIfdefHandler & ifdefHandler, ScriptScanner & scan, ScriptToken & tok, CString * pstrTestTitle, ScriptDocument * pdoc,CWnd * pparent /* = NULL */ )
			  : ScriptCommand(ScriptBase::scriptWait)
{	
	baseStatus = 0;
	baseLineStart = tok.tokenLine;
	baseLabel = "WAIT";
	scan.Next( tok );
	if ((tok.tokenType == scriptSymbol) && (tok.tokenSymbol == '{'))
	{
		//get value from EPICS
		CString strTime = "";
		scan.Next( tok );
		while ( !(tok.tokenType == scriptSymbol  &&  tok.tokenSymbol == '}') )
		{
			strTime += tok.tokenValue;
			scan.Next( tok );
			if (tok.tokenType == scriptEOL) 
			{
				throw "Close brace '}' expected";
			}
		}
		int nCode = ScriptToken::HashCode(strTime);
		int nIndex = ScriptToken::Lookup( nCode, scriptFailTimesMap );
		if (nIndex == -1) 
		{
			throw "unknown Fail Time";
		}
		if (!gPICSdb)
			throw "No EPICS information loaded";
		int nTime = gPICSdb->BACnetFailTimes[nIndex];
		if (nTime == ftNotSupported) {
			throw "This Fail Time is not support in EPICS database";
		}
		m_nwaittime = nTime;
	}
	else if(tok.tokenType == scriptKeyword)
	{	
		ScriptParmPtr pParm;
		// resolve the keyword as a parameter?
		if ((pParm = pdoc->m_pParmList->LookupParm( tok.tokenSymbol )) != 0)
		{	
			char* pNum = pParm->parmValue.GetBuffer(0);
			StringToValue(pNum);
		}
		else
		{
			throw "Can not find the wait value parm";
		}
	}
	else if(!tok.IsInteger(m_nwaittime))
	{
		throw "Packet Wait expected";
	}

	Parse(ifdefHandler, scan, tok);
	baseLineCount = baseLineStart - tok.tokenLine;
	CString str = "wait time can not be negative";
	if(m_nwaittime < 0)
		AfxMessageBox(str);

	// set the image list offset and status
	baseImage = 23;
}

void ScriptWaitCommand::StringToValue( const char *dec )
{
	BOOL				negValue = FALSE
	;
	int					t
	;

	// look for a sign
	if (*dec == '-') {
		negValue = TRUE;
		dec += 1;
	} else
	if (*dec == '+')
		dec += 1;

	// figure out what encoding to use
	if ( ((dec[0] == 'D') && (dec[1] == '\''))					// D'nnn'
		|| ((dec[0] == 'd') && (dec[1] == '\''))
		) {
		// decimal encoding
		dec += 2;
		if (((strlen(dec) - 1) % 3) != 0)			// must be triplet
			throw "The string for wait time value must be triplet.";
		for (m_nwaittime = 0; *dec != '\''; ) {
			if (!IsDigit(*dec))
				throw "There's a invalid character for wait time value parm.";
			t = (*dec++ - '0');

			if (!IsDigit(*dec))
				throw "There's a invalid character for wait time value parm.";
			t = (t * 10) + (*dec++ - '0');

			if (!IsDigit(*dec))
				throw "There's a invalid character for wait time value parm.";
			t = (t * 10) + (*dec++ - '0');

			m_nwaittime = (m_nwaittime * 256) + t;
		}
	} else
	if ( ((dec[0] == '0') && (dec[1] == 'x'))					// 0xFF, X'FF', &xFF
		|| ((dec[0] == 'X') && (dec[1] == '\''))
		|| ((dec[0] == 'x') && (dec[1] == '\''))
		|| ((dec[0] == '&') && (dec[1] == 'x'))
		|| ((dec[0] == '&') && (dec[1] == 'X'))
		) {
		// hex encoding
		dec += 2;
		for (m_nwaittime = 0; *dec && (*dec != '\''); dec++) {
			if (!IsXDigit(*dec))
				throw "There's a invalid character for wait time value parm.";
			m_nwaittime = (m_nwaittime * 16) + (IsDigit(*dec) ? (*dec - '0') : (*dec - 'A' + 10));
		}
	} else
	if ( ((dec[0] == '0') && (dec[1] == 'o'))					// 0o377, O'377', &O377
		|| ((dec[0] == 'O') && (dec[1] == '\''))
		|| ((dec[0] == 'o') && (dec[1] == '\''))
		|| ((dec[0] == '&') && (dec[1] == 'O'))
		|| ((dec[0] == '&') && (dec[1] == 'o'))
		) {
		// octal encoding
		dec += 2;
		for (m_nwaittime = 0; *dec && (*dec != '\''); dec++) {
			if ((*dec < '0') || (*dec > '7'))
				throw "There's a invalid character for wait time value parm.";
	
			m_nwaittime = (m_nwaittime * 8) + (*dec - '0');
		}
	} else
	if ( ((dec[0] == '0') && (dec[1] == 'b'))					// 0b11111111, B'11111111', &B11111111
		|| ((dec[0] == 'B') && (dec[1] == '\''))
		|| ((dec[0] == 'b') && (dec[1] == '\''))
		|| ((dec[0] == '&') && (dec[1] == 'B'))
		|| ((dec[0] == '&') && (dec[1] == 'b'))
		) {
		// binary encoding
		dec += 2;
		for (m_nwaittime = 0; *dec && (*dec != '\''); dec++) {
			if ((*dec < '0') || (*dec > '1'))
				throw "There's a invalid character for wait time value parm.";
			m_nwaittime = (m_nwaittime * 2) + (*dec - '0');
		}
	} else
	if (IsDigit(*dec)) {										// nnn
		// integer encoding
		for (m_nwaittime = 0; *dec; dec++)
			if (!IsDigit(*dec))
				throw "There's a invalid character for wait time value parm.";
			else
				m_nwaittime = (m_nwaittime * 10) + (*dec - '0');
	} else
		throw "There's a invalid character for wait time value parm.";
		//throw_(33) /* unknown or invalid encoding */;

	// update for sign
	if (negValue)
		m_nwaittime = (m_nwaittime * -1);
}
ScriptWaitCommand::~ScriptWaitCommand()
{
}
void ScriptWaitCommand::Parse( ScriptIfdefHandler & ifdefHandler, ScriptScanner & scan, ScriptToken & tok )
{
	bool fOpen = false;

	// start looking first for ')'... it might be where we left off or the next line.
	// either way... look for it first.

	scan.Next(tok);

	for(;;)
	{

		if (tok.tokenType != scriptEOL)
		 throw "End of line After WAIT text";
		 else
		 break;
		do
			scan.NextLine(tok);
		while ( ifdefHandler.IsIfdefExpression(tok, &scan) || ifdefHandler.IsSkipping() );
	}
}


bool ScriptWaitCommand::Execute(CString * pstrError)
{
	if(m_nwaittime >= 0)
	{
		Sleep(m_nwaittime);
		return true;
	}
	else
	{
		CString strFullText = "Wait time can't be negative ";
		*pstrError = strFullText;
		return false;
	}
}

ScriptMAKECommand::ScriptMAKECommand( ScriptIfdefHandler & ifdefHandler, ScriptScanner & scan, ScriptToken & tok, CString * pstrTestTitle, CWnd * pparent /* = NULL */ )
				   :ScriptMessage(ifdefHandler, scan, tok, pstrTestTitle, pparent )
{
	baseType = scriptMake;
	// set the image list offset and status
	baseImage = 25;
	m_pdlg = NULL;
	m_fHanging = false;
	m_nDestroyCode = 0;

}


ScriptMAKECommand::~ScriptMAKECommand()
{
	Kill();
}


void ScriptMAKECommand::Kill(int nDestroyCode /* = 1 */ )
{
	if ( m_pdlg != NULL  )
	{
		if ( m_pdlg->IsUp()  && m_pqueue != NULL )
			m_pqueue->Fire( new ScriptMsgMake(ScriptMsgMake::msgmakeDestroy, m_pdlg) );

		m_pdlg = NULL;
	}

	// -1 for failure, 1 for success
	m_nDestroyCode = nDestroyCode;
}



bool ScriptMAKECommand::IsUp()
{
	if ( m_pdlg )
	{
		if ( m_pdlg->IsUp() )
			m_nDestroyCode = 0;
		else
			Kill(m_pdlg->IsSuccess() ? 1 : -1);
	}

	return m_nDestroyCode == 0;
}


bool ScriptMAKECommand::IsSuccess()
{
	IsUp();
	return m_nDestroyCode == 1;
}


void ScriptMAKECommand::SetQueue(ScriptExecMsgQueue * pqueue)
{
	m_pqueue = pqueue;
}



bool ScriptMAKECommand::Execute(CString * pstrError)
{
	CString strFullText;
	bool fResult = true;

	strFullText.Format("TEST:  %s\nMAKE: %s\n_____________________________________________\n\n%s\n_____________________________________________\nPress OK when complete.",
						(LPCSTR) *m_pstrTestTitle, baseLabel, m_strText);

	// should only be called once... but just go with the flow...
	
	ASSERT(m_pdlg == NULL);
	if ( m_pdlg != NULL )
		return false;

	// called with string error pointer if we should do modal...
	if ( m_fHanging  &&  m_pqueue != NULL )
	{
		m_pdlg = new CScriptMakeDlg(true, strFullText, m_pParent );
		m_pqueue->Fire( new ScriptMsgMake(ScriptMsgMake::msgmakeCreate, m_pdlg) );
	}
	else
	{
		CScriptMakeDlg	dlg(true, strFullText, m_pParent );
		if ( dlg.DoModal() != IDOK )
		{
			if ( pstrError != NULL )
				pstrError->Format("MAKE \"%s\" failed", baseLabel);
			fResult = false;
		}
	}

	return fResult;
}
