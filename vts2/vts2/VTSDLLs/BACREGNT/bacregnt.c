/*--------------------------------------------------------------------
module:		BACREGNT.C
desc:		A BACMAC Registration Module for Windows NT
authors:	Jack Neyer
last edit:	02-Dec-99 [002] JN  Expand number of adapters top 8, MAXBINDS to 2
			16-Nov-98 [002] JN  Multiple Adapters
			17-Apr-98 [001] JN  First cut
----------------------------------------------------------------------*/
#include <windows.h>
#include <windowsx.h>
#include "resource.h"

///////////////////////////////////////////////////////////////////////
//	Module Constants
#define		WM_USER_INITIATE	(WM_USER+1)
#define		MAXBINDS			2								//			***003
#define		MAXADAPTERS			8								//			***003

///////////////////////////////////////////////////////////////////////
// Global variables
const char szAppName[]="BACREGNT";
const char version[] = "BACMAC NT Registration Module v0.03";	//			***003 Begin
#define		vstring		"v0.03"
#define		ver100		3									//version*100	***003 End

HWND		hWnd;
char		servicenames[MAXBINDS][128];
char		netcards[]="SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion\\NetworkCards\\";
char		bacmacnt[]="SYSTEM\\CurrentControlSet\\Services\\bacmacnt";
char		bacmacdevice[]="\\Device\\bacmac";					//			***002
char		device[]="\\Device\\";
int			knownAdapters;										//			***002

///////////////////////////////////////////////////////////////////////
//	function prototypes
int pascal WinMain(HANDLE, HANDLE, LPSTR, int);
LRESULT CALLBACK  MainWndProc(HWND, UINT, WPARAM, LPARAM);
////////////////////
boolean DoBind(void);															//	***002
boolean DoUnbind(void);
void CheckBindings(void);														//	***002
void GetAdapterTitle(int,char *,char *);
void ShowAdapters(void);
void ShowError (char *,int);
void StartUp(void);

///////////////////////////////////////////////////////////////////////
//	Main Window Procedure, Message Loop

int pascal WinMain(HANDLE hInstance,HANDLE hPrevInstance,LPSTR lpCmdLine,int nCmdShow)
{   MSG			msg;
	WNDCLASS	wc;

    if (!hPrevInstance)
    {
	    wc.style = CS_HREDRAW | CS_VREDRAW;
	    wc.lpfnWndProc = MainWndProc;
	    wc.cbClsExtra = 0;
//	    wc.cbWndExtra = 0;
    	wc.cbWndExtra = DLGWINDOWEXTRA;
	    wc.hInstance = hInstance;
	    wc.hIcon = LoadIcon(hInstance, "REGICON");
	    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	    wc.hbrBackground = GetStockObject(LTGRAY_BRUSH);
	    wc.lpszMenuName =  NULL;
	    wc.lpszClassName = szAppName;
    	if (!RegisterClass(&wc)) return (FALSE);
    }

//	hWnd=CreateWindow(szAppName,version,
//					WS_OVERLAPPEDWINDOW,
//					CW_USEDEFAULT,CW_USEDEFAULT,
//					CW_USEDEFAULT,CW_USEDEFAULT,
//					NULL,NULL,hInstance,NULL);
	hWnd = CreateDialog(hInstance,"BACREGNT",0,NULL);
    if (!hWnd) return (FALSE);
    SendMessage (hWnd,WM_USER_INITIATE,0,0L);		//get things going
    
    while (GetMessage(&msg, NULL, 0,0))
    {   
		TranslateMessage(&msg);
		DispatchMessage(&msg);
    }
    return (msg.wParam);
}

///////////////////////////////////////////////////////////////////////
//	Main Window Message Processor

