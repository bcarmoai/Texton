// CannyDlg.cpp : implementation file
//

#include "stdafx.h"
#include "Texton.h"
#include "CannyDlg.h"
#include "TextonView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CCannyDlg dialog


CCannyDlg::CCannyDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CCannyDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CCannyDlg)
	m_std = 2.6f;
	m_lowThres = 0;
	m_hiThres = 100;
	m_showOriginal = FALSE;
	m_magShade = FALSE;
	//}}AFX_DATA_INIT
}


void CCannyDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CCannyDlg)
	DDX_Control(pDX, IDC_CNY_SPINU, m_upperSpin);
	DDX_Control(pDX, IDC_CNY_SPINSTD, m_stdSPin);
	DDX_Control(pDX, IDC_CNY_SPINL, m_lowSpin);
	DDX_Text(pDX, IDC_CNY_STD, m_std);
	DDX_Text(pDX, IDC_CNY_THL, m_lowThres);
	DDV_MinMaxInt(pDX, m_lowThres, 1, 255);
	DDX_Text(pDX, IDC_CNY_THU, m_hiThres);
	DDV_MinMaxInt(pDX, m_hiThres, 1, 255);
	DDX_Check(pDX, IDC_EDGE_SHOW, m_showOriginal);
	DDX_Check(pDX, IDC_EDGE_RED, m_magShade);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CCannyDlg, CDialog)
	//{{AFX_MSG_MAP(CCannyDlg)
	ON_BN_CLICKED(IDOK, OnApply)
	ON_BN_CLICKED(IDC_EDGE_SAVE, OnEdgeSave)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CCannyDlg message handlers

void CCannyDlg::OnApply() 
{
	UpdateData(TRUE);
	((CTextonView*)pOwnerView)->bCannyDrawImage = m_showOriginal;
	((CTextonView*)pOwnerView)->bCannyMagShade = m_magShade;
	((CTextonView*)pOwnerView)->DrawCanny(m_lowThres, m_hiThres, m_std);
	
}

void CCannyDlg::OnEdgeSave() 
{
	((CTextonView*)pOwnerView)->CannySave();
}
