#pragma once

#include "PtclCodec_Global.h"
#include "PtclPacket.h"

class PTCLCODEC_EXPORT  CPtclCodec
{
public:
	CPtclCodec(void);
	~CPtclCodec(void);
/*
	static auto_ptr<CPtclPacket> GetPtclPacket(int nPtclID);
	static auto_ptr<CPtclPacket> GetPtclPacketByClassID(int classID);
	static int GetPtclClassID(int nPtclID);
*/
	static unsigned char binToBcd(unsigned char binCode);
	static unsigned char bcdToBin(unsigned char bcdCode);
};