#if !defined(AFX_SCRIPTEXECMSG_H__9221BBDD_15A7_400A_84B6_FA0B3269318D__INCLUDED_)
#define AFX_SCRIPTEXECMSG_H__9221BBDD_15A7_400A_84B6_FA0B3269318D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ScriptExecMsg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// ScriptExecMsg window

class ScriptExecMsg : public CObject
{
	public:
		enum ScriptMsgType
			{
				// We see occasional crashes in PreTranslateMessage,
				// probably due to accessing a ScriptExecMsg after
				// it has been deleted.
				// Until we can find the cause, assign fancy values
				// to the message types to avoid doing much
				msgDeleted = 0,
				msgStatus  = 0x44444444,
				msgMakeDlg = 0x55555555
			};

		ScriptMsgType	m_msgtype;

		ScriptExecMsg( ScriptMsgType msgtype );
		virtual ~ScriptExecMsg();

		ScriptMsgType GetType(void);
		DECLARE_DYNAMIC(ScriptExecMsg)
};

/////////////////////////////////////////////////////////////////////////////


#endif // !defined(AFX_SCRIPTEXECMSG_H__9221BBDD_15A7_400A_84B6_FA0B3269318D__INCLUDED_)
