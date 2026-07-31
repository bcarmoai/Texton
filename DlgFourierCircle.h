#if !defined(AFX_DLGFOURIERCIRCLE_H__EB5AF7B0_0D73_4C4E_AC58_05E4A93C11F5__INCLUDED_)
#define AFX_DLGFOURIERCIRCLE_H__EB5AF7B0_0D73_4C4E_AC58_05E4A93C11F5__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// DlgFourierCircle.h : header file
//

#include "FourierCircle.h"	// Added by ClassView

/////////////////////////////////////////////////////////////////////////////
// CDlgFourierCircle dialog

class CDlgFourierCircle : public CDialog
{
// Construction
public:
	CView* pView;
	CDlgFourierCircle(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDlgFourierCircle)
	enum { IDD = IDD_FOURIER };
	int		m_rotateAngle;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgFourierCircle)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	int imgHeight;
	int imgWidth;
//	CFourierCircle fftWorker;

	// Generated message map functions
	//{{AFX_MSG(CDlgFourierCircle)
	afx_msg void OnRotate();
	afx_msg void OnFftCanny();
	afx_msg void OnFftCannycopy();
	afx_msg void OnFftFft();
	afx_msg void OnFftRotate();
	afx_msg void OnFftInverse();
	afx_msg void OnFftFftrot();
	afx_msg void OnFftRotinverse();
	afx_msg void OnFftCombine();
	afx_msg void OnFftCominv();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGFOURIERCIRCLE_H__EB5AF7B0_0D73_4C4E_AC58_05E4A93C11F5__INCLUDED_)
