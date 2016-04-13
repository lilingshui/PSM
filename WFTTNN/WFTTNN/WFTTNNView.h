// WFTTNNView.h : interface of the CWFTTNNView class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_WFTTNNVIEW_H__665D69DE_A249_4E70_9D25_8A34F0E1E574__INCLUDED_)
#define AFX_WFTTNNVIEW_H__665D69DE_A249_4E70_9D25_8A34F0E1E574__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "DlgTerminalInfo.h"
#include "DlgRndRead.h"
#include "DlgDataBrowse.h"
#include "DlgSetParam.h"
#include "DlgSetChannel.h"
#include "DlgBatchInfo.h"
#include "DlgReadCurrentData.h"
#include "DlgReadHistoryData.h"
#include "DlgReadEventData.h"
#include "DlgReadTermInfo.h"
#include "DlgAutoTest.h"
#include "DlgAutoTask.h"
#include "DlgOtherTestInfo.h"
#include "DlgVisualMeter.h"
#include "DlgTaskManage.h"
#include "DlgReadAlarmData.h"
#include "DlgRereadTest.h"


class CWFTTNNView : public CFormView
{
protected: // create from serialization only
	CWFTTNNView();
	DECLARE_DYNCREATE(CWFTTNNView)

public:
	//{{AFX_DATA(CWFTTNNView)
	enum{ IDD = IDD_WFTTNN_FORM };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA

// Attributes
public:
	CWFTTNNDoc* GetDocument();

// Operations
public:
	CDlgTerminalInfo *pDlgTerminalInfo;
	CDlgRndRead *pDlgRndRead;
	CDlgDataBrowse *pDlgDataBrowse;
	CDlgSetParam *pDlgSetParam;	
	CDlgSetChannel *pDlgSetChannel;
	CDlgBatchInfo *pDlgBatchInfo;
	CDlgReadCurrentData *pDlgReadCurrentData;
	CDlgReadHistoryData *pDlgReadHistoryData;
	CDlgReadEventData *pDlgReadEventData;
	CDlgReadTermInfo *pDlgReadTermData;
	CDlgAutoTest *pDlgAutoTest;
	CDlgAutoTask *pDlgAutoTask;
	CDlgOtherTestInfo *pDlgOtherTestInfo;
	CDlgVisualMeter *pDlgVisualMeter;
	CDlgTaskManage *pDlgTaskManage;	
	CDlgReadAlarmData *pDlgReadAlarmData;
	CDlgRereadTest *pDlgRereadTest;
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CWFTTNNView)
	public:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual void OnInitialUpdate(); // called first time after construct
	virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
	virtual void OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnEndPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnPrint(CDC* pDC, CPrintInfo* pInfo);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CWFTTNNView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:
	void InitialDlgObject();

// Generated message map functions
protected:
	//{{AFX_MSG(CWFTTNNView)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnSize(UINT nType, int cx, int cy);
};

#ifndef _DEBUG  // debug version in WFTTNNView.cpp
inline CWFTTNNDoc* CWFTTNNView::GetDocument()
   { return (CWFTTNNDoc*)m_pDocument; }
#endif

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_WFTTNNVIEW_H__665D69DE_A249_4E70_9D25_8A34F0E1E574__INCLUDED_)