LRESULT CALLBACK MainWndProc(HWND hWnd,UINT message,UINT wParam,LONG lParam)
{		
	switch (message)
    {    	 
      case WM_CREATE:
 		return 0;
		
      case WM_COMMAND:
      	switch(wParam)
      	{
		case IDC_OK:														//***002
			if (DoBind())													//***003
				SendMessage (hWnd,WM_DESTROY,0,0L);
			return 0;
 		case IDC_UNDO:														//***002 Begin
			StartUp(); 
			return 0;														//***002 End
      	} //switch wParam
      // case WM_COMMAND
      	break;

	  case WM_USER_INITIATE:
        StartUp(); 
       	return 0;

      case WM_DESTROY:
       	PostQuitMessage(0);
       	return 0;     	  
    }
   	return (DefWindowProc(hWnd, message, wParam, lParam));
}

///////////////////////////////////////////////////////////////////////
//	Start Up Initialization (after initial window display)

void StartUp()
{																	//		***002 Begin	
	SetWindowText(hWnd,version);
	ShowAdapters();
}																	//		***002 End

///////////////////////////////////////////////////////////////////////
//	Show Adapters for binding.

void ShowAdapters(void)												//		*** 002 Begin
{	HKEY	hKey;
	LONG	r;
	DWORD	dwValueType,dwValueLen;
	byte	bValueData[1024];
	char	b[256],*bb;
	int		i,j,bslash='\\';
	
// Build the Adapter List
	knownAdapters=0;
	for (i=0;i<MAXADAPTERS;i++)										//		***003		
	{	GetAdapterTitle(i+1,b,&servicenames[i][0]);					//net card number = 1, 2, 3...
		if ((b[0]==0)&&(servicenames[i][0]==0)) break;
 		SetDlgItemText(hWnd,IDC_ADAPTERCHECK1+i,b);					//display the title
		ShowWindow(GetDlgItem(hWnd,IDC_ADAPTERCHECK1+i),SW_NORMAL);	//make him visible
		CheckDlgButton(hWnd,IDC_ADAPTERCHECK1+i,FALSE);				//uncheck him
		knownAdapters++;
	}
	if (knownAdapters==0)
	{	ShowError("No Adapters are Installed! ",0);
		return;
	}
//	Get the bindings and check the appropriate check boxes.
	strcpy(&b[0],&bacmacnt[0]);
	strcat(&b[0],"\\Linkage");
	r=RegOpenKeyEx(HKEY_LOCAL_MACHINE,b,0,KEY_QUERY_VALUE,&hKey);
	if (r==ERROR_SUCCESS)
	{	RegQueryValueEx(hKey,"Bind",0,&dwValueType,(LPBYTE)bValueData,&dwValueLen);
		bb=(char*)bValueData;
		while ((*bb!=0)&&(i<MAXADAPTERS))					//				***003
		{	bb=strchr(bb,bslash);							//because what we get is "\Device\xxxxxx(0)\device\xxxxxx(0)...(0)(0)"
			if (bb==NULL) break;							//this should never happen
			bb++;											//bb now points to Device\xxxxxx (service name)
			bb=strchr(bb,bslash);							//because what we get is "\xxxxxx(0)\device\xxxxxx(0)...(0)(0)"
			bb++;											//bb now points to xxxxxx (service name)
			for (j=0;j<knownAdapters;j++)
			{	if (strcmp(bb,&servicenames[j][0])==0)
				{	CheckDlgButton(hWnd,IDC_ADAPTERCHECK1+j,TRUE);
					break;
				}
			}
			i++;
			bb+=strlen(bb)+1;
		}
		RegCloseKey(hKey);
	}
	UpdateWindow(hWnd);
 	ShowWindow(hWnd,SW_SHOWNORMAL);
}															//				***002 End											

///////////////////////////////////////////////////////////////////////
//	Get an Adapter Title
//
// in:	nn			has network adapter number to get (1-x)
//		title		pointer to a string to put adapter title
//		sname		pointer to string to put service name
//
// out:	if adapter is not found, then *title=*sname=0

