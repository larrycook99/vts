#if !defined(AFX_SCRIPTMSGMAKE_H__9221BBDD_15A7_400A_84B6_FA0B3269318D__INCLUDED_)
#define AFX_SCRIPTMSGMAKE_H__9221BBDD_15A7_400A_84B6_FA0B3269318D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ScriptExecMsg.h : header file
//

#include "ScriptExecMsg.h"

class CScriptMakeDlg;

class ScriptMsgMake : public ScriptExecMsg
{
	public:

		enum ScriptMsgMakeType
		{
			msgmakeCreate,
			msgmakeDestroy,
			msgmakeSetText
		};

		ScriptMsgMakeType	m_maketype;
		CScriptMakeDlg *	m_pdlg;

		ScriptMsgMake(ScriptMsgMakeType nType, CScriptMakeDlg * pdlg);
		virtual ~ScriptMsgMake();
};


/////////////////////////////////////////////////////////////////////////////


#endif // !defined(AFX_SCRIPTEXECMSG_H__9221BBDD_15A7_400A_84B6_FA0B3269318D__INCLUDED_)
