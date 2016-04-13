#if !defined(AFX_DLGAUTOTASK_H__BCC08FDD_BC79_4329_82B4_1966F4D67558__INCLUDED_)
#define AFX_DLGAUTOTASK_H__BCC08FDD_BC79_4329_82B4_1966F4D67558__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DlgAutoTask.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDlgAutoTask dialog
#pragma pack (1)
/*
*/
typedef struct 
{
	CString strParamID;
	CString strParmName;
	CString striID;
}PARAM_TYPE;
#pragma pack ()

class CDlgAutoTask : public CDialog
{
// Construction
public:
	CDlgAutoTask(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDlgAutoTask)
	enum { IDD = IDD_DIALOG_AUTOTASK };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA
	CListCtrl	m_TaskListCtrl;
	CListCtrl	m_TaskItemListCtrl;
	CString		m_EdTaskName;
	CButton		m_BtnAddTask;
	CButton		m_BtnDelTask;
	CButton		m_BtnAddStep;
	CButton		m_BtnSaveStep;
	CButton		m_BtnDelStep;
	CDateTimeCtrl	m_EndDate;
	CDateTimeCtrl	m_EndTime;
	CDateTimeCtrl	m_StartTime;
	CDateTimeCtrl	m_StartDate;
	CComboBox	m_cbDataType;
	CComboBox	m_cbDataTypeHis;
	CButton		m_rdCur;
	CButton		m_rdHis;


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgAutoTask)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

	void InitTaskList();
	int VarianttoStr(_variant_t val,char *str);
	void AddOneTaskItem(CString strTasId,CString strCRS_NO,CString strCRS_DATA_TYPE,CString strPARAM_ID,CString strPARAM_TYPE,CString strCRS_S_TIME,CString strCRS_E_TIME);
	void InitialDataType(int itype);
	void SetRdState(int itype);
	int GetTaskidMaxItmNo(CString strTaskId);
	void AddOneTaskItemToDB(CString strTasId,CString strCRS_NO,CString strCRS_DATA_TYPE,CString strPARAM_ID,CString strPARAM_TYPE,CString strCRS_S_TIME,CString strCRS_E_TIME);
	void ModOneTaskItem(int icount,CString strCRS_DATA_TYPE,CString strPARAM_ID,CString strPARAM_TYPE,CString strCRS_S_TIME,CString strCRS_E_TIME);
	
// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDlgAutoTask)
	virtual BOOL OnInitDialog();
	afx_msg void OnAddtask();
	afx_msg void OnDeltask();
	afx_msg void OnClickListTask(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnRdcur();
	afx_msg void OnRdhis();
	afx_msg void OnAddstep();
	afx_msg void OnSavestep();
	afx_msg void OnClickTaskitem(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnDelstep();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGAUTOTASK_H__BCC08FDD_BC79_4329_82B4_1966F4D67558__INCLUDED_)
