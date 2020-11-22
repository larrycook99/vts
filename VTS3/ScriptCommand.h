// ScriptCommand.h: interface for the ScriptCHECKCommand class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SCRIPTCOMMAND_H__99CE0EF3_EAE9_42D2_AD4C_E0B3F2D056C7__INCLUDED_)
#define AFX_SCRIPTCOMMAND_H__99CE0EF3_EAE9_42D2_AD4C_E0B3F2D056C7__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "ScriptBase.h"
#include "ScriptExecMsg.h"
#include "VTSQueue.h"

class ScriptIfdefHandler;
class CScriptMakeDlg;

//class ScriptScanner;
//class ScriptToken;


class ScriptMessage : public ScriptCommand
{
	protected:
		CString * m_pstrTestTitle;
		CString	m_strText;
		CWnd * m_pParent;
	private:
		void Parse( ScriptIfdefHandler & ifdefHandler, ScriptScanner & scan, ScriptToken & tok );

	public:
		ScriptMessage( ScriptIfdefHandler & ifdefHandler, ScriptScanner & scan, ScriptToken & tok, CString * pstrTest, CWnd * pparent = NULL );
};


class ScriptCHECKCommand : public ScriptMessage
{
	public:
		ScriptCHECKCommand( ScriptIfdefHandler & ifdefHandler, ScriptScanner & scan, ScriptToken & tok, CString * pstrTest, CWnd * pparent = NULL );
		virtual ~ScriptCHECKCommand();

		bool Execute(CString * pstrError);
};


//Class ScriptAssignCommand Added by Zhu Zhenhua, 2003-12-24, to ASSIGN statement

class ScriptAssignCommand : public ScriptCommand
{
	public:
		CString  m_assigntext;
		ScriptAssignCommand(ScriptIfdefHandler & ifdefHandler, ScriptScanner & scan, ScriptToken & tok, CString * pstrTest, CWnd * pparent);
		virtual ~ScriptAssignCommand();
		bool Execute(CString * pstrError);
	private:
		void Parse( ScriptIfdefHandler & ifdefHandler, ScriptScanner & scan, ScriptToken & tok );
};

//Class ScriptWaitCommand Added by Zhu Zhenhua, 2003-12-31, to WAIT statement
class ScriptWaitCommand : public ScriptCommand
{
	public:
		int m_nwaittime;
		ScriptWaitCommand(ScriptIfdefHandler & ifdefHandler, ScriptScanner & scan, ScriptToken & tok, CString * pstrTest, ScriptDocument * pdoc,CWnd * pparent);
		virtual ~ScriptWaitCommand();
		bool Execute(CString * pstrError);
	private:
		void StringToValue( const char *dec );
		void Parse( ScriptIfdefHandler & ifdefHandler, ScriptScanner & scan, ScriptToken & tok );
};
class ScriptMAKECommand : public ScriptMessage
{
	private:
		CScriptMakeDlg		* m_pdlg;
		int					m_nDestroyCode;
		ScriptExecMsgQueue * m_pqueue;

	public:

		bool m_fHanging;			// true if this should be modeless... 

		ScriptMAKECommand( ScriptIfdefHandler & ifdefHandler, ScriptScanner & scan, ScriptToken & tok, CString * pstrTest, CWnd * pparent );
		virtual ~ScriptMAKECommand();

		void SetQueue(ScriptExecMsgQueue * pqueue);

		bool Execute(CString * pstrError);
		void Kill(int nDestroyCode = 1);
		bool IsUp(void);
		bool IsSuccess(void);
};


#endif // !defined(AFX_SCRIPTCOMMAND_H__99CE0EF3_EAE9_42D2_AD4C_E0B3F2D056C7__INCLUDED_)
