#pragma once
//#include "CommonUI_Global.h"

class CTreeListLine;
class CTreeListProp
{
public:
	enum PropType
	{
		PropType_Edit = 0,
		PropType_ComboBox = 1,
		PropType_DateTime = 2,
		PropType_IPAddress = 3,
	};
	CTreeListProp(void);
	virtual ~CTreeListProp(void);

	void SetColumnIndex(int nColumnIndex);
	void SetTreeListLine(CTreeListLine *pTreeListLine);

	int GetColumnIndex();
	CTreeListLine *GetTreeListLine();

	virtual void OnEditStart(CRect &rect) = 0;
	virtual void OnEditOver() = 0;
	virtual int GetPropType() = 0;
	virtual CString GetFormatValue() = 0;
protected:
	int m_nColumnIndex;
	CTreeListLine *m_pTreeListLine;
};
