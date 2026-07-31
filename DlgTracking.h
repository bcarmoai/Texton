#if !defined(AFX_DLGTRACKING_H__685FE302_4740_4C31_9D7E_BED5BA53B4C3__INCLUDED_)
#define AFX_DLGTRACKING_H__685FE302_4740_4C31_9D7E_BED5BA53B4C3__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DlgTracking.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDlgTracking dialog

class CDlgTracking : public CDialog
{
// Construction
public:
	CDlgTracking(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDlgTracking)
	enum { IDD = IDD_VTX_TRACKING };
	double	m_denseMean;
	double	m_denseScaling;
	float	m_denseSigma;
	CString	m_trackLoaded;
	int		m_trackSpaceRad;
	int		m_trackTimeRad;
	CString	m_trackTracked;
	CString	m_denseResX;
	CString	m_denseresY;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgTracking)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	float PointDistance2D(int x1, int y1, int x2, int y2);

	// Generated message map functions
	//{{AFX_MSG(CDlgTracking)
	afx_msg void OnCondensationGo();
	afx_msg void OnFindvortexFlowvisTracking();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGTRACKING_H__685FE302_4740_4C31_9D7E_BED5BA53B4C3__INCLUDED_)
