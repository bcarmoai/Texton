// MainFrm.cpp : implementation of the CMainFrame class
//

#include "stdafx.h"
#include "Texton.h"

#include "MainFrm.h"
#include "TextonView.h"
#include "ImagePgm.h"

//#include <ipl.h>
//#include <cv.h>
//#include "cookdraw.h"
//#include "cookroom.h"
#include "image.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CMainFrame

IMPLEMENT_DYNCREATE(CMainFrame, CFrameWnd)

BEGIN_MESSAGE_MAP(CMainFrame, CFrameWnd)
	//{{AFX_MSG_MAP(CMainFrame)
	ON_WM_CREATE()
	ON_COMMAND(ID_VIEW_TASKS, OnViewTasks)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

static UINT indicators[] =
{
	ID_SEPARATOR,           // status line indicator
	ID_INDICATOR_CAPS,
	ID_INDICATOR_NUM,
	ID_INDICATOR_SCRL,
};

/////////////////////////////////////////////////////////////////////////////
// CMainFrame construction/destruction

CMainFrame::CMainFrame()
{
}

CMainFrame::~CMainFrame()
{
}

int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CFrameWnd::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	if (!m_wndToolBar.Create(this) ||
		!m_wndToolBar.LoadToolBar(IDR_MAINFRAME))
	{
		TRACE0("Failed to create toolbar\n");
		return -1;      // fail to create
	}

	if (!m_wndStatusBar.Create(this) ||
		!m_wndStatusBar.SetIndicators(indicators,
		  sizeof(indicators)/sizeof(UINT)))
	{
		TRACE0("Failed to create status bar\n");
		return -1;      // fail to create
	}

	// TODO: Remove this if you don't want tool tips or a resizeable toolbar
	m_wndToolBar.SetBarStyle(m_wndToolBar.GetBarStyle() |
		CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC);

	// TODO: Delete these three lines if you don't want the toolbar to
	//  be dockable
	m_wndToolBar.EnableDocking(CBRS_ALIGN_ANY);
	EnableDocking(CBRS_ALIGN_ANY);
	DockControlBar(&m_wndToolBar);

	pDlgTasks = new CDlgTasks();
	pDlgTasks->pFrame = this;
	pDlgTasks->Create(IDD_TASKS, this);

	return 0;
}

BOOL CMainFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	return CFrameWnd::PreCreateWindow(cs);
}

/////////////////////////////////////////////////////////////////////////////
// CMainFrame diagnostics

#ifdef _DEBUG
void CMainFrame::AssertValid() const
{
	CFrameWnd::AssertValid();
}

void CMainFrame::Dump(CDumpContext& dc) const
{
	CFrameWnd::Dump(dc);
}

#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CMainFrame message handlers


void CMainFrame::TasksCanny()
{
	((CTextonView*)GetActiveView())->DrawEdges();
}

void CMainFrame::TasksThreshold()
{
	((CTextonView*)GetActiveView())->DrawThres();
}

void CMainFrame::TasksContourDraw()
{
	WINDOWPLACEMENT winInfo;
	pDlgTasks->GetWindowPlacement(&winInfo);
	pDlgContourOptions = new CDlgContourOptions();
	pDlgContourOptions->pFrame = this;
	pDlgContourOptions->Create(IDD_CONTOUR_OPTS, this);
	pDlgContourOptions->SetWindowPlacement(&winInfo);
	pDlgContourOptions->ShowWindow(SW_SHOW);

	pDlgTasks->ShowWindow(SW_HIDE);

	((CTextonView*)GetActiveView())->DrawContour();

}

void CMainFrame::ClosedDlgContour()
{
	pDlgTasks->ShowWindow(SW_SHOW);
}

void CMainFrame::TasksOriginal()
{
	if(((CTextonView*)GetActiveView())->pgmImage.imgData == NULL)
		((CTextonView*)GetActiveView())->OnFileImportPgm();
	else
		((CTextonView*)GetActiveView())->DrawPgm();

}

void CMainFrame::TasksTemplate()
{
	((CTextonView*)GetActiveView())->TemplateMatch();	
}

void CMainFrame::TasksHough()
{
	((CTextonView*)GetActiveView())->TasksHough();	

}

void CMainFrame::TasksArcFit()
{
	((CTextonView*)GetActiveView())->TasksArcFit();	
}

void CMainFrame::OnViewTasks() 
{
	delete pDlgTasks;

	pDlgTasks = new CDlgTasks();
	pDlgTasks->pFrame = this;
	pDlgTasks->Create(IDD_TASKS, this);
	
}

void CMainFrame::TasksArcFourier()
{
	((CTextonView*)GetActiveView())->TasksFourier();	
}

void CMainFrame::TasksVoorhees()
{
	((CTextonView*)GetActiveView())->TasksVoorhees();	

}

void CMainFrame::TasksIFTW()
{
	((CTextonView*)GetActiveView())->TasksIFTW();	

}

void CMainFrame::TasksNoiseImg()
{
	((CTextonView*)GetActiveView())->TasksNoiseImg();	
}

void CMainFrame::TasksStreamlines()
{
	((CTextonView*)GetActiveView())->TasksStreamlines();	
}

void CMainFrame::TasksRestoration()
{
	((CTextonView*)GetActiveView())->TasksRestoration();	
}

void CMainFrame::SetStatusBarText(CString text)
{
	m_wndStatusBar.SetPaneText(0, text);
}

void CMainFrame::TasksVortex()
{
	((CTextonView*)GetActiveView())->TasksVortex();	
}
