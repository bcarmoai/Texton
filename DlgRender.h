#if !defined(AFX_DLGRENDER_H__12793412_379A_46D7_B8B0_0C4FEA0DF99A__INCLUDED_)
#define AFX_DLGRENDER_H__12793412_379A_46D7_B8B0_0C4FEA0DF99A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DlgRender.h : header file
//

// contains drawing and resizing code from MSDN's glEasy example by Dale Rogerson
// and mouse interaction by Pierre Alliez at 
// http://www.codeproject.com/opengl/wrl_viewer.asp

/////////////////////////////////////////////////////////////////////////////
// CDlgRender dialog

class CDlgRender : public CDialog
{
// Construction
public:
	afx_msg BOOL OnEraseBkgnd( CDC* pDC);
	void PrepareScene();
	CDlgRender(CWnd* pParent = NULL);   // standard constructor
	BOOL CreateRGBPalette(HDC hDC);
	unsigned char ComponentFromIndex(int i, UINT nbits, UINT shift);
 	static unsigned char   m_oneto8[2];
    static unsigned char   m_twoto8[4];
    static unsigned char   m_threeto8[8];
    static int             m_defaultOverride[13];
    static PALETTEENTRY    m_defaultPalEntry[20];
	CPalette* m_pPal ;		//Palette
	void OutputGlError(char* label);

	//
	// Rotation support
	//
	CSize m_angle[4];
	BOOL m_bRotate ;

	// Mouse 
	BOOL m_LeftButtonDown;
	BOOL m_RightButtonDown;
	CPoint m_LeftDownPos;
	CPoint m_RightDownPos;
	HCURSOR m_CursorRotation;

	// Position, rotation ,scaling
	void InitGeometry(void);

	float m_xRotation;
	float m_yRotation;
	float m_zRotation;
	BOOL m_xyRotation;

	float m_xTranslation;
	float m_yTranslation;
	float m_zTranslation;

	float m_xScaling;
	float m_yScaling;
	float m_zScaling;

	float m_SpeedTranslation;
	float m_SpeedRotation;

	//void Tick() ;  // See GLEasy.cpp
	//void Rotate(BOOL bRotate) ;
	void DrawScene();


	//
	// Multiple object support
	//
	enum enum_OBJECTS {Box=1, Pyramid=2, Dodec=3, Dots=4} ;		
	enum_OBJECTS m_RotatingObject ;
	

// Dialog Data
	//{{AFX_DATA(CDlgRender)
	enum { IDD = IDD_GL_RENDER };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgRender)
	public:
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	//}}AFX_VIRTUAL

// Implementation
protected:
	HGLRC m_hrc;

	// Generated message map functions
	//{{AFX_MSG(CDlgRender)
	afx_msg void OnPaint();
	afx_msg void OnCancelMode();
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	virtual BOOL OnInitDialog();
	afx_msg void OnCaptureChanged(CWnd *pWnd);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGRENDER_H__12793412_379A_46D7_B8B0_0C4FEA0DF99A__INCLUDED_)
