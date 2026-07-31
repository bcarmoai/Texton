// TextonView.h : interface of the CTextonView class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_TEXTONVIEW_H__83124ED9_4682_435B_9EED_4EC9840473D5__INCLUDED_)
#define AFX_TEXTONVIEW_H__83124ED9_4682_435B_9EED_4EC9840473D5__INCLUDED_

#include "ImagePgm.h"	// Added by ClassView
#include "TextonDoc.h"
#include "VoorheesBlobs.h"	// Added by ClassView
#include "VoorheesBlobs.h"
#include "DlgStreamlines.h"
#include "ImageColour.h"
#include "DlgRestoration.h"
#include "DlgFindVortex.h"	// Added by ClassView
//#include "DlgIFTW.h"

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

class CTextonView : public CView
{
protected: // create from serialization only
	CTextonView();
	DECLARE_DYNCREATE(CTextonView)
	CArray<CPoint,CPoint>  mPointArray;   // series of connected points

// Attributes
public:
	CTextonDoc* GetDocument();

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CTextonView)
	public:
	virtual void OnDraw(CDC* pDC);  // overridden to draw this view
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	protected:
	virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
	virtual void OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnEndPrinting(CDC* pDC, CPrintInfo* pInfo);
	//}}AFX_VIRTUAL

// Implementation
public:
//	void DlgIFTWClosed();
//	CDlgIFTW* dlgIFTW;
	CDialog* pVolView;
	CDlgFindVortex* dlgVortexFinder;
	void TasksVortex();
	void RestorationClosed();
	void TasksRestoration();
	CImageColour imgColourTest;
	void DrawColourImageSpecial(CImageColour* image);
	void StreamlinesClosed();
	void TasksStreamlines();
	void TasksNoiseImg();
	void TasksIFTW();
	void DrawBlur(float m_gauss_sigma);
	void DrawThresLoG(int thres);
	void SaveResult();
	void TasksVoorhees();
	void DrawLoG(float thres);
	void DrawResultSpecial(int* image, int width, int height);
	void DrawResult(int* image);
	void DrawRotated(int angle);
	void TasksFourier();
	void DrawArcFitClaimArcs();
	int* img_seedclaims;
	void DrawArcFitSeedClaims();
	void CopyOriginalToCanny();
	void DrawOriginalMaxBin(void (*progress)(char*));
	void DrawArcBins(void (*progress)(char*));
	void DrawArcsStored();
	void DrawCannyStored();
	void DrawArcFit(int min_points, int max_points, float max_error, void (*progress)(char*));
	void TasksArcFit();
	void FitCircle();
	void CannySave();
	void SaveHough();
	void TasksHough();
	void DrawHough(float diam_max, float diam_min, int circles);
	void TemplateMatch();
	void FitEllipse();
	void OnFileImportPgm();
	void ContourImport();
	void DrawContour();
	void ContourClear();
	int iContourState;  // set by CDlgContourOptions; 0-no contour; 1-draw; 2-erase
	void DrawTest();
	BOOL bCannyReady;
	int * edgeImage;
	CImagePgm testImage;
	void DrawCanny(int lowThres, int hiThres, float std);
	BOOL bCannyDrawImage;
	float fCannyStd;
	int iCannyUpper;
	int iCannyLower;
	CDlgStreamlines* dlgStrm;
	void DrawEdges();
	BOOL bThresDrawImage;
	BOOL bCannyMagShade;
	void DrawThresRanges(int iLower, int iUpper);
	void DrawThres();
	void DrawPgm();
	CImagePgm pgmImage;
	virtual ~CTextonView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	CDlgRestoration* dlgRestor;
	CVoorheesBlobs m_voorBlobs;
	int* imgClaimArcs;
	int* imgArcBins;
	int* imgArcs;
	void ContourDeleteBetweenPoints(CPoint pointA, CPoint pointB, CClientDC* pDC);
	void ContourDeletePoint(CPoint point, CClientDC * pDC);
	void ContourAddBetweenPoints(int point, CClientDC* pDC);
	int iThresUpper;
	int iThresLower;
	//{{AFX_MSG(CTextonView)
	afx_msg void OnFileImportpgm();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
	afx_msg void OnCancelMode();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
private:
	CPoint contourOldPoint;
	int displayState; // 1-original, 2-edges, 3-threshold
};

#ifndef _DEBUG  // debug version in TextonView.cpp
inline CTextonDoc* CTextonView::GetDocument()
   { return (CTextonDoc*)m_pDocument; }
#endif

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TEXTONVIEW_H__83124ED9_4682_435B_9EED_4EC9840473D5__INCLUDED_)
