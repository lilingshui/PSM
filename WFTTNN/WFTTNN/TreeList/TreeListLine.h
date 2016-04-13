#pragma once

//#include "CommonUI_Global.h"
class CTreeList;
class CTreeListProp;
class CTreeListLine
{
public:
	CTreeListLine(CTreeList *pTreeList);
	~CTreeListLine(void);

	void SetItemText(int nIndex,CString strText);
	CString GetItemText(int nIndex);

	CTreeListProp *GetItemProp(int nIndex);
	void SetItemProp(int nIndex,CTreeListProp *pProp);

	void SetLineData(DWORD dwData);
	DWORD GetLineData();

	HTREEITEM GetTreeItem();
	void SetTreeItem(HTREEITEM hTreeItem);

	COLORREF GetTextColor();
	void SetTextColor(COLORREF color);
	COLORREF GetBKColor();
	void SetBKColor(COLORREF color);

	CTreeList *GetTreeList();

	void OnValueChanged(CRect &rect, int Column);

protected:
	map<int, CString> m_mapItemText;
	map<int, CTreeListProp *> m_mapItemProp;
	HTREEITEM m_hTreeItem;
	COLORREF m_colorText;
	COLORREF m_colorBK;
	CTreeList *m_pTreeList;
	DWORD m_dwUserData;
};
