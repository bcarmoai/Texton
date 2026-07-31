// DlgTasks.cpp : implementation file
//

#include "stdafx.h"
#include "Texton.h"
#include "DlgTasks.h"
#include "MainFrm.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDlgTasks dialog


CDlgTasks::CDlgTasks(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgTasks::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlgTasks)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	pFrame = (CFrameWnd*)pParent;
}


void CDlgTasks::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgTasks)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlgTasks, CDialog)
	//{{AFX_MSG_MAP(CDlgTasks)
	ON_BN_CLICKED(IDC_CANNY_GO, OnCanny)
	ON_BN_CLICKED(IDC_CONTDRW_GO, OnContourDraw)
	ON_BN_CLICKED(IDC_THRES_GO, OnThreshold)
	ON_BN_CLICKED(IDC_ORIGINAL_GO, OnOriginal)
	ON_BN_CLICKED(IDC_CONTTMPL_GO, OnConttmplGo)
	ON_BN_CLICKED(IDC_HOUGH_GO, OnHoughGo)
	ON_BN_CLICKED(IDC_ARC_GO, OnArcGo)
	ON_BN_CLICKED(IDC_FOURIER_GO, OnFourierGo)
	ON_BN_CLICKED(IDC_VOORHEES_GO, OnVoorheesGo)
	ON_BN_CLICKED(IDC_IFTW_GO, OnIftwGo)
	ON_BN_CLICKED(IDC_NOISEIMG_GO, OnNoiseimgGo)
	ON_BN_CLICKED(IDC_STREAMLNS, OnStreamlines)
	ON_BN_CLICKED(IDC_RESTORATION_GO, OnRestorationGo)
	ON_BN_CLICKED(IDC_VORTEX_GO, OnVortexGo)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgTasks message handlers

void CDlgTasks::OnCanny() 
{
	((CMainFrame*)pFrame)->TasksCanny();
}

void CDlgTasks::OnContourDraw() 
{
	((CMainFrame*)pFrame)->TasksContourDraw();
}

void CDlgTasks::OnThreshold() 
{
	((CMainFrame*)pFrame)->TasksThreshold();
}

void CDlgTasks::OnOriginal() 
{
	((CMainFrame*)pFrame)->TasksOriginal();	
}

void CDlgTasks::OnConttmplGo() 
{
	((CMainFrame*)pFrame)->TasksTemplate();		
}

void CDlgTasks::OnHoughGo() 
{
	((CMainFrame*)pFrame)->TasksHough();	
}

void CDlgTasks::OnArcGo() 
{
	((CMainFrame*)pFrame)->TasksArcFit();	
}

void CDlgTasks::OnFourierGo() 
{
	((CMainFrame*)pFrame)->TasksArcFourier();	

}

void CDlgTasks::OnVoorheesGo() 
{
	((CMainFrame*)pFrame)->TasksVoorhees();	
}

void CDlgTasks::OnIftwGo() 
{
	((CMainFrame*)pFrame)->TasksIFTW();	
	
}

void CDlgTasks::OnNoiseimgGo() 
{
	((CMainFrame*)pFrame)->TasksNoiseImg();	
	
}

void CDlgTasks::OnStreamlines() 
{
	// ShowWindow(SW_HIDE);
	((CMainFrame*)pFrame)->TasksStreamlines();	
	
}


void CDlgTasks::OnRestorationGo() 
{
	((CMainFrame*)pFrame)->TasksRestoration();	
}

void CDlgTasks::OnVortexGo() 
{
	((CMainFrame*)pFrame)->TasksVortex();	
}
