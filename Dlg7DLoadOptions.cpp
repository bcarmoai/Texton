// Dlg7DLoadOptions.cpp : implementation file
//

#include "stdafx.h"
#include "texton.h"
#include "Dlg7DLoadOptions.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDlg7DLoadOptions dialog


CDlg7DLoadOptions::CDlg7DLoadOptions(CWnd* pParent /*=NULL*/)
	: CDialog(CDlg7DLoadOptions::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlg7DLoadOptions)
	m_timeFrame = 1;
	//}}AFX_DATA_INIT
}


void CDlg7DLoadOptions::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlg7DLoadOptions)
	DDX_Text(pDX, IDC_RESTOR_7D_TIME, m_timeFrame);
	DDV_MinMaxInt(pDX, m_timeFrame, 1, 12);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlg7DLoadOptions, CDialog)
	//{{AFX_MSG_MAP(CDlg7DLoadOptions)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlg7DLoadOptions message handlers
