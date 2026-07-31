// TextonView.cpp : implementation of the CTextonView class
//

#include "stdafx.h"
#include "Texton.h"

#include "TextonDoc.h"
#include "TextonView.h"
#include "ThresDlg.h"
#include "CannyDlg.h"
#include "CannyDetector.h"
#include <math.h>
//#include <ipl.h>
//#include <cv.h>
#include "Image.h"
//#include "OpencvContour.h"
//#include "cookdraw.h"
//#include "cookroom.h"
#include "image.h"
//#include "DlgTemplateMatch.h"
#include "DlgHough.h"
#include "HoughPlotter.h"
#include "DlgArcFit.h"
#include "ArcFit.h"
//#include "DlgFourierCircle.h"
#include "DlgVoorhees.h"
//#include "DlgIFTW.h"
#include "DlgNoiseImage.h"
#include "DlgStreamlines.h"
#include "MainFrm.h"
#include "DlgFindVortex.h"
#include "DlgVolView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CTextonView

IMPLEMENT_DYNCREATE(CTextonView, CView)

BEGIN_MESSAGE_MAP(CTextonView, CView)
	//{{AFX_MSG_MAP(CTextonView)
	ON_COMMAND(ID_FILE_IMPORTPGM, OnFileImportpgm)
	ON_WM_LBUTTONDOWN()
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONUP()
	ON_WM_SETCURSOR()
	ON_WM_CANCELMODE()
	//}}AFX_MSG_MAP
	// Standard printing commands
	ON_COMMAND(ID_FILE_PRINT, CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, CView::OnFilePrintPreview)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTextonView construction/destruction

CTextonView::CTextonView()
{
	displayState = 1;
	iThresUpper = 0;
	iThresLower = 0;
	bThresDrawImage = FALSE;
	iCannyUpper = 255;
	iCannyLower = 1;
	fCannyStd = 1.0;
	bCannyDrawImage = FALSE;
	bCannyReady = FALSE;
	edgeImage = NULL;
	iContourState = 0;
	bCannyMagShade = TRUE;
	imgArcs = NULL;
	imgArcBins = NULL;
	img_seedclaims = NULL;
	imgClaimArcs = NULL;
	dlgStrm = NULL;
	dlgRestor = NULL;
	dlgVortexFinder = NULL;
	pVolView = NULL;
//	dlgIFTW = NULL;
}

CTextonView::~CTextonView()
{

}

BOOL CTextonView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	return CView::PreCreateWindow(cs);
}

/////////////////////////////////////////////////////////////////////////////
// CTextonView drawing

void CTextonView::OnDraw(CDC* pDC)
{
	CTextonDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);

	switch(displayState)
	{
	case 1:
		{
		// draw the pgm
		int x, y;		
		for(x=0; x<pgmImage.imgWidth; x++)
			for(y=0; y<pgmImage.imgHeight; y++)
			{
				pDC->SetPixel(x,y,RGB(pgmImage.imgData[pgmImage.imgWidth*y+x],
					pgmImage.imgData[pgmImage.imgWidth*y+x],
					pgmImage.imgData[pgmImage.imgWidth*y+x]));
			}
		}
		break;
	case 2:
		// draw edge detected image
		{
			if(bCannyReady == FALSE)
				return;

			int x, y;		
			for(x=0; x<pgmImage.imgWidth; x++)
				for(y=0; y<pgmImage.imgHeight; y++)
				{
					if(edgeImage[pgmImage.imgWidth*y+x]>0)
						if(!bCannyMagShade)
							pDC->SetPixel(x,y,RGB(255,0,0));
						else
							pDC->SetPixel(x,y,RGB(pgmImage.imgData[pgmImage.imgWidth*y+x],0,0));
					else
						if(bCannyDrawImage)
							pDC->SetPixel(x,y,RGB(pgmImage.imgData[pgmImage.imgWidth*y+x],
							pgmImage.imgData[pgmImage.imgWidth*y+x],
							pgmImage.imgData[pgmImage.imgWidth*y+x]));
						
				}			
		}
		break;
	case 3:
		// draw thresholded image
		{
			int x, y;		
			for(x=0; x<pgmImage.imgWidth; x++)
				for(y=0; y<pgmImage.imgHeight; y++)
				{
					if(pgmImage.imgData[pgmImage.imgWidth*y+x]>=iThresLower &&
						pgmImage.imgData[pgmImage.imgWidth*y+x]<=iThresUpper)
						pDC->SetPixel(x,y,RGB(255,0,0));
					else
						if(bThresDrawImage)
							pDC->SetPixel(x,y,RGB(pgmImage.imgData[pgmImage.imgWidth*y+x],
							pgmImage.imgData[pgmImage.imgWidth*y+x],
							pgmImage.imgData[pgmImage.imgWidth*y+x]));
						
				}
		}
		break;
	case 4:
		// draw test image
		{
		int x, y;		
		for(x=0; x<testImage.imgWidth; x++)
			for(y=0; y<testImage.imgHeight; y++)
			{
				if(testImage.imgData[testImage.imgWidth*y+x]<=255)
				pDC->SetPixel(x,y,RGB(testImage.imgData[testImage.imgWidth*y+x],
					testImage.imgData[testImage.imgWidth*y+x],
					testImage.imgData[testImage.imgWidth*y+x]));
				else
					if(testImage.imgData[testImage.imgWidth*y+x]!=455) // magic number!!!
						pDC->SetPixel(x,y,RGB(255,0,0));
					else
						pDC->SetPixel(x,y,RGB(0,255,0));

			}
		}
		break;
	case 5:
		// draw contour image
		{
			// draw the pgm
			int x, y;		
			for(x=0; x<pgmImage.imgWidth; x++)
				for(y=0; y<pgmImage.imgHeight; y++)
				{
					pDC->SetPixel(x,y,RGB(pgmImage.imgData[pgmImage.imgWidth*y+x],
						pgmImage.imgData[pgmImage.imgWidth*y+x],
						pgmImage.imgData[pgmImage.imgWidth*y+x]));
				}

			// draw the contour
			int i;
			for(i=0; i <= mPointArray.GetUpperBound(); i++)
				pDC->SetPixel(mPointArray.GetAt(i),RGB(0, 0, 255));

		}
		break;
	case 6:
		// draw colour test
		{
			int x, y;		
			for(x=0; x<imgColourTest.imgWidth; x++)
				for(y=0; y<imgColourTest.imgHeight; y++)
				{
					pDC->SetPixel(x,y,RGB(imgColourTest.imgData[imgColourTest.imgWidth*y+x].RED,
						imgColourTest.imgData[imgColourTest.imgWidth*y+x].GREEN,
						imgColourTest.imgData[imgColourTest.imgWidth*y+x].BLUE));
				}
		}
		break;
	default:
		break;

	}

	// reference point for overlaying
	// pDC->SetPixel(164,90,RGB(255,255,255));
	
}

