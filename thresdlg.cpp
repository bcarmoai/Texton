// ThresDlg.cpp : implementation file
//

#include "stdafx.h"
#include "Texton.h"
#include "ThresDlg.h"
#include "TextonView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CThresDlg dialog


CThresDlg::CThresDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CThresDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CThresDlg)
	m_bShowImage = FALSE;
	m_iLower = 0;
	m_iUpper = 0;
	m_iRangeWidth = 0;
	//}}AFX_DATA_INIT
}


void CThresDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CThresDlg)
	DDX_Control(pDX, IDC_THRES_SPINU, m_ctlSpinU);
	DDX_Control(pDX, IDC_THRES_SPINL, m_ctlSpinL);
	DDX_Check(pDX, IDC_ANIMATE_SHOW, m_bShowImage);
	DDX_Text(pDX, IDC_THRES_LOWER, m_iLower);
	DDV_MinMaxInt(pDX, m_iLower, 0, 255);
	DDX_Text(pDX, IDC_THRES_UPPER, m_iUpper);
	DDV_MinMaxInt(pDX, m_iUpper, 0, 255);
	DDX_Text(pDX, IDC_THRES_WIDTH, m_iRangeWidth);
	DDV_MinMaxInt(pDX, m_iRangeWidth, 0, 255);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CThresDlg, CDialog)
	//{{AFX_MSG_MAP(CThresDlg)
	ON_BN_CLICKED(ID_APPLY, OnApply)
	ON_BN_CLICKED(IDC_THRES_ANIMATE, OnThresAnimate)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CThresDlg message handlers

void CThresDlg::OnApply() 
{
	UpdateData(TRUE);
	((CTextonView*)pOwnerView)->bThresDrawImage = m_bShowImage;
	((CTextonView*)pOwnerView)->DrawThresRanges(m_iLower, m_iUpper);
}

void CThresDlg::OnThresAnimate() 
{
	UpdateData(TRUE);
	((CTextonView*)pOwnerView)->bThresDrawImage = m_bShowImage;

	for(int i = 0; i+m_iRangeWidth <= 255; i++)
	{
		if(AfxMessageBox("Continue?", MB_YESNO)==IDYES)
		{
			m_iLower = i;
			m_iUpper = i+m_iRangeWidth;
			((CTextonView*)pOwnerView)->DrawThresRanges(m_iLower, m_iUpper);
			UpdateData(FALSE);
		}
		else
			break;
	}

	UpdateData(FALSE);
}
