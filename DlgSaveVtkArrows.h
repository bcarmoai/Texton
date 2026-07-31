#if !defined(AFX_DLGSAVEVTKARROWS_H__9EA431D6_53CD_4915_811B_C5F47DDD9356__INCLUDED_)
#define AFX_DLGSAVEVTKARROWS_H__9EA431D6_53CD_4915_811B_C5F47DDD9356__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DlgSaveVtkArrows.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDlgSaveVtkArrows dialog

class CDlgSaveVtkArrows : public CDialog
{
// Construction
public:
	CDialog* pCaller;
	CString mps_imgSize;
	CDlgSaveVtkArrows(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDlgSaveVtkArrows)
	enum { IDD = IDD_VTK_SAVE_ARROWS };
	CString	m_imgSize;
	int		m_coordX;
	int		m_coordY;
	int		m_coordZbS;
	BOOL	m_bWholeData;
	BOOL	m_bS;
	BOOL	m_bX;
	BOOL	m_bY;
	BOOL	m_bHCFD;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgSaveVtkArrows)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDlgSaveVtkArrows)
	virtual void OnOK();
	virtual BOOL OnInitDialog();
	afx_msg void OnCancelMode();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGSAVEVTKARROWS_H__9EA431D6_53CD_4915_811B_C5F47DDD9356__INCLUDED_)
