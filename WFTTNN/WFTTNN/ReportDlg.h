#pragma once

#include "WorkView.h"
#include "LayoutCtrl.h"
#include "resource.h"
#include "DlgAutoTest.h"
#include <map>

// CReportDlg dialog

class CReportDlg : public CDialog
{
	DECLARE_DYNAMIC(CReportDlg)

public:
	typedef struct ComReportParam
	{
		int res;
		CString dataitem;
	};

public:
	CReportDlg(int termId, int batchId, CWnd* pParent = NULL);   // standard constructor
	virtual ~CReportDlg();

// Dialog Data
	enum { IDD = IDD_REPORT_VIEW };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	void TitleReport(CRect *rect);
	void ConfigurationReport(CRect *rect);
	void PowerCompatibilityReport(CRect *rect);
	void CommunicationReport(CRect *rect);


	DECLARE_MESSAGE_MAP()
private:
	CWorkView *m_pWorkView;
	CLayoutCtrl m_layoutCtrl;
	int m_termId;
	int m_batchId;

	std::map <CString, ComReportParam> map_param;
public:
	virtual BOOL OnInitDialog();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnBnClickedButton1();

};
