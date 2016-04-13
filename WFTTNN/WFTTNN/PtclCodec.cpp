#include "stdafx.h"
#include "PtclCodec.h"

CPtclCodec::CPtclCodec(void)
{
}

CPtclCodec::~CPtclCodec(void)
{
}
/*
int CPtclCodec::GetPtclClassID(int nPtclID)
{
	if (nPtclID == Ptcl_ID_Integrate)
	{
		return Ptcl_Class_Integrate;
	}

	if (nPtclID == Ptcl_ID_Integrate_old)
	{
		return Ptcl_Class_Integrate_old;
	}

	if (nPtclID == Ptcl_ID_Official96)
	{
		return Ptcl_Class_Official96;
	}

	if (nPtclID == Ptcl_ID_719 || nPtclID == Ptcl_ID_719_Ex)
	{
		return Ptcl_Class_719;
	}

	if (nPtclID == Ptcl_ID_Willfar_102)
	{
		return Ptcl_Class_WF102;
	}

	if (nPtclID == Ptcl_ID_Official05_SX ||
		nPtclID == Ptcl_ID_Official_HNDY ||
		nPtclID == Ptcl_ID_Official04_HN ||
		nPtclID == Ptcl_ID_YNDY ||
		nPtclID == Ptcl_ID_Official698)
	{
		return Ptcl_Class_Official;
	}
	
	if (nPtclID == Ptcl_ID_ZJ_PB   ||
		nPtclID == Ptcl_ID_ZJ_PB_0903   ||
		nPtclID == Ptcl_ID_ZJ_GDDY ||
		nPtclID == Ptcl_ID_ZJ_02   ||
		nPtclID == Ptcl_ID_ZJ_Willfar ||
		nPtclID == Ptcl_ID_ZJ ||
		nPtclID == Ptcl_ID_ZJ_0903)
	{
		return Ptcl_Class_ZJ;
	}
	return 0;
}

auto_ptr<CPtclPacket> CPtclCodec::GetPtclPacketByClassID(int classID)
{
	if (classID == Ptcl_Class_Integrate)
	{
		auto_ptr<CPtclPacket> ptclPacket(new CPtclPacket_Integrate);
		return ptclPacket;
	}

	if (classID == Ptcl_Class_Integrate_old)
	{
		auto_ptr<CPtclPacket> ptclPacket(new CPtclPacket_Integrate_old);
		return ptclPacket;
	}

	if (classID == Ptcl_Class_Official96)
	{
		auto_ptr<CPtclPacket> ptclPacket(new CPtclPacket_Official96);
		return ptclPacket;
	}

	if (classID == Ptcl_Class_719)
	{
		auto_ptr<CPtclPacket> ptclPacket(new CPtclPacket_719);
		return ptclPacket;
	}

	if (classID == Ptcl_Class_WF102)
	{
		auto_ptr<CPtclPacket> ptclPacket(new CPtclPacket_WF102);
		return ptclPacket;
	}


	if (classID == Ptcl_Class_Official)
	{
		auto_ptr<CPtclPacket> ptclPacket(new CPtclPacket_Official);
		return ptclPacket;
	}
	
	if (classID == Ptcl_Class_ZJ)
	{
		auto_ptr<CPtclPacket> ptclPacket(new CPtclPacket_ZJ);
		return ptclPacket;
	}

	if (classID == Ptcl_Class_EQ)
	{
		auto_ptr<CPtclPacket> ptclPacket(new CPtclPacket_EQ);
		return ptclPacket;
	}


	auto_ptr<CPtclPacket> ptclPacket;
	return ptclPacket;
}

auto_ptr<CPtclPacket> CPtclCodec::GetPtclPacket(int nPtclID)
{
	int classID = GetPtclClassID(nPtclID);
	return GetPtclPacketByClassID(classID);
}
*/

unsigned char CPtclCodec::binToBcd(unsigned char binCode)
{
	return ((binCode/10)*16 + binCode%10);
}

unsigned char CPtclCodec::bcdToBin(unsigned char bcdCode)
{
	return ((bcdCode/16)*10 + bcdCode%16);
}