// TreeListHeaderCtrl.cpp : 实现文件
//

#include "stdafx.h"
#include <CommonUI/TreeListHeader.h>
#include "..\include\commonui\treelistheader.h"


// CTreeListHeaderCtrl

IMPLEMENT_DYNAMIC(CTreeListHeader, CXTFlatHeaderCtrl)
CTreeListHeader::CTreeListHeader()
{
}

CTreeListHeader::~CTreeListHeader()
{
}


BEGIN_MESSAGE_MAP(CTreeListHeader, CXTFlatHeaderCtrl)
	ON_WM_CREATE()
END_MESSAGE_MAP()

// CTreeListHeaderCtrl 消息处理程序

int CTreeListHeader::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CXTFlatHeaderCtrl::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	return 0;
}

int CTreeListHeader::GetColumnsWidth()
{
	int nTatalWidth = 0;
	int nCount = GetItemCount();
	for (int n=0;n<nCount;n++)
	{
		HD_ITEM hItem;
		hItem.mask = HDI_WIDTH;
		GetItem(n, &hItem);
		nTatalWidth = nTatalWidth + hItem.cxy;
	}
	
	return nTatalWidth;
}

int CTreeListHeader::GetColumnWidth(int nIndex)
{
	HD_ITEM hItem;
	hItem.mask = HDI_WIDTH;
	GetItem(nIndex, &hItem);
	return hItem.cxy;
}

int CTreeListHeader::GetColumnIndex(int xPos)
{
	int nCount = GetItemCount();
	int nTatal = 0;
	for (int n=0;n<nCount;n++)
	{
		int nWidth = nTatal + GetColumnWidth(n);
		if (xPos >= nTatal && xPos < nWidth)
			return n;
		nTatal =  nWidth;
	}
	return -1;
}