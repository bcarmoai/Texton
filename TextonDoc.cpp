// TextonDoc.cpp : implementation of the CTextonDoc class
//

#include "stdafx.h"
#include "Texton.h"

#include "TextonDoc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CTextonDoc

IMPLEMENT_DYNCREATE(CTextonDoc, CDocument)

BEGIN_MESSAGE_MAP(CTextonDoc, CDocument)
	//{{AFX_MSG_MAP(CTextonDoc)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTextonDoc construction/destruction

CTextonDoc::CTextonDoc()
{
	// TODO: add one-time construction code here

}

CTextonDoc::~CTextonDoc()
{
}

BOOL CTextonDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

	// TODO: add reinitialization code here
	// (SDI documents will reuse this document)

	return TRUE;
}



/////////////////////////////////////////////////////////////////////////////
// CTextonDoc serialization

void CTextonDoc::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{
		// TODO: add storing code here
	}
	else
	{
		// TODO: add loading code here
	}
}

/////////////////////////////////////////////////////////////////////////////
// CTextonDoc diagnostics

#ifdef _DEBUG
void CTextonDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CTextonDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CTextonDoc commands
