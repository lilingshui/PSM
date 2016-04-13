#pragma once
#include "afxcmn.h"
#include "EventRecver.h"
#include "afxwin.h"

// CDlgVisualMeter 对话框

class CDlgVisualMeter : public CDialog, public CEventRecver
{
	DECLARE_DYNAMIC(CDlgVisualMeter)

public:
	CDlgVisualMeter(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CDlgVisualMeter();

// 对话框数据
	enum { IDD = IDD_FORM_VISUAL_METER };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	virtual BOOL OnInitDialog();
	virtual void OnOK()	{}
	virtual void OnCancel()	{}
	virtual void OnEvent(unsigned int nEventID,	void *pParam);

	DECLARE_MESSAGE_MAP()

private:
	void InitDataItemList();
	void InitMeterList();

	int m_nTermNo;
	CListCtrl m_ctrlDataItemList;
	CListCtrl m_ctrlResult;
	CComboBox m_cmbMeterList;
};