/////////////////////////////////////////////////////////////////////////////
// CTextonView printing

BOOL CTextonView::OnPreparePrinting(CPrintInfo* pInfo)
{
	// default preparation
	return DoPreparePrinting(pInfo);
}

void CTextonView::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add extra initialization before printing
}

void CTextonView::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add cleanup after printing
}

/////////////////////////////////////////////////////////////////////////////
// CTextonView diagnostics

#ifdef _DEBUG
void CTextonView::AssertValid() const
{
	CView::AssertValid();
}

void CTextonView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CTextonDoc* CTextonView::GetDocument() // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CTextonDoc)));
	return (CTextonDoc*)m_pDocument;
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CTextonView message handlers

void CTextonView::OnFileImportpgm() 
{
	// get image filename
	CString fileName;

	CFileDialog dDlg(TRUE,"pgm","",
		OFN_PATHMUSTEXIST | OFN_HIDEREADONLY | OFN_FILEMUSTEXIST,
		_T("PGM ASCII files (*.pgm)|*.pgm||"),
		NULL);
	
	if(dDlg.DoModal()!=IDOK)
		return;
	
	fileName = dDlg.GetPathName();

	// load the pgm
	pgmImage.LoadPgm(fileName);

	displayState = 1;

	// draw the pgm
	Invalidate(TRUE);
	
}

void CTextonView::DrawPgm()
{
	displayState = 1;
	Invalidate();
}

void CTextonView::DrawThres()
{
	displayState = 3;
	CThresDlg thresDlg;
	thresDlg.pOwnerView = this;
	thresDlg.m_iLower = iThresLower;
	thresDlg.m_iUpper = iThresUpper;
	thresDlg.DoModal();
	iThresLower = thresDlg.m_iLower ;
	iThresUpper = thresDlg.m_iUpper ;

	Invalidate();

}

void CTextonView::DrawThresRanges(int iLower, int iUpper)
{
	iThresLower = iLower;
	iThresUpper = iUpper;
	Invalidate();
}

void CTextonView::DrawEdges()
{
	displayState = 2;
	CCannyDlg cannyDlg;
	cannyDlg.m_lowThres = iCannyLower;
	cannyDlg.m_hiThres = iCannyUpper;
	cannyDlg.m_std = fCannyStd;
	cannyDlg.m_magShade = bCannyMagShade;
	cannyDlg.m_showOriginal = bCannyDrawImage;
	cannyDlg.pOwnerView = this;
	cannyDlg.DoModal();
	iCannyLower = cannyDlg.m_lowThres;
	iCannyUpper = cannyDlg.m_hiThres;
	fCannyStd = cannyDlg.m_std;
	bCannyMagShade = cannyDlg.m_magShade;
	bCannyDrawImage = cannyDlg.m_showOriginal;

	Invalidate();

}

void CTextonView::DrawCanny(int lowThres, int hiThres, float std)
{
	CCannyDetector cannyDetector;

	// copy original image
	int* originalImage = new int[pgmImage.imgHeight*pgmImage.imgWidth];
	for(int x=0; x<pgmImage.imgWidth; x++)
		for(int y=0; y<pgmImage.imgHeight; y++)
			originalImage[pgmImage.imgWidth*y+x] =
				pgmImage.imgData[pgmImage.imgWidth*y+x];

	// allocate memory for edge data
	if(edgeImage!=NULL)
	{
		delete edgeImage;
		edgeImage = NULL;
		bCannyReady = FALSE;
	}

	edgeImage = new int[pgmImage.imgHeight*pgmImage.imgWidth];

	// do it!!!
	if(cannyDetector.EdgeDetect(originalImage, pgmImage.imgWidth, pgmImage.imgHeight,
		lowThres, hiThres, std, edgeImage) == 0)
		AfxMessageBox("Error calculating canny image.");
	else
		bCannyReady = TRUE;
	
	delete originalImage;
	originalImage = NULL;
	Invalidate();

}

void CTextonView::DrawTest()
{
	displayState = 4;
	Invalidate();
}

void CTextonView::DrawContour()
{
	displayState = 5;
	Invalidate();
}

void CTextonView::ContourClear()
{

	CClientDC dc(this);
	OnPrepareDC(&dc);
	CPoint point;
	
	int i;
	int iMax = mPointArray.GetUpperBound();

	for(i=0; i<=iMax; i++)
	{
		point = mPointArray.GetAt(i);
		if(point.y < pgmImage.imgHeight && point.x < pgmImage.imgWidth &&
			point.x>=0 && point.y>=0)
				dc.SetPixel(point, 
				RGB(pgmImage.imgData[pgmImage.imgWidth*point.y+point.x],
				pgmImage.imgData[pgmImage.imgWidth*point.y+point.x],
				pgmImage.imgData[pgmImage.imgWidth*point.y+point.x]));
			
		
	}

	mPointArray.RemoveAll();
	
}

void CTextonView::OnLButtonDown(UINT nFlags, CPoint point) 
{

	CPoint toPass;
	
	// convert point (may be unnecessary)
	toPass = point;
	CClientDC dc(this);
	OnPrepareDC(&dc);
	dc.DPtoLP(&toPass);

/*	if(dlgIFTW != NULL && dlgIFTW->bInteract == TRUE)
	{
		// give point to zoom function
		dlgIFTW->TakeDownPoint(toPass);
	}
*/
	if(dlgStrm != NULL && dlgStrm->bZoom == TRUE)
	{
		// give point to zoom function
		dlgStrm->TakeDownPoint(toPass);
	}

	if(dlgRestor != NULL && dlgRestor->bZoom == TRUE)
	{
		// give point to roi function
		dlgRestor->TakeDownPoint(toPass);
	}

	switch(iContourState)
	{
	case 0:
		break;
	case 1:     // draw
		{
			if(toPass.y < pgmImage.imgHeight && toPass.x < pgmImage.imgWidth
				&& toPass.y >= 0 && toPass.x >= 0)
			{
				mPointArray.Add(toPass);
				dc.SetPixel(toPass,RGB(0, 0, 255));
			}
		}
	break;
	case 2:     // erase
		{
			if(toPass.y < pgmImage.imgHeight && toPass.x < pgmImage.imgWidth
				&& toPass.y >= 0 && toPass.x >= 0)
			{
				ContourDeletePoint(toPass, &dc);
				contourOldPoint = toPass;
			}
		}
		break;
	default:
		break;
	}

	SetCapture();

	CView::OnLButtonDown(nFlags, point);
}


