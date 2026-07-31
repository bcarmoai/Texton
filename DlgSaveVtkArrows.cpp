// DlgSaveVtkArrows.cpp : implementation file
//

#include "stdafx.h"
#include "texton.h"
#include "DlgSaveVtkArrows.h"
#include "DlgFindVortex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDlgSaveVtkArrows dialog


CDlgSaveVtkArrows::CDlgSaveVtkArrows(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgSaveVtkArrows::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlgSaveVtkArrows)
	m_imgSize = _T("");
	m_coordX = 0;
	m_coordY = 0;
	m_coordZbS = 0;
	m_bWholeData = FALSE;
	m_bS = FALSE;
	m_bX = FALSE;
	m_bY = FALSE;
	m_bHCFD = FALSE;
	//}}AFX_DATA_INIT
}


void CDlgSaveVtkArrows::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgSaveVtkArrows)
	DDX_Text(pDX, IDC_SAVE_VTK_ARROWS_IMGSIZE, m_imgSize);
	DDX_Text(pDX, IDC_SAVE_VTK_ARROWS_XCOORD, m_coordX);
	DDX_Text(pDX, IDC_SAVE_VTK_ARROWS_YCOORD, m_coordY);
	DDX_Text(pDX, IDC_SAVE_VTK_ARROWS_ZCOORD, m_coordZbS);
	DDX_Check(pDX, IDC_SAVE_VTK_ARROWS_3D, m_bWholeData);
	DDX_Check(pDX, IDC_SAVE_VTK_ARROWS_S, m_bS);
	DDX_Check(pDX, IDC_SAVE_VTK_ARROWS_X, m_bX);
	DDX_Check(pDX, IDC_SAVE_VTK_ARROWS_Y, m_bY);
	DDX_Check(pDX, IDC_SAVE_VTK_ARROWS_HCFD, m_bHCFD);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlgSaveVtkArrows, CDialog)
	//{{AFX_MSG_MAP(CDlgSaveVtkArrows)
	ON_WM_CANCELMODE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgSaveVtkArrows message handlers

void CDlgSaveVtkArrows::OnOK() 
{
	UpdateData(TRUE);

	CDialog::OnOK();
}

BOOL CDlgSaveVtkArrows::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	m_imgSize = mps_imgSize;
	UpdateData(FALSE);
	
	return TRUE;
	              
}

void CDlgSaveVtkArrows::OnCancelMode() 
{
	CDialog::OnCancelMode();
	
	// TODO: Add your message handler code here
	
}
