// Texton.h : main header file for the TEXTON application
//

#if !defined(AFX_TEXTON_H__4A2C8652_DF05_4D8F_A26E_F1BCA6ED11F3__INCLUDED_)
#define AFX_TEXTON_H__4A2C8652_DF05_4D8F_A26E_F1BCA6ED11F3__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"       // main symbols

/////////////////////////////////////////////////////////////////////////////
// CTextonApp:
// See Texton.cpp for the implementation of this class
//

class CTextonApp : public CWinApp
{
public:
	CTextonApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CTextonApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementation

	//{{AFX_MSG(CTextonApp)
	afx_msg void OnAppAbout();
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TEXTON_H__4A2C8652_DF05_4D8F_A26E_F1BCA6ED11F3__INCLUDED_)