void CTextonView::OnMouseMove(UINT nFlags, CPoint point) 
{

	if (GetCapture() != this)
		return; // If this window (view) didn't capture the mouse,
				// then the user isn't drawing in this window.

	CClientDC dc(this);
	OnPrepareDC(&dc);
	dc.DPtoLP(&point);
/*	
	if(dlgIFTW != NULL && (dlgIFTW->bZoom == TRUE || dlgIFTW->bInteract == TRUE))
	{
		if(point.y < testImage.imgHeight && point.x < testImage.imgWidth
			&& point.y >= 0 && point.x >= 0)
			// give point to zoom function
			dlgIFTW->TakeMovePoint(point);

	}
*/
	if(((CDlgFindVortex*)dlgVortexFinder) != NULL && ((CDlgFindVortex*)dlgVortexFinder)->bTakeMouse == TRUE)
	{
		if(point.y < testImage.imgHeight && point.x < testImage.imgWidth
			&& point.y >= 0 && point.x >= 0)
			// give point to zoom function
			((CDlgFindVortex*)dlgVortexFinder)->TakeMovePoint(point);
	}


	if(((CDlgVolView*)pVolView) != NULL && ((CDlgVolView*)pVolView)->bTakeMouse == TRUE)
	{
		if(point.y < testImage.imgHeight && point.x < testImage.imgWidth
			&& point.y >= 0 && point.x >= 0)
			// give point to zoom function
			((CDlgVolView*)pVolView)->TakeMovePoint(point);
	}

	if(dlgStrm != NULL && dlgStrm->bZoom == TRUE)
	{
		if(point.y < testImage.imgHeight && point.x < testImage.imgWidth
			&& point.y >= 0 && point.x >= 0)
			// give point to zoom function
			dlgStrm->TakeMovePoint(point);
	}

	if(dlgRestor != NULL && dlgRestor->bZoom == TRUE)
	{
		if(point.y < testImage.imgHeight && point.x < testImage.imgWidth
			&& point.y >= 0 && point.x >= 0)
			// give point to roi function
			dlgRestor->TakeMovePoint(point);

	}

	switch(iContourState)
	{
	case 0:
		break;
	case 1:     // draw
		{
			int newPoint;
			if(point.y < pgmImage.imgHeight && point.x < pgmImage.imgWidth
				&& point.y >= 0 && point.x >= 0)
			{
				
				newPoint = mPointArray.Add(point);
				dc.SetPixel(point,RGB(0, 0, 255));
				ContourAddBetweenPoints(newPoint, &dc);
			}
		}
		break;
	case 2:     // erase
		{
			if(point.y < pgmImage.imgHeight && point.x < pgmImage.imgWidth
				&& point.y >= 0 && point.x >= 0)
			{
				ContourDeleteBetweenPoints(contourOldPoint, point, &dc);
				contourOldPoint = point;
			}
		}
		break;
	default:
		break;
	}
	
	CView::OnMouseMove(nFlags, point);
}

void CTextonView::OnLButtonUp(UINT nFlags, CPoint point) 
{
	CClientDC dc(this);
	OnPrepareDC(&dc);
	dc.DPtoLP(&point);

	if(dlgStrm != NULL && dlgStrm->bZoom == TRUE)
	{
		if(point.y < testImage.imgHeight && point.x < testImage.imgWidth
			&& point.y >= 0 && point.x >= 0)
			// give point to zoom function
			dlgStrm->TakeUpPoint(point);
	}

	if(dlgRestor != NULL && dlgRestor->bZoom == TRUE)
	{
		if(point.y < testImage.imgHeight && point.x < testImage.imgWidth
			&& point.y >= 0 && point.x >= 0)
			// give point to roi function
			dlgRestor->TakeUpPoint(point);
	}

	if (GetCapture() != this)
		return; // If this window (view) didn't capture the mouse,
				// then the user isn't drawing in this window.

	ReleaseCapture();   // Release the mouse capture established at
						// the beginning of the mouse drag.
	
	CView::OnLButtonUp(nFlags, point);
}

void CTextonView::ContourAddBetweenPoints(int point, CClientDC* pDC)
{
	CPoint pointA, pointB;
	pointA=mPointArray.GetAt(point);
	pointB=mPointArray.GetAt(point-1);

	// see log entry 25/1/02
	double modulus = sqrt(pow(pointA.y-pointB.y,2)+pow(pointA.x-pointB.x,2));
	double y_m = (pointA.y-pointB.y)/modulus;
	double x_m = (pointA.x-pointB.x)/modulus;
	double xy_l=0.4;
	double d_y, d_x;
	double integ;
	int newPoint = point;
	CPoint toAdd;

	while(xy_l<modulus)
	{
		xy_l = xy_l + 1.0;
		d_x = pointB.x + xy_l*x_m;
		d_y = pointB.y + xy_l*y_m;

		if(fabs(modf(d_x,&integ))>=0.5)
			d_x = ceil(d_x);
		else
			d_x = floor(d_x);

		if(fabs(modf(d_y,&integ))>=0.5)
			d_y = ceil(d_y);
		else
			d_y = floor(d_y);

		toAdd.x = (int)d_x;
		toAdd.y = (int)d_y;

		if(toAdd == pointB || toAdd == pointA)
			break;

		if(toAdd != mPointArray.GetAt(newPoint))
		{
			mPointArray.Add(toAdd);
			pDC->SetPixel(toAdd, RGB(0,0,255));
		}

	}

}

