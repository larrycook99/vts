// EPICSViewNode.h: interface for the CEPICSViewNode class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_EPICSVIEWNODE_H__368688EE_2735_4BD5_AEE5_91CDC77F77B6__INCLUDED_)
#define AFX_EPICSVIEWNODE_H__368688EE_2735_4BD5_AEE5_91CDC77F77B6__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CEPICSTreeView;
class CEPICSViewInfoPanel;

class CEPICSViewNode : public CObject
{
protected:
	CEPICSTreeView * m_ptreeview;

	CEPICSViewNode(){};
	DECLARE_DYNCREATE(CEPICSViewNode)


public:
	CEPICSViewNode( CEPICSTreeView * ptreeview );
	virtual ~CEPICSViewNode();

	virtual void LoadInfoPanel();
};



class CEPICSViewNodeRoot : public CEPICSViewNode
{
private:
	int m_nSyntaxE;
	int m_nConE;

	void LoadErrorFile( CEPICSViewInfoPanel * ppanel );


public:
	CEPICSViewNodeRoot( CEPICSTreeView * ptreeview, int nSyntaxErrors, int nConErrors );
	virtual ~CEPICSViewNodeRoot();

	virtual void LoadInfoPanel();
};



class CEPICSViewNodeAppService : public CEPICSViewNode
{
private:


public:
	CEPICSViewNodeAppService( CEPICSTreeView * ptreeview);
	virtual ~CEPICSViewNodeAppService();

	virtual void LoadInfoPanel();
};



class CEPICSViewNodeBIBB : public CEPICSViewNode
{
private:


public:
	CEPICSViewNodeBIBB( CEPICSTreeView * ptreeview);
	virtual ~CEPICSViewNodeBIBB();

	virtual void LoadInfoPanel();
};



class CEPICSViewNodeObjTypes : public CEPICSViewNode
{
private:


public:
	CEPICSViewNodeObjTypes( CEPICSTreeView * ptreeview);
	virtual ~CEPICSViewNodeObjTypes();

	virtual void LoadInfoPanel();
};


class CEPICSViewNodeDataLink : public CEPICSViewNode
{
private:


public:
	CEPICSViewNodeDataLink( CEPICSTreeView * ptreeview);
	virtual ~CEPICSViewNodeDataLink();

	virtual void LoadInfoPanel();
};


class CEPICSViewNodeObject : public CEPICSViewNode
{
private:
	void far * m_pObj;
	CStringArray m_astrDevValues;


protected:
	CEPICSViewNodeObject() {};
	DECLARE_DYNCREATE(CEPICSViewNodeObject)

public:
	CEPICSViewNodeObject( CEPICSTreeView * ptreeview, void far * pObj );
	virtual ~CEPICSViewNodeObject();

	virtual void LoadInfoPanel();
	void SetDeviceValue( int iIndex, LPCSTR lpszValue );
	void ClearDeviceValues(void);
	unsigned long GetObjectID(void);
	int GetPropertyIndex( DWORD dwPropID, unsigned short * pParseType );
	unsigned long GetPropertyID( int nLocalIndex );
	unsigned long GetPropertyIDAndValue( int nLocalIndex, BACnetAnyValue * pany );
	BOOL HasDeviceValues(void);
};


class CEPICSViewNodeCharsets : public CEPICSViewNode
{
private:


public:
	CEPICSViewNodeCharsets( CEPICSTreeView * ptreeview);
	virtual ~CEPICSViewNodeCharsets();

	virtual void LoadInfoPanel();
};

class CEPICSViewNodeSpecialFunctionality : public CEPICSViewNode
{
private:


public:
	CEPICSViewNodeSpecialFunctionality( CEPICSTreeView * ptreeview);
	virtual ~CEPICSViewNodeSpecialFunctionality();

	virtual void LoadInfoPanel();
};


#endif // !defined(AFX_EPICSVIEWNODE_H__368688EE_2735_4BD5_AEE5_91CDC77F77B6__INCLUDED_)
