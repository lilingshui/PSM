# Microsoft Developer Studio Project File - Name="WFTTNN" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=WFTTNN - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "WFTTNN.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "WFTTNN.mak" CFG="WFTTNN - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "WFTTNN - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "WFTTNN - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "WFTTNN - Win32 Release"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 6
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /Yu"stdafx.h" /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x804 /d "NDEBUG" /d "_AFXDLL"
# ADD RSC /l 0x804 /d "NDEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /machine:I386
# ADD LINK32 /nologo /subsystem:windows /machine:I386

!ELSEIF  "$(CFG)" == "WFTTNN - Win32 Debug"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 6
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /Yu"stdafx.h" /FD /GZ /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /FR /Yu"stdafx.h" /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x804 /d "_DEBUG" /d "_AFXDLL"
# ADD RSC /l 0x804 /d "_DEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept
# ADD LINK32 /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept

!ENDIF 

# Begin Target

# Name "WFTTNN - Win32 Release"
# Name "WFTTNN - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\ADODataBase.cpp
# End Source File
# Begin Source File

SOURCE=.\ADORecordSet.cpp
# End Source File
# Begin Source File

SOURCE=.\CoolTabCtrl.cpp
# End Source File
# Begin Source File

SOURCE=.\DBAccess.cpp
# End Source File
# Begin Source File

SOURCE=.\DlgAutoCeShi.cpp
# End Source File
# Begin Source File

SOURCE=.\DlgAutoTask.cpp
# End Source File
# Begin Source File

SOURCE=.\DlgBatchInfo.cpp
# End Source File
# Begin Source File

SOURCE=.\DlgConnect.cpp
# End Source File
# Begin Source File

SOURCE=.\DlgDataBrowse.cpp
# End Source File
# Begin Source File

SOURCE=.\DlgOperateLog.cpp
# End Source File
# Begin Source File

SOURCE=.\DlgRndRead.cpp
# End Source File
# Begin Source File

SOURCE=.\DlgSetChannel.cpp
# End Source File
# Begin Source File

SOURCE=.\DlgSetParam.cpp
# End Source File
# Begin Source File

SOURCE=.\DlgTerminalBaoWen.cpp
# End Source File
# Begin Source File

SOURCE=.\DlgTerminalInfo.cpp
# End Source File
# Begin Source File

SOURCE=.\DlgTerminalParent.cpp
# End Source File
# Begin Source File

SOURCE=.\GlobalFunc.cpp
# End Source File
# Begin Source File

SOURCE=.\MainFrm.cpp
# End Source File
# Begin Source File

SOURCE=.\MsgOutput.cpp
# End Source File
# Begin Source File

SOURCE=.\PacketTransfer.cpp
# End Source File
# Begin Source File

SOURCE=.\Protocol.cpp
# End Source File
# Begin Source File

SOURCE=.\PtclCodec.cpp
# End Source File
# Begin Source File

SOURCE=.\PtclModule_NN.cpp
# End Source File
# Begin Source File

SOURCE=.\PtclPacket.cpp
# End Source File
# Begin Source File

SOURCE=.\PtclPacket_NN.cpp
# End Source File
# Begin Source File

SOURCE=.\RecordLog.cpp
# End Source File
# Begin Source File

SOURCE=.\scbarg.cpp
# End Source File
# Begin Source File

SOURCE=.\sizecbar.cpp
# End Source File
# Begin Source File

SOURCE=.\StdAfx.cpp
# ADD CPP /Yc"stdafx.h"
# End Source File
# Begin Source File

SOURCE=.\TerminalTreeCtrl.cpp
# End Source File
# Begin Source File

SOURCE=.\WFTTNN.cpp
# End Source File
# Begin Source File

SOURCE=.\WFTTNN.rc
# End Source File
# Begin Source File

SOURCE=.\WFTTNNDoc.cpp
# End Source File
# Begin Source File

SOURCE=.\WFTTNNView.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\ADODataBase.h
# End Source File
# Begin Source File

SOURCE=.\ADORecordSet.h
# End Source File
# Begin Source File

SOURCE=.\CoolTabCtrl.h
# End Source File
# Begin Source File

SOURCE=.\DBAccess.h
# End Source File
# Begin Source File

SOURCE=.\DBAccess_Global.h
# End Source File
# Begin Source File

SOURCE=.\DlgAutoCeShi.h
# End Source File
# Begin Source File

SOURCE=.\DlgAutoTask.h
# End Source File
# Begin Source File

SOURCE=.\DlgBatchInfo.h
# End Source File
# Begin Source File

SOURCE=.\DlgConnect.h
# End Source File
# Begin Source File

SOURCE=.\DlgDataBrowse.h
# End Source File
# Begin Source File

SOURCE=.\DlgOperateLog.h
# End Source File
# Begin Source File

SOURCE=.\DlgRndRead.h
# End Source File
# Begin Source File

SOURCE=.\DlgSetChannel.h
# End Source File
# Begin Source File