void CTextonView::ContourDeletePoint(CPoint point, CClientDC * pDC)
{
	int i;
	int iMax = mPointArray.GetUpperBound();

	int x,y;

	for(x=point.x-3;x<=point.x+3;x++)
		for(y=point.y-3;y<=point.y+3;y++)
			for(i=0; i<=iMax; i++)
			{
				if(mPointArray.GetAt(i).x==x &&
					mPointArray.GetAt(i).y==y)
				{	
					mPointArray.RemoveAt(i);
					i--;
					iMax--;
					if(y < pgmImage.imgHeight && x < pgmImage.imgWidth && x>=0 && y>=0)
						pDC->SetPixel(x,y, 
						RGB(pgmImage.imgData[pgmImage.imgWidth*y+x],
						pgmImage.imgData[pgmImage.imgWidth*y+x],
						pgmImage.imgData[pgmImage.imgWidth*y+x]));
					
				}
			}
}

void CTextonView::ContourDeleteBetweenPoints(CPoint pointA, CPoint pointB,
											 CClientDC * pDC)
{

	// see log entry 25/1/02
	double modulus = sqrt(pow(pointA.y-pointB.y,2)+pow(pointA.x-pointB.x,2));
	double y_m = (pointA.y-pointB.y)/modulus;
	double x_m = (pointA.x-pointB.x)/modulus;
	double xy_l=0.4;
	double d_y, d_x;
	double integ;
	CPoint toAdd;

	while(xy_l<modulus)
	{
		xy_l = xy_l + 1.0;
		d_x = pointB.x + xy_l*x_m;
		d_y = pointB.y + xy_l*y_m;

		if(fabs(modf(d_x,&integ))>=0.5)
			d_x = ceil(d_x);
		else
			d_x = floor(d_x);

		if(fabs(modf(d_y,&integ))>=0.5)
			d_y = ceil(d_y);
		else
			d_y = floor(d_y);

		toAdd.x = (int)d_x;
		toAdd.y = (int)d_y;

		ContourDeletePoint(toAdd, pDC);

	}	
}

void CTextonView::ContourImport()
{
/*
	// import contour data from saved colour image
	CIplImage contourImage;

	// get image filename
	CString fileName;
	CFileDialog dDlg(TRUE,"pgm","",
		OFN_PATHMUSTEXIST | OFN_HIDEREADONLY | OFN_FILEMUSTEXIST,
		_T("Bitmap files (*.bmp)|*.bmp|JPEG files (*.jpg)|(*.jpg)|All Files (*.*)|*.*||"),
		NULL);
	
	if(dDlg.DoModal()!=IDOK)
		return;
	
	fileName = dDlg.GetPathName();

	if(!contourImage.Load(fileName, 1))
	{
		AfxMessageBox("Cannot load file "+fileName+
			". Please check that path and name are correct and the file is of the correct type");
		return;
	}

	int width, height;
	IplImage* pIplImage = contourImage.GetImage();
	width = pIplImage->width;
	height = pIplImage->height;

	int x, y;
	char pixColour[4];
	for(x=0; x<width; x++)
		for(y=0; y<height; y++)
		{			
			iplGetPixel(pIplImage, x, y, pixColour);
			if(GetBValue(RGB(pixColour[2], pixColour[1], pixColour[0]))>
				GetRValue(RGB(pixColour[2], pixColour[1], pixColour[0])) &&
				GetBValue(RGB(pixColour[2], pixColour[1], pixColour[0]))>
				GetGValue(RGB(pixColour[2], pixColour[1], pixColour[0])) &&
				y < pgmImage.imgHeight && x < pgmImage.imgWidth
				&& y >= 0 && x >= 0)
			{
				mPointArray.Add(CPoint(x,y));
				Invalidate();
			}
		}
		*/
}

void CTextonView::OnFileImportPgm()
{
	OnFileImportpgm();
}

void CTextonView::FitEllipse()
{
	/*
	// fit ellipse onto user points
	COpencvContour contour;
	CArray<CPoint, CPoint> ellipse;
	contour.FitEllipse(mPointArray, ellipse, pgmImage.imgWidth, pgmImage.imgHeight);

	// draw ellipse on screen
	CClientDC dc(this);
	OnPrepareDC(&dc);
	int i, nPoints = ellipse.GetSize();
	char sz455[100];
	sprintf(sz455,"%i",nPoints);
	AfxMessageBox("ellipse has "+CString(sz455)+" points");
	sprintf(sz455,"%i",mPointArray.GetSize());
	AfxMessageBox("pointArray has "+CString(sz455)+" points");

	for(i=0; i<nPoints; i++)
//		if(ellipse[i].y < pgmImage.imgHeight && ellipse[i].x < pgmImage.imgWidth
//			&& ellipse[i].y >= 0 && ellipse[i].x >= 0)
		{
			dc.SetPixel(ellipse[i],RGB(0, 255, 0));
		}

*/

}

void CTextonView::TemplateMatch()
{
	/*
	CDlgTemplateMatch dlgTmpl;
	dlgTmpl.pView = this;
	((CMainFrame*)AfxGetMainWnd())->pDlgTasks->ShowWindow(SW_HIDE);
	dlgTmpl.DoModal();
	((CMainFrame*)AfxGetMainWnd())->pDlgTasks->ShowWindow(SW_SHOW);
	*/
}

void CTextonView::DrawHough(float diam_max, float diam_min, int circles)
{
	CHoughPlotter plotter;
	displayState = 4;	// we're writing to the generic test image

	if(edgeImage == NULL)
	{
		AfxMessageBox("You must generated an edge-detected image\nbefore applying the arc detector.");
	}

	if(testImage.imgData != NULL)
		delete testImage.imgData;
	
	testImage.imgWidth = pgmImage.imgWidth;
	testImage.imgHeight = pgmImage.imgHeight;
	testImage.imgData = new int[testImage.imgWidth*testImage.imgHeight];

	if(plotter.PlotHough(edgeImage, testImage.imgData, pgmImage.imgWidth, pgmImage.imgHeight,
		diam_min, diam_max, circles)==FALSE)
	{
		AfxMessageBox("BUG: error in CTextonView::DrawHough()");
		delete testImage.imgData;
		testImage.imgData = NULL;
	}

	DrawTest();

}

void CTextonView::TasksHough()
{
	CDlgHough dlgHough;
	dlgHough.pView = this;
	((CMainFrame*)AfxGetMainWnd())->pDlgTasks->ShowWindow(SW_HIDE);
	dlgHough.DoModal();
	((CMainFrame*)AfxGetMainWnd())->pDlgTasks->ShowWindow(SW_SHOW);
}

