// stdafx.h : include file for standard system include files,
//  or project specific include files that are used frequently, but
//      are changed infrequently
//

#if !defined(AFX_STDAFX_H__4087543B_D098_4A7A_B8F7_C051B8916C04__INCLUDED_)
#define AFX_STDAFX_H__4087543B_D098_4A7A_B8F7_C051B8916C04__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define VC_EXTRALEAN		// Exclude rarely-used stuff from Windows headers

#include <afxwin.h>         // MFC core and standard components
#include <afxext.h>         // MFC extensions
#include <afxdisp.h>        // MFC Automation classes
#include <afxdtctl.h>		// MFC support for Internet Explorer 4 Common Controls
#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>			// MFC support for Windows Common Controls
#endif // _AFX_NO_AFXCMN_SUPPORT

#include <vector>
#include <list>
#include <algorithm>
#include <string>

using namespace std;

typedef basic_string<unsigned char> unstring;

#include "DBAccess.h"
#include "ADODataBase.h"

#define	_SCB_REPLACE_MINIFRAME  ///浮动时候替换原来TOOL WINDOWS的框架
#include "sizecbar.h"
#include "scbarg.h"

#include "MsgOutput.h"

#include "PacketTransfer.h"

#include "GlobalFunc.h"

#include "HandlerMgr.h"
#include <afxsock.h>
#include <afxcontrolbars.h>
#include "LayoutCtrl.h"

//#include <XTToolkitPro.h>   // Codejock Software Components

CString TraceBuf(BYTE buf[],int len,char *szHint);

extern int g_nModuleControl;

enum Module_Code
{
	Module_SetChannel,
	Module_SetParam,
	Module_RndRead,
	Module_AutoCeShi,
	Module_RndRead_CurrentData,	
	Module_RndRead_TerminalInfo,
	Moudle_RndRead_HistoryData,
	Moudle_RndRead_EventData,
	Moudle_RndRead_AlarmData,
	Moudle_Test_Reread,
};

#define APP_TITLE		"广东电网公司厂站电能量采集终端综合检测软件"
#define MSG_DUPLICATE	"数据已存在，不能重复录入！"


//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_STDAFX_H__4087543B_D098_4A7A_B8F7_C051B8916C04__INCLUDED_)
