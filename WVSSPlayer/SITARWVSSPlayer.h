#pragma once
#ifndef __AFXWIN_H__
	#error "include 'stdafx.h' before including this file for PCH"
#endif
#include "resource.h"		
#include "Cmdline.h"
#include <map>
#include "WVSSIpProtocol.h"
class CSITARWVSSPlayerApp : public CWinApp
{
private:
protected:
public:
 char m_ThisDir[MAX_PATH], m_IniFileName[MAX_PATH];
 CSWPCommandLineInfo m_CmdLineInfo;
 std::map<eWVSSMsgType, const char *>m_MsgNameMap;
 short m_nMajorVersion;
 short m_nMinorVersion;
 char m_BuildDate[32];
 char m_BuildTime[32];
public:
	CSITARWVSSPlayerApp();
	virtual BOOL InitInstance();
 short GetMajorVersion() { return(m_nMajorVersion); }
 short GetMinorVersion() { return(m_nMinorVersion); }
 const char *GetBuildDate() { return(m_BuildDate); }
 const char *GetBuildTime() { return(m_BuildTime); }
	DECLARE_MESSAGE_MAP()
};
extern CSITARWVSSPlayerApp g_theApp;
