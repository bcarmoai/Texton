// DlgStrmZoom.cpp : implementation file
//

#include "stdafx.h"
#include "Texton.h"
#include "DlgStrmZoom.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDlgStrmZoom dialog


CDlgStrmZoom::CDlgStrmZoom(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgStrmZoom::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlgStrmZoom)
	m_ZoomCoords = _T("");
	//}}AFX_DATA_INIT
}


void CDlgStrmZoom::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgStrmZoom)
	DDX_Text(pDX, IDC_STR_ZOOM_TEXT, m_ZoomCoords);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlgStrmZoom, CDialog)
	//{{AFX_MSG_MAP(CDlgStrmZoom)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgStrmZoom message handlers

void CDlgStrmZoom::OnOK() 
{
	CDialog::OnOK();
	pDlgStrm->ZoomOK();
		
}

void CDlgStrmZoom::OnCancel() 
{
	CDialog::OnCancel();
	pDlgStrm->ZoomCancel();
	
}

void CDlgStrmZoom::TakeZoomText(CString text)
{
	m_ZoomCoords = text;
	UpdateData(FALSE);
}
