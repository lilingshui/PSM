#include "stdafx.h"
#include "CheckListCtrl.h"
#include "resource.h"


CCheckListCtrl::CCheckListCtrl(void)
{
}


CCheckListCtrl::~CCheckListCtrl(void)
{
}
BEGIN_MESSAGE_MAP(CCheckListCtrl, CListCtrl)
	ON_NOTIFY_REFLECT(NM_RCLICK, &CCheckListCtrl::OnNMRClick)
	ON_COMMAND(ID_MENUITEM_SELECTALL, &CCheckListCtrl::OnMenuitemSelectall)
	ON_COMMAND(ID_MENUITEM_SELECTINVERT, &CCheckListCtrl::OnMenuitemSelectinvert)
END_MESSAGE_MAP()


void CCheckListCtrl::OnNMRClick(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);

	CMenu menu;
	POINT pt = {0};
	GetCursorPos(&pt);	
	menu.LoadMenuA(IDR_MENU_CHECKLIST);
	menu.GetSubMenu(0)->TrackPopupMenu(0, pt.x, pt.y, this);

	*pResult = 0;
}


void CCheckListCtrl::OnMenuitemSelectall()
{
	for (int i = 0; i < this->GetItemCount(); i++)
	{
		this->GetCheck(i) ? true : this->SetCheck(i);		
	}
}


void CCheckListCtrl::OnMenuitemSelectinvert()
{
	for (int i = 0; i < this->GetItemCount(); i++)
	{
		this->GetCheck(i) ? this->SetCheck(i, FALSE) : this->SetCheck(i, TRUE);		
	}
}
