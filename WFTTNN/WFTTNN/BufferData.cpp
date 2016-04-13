// BufferData.cpp: implementation of the CBufferData class.
//
//////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "BufferData.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CBufferData::CBufferData()
{

}

CBufferData::~CBufferData()
{

}

void CBufferData::Add(BYTE *buf,int nLen)
{
    m_buf.insert(m_buf.end(),buf,buf+nLen);
}

int CBufferData::Find(int nBegin,BYTE byValue)
{
	for (int i=nBegin;i<m_buf.size();i++)
	{
		if (m_buf[i]==byValue)
			return i;
	}
	return -1;
}

BYTE CBufferData::operator[](int nPos)
{
	if (nPos>=m_buf.size())
		return 0;
	return m_buf[nPos];
}

int CBufferData::GetSize()
{
	return m_buf.size();
}

bool CBufferData::GetValue(int nPos,BYTE &byValue)
{
	if (nPos>=m_buf.size())
		return false;
	byValue=m_buf[nPos];
	return true;
}

//得到指定位置上的校验和
BYTE CBufferData::CheckSum(int nBegin,int nEnd)
{
	int nSum=0;
    for (int i=nBegin;i<=nEnd;i++)
		nSum+=m_buf[i];
	return nSum%256;
}

//移除一段数据
void CBufferData::Remove(int nBegin,int nLength)
{
    VEC_ITER itBegin=m_buf.begin()+nBegin;
	VEC_ITER itEnd=itBegin+nLength;
	m_buf.erase(itBegin,itEnd);
}

void CBufferData::GetBuffer(int nBegin,int nLen,BYTE buf[])
{
	for (int i=0;i<nLen;i++)
		buf[i]=m_buf[nBegin+i];
}