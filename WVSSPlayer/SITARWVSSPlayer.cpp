#include "stdafx.h"
#include "SITARWVSSPlayer.h"
#include "Global.h"
#include "MainDialog.h"
#ifdef _DEBUG
#define new DEBUG_NEW
#endif
using namespace WVSS;
BEGIN_MESSAGE_MAP(CSITARWVSSPlayerApp, CWinApp)
	ON_COMMAND(ID_HELP, &CWinApp::OnHelp)
END_MESSAGE_MAP()
CSITARWVSSPlayerApp g_theApp;
CSITARWVSSPlayerApp::CSITARWVSSPlayerApp()
{
 m_MsgNameMap.insert(std::pair<eWVSSMsgType, const char *>(WVSS_msg_unknown, "unknown"));
 m_MsgNameMap.insert(std::pair<eWVSSMsgType, const char *>(WVSS_msg_data, "data"));
 m_MsgNameMap.insert(std::pair<eWVSSMsgType, const char *>(WVSS_msg_area, "area"));
 m_MsgNameMap.insert(std::pair<eWVSSMsgType, const char *>(WVSS_msg_planepos, "plane_position"));
 m_MsgNameMap.insert(std::pair<eWVSSMsgType, const char *>(WVSS_msg_loadplane, "loadplane"));
 m_nMajorVersion = 2;
 m_nMinorVersion = 0;
 strcpy_s(m_BuildDate, sizeof(m_BuildDate), __DATE__);
 strcpy_s(m_BuildTime, sizeof(m_BuildTime), __TIME__);
}
BOOL CSITARWVSSPlayerApp::InitInstance()
{
	CWinApp::InitInstance();
	SetRegistryKey(_T("WVSS"));
 GetModuleFileName(NULL, m_ThisDir, sizeof(m_ThisDir));
 *strrchr(m_ThisDir, '\\') = '\0';
 GetIniFileName(m_IniFileName);
	ParseCommandLine(m_CmdLineInfo);
	CMainDialog Dlg;
	m_pMainWnd = &Dlg;
	Dlg.DoModal();
	return FALSE;
}
