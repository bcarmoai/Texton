// DlgArcFit.cpp : implementation file
//

#include "stdafx.h"
#include "Texton.h"
#include "DlgArcFit.h"
#include "TextonView.h"
#include <math.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDlgArcFit dialog

CDlgArcFit* pCbkArcFit;

void callback_CDlgArcFit(char* text)
{
	pCbkArcFit->CallbackText(text);
}

CDlgArcFit::CDlgArcFit(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgArcFit::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlgArcFit)
	m_minPoints = 25;
	m_maxError = 15.0f;
	m_ProgressText = _T("");
	m_ImgSizeTxt = _T("");
	m_maxPoints = 25;
	//}}AFX_DATA_INIT
}


void CDlgArcFit::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgArcFit)
	DDX_Text(pDX, IDC_ARC_MIN_PNTS, m_minPoints);
	DDX_Text(pDX, IDC_ARC_MAX_ERROR, m_maxError);
	DDX_Text(pDX, IDC_ARCFIT_TEXT, m_ProgressText);
	DDX_Text(pDX, IDC_ARCFIT_IMGSIZE, m_ImgSizeTxt);
	DDX_Text(pDX, IDC_ARC_MAX_PNTS, m_maxPoints);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlgArcFit, CDialog)
	//{{AFX_MSG_MAP(CDlgArcFit)
	ON_BN_CLICKED(ID_APPLY, OnApply)
	ON_BN_CLICKED(IDC_BUTTON1, OnButton1)
	ON_BN_CLICKED(IDC_ARCFIT_REDRAW, OnArcfitRedraw)
	ON_BN_CLICKED(IDC_ARCFIT_CANNY, OnArcfitCanny)
	ON_BN_CLICKED(IDC_ARCFIT_BINS, OnArcfitBins)
	ON_BN_CLICKED(IDC_ARCFIT_ORIGREEN, OnArcfitOrigreen)
	ON_BN_CLICKED(IDC_ARCFIT_ORYTOCANNY, OnArcfitOrytocanny)
	ON_BN_CLICKED(IDC_ARCFIT_CLAIMS, OnArcfitClaims)
	ON_BN_CLICKED(IDC_ARCFIT_CLAIMARCS, OnArcfitClaimarcs)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgArcFit message handlers

void CDlgArcFit::OnApply() 
{
	UpdateData(TRUE);
	pCbkArcFit = this;
	char sz266[100];
	sprintf(sz266, "image %ix%i", ((CTextonView*)pView)->pgmImage.imgWidth, 
		((CTextonView*)pView)->pgmImage.imgHeight);
	m_ImgSizeTxt = sz266;
	((CTextonView*)pView)->DrawArcFit(m_minPoints, m_maxPoints, m_maxError, callback_CDlgArcFit);

}

void CDlgArcFit::CallbackText(char * text)
{
	m_ProgressText = text;
	UpdateData(FALSE);
}	

void CDlgArcFit::OnButton1() 
{
	((CTextonView*)pView)->DrawPgm();
}

void CDlgArcFit::OnArcfitRedraw() 
{
	((CTextonView*)pView)->DrawArcsStored();	
}

void CDlgArcFit::OnArcfitCanny() 
{
	((CTextonView*)pView)->DrawCannyStored();	
}

void CDlgArcFit::OnArcfitBins() 
{
	((CTextonView*)pView)->DrawArcBins(callback_CDlgArcFit);	
}

void CDlgArcFit::OnArcfitOrigreen() 
{
	((CTextonView*)pView)->DrawOriginalMaxBin(callback_CDlgArcFit);	
}

void CDlgArcFit::OnArcfitOrytocanny() 
{
	((CTextonView*)pView)->CopyOriginalToCanny();	
}

void CDlgArcFit::OnArcfitClaims() 
{
	((CTextonView*)pView)->DrawArcFitSeedClaims();	
}

void CDlgArcFit::OnArcfitClaimarcs() 
{
	((CTextonView*)pView)->DrawArcFitClaimArcs();	
}
