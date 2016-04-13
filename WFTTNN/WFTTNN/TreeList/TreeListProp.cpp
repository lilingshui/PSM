#include "StdAfx.h"
#include "TreeListProp.h"

CTreeListProp::CTreeListProp(void)
{
	m_nColumnIndex = -1;
	m_pTreeListLine = NULL;
}

CTreeListProp::~CTreeListProp(void)
{
}

void CTreeListProp::SetColumnIndex(int nColumnIndex)
{
	m_nColumnIndex = nColumnIndex;
}

void CTreeListProp::SetTreeListLine(CTreeListLine *pTreeListLine)
{
	m_pTreeListLine = pTreeListLine;
}

int CTreeListProp::GetColumnIndex()
{
	return m_nColumnIndex;
}

CTreeListLine *CTreeListProp::GetTreeListLine()
{
	return m_pTreeListLine;
}