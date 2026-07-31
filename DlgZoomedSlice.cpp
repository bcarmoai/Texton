// DlgZoomedSlice.cpp : implementation file
//

#include "stdafx.h"
#include "texton.h"
#include "DlgZoomedSlice.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDlgZoomedSlice dialog


CDlgZoomedSlice::CDlgZoomedSlice(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgZoomedSlice::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlgZoomedSlice)
	m_sImgSize = _T("");
	m_z = 1;
	m_x = 0;
	m_y = 0;
	//}}AFX_DATA_INIT
}


void CDlgZoomedSlice::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgZoomedSlice)
	DDX_Text(pDX, IDC_ZOOMED_SLICE_IMGSIZE, m_sImgSize);
	DDX_Text(pDX, IDC_ZOOM_ZCOORD, m_z);
	DDV_MinMaxInt(pDX, m_z, 1, 100);
	DDX_Text(pDX, IDC_ZOOM_XCOORD, m_x);
	DDX_Text(pDX, IDC_ZOOM_YCOORD, m_y);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlgZoomedSlice, CDialog)
	//{{AFX_MSG_MAP(CDlgZoomedSlice)
	ON_BN_CLICKED(IDOK, OnOk)
	ON_EN_SETFOCUS(IDC_ZOOM_ZCOORD, OnSetfocusZ)
	ON_EN_SETFOCUS(IDC_ZOOM_XCOORD, OnSetfocusX)
	ON_EN_SETFOCUS(IDC_ZOOM_YCOORD, OnSetfocusY)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgZoomedSlice message handlers

void CDlgZoomedSlice::OnOk() 
{
	UpdateData(TRUE);

	if(	((CButton*)GetDlgItem(IDC_ZOOMED_S))->GetCheck() == 1)
		m_dirSelect = 1;
	else
		if(	((CButton*)GetDlgItem(IDC_ZOOMED_X))->GetCheck() == 1)
			m_dirSelect = 2;
		else
			if(	((CButton*)GetDlgItem(IDC_ZOOMED_Y))->GetCheck() == 1)
				m_dirSelect = 3;
			else
				if(	((CButton*)GetDlgItem(IDC_ZOOMED_3D))->GetCheck() == 1)
					m_dirSelect = 4;
				else
					m_dirSelect = 0;

	CDialog::OnOK();

}

BOOL CDlgZoomedSlice::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	((CButton*)GetDlgItem(IDC_ZOOMED_3D))->SetCheck(1);
	m_sImgSize = m_psImgSize;
	UpdateData(FALSE);

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CDlgZoomedSlice::OnSetfocusZ() 
{
	((CButton*)GetDlgItem(IDC_ZOOMED_S))->SetCheck(1);	
	((CButton*)GetDlgItem(IDC_ZOOMED_X))->SetCheck(0);	
	((CButton*)GetDlgItem(IDC_ZOOMED_Y))->SetCheck(0);	
}

void CDlgZoomedSlice::OnSetfocusX() 
{
	((CButton*)GetDlgItem(IDC_ZOOMED_X))->SetCheck(1);	
	((CButton*)GetDlgItem(IDC_ZOOMED_S))->SetCheck(0);	
	((CButton*)GetDlgItem(IDC_ZOOMED_Y))->SetCheck(0);	
}

void CDlgZoomedSlice::OnSetfocusY() 
{
	((CButton*)GetDlgItem(IDC_ZOOMED_Y))->SetCheck(1);	
	((CButton*)GetDlgItem(IDC_ZOOMED_X))->SetCheck(0);	
	((CButton*)GetDlgItem(IDC_ZOOMED_S))->SetCheck(0);	
}
