// WFTTNNDoc.h : interface of the CWFTTNNDoc class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_WFTTNNDOC_H__7D235508_DC00_402F_BC55_4B1D2D851FFF__INCLUDED_)
#define AFX_WFTTNNDOC_H__7D235508_DC00_402F_BC55_4B1D2D851FFF__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


class CWFTTNNDoc : public CDocument
{
protected: // create from serialization only
	CWFTTNNDoc();
	DECLARE_DYNCREATE(CWFTTNNDoc)

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CWFTTNNDoc)
	public:
	virtual BOOL OnNewDocument();
	virtual void Serialize(CArchive& ar);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CWFTTNNDoc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	//{{AFX_MSG(CWFTTNNDoc)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_WFTTNNDOC_H__7D235508_DC00_402F_BC55_4B1D2D851FFF__INCLUDED_)
