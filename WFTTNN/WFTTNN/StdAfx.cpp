// stdafx.cpp : source file that includes just the standard includes
//	WFTTNN.pch will be the pre-compiled header
//	stdafx.obj will contain the pre-compiled type information

#include "stdafx.h"


CString TraceBuf(BYTE buf[],int len,char *szHint)
{
	CString strBuf;
	strBuf.Format("%s: ",szHint);
	for (int i=0;i<len;i++)
	{
		CString str;
		str.Format("%02X ",buf[i]);
		strBuf+=str;
	}
	return strBuf;
}



