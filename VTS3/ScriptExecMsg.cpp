// ScriptExecMsg.cpp : implementation file
//

#include "stdafx.h"
#include "vts.h"
#include "ScriptExecMsg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// ScriptExecMsg
IMPLEMENT_DYNAMIC(ScriptExecMsg, CObject)

ScriptExecMsg::ScriptExecMsg(ScriptMsgType msgtype)
{
	m_msgtype = msgtype;
}

ScriptExecMsg::~ScriptExecMsg()
{
	// Mark as deleted, in case some naughty boy accessess
	// us after death...
	m_msgtype = msgDeleted;
}


ScriptExecMsg::ScriptMsgType ScriptExecMsg::GetType()
{
	return m_msgtype;
}



/////////////////////////////////////////////////////////////////////////////
// ScriptExecMsg message handlers
