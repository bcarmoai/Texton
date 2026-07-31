// DlgVoorhees.cpp : implementation file
//

#include "stdafx.h"
#include "Texton.h"
#include "DlgVoorhees.h"
#include "TextonView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDlgVoorhees dialog


CDlgVoorhees::CDlgVoorhees(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgVoorhees::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlgVoorhees)
	m_logThres = 2.0f;
	m_thres_log_at = 10;
	m_gauss_sigma = 0.0f;
	//}}AFX_DATA_INIT
}


void CDlgVoorhees::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgVoorhees)
	DDX_Text(pDX, IDC_LOG_THRES, m_logThres);
	DDX_Text(pDX, IDC_AT_THRES, m_thres_log_at);
	DDX_Text(pDX, IDC_BLUR_SIGMA, m_gauss_sigma);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlgVoorhees, CDialog)
	//{{AFX_MSG_MAP(CDlgVoorhees)
	ON_BN_CLICKED(IDC_LOG, OnLoG)
	ON_BN_CLICKED(IDC_SHOW_ORI, OnShowOri)
	ON_BN_CLICKED(IDC_SAVE, OnSave)
	ON_BN_CLICKED(IDC_THRES_AT, OnThresAt)
	ON_BN_CLICKED(IDC_GAUSS_BLUR, OnGaussBlur)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgVoorhees message handlers

void CDlgVoorhees::OnLoG() 
{
	UpdateData(TRUE);
	((CTextonView*)pView)->DrawLoG(m_logThres);

}


void CDlgVoorhees::OnShowOri() 
{
	((CTextonView*)pView)->DrawPgm();
		
}

void CDlgVoorhees::OnSave() 
{
	((CTextonView*)pView)->SaveResult();
	
}

void CDlgVoorhees::OnThresAt() 
{
	UpdateData(TRUE);
	((CTextonView*)pView)->DrawThresLoG(m_thres_log_at);
	
}

void CDlgVoorhees::OnGaussBlur() 
{
	UpdateData(TRUE);
	((CTextonView*)pView)->DrawBlur(m_gauss_sigma);
	
}
