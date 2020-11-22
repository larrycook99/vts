// ScriptDocument.h : interface of the ScriptDocument class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_SCRIPTDOCUMENT_H__C9E6602D_ABC2_11D4_BEE8_00A0C95A9812__INCLUDED_)
#define AFX_SCRIPTDOCUMENT_H__C9E6602D_ABC2_11D4_BEE8_00A0C95A9812__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "ScriptBase.h"
#include "ScriptParmList.h"
#include "ScriptContentTree.h"
#include "ScriptPacket.h"
#include "ScriptIfdefHandler.h"

class ScriptExecutor;
class ScriptIfdefHandler;

typedef ScriptExecutor *ScriptExecutorPtr;

class ScriptDocument : public CDocument
{
protected: // create from serialization only
   ScriptDocument();
   DECLARE_DYNCREATE(ScriptDocument)
   virtual ~ScriptDocument();

// Attributes
public:

// Operations
public:

// Overrides
   // ClassWizard generated virtual function overrides
   //{{AFX_VIRTUAL(ScriptDocument)
public:
   virtual BOOL OnNewDocument();
   virtual void OnCloseDocument();
   virtual void Serialize(CArchive& ar);
   //}}AFX_VIRTUAL

// Implementation
public:
   CEdit*               m_editData;
   ScriptParmList*      m_pParmList;
   ScriptContentTree*   m_pContentTree;
   unsigned char        m_digest[16];        // checksum calculated during CheckSyntax

protected:
   // Made these protected to track down use of dead objects.
   ScriptTestPtr        m_pSelectedTest;
   ScriptSectionPtr     m_pSelectedSection;
   bool                 m_bExecBound;
public:

   ScriptTestPtr SelectedTest();
   void SetSelectedTest( ScriptTestPtr pTest );

   ScriptSectionPtr SelectedSection();
   void SetSelectedSection( ScriptSectionPtr pSection );

   BOOL CheckSyntax( void );
   void ParsePacket( ScriptIfdefHandler &ifdefHandler, ScriptScanner& scan, ScriptToken& tok, ScriptPacketPtr spp, bool isSend );
   void SequenceTest( ScriptTestPtr stp );
// ScriptPacketPtr SequenceLevel( ScriptBasePtr sbp, ScriptPacketPtr pPass, ScriptPacketPtr &pChain );
// ScriptPacketPtr SequencePacket( ScriptPacketPtr spp, ScriptPacketPtr pPass );
   ScriptCommandPtr SequenceLevel( ScriptBasePtr sbp, ScriptCommandPtr pPass, ScriptCommandPtr &pChain );
   ScriptCommandPtr SequencePacket( ScriptCommandPtr spp, ScriptCommandPtr pPass );
   void CalcDigest( void );

   void BindExecutor( void );
   void UnbindExecutor( void );

   void Reset( void );

   void SetImageStatus( ScriptBasePtr sbp, int stat );      // change the section/test/deps/packet status

#ifdef _DEBUG
   virtual void AssertValid() const;
   virtual void Dump(CDumpContext& dc) const;
#endif

protected:

   bool EvaluateConditional( ScriptScanner & scan, ScriptToken & tok );
   CStdioFile * OpenIncludeFile(LPCSTR lpszBasePath, CString * pstrFile);

// Generated message map functions
protected:
   //{{AFX_MSG(ScriptDocument)
      // NOTE - the ClassWizard will add and remove member functions here.
      //    DO NOT EDIT what you see in these blocks of generated code !
   //}}AFX_MSG
   DECLARE_MESSAGE_MAP()
};

typedef ScriptDocument *ScriptDocumentPtr;

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SCRIPTDOCUMENT_H__C9E6602D_ABC2_11D4_BEE8_00A0C95A9812__INCLUDED_)
