// DlgContourOptions.cpp : implementation file
//

#include "stdafx.h"
#include "Texton.h"
#include "DlgContourOptions.h"
#include "MainFrm.h"
#include "TextonView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDlgContourOptions dialog


CDlgContourOptions::CDlgContourOptions(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgContourOptions::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlgContourOptions)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CDlgContourOptions::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgContourOptions)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlgContourOptions, CDialog)
	//{{AFX_MSG_MAP(CDlgContourOptions)
	ON_BN_CLICKED(IDC_CTR_DRAW, OnDraw)
	ON_BN_CLICKED(IDC_CTR_ERS, OnErase)
	ON_BN_CLICKED(IDC_CTR_CLEAR, OnClear)
	ON_BN_CLICKED(IDCANCEL, OnClose)
	ON_BN_CLICKED(IDC_CTR_IPRT, ContourOnImport)
	ON_BN_CLICKED(IDC_ELLIPSE, OnEllipse)
	ON_BN_CLICKED(IDC_CIRCLE, OnCircle)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgContourOptions message handlers

void CDlgContourOptions::OnDraw() 
{
	((CTextonView*)((CMainFrame*)pFrame->GetActiveView()))->iContourState=1;
	
}

void CDlgContourOptions::OnErase() 
{
	((CTextonView*)((CMainFrame*)pFrame->GetActiveView()))->iContourState=2;
	
}

void CDlgContourOptions::OnClear() 
{
	((CTextonView*)((CMainFrame*)pFrame->GetActiveView()))->ContourClear();
	
}

void CDlgContourOptions::OnClose() 
{
	((CMainFrame*)pFrame)->ClosedDlgContour();	
	((CTextonView*)((CMainFrame*)pFrame->GetActiveView()))->iContourState=0;
	CDialog::OnCancel();
}


void CDlgContourOptions::ContourOnImport() 
{
	((CTextonView*)((CMainFrame*)pFrame->GetActiveView()))->ContourImport();
}

void CDlgContourOptions::OnEllipse() 
{
	((CTextonView*)((CMainFrame*)pFrame->GetActiveView()))->FitEllipse();
}

void CDlgContourOptions::OnCircle() 
{
	((CTextonView*)((CMainFrame*)pFrame->GetActiveView()))->FitCircle();
}
