// DlgHough.cpp : implementation file
//

#include "stdafx.h"
#include "Texton.h"
#include "DlgHough.h"
#include "TextonView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDlgHough dialog


CDlgHough::CDlgHough(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgHough::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlgHough)
	m_DiamMax = 0.0f;
	m_DiamMin = 0.0f;
	//}}AFX_DATA_INIT

	m_iCircles = 1;
}


void CDlgHough::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgHough)
	DDX_Text(pDX, IDC_HOUGH_MAX, m_DiamMax);
	DDX_Text(pDX, IDC_HOUGH_MIN, m_DiamMin);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlgHough, CDialog)
	//{{AFX_MSG_MAP(CDlgHough)
	ON_BN_CLICKED(ID_HOUGH_APPLY, OnHoughApply)
	ON_BN_CLICKED(IDC_HOUGH_CIRCLES, OnHoughCircles)
	ON_BN_CLICKED(IDC_HOUGH_ELLIPSES, OnHoughEllipses)
	ON_BN_CLICKED(IDC_HOUGH_SAVE, OnHoughSave)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgHough message handlers

void CDlgHough::OnHoughApply() 
{
	UpdateData(TRUE);

	((CTextonView*)pView)->DrawHough(m_DiamMax, m_DiamMin, m_iCircles);

}

void CDlgHough::OnHoughCircles() 
{
	m_iCircles = 1;	
}

void CDlgHough::OnHoughEllipses() 
{
	m_iCircles = 0;	
}

BOOL CDlgHough::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	((CButton*)GetDlgItem(IDC_HOUGH_CIRCLES))->SetCheck(1);
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}


void CDlgHough::OnHoughSave() 
{
	((CTextonView*)pView)->SaveHough();
}
