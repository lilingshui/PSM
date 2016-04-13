#ifndef FRAME_H
#define FRAME_H

class IFrame
{
public:
	IFrame(){}
	virtual ~IFrame(){}
	virtual void MakeFrame(BYTE buf[],int &len)=0;
	virtual void ParseFrame(BYTE buf[],int len)=0;
	virtual DWORD GetTermAddr()=0;
	virtual int GetMasterAddr()=0;
};
#endif//FRAME_H