void CTextonView::SaveHough()
{
	CFileDialog* dDlg = new CFileDialog(FALSE,"pgm","hough",OFN_CREATEPROMPT |
		OFN_HIDEREADONLY | OFN_NOREADONLYRETURN,
		_T("PGM Images (*.pgm)|*.pgm||"),
		NULL);
	if(dDlg->DoModal()!=IDOK)
		return;
	testImage.SaveImagePgm(dDlg->GetPathName().GetBuffer(1));
	delete dDlg;

}

void CTextonView::CannySave()
{
	CFileDialog* dDlg = new CFileDialog(FALSE,"pgm","canny",OFN_CREATEPROMPT |
		OFN_HIDEREADONLY | OFN_NOREADONLYRETURN,
		_T("PGM Images (*.pgm)|*.pgm||"),
		NULL);
	if(dDlg->DoModal()!=IDOK)
		return;
	CImagePgm dmy;
	dmy.imgData = edgeImage;
	dmy.imgWidth = pgmImage.imgWidth;
	dmy.imgHeight = pgmImage.imgHeight;
	dmy.SaveImagePgm(dDlg->GetPathName().GetBuffer(1));
	delete dDlg;
	dmy.imgData = NULL;

}

void CTextonView::FitCircle()
{
	/*
	// fit circle onto user points
	COpencvContour contour;
	CArray<CPoint, CPoint> circle;
	contour.FitCircle(mPointArray, circle, pgmImage.imgWidth, pgmImage.imgHeight);

	// draw circle on screen
	CClientDC dc(this);
	OnPrepareDC(&dc);
	int i, nPoints = circle.GetSize();
	char sz455[100];
	sprintf(sz455,"%i",nPoints);
	AfxMessageBox("circle has "+CString(sz455)+" points");
	sprintf(sz455,"%i",mPointArray.GetSize());
	AfxMessageBox("pointArray has "+CString(sz455)+" points");

	for(i=0; i<nPoints; i++)
//		if(circle[i].y < pgmImage.imgHeight && circle[i].x < pgmImage.imgWidth
//			&& circle[i].y >= 0 && circle[i].x >= 0)
		{
			dc.SetPixel(circle[i],RGB(0, 255, 0));
		}
*/
}

void CTextonView::TasksArcFit()
{
	CDlgArcFit dlgArc;
	dlgArc.pView = this;
	((CMainFrame*)AfxGetMainWnd())->pDlgTasks->ShowWindow(SW_HIDE);
	dlgArc.DoModal();
	((CMainFrame*)AfxGetMainWnd())->pDlgTasks->ShowWindow(SW_SHOW);
}

void CTextonView::DrawArcFit(int min_points, int max_points, float max_error, void (*progress)(char*))
{
/*
	if(edgeImage == NULL)
	{
		AfxMessageBox("You must generate an edge-detected image\nbefore applying the arc detector.");
		return;
	}

	CArcFit arcFitter;
	displayState = 4;	// we're writing to the generic test image

	if(testImage.imgData != NULL)
		delete testImage.imgData;
	
	testImage.imgWidth = pgmImage.imgWidth;
	testImage.imgHeight = pgmImage.imgHeight;
	testImage.imgData = new int[testImage.imgWidth*testImage.imgHeight];

	if(imgClaimArcs!=NULL)
	{
		delete imgClaimArcs;
		imgClaimArcs = NULL;
	}
	imgClaimArcs = new int[pgmImage.imgWidth*pgmImage.imgHeight];

	if(imgArcs!=NULL)
	{
		delete imgArcs;
		imgArcs = NULL;
	}
	imgArcs = new int[pgmImage.imgWidth*pgmImage.imgHeight];

	if(img_seedclaims!=NULL)
	{
		delete img_seedclaims;
		img_seedclaims = NULL;
	}
	img_seedclaims = new int[pgmImage.imgWidth*pgmImage.imgHeight];

	// fit arcs to edge-detected data
	if(arcFitter.Fit(edgeImage, imgArcs, testImage.imgData, pgmImage.imgWidth, pgmImage.imgHeight,
		min_points, max_points, max_error, progress)==FALSE)
	{
		AfxMessageBox("BUG: error in CTextonView::DrawArcFit()");
		delete testImage.imgData;
		testImage.imgData = NULL;
		return;
	}

	arcFitter.PlotClaimedSeeds(img_seedclaims, pgmImage.imgWidth, pgmImage.imgHeight);
	arcFitter.PlotClaimedArcs(imgClaimArcs, edgeImage, pgmImage.imgWidth, pgmImage.imgHeight);

	// normalise
	int max_pixel = 0;
	int max_x, max_y;
	float ratio;
	double dmy;
	const int max_gray = 255;
	int x, y;
	for(x=0; x<pgmImage.imgWidth; x++)
		for(y=0; y<pgmImage.imgHeight; y++)
			if(testImage.imgData[pgmImage.imgWidth*y+x]>max_pixel)
			{
				max_pixel = testImage.imgData[pgmImage.imgWidth*y+x];
				max_x = x;
				max_y = y;
			}
	ratio = (float)max_gray/(float)max_pixel;
	for(x=0; x<pgmImage.imgWidth; x++)
		for(y=0; y<pgmImage.imgHeight; y++)
			testImage.imgData[pgmImage.imgWidth*y+x] = 
			(int)(modf(testImage.imgData[pgmImage.imgWidth*y+x]*ratio,&dmy)>0.5?
			ceil(testImage.imgData[pgmImage.imgWidth*y+x]*ratio):
		floor(testImage.imgData[pgmImage.imgWidth*y+x]*ratio));

	// copy arcs image
	if(imgArcBins!=NULL)
	{
		delete imgArcBins;
		imgArcBins = NULL;
	}

	imgArcBins = new int[pgmImage.imgWidth*pgmImage.imgHeight];
	for(x=0; x<pgmImage.imgWidth; x++)
		for(y=0; y<pgmImage.imgHeight; y++)
			imgArcBins[pgmImage.imgWidth*y+x]=testImage.imgData[pgmImage.imgWidth*y+x];

	char sz266[100];
	sprintf(sz266, "Max count at (%i, %i)", max_x, max_y);
	progress(sz266);

	if(max_x<pgmImage.imgWidth && max_y<pgmImage.imgHeight)
		testImage.imgData[pgmImage.imgWidth*max_y+max_x]=1000;

	DrawTest();
	*/

}

void CTextonView::DrawCannyStored()
{
	displayState = 2;
	Invalidate();

}

