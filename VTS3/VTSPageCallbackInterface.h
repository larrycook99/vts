#if !defined(AFX_VTSPAGECALLBACKINTERFACE_H__DBE0B95A_1E36_4827_BC7A_52B9431F6F0E__INCLUDED_)
#define AFX_VTSPAGECALLBACKINTERFACE_H__DBE0B95A_1E36_4827_BC7A_52B9431F6F0E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

//============== Define property page callback interface

// define page callback interface

class VTSPageCallbackInterface
{
	public:
		virtual void DataChangeNotification(void) = 0;
};


//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_VTSPAGECALLBACKINTERFACE_H__DBE0B95A_1E36_4827_BC7A_52B9431F6F0E__INCLUDED_)
