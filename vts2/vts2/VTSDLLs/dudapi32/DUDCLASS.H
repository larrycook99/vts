// DUDCLASS.H


#ifndef __DUDCLASS_H_INCLUDED
#define __DUDCLASS_H_INCLUDED



// derived class from CWinApp ==================================================================================

class CDudapiDLL : public CWinApp
{
public:
	virtual BOOL InitInstance(); // Initialization
	virtual int ExitInstance();  // Termination (WEP-like code)

	CDudapiDLL(const char* pszAppName);
};



// derived classes from CDialog ================================================================================


// modeless dialog box class
// This class provides special functionality for modeless dialog boxes. Only one object/dialog box
// derived from this class can exist at the same time.
class CModelessDlg: public CDialog
{ 
  public:
  
    static CModelessDlg* pCurrent; // points to current CModelessDlg
  
    CModelessDlg( LPCSTR lpszTemplateName, CWnd* pParentWnd = NULL );
    CModelessDlg( UINT nIDTemplate, CWnd* pParentWnd = NULL );
    
    virtual void OnOk();
    virtual void OnCancel();
    virtual void PostNcDestroy();
    DECLARE_MESSAGE_MAP()
};


class CBACnetDateTimeDlg: public CModelessDlg
{ 
  private:
    BACnetDateTime far* mDT;  
  public:
    CBACnetDateTimeDlg(BACnetDateTime far* dt, CWnd* pParentWnd= NULL);
    virtual BOOL OnInitDialog();
    virtual void OnCancel();
    virtual void OnOk();
};




// application object is defined in DUDCLASS.CPP
extern CDudapiDLL DudapiDLL;




#endif // __DUDAPI_H_INCLUDED
