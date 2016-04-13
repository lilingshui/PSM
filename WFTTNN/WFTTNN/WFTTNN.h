// WFTTNN.h : main header file for the WFTTNN application
//

#if !defined(AFX_WFTTNN_H__9927E00A_B7FB_45CE_AC71_B1781F776850__INCLUDED_)
#define AFX_WFTTNN_H__9927E00A_B7FB_45CE_AC71_B1781F776850__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"       // main symbols

/////////////////////////////////////////////////////////////////////////////
// CWFTTNNApp:
// See WFTTNN.cpp for the implementation of this class
//

class CWFTTNNApp : public CWinApp
{
public:
	CWFTTNNApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CWFTTNNApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementation
	//{{AFX_MSG(CWFTTNNApp)
	afx_msg void OnAppAbout();
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


bool IsValidData(double val);


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_WFTTNN_H__9927E00A_B7FB_45CE_AC71_B1781F776850__INCLUDED_)
