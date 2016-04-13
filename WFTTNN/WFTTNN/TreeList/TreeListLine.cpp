#include "StdAfx.h"
#include "TreeListLine.h"
#include "TreeListProp.h"
#include "PropEdit.h"
#include "PropIPAddress.h"
#include "PropComboBox.h"
#include "PropDateTime.h"
#include "TreeList.h"

CTreeListLine::CTreeListLine(CTreeList *pTreeList)
{
	m_hTreeItem = NULL;
	m_colorText = ::GetSysColor(COLOR_WINDOWTEXT);
	m_colorBK = RGB(255, 255, 255);
	m_pTreeList = pTreeList;
	m_dwUserData = 0;
}

CTreeListLine::~CTreeListLine(void)
{
	map <int,CTreeListProp *>::iterator it;
	for(it = m_mapItemProp.begin(); it != m_mapItemProp.end(); it++)
	{
		CTreeListProp *pProp = it->second;
		if (pProp == NULL)
			continue;

		if (pProp->GetPropType() == CTreeListProp::PropType_Edit)
		{
			CPropEdit *p = (CPropEdit *)pProp;
			delete p;
		}
		else if (pProp->GetPropType() == CTreeListProp::PropType_ComboBox)
		{
			CPropIPAddress *p = (CPropIPAddress *)pProp;
			delete p;
		}
		else if (pProp->GetPropType() == CTreeListProp::PropType_DateTime)
		{
			CPropComboBox *p = (CPropComboBox *)pProp;
			delete p;
		}
		else if (pProp->GetPropType() == CTreeListProp::PropType_IPAddress)
		{
			CPropDateTime *p = (CPropDateTime *)pProp;
			delete p;
		}
	}
}

CTreeList *CTreeListLine::GetTreeList()
{
	return m_pTreeList;
}

void CTreeListLine::SetItemText(int nIndex,CString strText)
{
	m_mapItemText[nIndex] = strText;
	if (m_pTreeList)
		m_pTreeList->Invalidate();
}

CString CTreeListLine::GetItemText(int nIndex)
{
	return m_mapItemText[nIndex];
}

CTreeListProp *CTreeListLine::GetItemProp(int nIndex)
{
	map<int, CTreeListProp *>::iterator it = m_mapItemProp.find(nIndex);
	if (it != m_mapItemProp.end())
		return it->second;
	return NULL;
}

void CTreeListLine::SetItemProp(int nIndex,CTreeListProp *pProp)
{
	if (pProp == NULL)
		return ;
	pProp->SetColumnIndex(nIndex);
	pProp->SetTreeListLine(this);
	m_mapItemProp[nIndex] = pProp;

	SetItemText(nIndex,pProp->GetFormatValue());
}


void CTreeListLine::SetLineData(DWORD dwData)
{
	m_dwUserData = dwData;
}

DWORD CTreeListLine::GetLineData()
{
	return m_dwUserData;
}

HTREEITEM CTreeListLine::GetTreeItem()
{
	return m_hTreeItem;
}

void CTreeListLine::SetTreeItem(HTREEITEM hTreeItem)
{
	m_hTreeItem = hTreeItem;
}

COLORREF CTreeListLine::GetTextColor()
{
	return m_colorText;
}

void CTreeListLine::SetTextColor(COLORREF color)
{
	m_colorText = color;
}

COLORREF CTreeListLine::GetBKColor()
{
	return m_colorBK;
}

void CTreeListLine::SetBKColor(COLORREF color)
{
	m_colorBK = color;
}