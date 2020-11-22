// stdafx.h : include file for standard system include files,
//  or project specific include files that are used frequently, but
//      are changed infrequently
//

#if !defined(AFX_STDAFX_H__BDE65082_B82F_11D3_BE52_00A0C95A9812__INCLUDED_)
#define AFX_STDAFX_H__BDE65082_B82F_11D3_BE52_00A0C95A9812__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define VC_EXTRALEAN		// Exclude rarely-used stuff from Windows headers

// We are old enough to use strcpy without hurting ourselves, than you very much
#define _CRT_SECURE_NO_WARNINGS

#include <afxwin.h>         // MFC core and standard components
#include <afxext.h>         // MFC extensions
#include <afxdisp.h>        // MFC Automation classes
#include <afxdtctl.h>		// MFC support for Internet Explorer 4 Common Controls
#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>			// MFC support for Windows Common Controls
#endif // _AFX_NO_AFXCMN_SUPPORT

#include <afxsock.h>		// MFC socket extensions
#define _SCB_REPLACE_MINIFRAME
#include "sizecbar.h"
#include "scbarg.h"
#include "scbarcf.h"
#include <afxdlgs.h>

// Replacements for character classification functions that can handle ISO-8859-1 character
// values, which appear as values -128 to -1, since char is signed.
// Microsoft's debug versions of the "normal" versions assert on int values >255 or less than 0.
// The release versions index into a table.  Either way, we need to deal with it by using
// these replacement functions.
// We find experimentally that sscanf, strtoul, atoi, atof and strtok are safe
int IsSpace( int theChar );
int IsDigit( int theChar );
int IsXDigit( int theChar );
int IsAlpha( int theChar );
int IsAlnum( int theChar );
int IsUpper( int theChar );
int IsLower( int theChar );
int ToUpper( int theChar );
int ToLower( int theChar );

// Declare matching prototypes for globals defined in ScriptExecutor...
bool Match( int op, int a, int b );
bool Match( int op, unsigned long a, unsigned long b );
bool Match( int op, float a, float b );
bool Match( int op, double a, double b );
LPCSTR OperatorToString(int iOperator);

// Return true if the value is Not a Number
bool IsNaN( float theValue );
bool IsNaN( double theValue );

// Convert theValue into a string.  Special values are shown as NaN, +inf, and -inf.
// If fullResolution is true, show full float/double resolution, else a shorter default
const char* FloatToString( CString &theString, float theValue, bool fullResolution = false );
const char* DoubleToString( CString &theString, double theValue, bool fullResolution = false );

// Convert string into a floating point number.
// Accepts special values NaN, inf, +inf, and -inf.
// Returns the number of characters (if any) read from the string.
int StringToFloat( const char *pString, float &theValue );
int StringToDouble( const char *pString, double &theValue );

// The maximum size of most character strings, especially those of type ch_string.
// Ideally, these would all use CString and avoid max length, but that would be
// WAY too much work.
#define MAX_TEXT_STRING 256


//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_STDAFX_H__BDE65082_B82F_11D3_BE52_00A0C95A9812__INCLUDED_)