void CTextonView::DrawArcsStored()
{
	// normalise
	int max_pixel = 0;
	int max_x, max_y;
	float ratio;
	double dmy;
	const int max_gray = 255;
	int x, y;
	for(x=0; x<pgmImage.imgWidth; x++)
		for(y=0; y<pgmImage.imgHeight; y++)
			if(imgArcs[pgmImage.imgWidth*y+x]>max_pixel)
			{
				max_pixel = imgArcs[pgmImage.imgWidth*y+x];
				max_x = x;
				max_y = y;
			}
	ratio = (float)max_gray/(float)max_pixel;
	for(x=0; x<pgmImage.imgWidth; x++)
		for(y=0; y<pgmImage.imgHeight; y++)
			testImage.imgData[pgmImage.imgWidth*y+x] = 
			(int)(modf(imgArcs[pgmImage.imgWidth*y+x]*ratio,&dmy)>0.5?
			ceil(imgArcs[pgmImage.imgWidth*y+x]*ratio):
		floor(imgArcs[pgmImage.imgWidth*y+x]*ratio));

	DrawTest();
	
}

void CTextonView::DrawArcBins(void (*progress)(char*))
{
	// normalise
	int max_pixel = 0;
	int max_x, max_y;
	float ratio;
	double dmy;
	const int max_gray = 255;
	int x, y;
	for(x=0; x<pgmImage.imgWidth; x++)
		for(y=0; y<pgmImage.imgHeight; y++)
			if(imgArcBins[pgmImage.imgWidth*y+x]>max_pixel)
			{
				max_pixel = imgArcBins[pgmImage.imgWidth*y+x];
				max_x = x;
				max_y = y;
			}
	ratio = (float)max_gray/(float)max_pixel;
	for(x=0; x<pgmImage.imgWidth; x++)
		for(y=0; y<pgmImage.imgHeight; y++)
			testImage.imgData[pgmImage.imgWidth*y+x] = 
			(int)(modf(imgArcBins[pgmImage.imgWidth*y+x]*ratio,&dmy)>0.5?
			ceil(imgArcBins[pgmImage.imgWidth*y+x]*ratio):
		floor(imgArcBins[pgmImage.imgWidth*y+x]*ratio));

	char sz266[100];
	sprintf(sz266, "Max count at (%i, %i)", max_x, max_y);
	progress(sz266);

	if(max_x<pgmImage.imgWidth && max_y<pgmImage.imgHeight)
		testImage.imgData[pgmImage.imgWidth*max_y+max_x]=1000;

	DrawTest();

}

void CTextonView::DrawOriginalMaxBin(void (*progress)(char*))
{
	int max_pixel = 0;
	int max_x, max_y;
	const int max_gray = 255;
	int x, y;
	for(x=0; x<pgmImage.imgWidth; x++)
		for(y=0; y<pgmImage.imgHeight; y++)
			if(imgArcBins[pgmImage.imgWidth*y+x]>max_pixel)
			{
				max_pixel = imgArcBins[pgmImage.imgWidth*y+x];
				max_x = x;
				max_y = y;
			}

	for(x=0; x<pgmImage.imgWidth; x++)
		for(y=0; y<pgmImage.imgHeight; y++)
			testImage.imgData[pgmImage.imgWidth*y+x] = 
			pgmImage.imgData[pgmImage.imgWidth*y+x];

	char sz266[100];
	sprintf(sz266, "Max count at (%i, %i)", max_x, max_y);
	progress(sz266);

	if(max_x<pgmImage.imgWidth && max_y<pgmImage.imgHeight)
		testImage.imgData[pgmImage.imgWidth*max_y+max_x]=1000;

	DrawTest();

}

void CTextonView::CopyOriginalToCanny()
{
	// allocate memory for edge data
	if(edgeImage!=NULL)
	{
		delete edgeImage;
		edgeImage = NULL;
		bCannyReady = FALSE;
	}

	edgeImage = new int[pgmImage.imgHeight*pgmImage.imgWidth];

	// copy original image
	int x, y;
	for( x=0; x<pgmImage.imgWidth; x++)
		for( y=0; y<pgmImage.imgHeight; y++)
			edgeImage[pgmImage.imgWidth*y+x] =
			pgmImage.imgData[pgmImage.imgWidth*y+x]*(-1)+255;

	bCannyReady = TRUE;
	bCannyMagShade = TRUE;
	bCannyDrawImage = FALSE;

}

void CTextonView::DrawArcFitSeedClaims()
{
	// normalise
	int max_pixel = 0;
	int max_x, max_y;
	float ratio;
	double dmy;
	const int max_gray = 255;
	int x, y;
	for(x=0; x<pgmImage.imgWidth; x++)
		for(y=0; y<pgmImage.imgHeight; y++)
			if(img_seedclaims[pgmImage.imgWidth*y+x]>max_pixel)
			{
				max_pixel = img_seedclaims[pgmImage.imgWidth*y+x];
				max_x = x;
				max_y = y;
			}
	ratio = (float)max_gray/(float)max_pixel;
	for(x=0; x<pgmImage.imgWidth; x++)
		for(y=0; y<pgmImage.imgHeight; y++)
			testImage.imgData[pgmImage.imgWidth*y+x] = 
			(int)(modf(img_seedclaims[pgmImage.imgWidth*y+x]*ratio,&dmy)>0.5?
			ceil(img_seedclaims[pgmImage.imgWidth*y+x]*ratio):
		    floor(img_seedclaims[pgmImage.imgWidth*y+x]*ratio));

	DrawTest();

}

void CTextonView::DrawArcFitClaimArcs()
{
	// normalise
	int max_pixel = 0;
	int max_x, max_y;
	float ratio;
	double dmy;
	const int max_gray = 255;
	int x, y;
	for(x=0; x<pgmImage.imgWidth; x++)
		for(y=0; y<pgmImage.imgHeight; y++)
			if(imgClaimArcs[pgmImage.imgWidth*y+x]>max_pixel)
			{
				max_pixel = imgClaimArcs[pgmImage.imgWidth*y+x];
				max_x = x;
				max_y = y;
			}
	ratio = (float)max_gray/(float)max_pixel;
	for(x=0; x<pgmImage.imgWidth; x++)
		for(y=0; y<pgmImage.imgHeight; y++)
			testImage.imgData[pgmImage.imgWidth*y+x] = 
			(int)(modf(imgClaimArcs[pgmImage.imgWidth*y+x]*ratio,&dmy)>0.5?
			ceil(imgClaimArcs[pgmImage.imgWidth*y+x]*ratio):
		    floor(imgClaimArcs[pgmImage.imgWidth*y+x]*ratio));

	DrawTest();


}

