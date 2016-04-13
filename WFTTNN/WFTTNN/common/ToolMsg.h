#ifndef TOOL_MSG_H
#define TOOL_MSG_H
#include "vector"

using namespace std;

typedef vector<CString> STR_VEC;
typedef vector<CString>::iterator STR_ITER;

enum
{
    MSG_SC_GET_IP_LIST=20
};

//=================================================================
enum
{
    MSG_CF_ADD_COPY_FILE=40,
	MSG_CF_ADD_DOWNLOAD_FILE,
	MSG_CF_ADD_HTTP_FILE
};
//=================================================================
typedef struct _SHARE_FILE_INFO
{
	char    szFile[256];
	DWORD   dwSize;
	char    szUser[32];
	DWORD   dwIPAddr;
	WORD    wPort;
}SHARE_FILE_INFO;

typedef vector<SHARE_FILE_INFO*> SFI_VEC;

enum
{
	PT_COPY_FILE,
	PT_USER_CHAT
};

typedef struct _PACKET_DATA_HEADER
{
	int nType;
	int nLength;
	BYTE *GetData(){return (BYTE *)(this+1);}
}PACKET_DATA_HEADER;

typedef struct _COPY_FILE_REQ
{
	char  szFile[256];
	DWORD dwOffset;
}COPY_FILE_REQ;

typedef struct _COPY_FILE_ACK
{
	int nStatus;
	DWORD dwLength;
	BYTE *GetData(){return (BYTE *)(this+1);}
}COPY_FILE_ACK;
#endif//TOOL_MSG_H