#if !defined(AFX_DLGVOORHEES_H__6A3A5DB5_F072_4973_9721_1788C959FBEA__INCLUDED_)
#define AFX_DLGVOORHEES_H__6A3A5DB5_F072_4973_9721_1788C959FBEA__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// DlgVoorhees.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDlgVoorhees dialog

class CDlgVoorhees : public CDialog
{
// Construction
public:
	CView* pView;
	CDlgVoorhees(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDlgVoorhees)
	enum { IDD = IDD_VOORHEES };
	float	m_logThres;
	int		m_thres_log_at;
	float	m_gauss_sigma;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgVoorhees)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDlgVoorhees)
	afx_msg void OnLoG();
	afx_msg void OnShowOri();
	afx_msg void OnSave();
	afx_msg void OnThresAt();
	afx_msg void OnGaussBlur();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGVOORHEES_H__6A3A5DB5_F072_4973_9721_1788C959FBEA__INCLUDED_)
