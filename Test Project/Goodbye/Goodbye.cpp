// Goodbye.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "Goodbye.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// The one and only application object

CWinApp theApp;

using namespace std;

int _tmain(int argc, TCHAR* argv[], TCHAR* envp[])
{
	int nRetCode = 0;

	// initialize MFC and print and error on failure
	if (!AfxWinInit(::GetModuleHandle(NULL), NULL, ::GetCommandLine(), 0)) {
		// TODO: change error code to suit your needs
		cerr << _T("Fatal Error: MFC initialization failed") << endl;
		return 1;
	}

	// TODO: code your application's behavior here.
	CString strHello;
	strHello.LoadString(IDS_HELLO);
	cout << (LPCTSTR)strHello << endl;

	cout << endl;
	cout << "Assuming that this works, amazing!" << endl;
	cout << "And it gets even better..." << endl;
        cout << "Until it gets worse." << endl;
	cout << endl;

	return 0;
}


