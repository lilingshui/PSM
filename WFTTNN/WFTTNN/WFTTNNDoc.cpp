// WFTTNNDoc.cpp : implementation of the CWFTTNNDoc class
//

#include "stdafx.h"
#include "WFTTNN.h"

#include "WFTTNNDoc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CWFTTNNDoc

IMPLEMENT_DYNCREATE(CWFTTNNDoc, CDocument)

BEGIN_MESSAGE_MAP(CWFTTNNDoc, CDocument)
	//{{AFX_MSG_MAP(CWFTTNNDoc)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CWFTTNNDoc construction/destruction

CWFTTNNDoc::CWFTTNNDoc()
{
	// TODO: add one-time construction code here

}

CWFTTNNDoc::~CWFTTNNDoc()
{
}

BOOL CWFTTNNDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

	// TODO: add reinitialization code here
	// (SDI documents will reuse this document)

	return TRUE;
}



/////////////////////////////////////////////////////////////////////////////
// CWFTTNNDoc serialization

void CWFTTNNDoc::Serialize(CArchive& ar)
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
// CWFTTNNDoc diagnostics

#ifdef _DEBUG
void CWFTTNNDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CWFTTNNDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CWFTTNNDoc commands
