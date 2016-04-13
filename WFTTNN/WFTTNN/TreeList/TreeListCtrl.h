#pragma once


// CTreeListView
//#include "CommonUI_Global.h"
#include "TreeList.h"
#include "TreeListHeader.h"
#include "TreeListLine.h"

#define UM_ITEM_L_CLICK WM_USER+546
#define UM_ITEM_R_CLICK WM_USER+547
#define UM_ITEM_LDB_CLICK WM_USER+548
#define UM_ITEM_PROP_CHANGED WM_USER+549

class CTreeListCtrl : public CWnd
{
	DECLARE_DYNAMIC(CTreeListCtrl)

public:
	CTreeListCtrl();
	virtual ~CTreeListCtrl();

protected:
	DECLARE_MESSAGE_MAP()

public:
	CTreeList & GetTreeList();
	CTreeListHeader & GetHeader();
	CWnd *GetMessageRecver();
	void SetMessageRecver(CWnd *pWnd);

	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	

	int InsertColumn(int nCol,CString strColumnText,int nFormat = LVCFMT_LEFT,int nWidth = -1);
	CTreeListLine *InsertLine(CString strLineText,int nImage = -1, int nSelectedImage = -1,
								CTreeListLine *pParent = NULL,CTreeListLine *pInsertAfter = NULL);

	//遍历用CTreeList对象

	void DeleteLine(CTreeListLine *pLine);
	void DeleteLine(HTREEITEM hItem);
	void DeleteAll();

	CTreeListLine * GetLine(HTREEITEM hItem);
	
	int GetChildCount(CTreeListLine *pLine);

	void UpdateCtrl();

protected:
	void ResetScrollBar();
	BOOL VerticalScrollVisible();

	CTreeList m_TreeList;
	CTreeListHeader m_Header;
	CWnd *m_pMessageRecver;
	CFont m_headerFont;
	vector <CTreeListLine *> m_vLine;

	virtual BOOL OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult);
	afx_msg LRESULT OnNMItemLDBClick(WPARAM wParam,LPARAM lParam);
	afx_msg LRESULT OnNMItemLClick(WPARAM wParam,LPARAM lParam);
	afx_msg LRESULT OnNMItemRClick(WPARAM wParam,LPARAM lParam);
	afx_msg LRESULT OnNMItemPropChanged(WPARAM wParam,LPARAM lParam);

public:
	afx_msg void OnSize(UINT nType, int cx, int cy);
};