void CTextonView::TasksFourier()
{
/*	CDlgFourierCircle dlgFourier;
	dlgFourier.pView = this;
	((CMainFrame*)AfxGetMainWnd())->pDlgTasks->ShowWindow(SW_HIDE);
	dlgFourier.DoModal();
	((CMainFrame*)AfxGetMainWnd())->pDlgTasks->ShowWindow(SW_SHOW);
*/}

void CTextonView::DrawRotated(int angle)
{
/*
	IplImage *oriImage;
	oriImage = iplCreateImageHeader(1,0,IPL_DEPTH_8U,"GRAY","GRAY", 
		IPL_DATA_ORDER_PIXEL, IPL_ORIGIN_TL, IPL_ALIGN_DWORD,
		pgmImage.imgWidth, pgmImage.imgHeight, NULL, NULL, NULL, NULL );
	if( !oriImage ) return;
	iplAllocateImage( oriImage, 0, 0 );
	if( !oriImage->imageData ) return;
			
		// copy source
	int x,y, buffer;
	for(x=0; x<pgmImage.imgWidth; x++)
		for(y=0; y<pgmImage.imgHeight; y++)
		{	
			buffer = pgmImage.imgData[pgmImage.imgWidth*y+x];
			iplPutPixel(oriImage, x, y, &buffer);
		}

		// allocate target
	IplImage *smlImage;
	smlImage = iplCreateImageHeader(1,0,IPL_DEPTH_8U,"GRAY","GRAY", 
		IPL_DATA_ORDER_PIXEL, IPL_ORIGIN_TL, IPL_ALIGN_DWORD,
		pgmImage.imgWidth, pgmImage.imgHeight, NULL, NULL, NULL, NULL );
	if( !smlImage ) return;
	iplAllocateImage( smlImage, 0, 0 );
	if( !smlImage->imageData ) return;

		// decimate image
	iplSetBorderMode( oriImage, IPL_BORDER_WRAP, IPL_SIDE_TOP|
		IPL_SIDE_BOTTOM | IPL_SIDE_LEFT | IPL_SIDE_RIGHT, 0 );
	iplSetBorderMode( smlImage, IPL_BORDER_WRAP, IPL_SIDE_TOP|
		IPL_SIDE_BOTTOM | IPL_SIDE_LEFT | IPL_SIDE_RIGHT, 0 );
	iplRotate(oriImage, smlImage, (double)angle, 0, 0, IPL_INTER_LINEAR);

	if(testImage.imgData != NULL)
		delete testImage.imgData;
	
	testImage.imgWidth = pgmImage.imgWidth;
	testImage.imgHeight = pgmImage.imgHeight;
	testImage.imgData = new int[testImage.imgWidth*testImage.imgHeight];

	// copy ipl to array
	for(x=0;x<testImage.imgWidth;x++)
		for(y=0;y<testImage.imgHeight;y++)
		{
			iplGetPixel(smlImage, x, y, &buffer);
			testImage.imgData[testImage.imgWidth*y+x] = buffer;
		}

	// normalise array
	int max_pixel = 0;
	int max_x, max_y;
	float ratio;
	double dmy;
	const int max_gray = 255;
	for(x=0; x<pgmImage.imgWidth; x++)
		for(y=0; y<pgmImage.imgHeight; y++)
			if(testImage.imgData[pgmImage.imgWidth*y+x]>max_pixel)
			{
				max_pixel = testImage.imgData[pgmImage.imgWidth*y+x];
				max_x = x;
				max_y = y;
			}
	ratio = (float)max_gray/(float)max_pixel;
	for(x=0; x<pgmImage.imgWidth; x++)
		for(y=0; y<pgmImage.imgHeight; y++)
			testImage.imgData[pgmImage.imgWidth*y+x] = 
			(int)(modf(testImage.imgData[pgmImage.imgWidth*y+x]*ratio,&dmy)>0.5?
			ceil(testImage.imgData[pgmImage.imgWidth*y+x]*ratio):
		    floor(testImage.imgData[pgmImage.imgWidth*y+x]*ratio));

	DrawTest();

	iplDeallocate( oriImage, IPL_IMAGE_ALL );
	iplDeallocate( smlImage, IPL_IMAGE_ALL );
	*/

}

void CTextonView::DrawResult(int * image)
{
	if(testImage.imgData != NULL)
		delete [] testImage.imgData;
	
	testImage.imgWidth = pgmImage.imgWidth;
	testImage.imgHeight = pgmImage.imgHeight;
	testImage.imgData = new int[testImage.imgWidth*testImage.imgHeight];

	int x,y;
	for(x=0; x<pgmImage.imgWidth; x++)
		for(y=0; y<pgmImage.imgHeight; y++)
			testImage.imgData[pgmImage.imgWidth*y+x]=image[pgmImage.imgWidth*y+x];

	DrawTest();
}

void CTextonView::DrawResultSpecial(int * image, int width, int height)
{
	if(testImage.imgData != NULL)
		delete [] testImage.imgData;
	
	testImage.imgWidth = width;
	testImage.imgHeight = height;
	testImage.imgData = new int[testImage.imgWidth*testImage.imgHeight];

	int x,y;
	for(x=0; x<width; x++)
		for(y=0; y<height; y++)
			testImage.imgData[width*y+x]=image[width*y+x];

	DrawTest();

}

void CTextonView::DrawLoG(float thres)
{
	int* result = new int[pgmImage.imgWidth*pgmImage.imgHeight];

	m_voorBlobs.Reset();
	m_voorBlobs.LoG(pgmImage.imgData, result, pgmImage.imgWidth, pgmImage.imgHeight, thres);

	DrawResult(result);

	delete result;

}

void CTextonView::TasksVoorhees()
{
	CDlgVoorhees dlgVoorhees;
	dlgVoorhees.pView = this;
	((CMainFrame*)AfxGetMainWnd())->pDlgTasks->ShowWindow(SW_HIDE);
	dlgVoorhees.DoModal();
	((CMainFrame*)AfxGetMainWnd())->pDlgTasks->ShowWindow(SW_SHOW);

}