SOURCE=.\DlgSetParam.h
# End Source File
# Begin Source File

SOURCE=.\DlgTerminalBaoWen.h
# End Source File
# Begin Source File

SOURCE=.\DlgTerminalInfo.h
# End Source File
# Begin Source File

SOURCE=.\DlgTerminalParent.h
# End Source File
# Begin Source File

SOURCE=.\GlobalFunc.h
# End Source File
# Begin Source File

SOURCE=.\MainFrm.h
# End Source File
# Begin Source File

SOURCE=.\MsgOutput.h
# End Source File
# Begin Source File

SOURCE=.\PacketTransfer.h
# End Source File
# Begin Source File

SOURCE=.\Protocol.h
# End Source File
# Begin Source File

SOURCE=.\PtclCodec.h
# End Source File
# Begin Source File

SOURCE=.\PtclCodec_Global.h
# End Source File
# Begin Source File

SOURCE=.\PtclModule.h
# End Source File
# Begin Source File

SOURCE=.\PtclModule_NN.h
# End Source File
# Begin Source File

SOURCE=.\PtclPacket.h
# End Source File
# Begin Source File

SOURCE=.\PtclPacket_NN.h
# End Source File
# Begin Source File

SOURCE=.\RecordLog.h
# End Source File
# Begin Source File

SOURCE=.\Resource.h
# End Source File
# Begin Source File

SOURCE=.\scbarg.h
# End Source File
# Begin Source File

SOURCE=.\sizecbar.h
# End Source File
# Begin Source File

SOURCE=.\StdAfx.h
# End Source File
# Begin Source File

SOURCE=.\TerminalTreeCtrl.h
# End Source File
# Begin Source File

SOURCE=.\WFTTNN.h
# End Source File
# Begin Source File

SOURCE=.\WFTTNNDoc.h
# End Source File
# Begin Source File

SOURCE=.\WFTTNNView.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=.\ico\about.ico
# End Source File
# Begin Source File

SOURCE=.\ico\channel_set.ico
# End Source File
# Begin Source File

SOURCE=.\ico\cs.ico
# End Source File
# Begin Source File

SOURCE=.\ico\data.ico
# End Source File
# Begin Source File

SOURCE=.\ico\Delete.ico
# End Source File
# Begin Source File

SOURCE=.\ico\exit.ico
# End Source File
# Begin Source File

SOURCE=.\ico\icon1.ico
# End Source File
# Begin Source File

SOURCE=.\ico\icon28.ico
# End Source File
# Begin Source File

SOURCE=.\res\Toolbar.bmp
# End Source File
# Begin Source File

SOURCE=.\ico\tx.ico
# End Source File
# Begin Source File

SOURCE=.\res\WFTTNN.ico
# End Source File
# Begin Source File

SOURCE=.\res\WFTTNN.rc2
# End Source File
# Begin Source File

SOURCE=.\res\WFTTNNDoc.ico
# End Source File
# Begin Source File

SOURCE=.\ico\zc.ico
# End Source File
# Begin Source File

SOURCE=.\ico\zd.ico
# End Source File
# End Group
# Begin Group "通道处理"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\AcceptChannel.cpp
# End Source File
# Begin Source File

SOURCE=.\AcceptChannel.h
# End Source File
# Begin Source File

SOURCE=.\BufferData.cpp
# End Source File
# Begin Source File

SOURCE=.\BufferData.h
# End Source File
# Begin Source File

SOURCE=.\ClientChannel.cpp
# End Source File
# Begin Source File

SOURCE=.\ClientChannel.h
# End Source File
# Begin Source File

SOURCE=.\Communication\Com.cpp
# End Source File
# Begin Source File

SOURCE=.\Communication\Com.h
# End Source File
# Begin Source File

SOURCE=.\Reactor\EventHandler.h
# End Source File
# Begin Source File

SOURCE=.\HandlerMgr.cpp
# End Source File
# Begin Source File

SOURCE=.\HandlerMgr.h
# End Source File
# Begin Source File

SOURCE=.\Communication\Modem.cpp
# End Source File
# Begin Source File

SOURCE=.\Communication\Modem.h
# End Source File
# Begin Source File

SOURCE=.\Reactor\Reactor.cpp
# End Source File
# Begin Source File

SOURCE=.\Reactor\Reactor.h
# End Source File
# Begin Source File

SOURCE=.\Communication\RS232.cpp
# End Source File
# Begin Source File

SOURCE=.\Communication\RS232.h
# End Source File
# Begin Source File

SOURCE=.\Reactor\SockWrapper.cpp
# End Source File
# Begin Source File

SOURCE=.\Reactor\SockWrapper.h
# End Source File
# Begin Source File

SOURCE=.\TH_Thread.cpp
# End Source File
# Begin Source File

SOURCE=.\TH_Thread.h
# End Source File
# End Group
# Begin Group "TreeList"

# PROP Default_Filter ""
# End Group
# Begin Source File

SOURCE=.\ReadMe.txt
# End Source File
# End Target
# End Project