void GetAdapterTitle(int nn,char *title,char *sname)
{	HKEY	hKey;
	LONG	r;
	DWORD	dwValueType,dwValueLen;
	byte	bValueData[1024];
	char	b[128],num[3];
	
	itoa(nn,num,10);												//net card number = 1, 2, 3...
	strcpy(b,netcards);											//net card path in registry
	strcat(b,num);												//append net card number
	r=RegOpenKeyEx(HKEY_LOCAL_MACHINE,b,0,KEY_QUERY_VALUE,&hKey);
	if (r==ERROR_SUCCESS)
	{	dwValueLen=sizeof(bValueData);
		r=RegQueryValueEx(hKey,"ServiceName",0,&dwValueType,(LPBYTE)bValueData,&dwValueLen);	//get service name (this is what gets bound!!)
		strcpy(sname,(char *)bValueData);			// move to buffer for binding
		dwValueLen=sizeof(bValueData);
		r=RegQueryValueEx(hKey,"Title",0,&dwValueType,(LPBYTE)bValueData,&dwValueLen);	//get title for humans
		strcpy(title,(char *)bValueData);
		strcat(title," (");											//let's make it title(servicename)
		strcat(title,sname);
		strcat(title,")");
		r=RegCloseKey(hKey);
	}
	else
	{	*title=0;
		*sname=0;
	}
}

///////////////////////////////////////////////////////////////////////
//	Do the Bindings
//
// out:	true	if success

boolean DoBind(void)
{	char	b[1024]={0};									//								***002 Begin
	char	*bb=&b[0];
	char	e[1024]={0};
	char	*ee=&e[0];										//								***002 End
	long	r;
	ULONG	disposition;
	HKEY	hKey,hSubKey;
	int		i,n=0;											//								***002
	boolean	success=TRUE;									//assume success

	DoUnbind();												//start with a clean slate		***002 Begin
	if (knownAdapters==0) return success;					//we never had any adapters installed
	for (i=0;i<knownAdapters;i++)							//count the number checked (ie. bound)
	{	if (IsDlgButtonChecked(hWnd,IDC_ADAPTERCHECK1+i))
		{	n++;
			strcat(bb,device);
			strcat(bb,&servicenames[i][0]);
			bb+=strlen(bb)+1;
			*bb=0;											//this is a REG_MULTI_SZ string
			strcat(ee,bacmacdevice);
			ee+=strlen(ee);
			*ee++=(char)(n+0x30);
			*ee++=0;
			*ee=0;											//this is a REG_MULTI_SZ string
		}
	}
	if (n==0) return success;								//we've unbound everything		***002 End
	if (n>MAXBINDS)											//								***003 Begin
	{	SetDlgItemText(hWnd,IDC_ERROR,"Error: Maximum Number of Bindings (2) Exceeded!");
		return FALSE;
	}														//								***003 End
	r=RegCreateKeyEx(HKEY_LOCAL_MACHINE,bacmacnt,0,NULL,REG_OPTION_NON_VOLATILE,KEY_ALL_ACCESS,NULL,&hKey,&disposition);
	if (r!=ERROR_SUCCESS) 
	{	ShowError("Error Creating BACMAC Registry Key: ",(int)r);
		return FALSE;
	}
	r=RegCreateKeyEx(hKey,"Parameters",0,NULL,REG_OPTION_NON_VOLATILE,KEY_ALL_ACCESS,NULL,&hSubKey,&disposition);
	if (r!=ERROR_SUCCESS) 
	{	ShowError("Error Creating Parameters Registry SubKey: ",(int)r);
		success=FALSE;
		goto donex;
	}
	r=RegCloseKey(hSubKey);
	r=RegCreateKeyEx(hKey,"Linkage",0,NULL,REG_OPTION_NON_VOLATILE,KEY_ALL_ACCESS,NULL,&hSubKey,&disposition);
	if (r!=ERROR_SUCCESS) 
	{	ShowError("Error Creating Linkage Registry SubKey: ",(int)r);
		success=FALSE;
		goto donex;
	}
	n=bb-b+1;														//		***002												
	r=RegSetValueEx(hSubKey,"Bind",0,REG_MULTI_SZ,(LPBYTE)b,n);
	if (r!=ERROR_SUCCESS)
	{	ShowError("Error Creating Linkage\\Bind Value: ",(int)r);
		success=FALSE;
		goto done;
	}
	n=ee-e+1;														//		***002
	r=RegSetValueEx(hSubKey,"Export",0,REG_MULTI_SZ,(LPBYTE)e,n);	//		***002
	if (r!=ERROR_SUCCESS)
	{	ShowError("Error Creating Linkage\\Export Value: ",(int)r);
		success=FALSE;
		goto done;
	}
	n=1;
	r=RegSetValueEx(hKey,"ErrorControl",0,REG_DWORD,(const BYTE*)&n,sizeof(n));
	if (r!=ERROR_SUCCESS)
	{	ShowError("Error Creating ErrorControl Value: ",(int)r);
		success=FALSE;
		goto done;
	}
	n=2;													//AutoStart
	r=RegSetValueEx(hKey,"Start",0,REG_DWORD,(const BYTE*)&n,sizeof(n));
	if (r!=ERROR_SUCCESS)
	{	ShowError("Error Creating Start Value: ",(int)r);
		success=FALSE;
		goto done;
	}
	n=1;
	r=RegSetValueEx(hKey,"Type",0,REG_DWORD,(const BYTE*)&n,sizeof(n));
	if (r!=ERROR_SUCCESS)
	{	ShowError("Error Creating Type Value: ",(int)r);
		success=FALSE;
		goto done;
	}
done:
	r=RegCloseKey(hSubKey);
donex:
	r=RegCloseKey(hKey);
	return success;
}