void CTextonView::SaveResult()
{
	CFileDialog* dDlg = new CFileDialog(FALSE,"pgm","result",OFN_CREATEPROMPT |
		OFN_HIDEREADONLY | OFN_NOREADONLYRETURN,
		_T("PGM Images (*.pgm)|*.pgm||"),
		NULL);
	if(dDlg->DoModal()!=IDOK)
		return;

	// reference point for overlaying
	if(testImage.imgWidth >= 40 && testImage.imgHeight >= 240)
		testImage.imgData[testImage.imgWidth*240+40] = 255;

	testImage.SaveImagePgm(dDlg->GetPathName().GetBuffer(1));
	delete dDlg;

}

void CTextonView::DrawThresLoG(int thres)
{
	int* result = new int[pgmImage.imgWidth*pgmImage.imgHeight];

	m_voorBlobs.ThresLoG(result, pgmImage.imgWidth, pgmImage.imgHeight, thres);

	DrawResult(result);

	delete result;

}

void CTextonView::DrawBlur(float m_gauss_sigma)
{
	int* result = new int[pgmImage.imgWidth*pgmImage.imgHeight];

	m_voorBlobs.GaussBlur(pgmImage.imgData, result, pgmImage.imgWidth, pgmImage.imgHeight, m_gauss_sigma);

	DrawResult(result);

	delete result;


}

void CTextonView::TasksIFTW()
{
/*	dlgIFTW = new CDlgIFTW;
	dlgIFTW->pView = this;
	dlgIFTW->Create(IDD_IFTW, ((CMainFrame*)AfxGetMainWnd())->pDlgTasks);
	((CMainFrame*)AfxGetMainWnd())->pDlgTasks->ShowWindow(SW_HIDE);
	dlgIFTW->ShowWindow(SW_SHOW);
*/
}

void CTextonView::TasksNoiseImg()
{
/*	CDlgNoiseImage dlgNoise;
	dlgNoise.pView = this;
	((CMainFrame*)AfxGetMainWnd())->pDlgTasks->ShowWindow(SW_HIDE);
	dlgNoise.DoModal();
	((CMainFrame*)AfxGetMainWnd())->pDlgTasks->ShowWindow(SW_SHOW);
*/}

void CTextonView::TasksStreamlines()
{
	if(dlgStrm != NULL)
		delete dlgStrm;

	dlgStrm = new CDlgStreamlines;
	dlgStrm->pView = this;
	dlgStrm->Create(IDD_STREAMLS, ((CMainFrame*)AfxGetMainWnd())->pDlgTasks);
	dlgStrm->ShowWindow(SW_SHOW);
	((CMainFrame*)AfxGetMainWnd())->pDlgTasks->ShowWindow(SW_HIDE);
}

void CTextonView::StreamlinesClosed()
{
	if(dlgRestor!=NULL)
		dlgRestor->StreamlinesClosed();
	else
	{
		((CMainFrame*)AfxGetMainWnd())->pDlgTasks->ShowWindow(SW_SHOW);
		dlgStrm = NULL;
	}
}

void CTextonView::DrawColourImageSpecial(CImageColour * image)
{
	if(image == NULL)
	{
		AfxMessageBox("Null pointer argument to DrawColourImage.");
		return;
	}

	if(image->imgWidth <=0 || image->imgHeight <= 0)
	{
		AfxMessageBox("Invalid image argument to DrawColourImage.");
		return;
	}

	imgColourTest.AllocateData(image->imgWidth, image->imgHeight);

	int x, y;
	for(x=0; x<image->imgWidth; x++)
		for(y=0; y<image->imgHeight; y++)
		{
			imgColourTest.imgData[image->imgWidth*y+x].RED = 
				image->imgData[image->imgWidth*y+x].RED;
			imgColourTest.imgData[image->imgWidth*y+x].GREEN = 
				image->imgData[image->imgWidth*y+x].GREEN;
			imgColourTest.imgData[image->imgWidth*y+x].BLUE = 
				image->imgData[image->imgWidth*y+x].BLUE;
		}

	displayState = 6;
	Invalidate();

}

void CTextonView::TasksRestoration()
{
	if(dlgRestor != NULL)
		delete dlgRestor;

	dlgRestor = new CDlgRestoration;
	dlgRestor->pView = this;
	dlgRestor->Create(IDD_RESTORATION, ((CMainFrame*)AfxGetMainWnd())->pDlgTasks);
	dlgRestor->ShowWindow(SW_SHOW);
	((CMainFrame*)AfxGetMainWnd())->pDlgTasks->ShowWindow(SW_HIDE);

}

void CTextonView::RestorationClosed()
{
	((CMainFrame*)AfxGetMainWnd())->pDlgTasks->ShowWindow(SW_SHOW);
	dlgRestor = NULL;
}

BOOL CTextonView::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message) 
{
	if(dlgRestor != NULL && dlgRestor->bZoom == TRUE)
	{
		CPoint toPass;
		GetCursorPos(&toPass);

		// convert point (may be unnecessary)
		toPass = toPass;
		CClientDC dc(this);
		OnPrepareDC(&dc);
		dc.DPtoLP(&toPass);
		ScreenToClient(&toPass);

		// give point to zoom function
		dlgRestor->TakeMouseOverPoint(toPass);
	}

	if(dlgStrm != NULL && dlgStrm->bZoom == TRUE)
	{
		CPoint toPass;
		GetCursorPos(&toPass);

		// convert point (may be unnecessary)
		toPass = toPass;
		CClientDC dc(this);
		OnPrepareDC(&dc);
		dc.DPtoLP(&toPass);
		ScreenToClient(&toPass);

		// give point to zoom function
		dlgStrm->TakeMouseOverPoint(toPass);
	}
	
	return CView::OnSetCursor(pWnd, nHitTest, message);
}

void CTextonView::OnCancelMode() 
{
	CView::OnCancelMode();
	
}

void CTextonView::TasksVortex()
{
	if(dlgVortexFinder != NULL)
		delete dlgVortexFinder;

	dlgVortexFinder = new CDlgFindVortex;
	dlgVortexFinder->pView = this;
	dlgVortexFinder->Create(IDD_FINDVORTEX, ((CMainFrame*)AfxGetMainWnd())->pDlgTasks);
	dlgVortexFinder->ShowWindow(SW_SHOW);
	((CMainFrame*)AfxGetMainWnd())->pDlgTasks->ShowWindow(SW_HIDE);

}
/*
void CTextonView::DlgIFTWClosed()
{
	((CMainFrame*)AfxGetMainWnd())->pDlgTasks->ShowWindow(SW_SHOW);
	dlgIFTW = NULL;
}
*/