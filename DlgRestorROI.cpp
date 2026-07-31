// DlgRestorROI.cpp : implementation file
//

#include "stdafx.h"
#include "Texton.h"
#include "DlgRestorROI.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDlgRestorROI dialog


CDlgRestorROI::CDlgRestorROI(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgRestorROI::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlgRestorROI)
	m_CoordsText = _T("");
	m_bUserTyped = FALSE;
	m_edTopX = 0;
	m_edTopY = 0;
	m_edBottomX = 0;
	m_edBottomY = 0;
	//}}AFX_DATA_INIT
}


void CDlgRestorROI::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgRestorROI)
	DDX_Text(pDX, IDC_ROI_COORDS, m_CoordsText);
	DDX_Check(pDX, IDC_ROI_BOOL, m_bUserTyped);
	DDX_Text(pDX, IDC_ROI_TX, m_edTopX);
	DDX_Text(pDX, IDC_ROI_TY, m_edTopY);
	DDX_Text(pDX, IDC_ROI_BX, m_edBottomX);
	DDX_Text(pDX, IDC_ROI_BY, m_edBottomY);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlgRestorROI, CDialog)
	//{{AFX_MSG_MAP(CDlgRestorROI)
	ON_BN_CLICKED(IDOK, OnOk)
	ON_BN_CLICKED(ID_ROI_WHOLE, OnRoiWhole)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgRestorROI message handlers

void CDlgRestorROI::TakeRoiText(CString text)
{
	m_CoordsText = text;
	UpdateData(FALSE);
}

void CDlgRestorROI::OnOk() 
{
	CDialog::OnOK();
	pDlgRestor->RoiOK();
}

void CDlgRestorROI::OnCancel() 
{
	CDialog::OnCancel();
	pDlgRestor->RoiCancel();
}

CString CDlgRestorROI::GetText() 
{
	return m_CoordsText;
}

void CDlgRestorROI::OnRoiWhole() 
{
	pDlgRestor->RoiWhole();
	CDialog::OnOK();
}
