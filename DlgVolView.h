#if !defined(AFX_DLGVOLVIEW_H__2CA9F23A_CA79_4C34_8227_8A88E1368DF6__INCLUDED_)
#define AFX_DLGVOLVIEW_H__2CA9F23A_CA79_4C34_8227_8A88E1368DF6__INCLUDED_

#include "Data3DMRI.h"	// Added by ClassView
#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DlgVolView.h : header file
//

#include "ImageColour.h"

/////////////////////////////////////////////////////////////////////////////
// CDlgVolView dialog

class CDlgVolView : public CDialog
{
// Construction
public:
	void GetCurrentPlane(char &cPlane, int& zero_based);
	void GetMouseProjection(int &x, int&y, int& s, CPoint point);
	void TakeMovePoint(CPoint point);
	BOOL bTakeMouse;
	void InitialiseVolume(CImageColour& init_vol, int width, int height, int slices);
	void InitialiseVolume(int* init_vol, int width, int height, int slices);
	void InitialiseVolume(float* init_vol, int width, int height, int slices);
	void InitialiseVolume(short* init_vol, int width, int height, int slices);
	CView* pView;
	CDlgVolView(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDlgVolView)
	enum { IDD = IDD_VOL_VIEW };
	int		m_SelS;
	int		m_SelX;
	int		m_SelY;
	CString	m_imgInfo;
	CString	m_strVolValue;
	CString	m_sVolumeTitle;
	float	m_intMult;
	CString	m_minMaxText;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgVolView)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	int iCurrentPlane;
	int iCurrentS;
	int iCurrentY;
	int iCurrentX;
	float* m_floatRawVolData;
	float* m_floatNormVolData;
	int* m_intRawVolData;
	int* m_intNormVolData;
	void NormaliseFloatVolume();
	int FloatToInt(float fnum);
	int img_width;
	int img_height;
	int img_slices;
	void NormaliseVolume();
	CImageColour* m_colourVolData;

	// Generated message map functions
	//{{AFX_MSG(CDlgVolView)
	afx_msg void OnSMinus();
	afx_msg void OnSPlus();
	afx_msg void OnXMinus();
	afx_msg void OnXPlus();
	afx_msg void OnYMinus();
	afx_msg void OnYPlus();
	virtual void OnCancel();
	afx_msg void OnVolviewSelectWindow();
	afx_msg void OnVolviewSave();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGVOLVIEW_H__2CA9F23A_CA79_4C34_8227_8A88E1368DF6__INCLUDED_)
