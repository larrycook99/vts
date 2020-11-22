/*	23-Mar-98 [001]  JN  32-bit version (not all marked) 
						 eg. far pascal __export --> APIENTRY
*/

#include "stdafx.h"									//			***001 Begin
//#include <afx.h>			
//#include <afxwin.h>								//			***001 End

#include "db.h"
#include "resource.h"   // resource identifier 
#include "dudclass.h" // class declarations

CDudapiDLL DudapiDLL("dudapi32.dll");          // application object, you need it.	***001
CModelessDlg* CModelessDlg::pCurrent= NULL;  // points to current CModelessDlg



// =============================================================================================================
//
// class CDudapiDLL
//
// =============================================================================================================


// nothing special for the constructor
CDudapiDLL::CDudapiDLL(const char* pszAppName) : CWinApp(pszAppName)
{ }


BOOL CDudapiDLL::InitInstance()
{
	// any DLL initialization goes here
	return TRUE;
}

int CDudapiDLL::ExitInstance()
{
	// any DLL termination goes here (WEP-like code)
	return CWinApp::ExitInstance();
}

       

// =============================================================================================================
//
// class CModelessDialog
//
// =============================================================================================================


CModelessDlg::CModelessDlg( LPCSTR lpszTemplateName, CWnd* pParentWnd)
 : CDialog()
{ 
  if (pCurrent) pCurrent->DestroyWindow();
  pCurrent= this;

}

CModelessDlg::CModelessDlg( UINT nIDTemplate, CWnd* pParentWnd)
 : CDialog()
{
  if (pCurrent) pCurrent->DestroyWindow();
  pCurrent= this;
}
                   

void CModelessDlg::OnOk()
{
  DestroyWindow();
}

void CModelessDlg::OnCancel()
{
  DestroyWindow();
}

void CModelessDlg::PostNcDestroy( )
{
  delete this;    // delete the attached dynamic object
  pCurrent= NULL; // no modeless dialog opened
}


BEGIN_MESSAGE_MAP(CModelessDlg,CDialog) 
  ON_COMMAND( IDOK, OnOk )
  ON_COMMAND( IDCANCEL, OnCancel )
END_MESSAGE_MAP()



// ===========================================================================================================
//
// modeless dialog box classes
//
// ===========================================================================================================



// class CBACnetDateTimeDlg ==================================================================================


CBACnetDateTimeDlg::CBACnetDateTimeDlg(BACnetDateTime far* dt, CWnd* pParentWnd)
  : CModelessDlg(IDD_DATETIME,pParentWnd) 
{ 
  mDT= dt;
  Create(IDD_DATETIME, pParentWnd); // create window in this object ( WM_INITDIALOG will be send )
}  

 
BOOL CBACnetDateTimeDlg::OnInitDialog()
{ 
  CModelessDlg::OnInitDialog();
  // read data ---
  char dt[10];
  if (mDT->date.month==dontcare) strcpy(dt,"?");
  else itoa(mDT->date.month,dt,10);          
  SetDlgItemText(IDC_MONTH,dt);
  if (mDT->date.day_of_month==dontcare) strcpy(dt,"?");
  else itoa(mDT->date.day_of_month,dt,10);   
  SetDlgItemText(IDC_DAY,dt);
  if (mDT->date.day_of_week==dontcare) strcpy(dt,"?");
  else itoa(mDT->date.day_of_week,dt,10);    
  SetDlgItemText(IDC_WEEK,dt);
  if (mDT->date.year==dontcare) strcpy(dt,"?");
  else itoa(mDT->date.year,dt,10);           
  SetDlgItemText(IDC_YEAR,dt);
  if (mDT->time.hour==dontcare) strcpy(dt,"?");
  else itoa(mDT->time.hour,dt,10);           
  SetDlgItemText(IDC_HOUR,dt);
  if (mDT->time.minute==dontcare) strcpy(dt,"?");
  else itoa(mDT->time.minute,dt,10);         
  SetDlgItemText(IDC_MINUTE,dt);
  if (mDT->time.second==dontcare) strcpy(dt,"?");
  else itoa(mDT->time.second,dt,10);         
  SetDlgItemText(IDC_SECOND,dt);
  if (mDT->time.hundredths==dontcare) strcpy(dt,"?");
  else itoa(mDT->time.hundredths,dt,10);     
  SetDlgItemText(IDC_HUNDREDTHS,dt);
  return(TRUE);
}



void CBACnetDateTimeDlg::OnOk()
{
  // write back ---
  char dt[10]; int c1, c2;
  GetDlgItemText(IDC_MONTH,dt,10);
  c1= atoi(dt); (c1==0)?(c2= dontcare):(c2= c1);
  mDT->date.month= c2;  
  GetDlgItemText(IDC_DAY,dt,10);
  c1= atoi(dt); (c1==0)?(c2= dontcare):(c2= c1);
  mDT->date.day_of_month= c2;
  GetDlgItemText(IDC_WEEK,dt,10);
  c1= atoi(dt); (c1==0)?(c2= dontcare):(c2= c1);
  mDT->date.day_of_week= c2;
  GetDlgItemText(IDC_YEAR,dt,10);
  c1= atoi(dt); (c1==0)?(c2= dontcare):(c2= c1);
  mDT->date.year= c2;
  GetDlgItemText(IDC_HOUR,dt,10);
  c1= atoi(dt); (c1==0)?(c2= dontcare):(c2= c1);
  mDT->time.hour= c2;
  GetDlgItemText(IDC_MINUTE,dt,10);
  c1= atoi(dt); (c1==0)?(c2= dontcare):(c2= c1);
  mDT->time.minute= c2;
  GetDlgItemText(IDC_SECOND,dt,10);
  c1= atoi(dt); (c1==0)?(c2= dontcare):(c2= c1);
  mDT->time.second= c2;
  GetDlgItemText(IDC_HUNDREDTHS,dt,10);
  c1= atoi(dt); (c1==0)?(c2= dontcare):(c2= c1);
  mDT->time.hundredths= c2;
  
  CModelessDlg::OnOk(); // pass IDOK-Command to CModelessDlg
}

void CBACnetDateTimeDlg::OnCancel()
{ 
  CModelessDlg::OnCancel(); // pass IDCANCEL-Command to CModelessDlg
}