///////////////////////////////////////////////////////////////////////
//	Do the Unbinding
//
//  We can never get here if nothing is bound. Also we know to whom we're bound 
//  since currently we allow only one binding.
//
// out:	true	if success 

boolean DoUnbind(void)
{	char	subkey[1024];
	long	r;
	HKEY	hKey;
	int		i;
	boolean	success=TRUE;

/*	strcpy(subkey,bacmacnt);							//use bacmacnt key
	strcat(subkey,linkage);
	r=RegDeleteKey(HKEY_LOCAL_MACHINE,subkey);			//remove Linkage subkey and all its values
	strcpy(subkey,bacmacnt);
	strcat(subkey,parameters);
	r=RegDeleteKey(HKEY_LOCAL_MACHINE,subkey);			//remove Parameters subkey and all its values
	r=RegDeleteKey(HKEY_LOCAL_MACHINE,subbacmacnt);		//remove bacmacnt and all its values
*/
	r=RegOpenKeyEx(HKEY_LOCAL_MACHINE,bacmacnt,0,KEY_ALL_ACCESS,&hKey);
	if (r!=ERROR_SUCCESS) return FALSE;
	i=0;												//start at first index
	while (1)
	{	r=RegEnumKey(hKey,i,subkey,sizeof(subkey));
		if (r==ERROR_NO_MORE_ITEMS) break;				//we found all subkeys
		if (r!=ERROR_SUCCESS)
		{	success=FALSE;
			break;										//oops
		}
		r=RegDeleteKey(hKey,subkey);
//		i++;											//next index (as keys are deleted, their indices bet adjusted so the next to delete is always 0)
	}
	r=RegDeleteKey(HKEY_LOCAL_MACHINE,bacmacnt);		//remove bacmacnt and all its values (subkeys werer already removed)
	r=RegCloseKey(hKey);
	return success;
}


///////////////////////////////////////////////////////////////////////
//  Display an error Message
//
//	in:	etxt	The error text
//		errnum	the error number for FormatMessage

void ShowError (char *errtext,int errnum)
{	char	etxt[1024],b[1024];

	strcpy(b,errtext);
	FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,NULL,
		GetLastError(),MAKELANGID(LANG_NEUTRAL,SUBLANG_DEFAULT), // Default language
		(LPTSTR) etxt,0,NULL);
	strcat(b,etxt);
	SetDlgItemText(hWnd,IDC_ERROR,b);
}


