#if !defined(AFX_DLGVOLUMEDICOM_H__9DFD541D_1FCF_4206_9C2D_B0449B7D76BC__INCLUDED_)
#define AFX_DLGVOLUMEDICOM_H__9DFD541D_1FCF_4206_9C2D_B0449B7D76BC__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DlgVolumeDicom.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDlgVolumeDicom dialog

class CDlgVolumeDicom : public CDialog
{
// Construction
public:
	void SaveDicomFile(CString filename);
	int img_slices;
	int img_height;
	int img_width;
	CDialog* pFindVortex;
	CDlgVolumeDicom(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDlgVolumeDicom)
	enum { IDD = IDD_FINDVORTEX_DICOM };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgVolumeDicom)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	void NormaliseFloat16bit(float * in_out, int width, int height, int slices);
	int FloatToInt(float fnum);
	// Generated message map functions
	//{{AFX_MSG(CDlgVolumeDicom)
	afx_msg void OnDicomSave();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGVOLUMEDICOM_H__9DFD541D_1FCF_4206_9C2D_B0449B7D76BC__INCLUDED_)
