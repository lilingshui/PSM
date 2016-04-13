#ifndef ITEM_DATA_H_INC
#define ITEM_DATA_H_INC

enum ItemType {
	ITEM_TYPE_GONGBIAN,
	ITEM_TYPE_BATCH,
	ITEM_TYPE_TERMINAL,	
};

struct ItemData {
	ItemType m_type;
	int m_id;

	ItemData(ItemType type, int id)
	{
		m_type = type;
		m_id = id;
	}
};

struct TerminalItemData : ItemData {	
	CString m_addr;
	//CString t_addr;//MODIFY BY WM 20131126

	TerminalItemData(ItemType type, int id, CString &addr) : ItemData(type, id)
	{		
		m_addr = addr;
		//t_addr = teraddr;
	}
};

#endif
