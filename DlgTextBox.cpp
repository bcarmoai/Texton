// DlgTextBox.cpp : implementation file
//

#include "stdafx.h"
#include "Texton.h"
#include "DlgTextBox.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDlgTextBox dialog


CDlgTextBox::CDlgTextBox(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgTextBox::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlgTextBox)
	//}}AFX_DATA_INIT
}


void CDlgTextBox::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgTextBox)
	DDX_Control(pDX, IDC_EDITTEXT, m_editText);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlgTextBox, CDialog)
	//{{AFX_MSG_MAP(CDlgTextBox)
	ON_WM_CLOSE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgTextBox message handlers

BOOL CDlgTextBox::PreCreateWindow(CREATESTRUCT& cs) 
{
	return CDialog::PreCreateWindow(cs);
}

void CDlgTextBox::CalcWindowRect(LPRECT lpClientRect, UINT nAdjustType) 
{
	CDialog::CalcWindowRect(lpClientRect, nAdjustType);
}

void CDlgTextBox::AddText(CString text)
{
	m_editText.ReplaceSel(text);
	m_editText.LineScroll(m_editText.GetLineCount());

}

void CDlgTextBox::ClearAllText()
{
	m_editText.SetSel(0,-1);
	m_editText.Clear();
}

BOOL CDlgTextBox::DestroyWindow() 
{
	*isActive = FALSE;

	return CDialog::DestroyWindow();
}

void CDlgTextBox::OnClose() 
{
	*isActive = FALSE;
	
	CDialog::OnClose();
}

void CDlgTextBox::OnCancel() 
{
	*isActive = FALSE;

	CDialog::OnCancel();
}
