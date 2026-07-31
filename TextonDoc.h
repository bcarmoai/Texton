// TextonDoc.h : interface of the CTextonDoc class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_TEXTONDOC_H__68D2C4FF_8540_4F56_A812_3A80F3CF9085__INCLUDED_)
#define AFX_TEXTONDOC_H__68D2C4FF_8540_4F56_A812_3A80F3CF9085__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000


class CTextonDoc : public CDocument
{
protected: // create from serialization only
	CTextonDoc();
	DECLARE_DYNCREATE(CTextonDoc)

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CTextonDoc)
	public:
	virtual BOOL OnNewDocument();
	virtual void Serialize(CArchive& ar);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CTextonDoc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	//{{AFX_MSG(CTextonDoc)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TEXTONDOC_H__68D2C4FF_8540_4F56_A812_3A80F3CF9085__INCLUDED_)
