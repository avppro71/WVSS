#include "stdafx.h"
#include "SITARWVSSPLayer.h"
#include "PageInfo.h"
#include "MainDialog.h"
#pragma warning(disable : 4996)
using namespace WVSS;
BEGIN_MESSAGE_MAP(CPageInfo, CDialog)
 ON_BN_CLICKED(IDB_CLEAR_HISTORY, &CPageInfo::OnClearHistoryButtonClicked)
 ON_BN_CLICKED(IDB_SAVE_HISTORY, &CPageInfo::OnSaveMessagesButtonClicked)
 ON_BN_CLICKED(IDB_HELP, &CPageInfo::OnHelpButtonClicked)
END_MESSAGE_MAP()
CPageInfo::CPageInfo(CWnd * ) : CPropertyPage(CPageInfo::IDD)
{
 m_bPrintMessages = true;
 m_pMainDialog = NULL;
}
CPageInfo::~CPageInfo()
{
}
BOOL CPageInfo::OnInitDialog()
{
 char tstr[128];
 CPropertyPage::OnInitDialog();
 m_StatusList.SubclassDlgItem(IDL_STATUS, this);
 if(m_pMainDialog) {
   AddToMessageList("WVSS Player version %d.%d, built on %s at %s", g_theApp.GetMajorVersion(), g_theApp.GetMinorVersion(), g_theApp.GetBuildDate(), g_theApp.GetBuildTime());
   AddToMessageList("WVSS core V%d.%d, built on %s at %s", m_pMainDialog->m_Severity.GetMajorVersion(), m_pMainDialog->m_Severity.GetMinorVersion(), m_pMainDialog->m_Severity.GetBuildDate(), m_pMainDialog->m_Severity.GetBuildTime());
   SetDlgItemText(IDS_BUILD_INFO, tstr); }
 return(TRUE);
}
void CPageInfo::OnClearHistoryButtonClicked()
{
 m_StatusList.ResetContent();
}
void CPageInfo::OnSaveMessagesButtonClicked()
{
 size_t i, Count;
 char tstr[256];
 FILE *fFile;
 CFileDialog Dlg(FALSE, "txt", "*.txt", 0, "Text files (*.txt)|*.txt|All Files (*.*)|*.*|");
 Dlg.m_pOFN->lpstrInitialDir = g_theApp.m_ThisDir;
 if(Dlg.DoModal() != IDOK) return;
 strcpy(tstr, (LPCSTR)Dlg.GetPathName());
 fFile = fopen(tstr, "wb");
 if(!fFile) return;
 Count = m_StatusList.GetCount();
 for(i = 0; i < Count; i ++) {
   m_StatusList.GetText(i, tstr);
   fprintf(fFile, "%s\r\n", tstr);
   }
 fclose(fFile);
 AddToMessageList("Messages saved to %s", (LPCSTR)Dlg.GetPathName());
}
void CPageInfo::AddToMessageList(const char *p_pFormat, ...)
{
 char tstr[256];
 va_list ArgList;
 if(!p_pFormat || !m_bPrintMessages) return;
 va_start(ArgList, p_pFormat);
 vsprintf_s(tstr, sizeof(tstr), p_pFormat, ArgList);
 va_end(ArgList);
 m_StatusList.InsertString(-1, tstr);
 m_StatusList.SetTopIndex(m_StatusList.GetCount() - 1);
}
void CPageInfo::OnHelpButtonClicked()
{
 short i;
 char tstr[256];
 CString Str;
 Str = "Keyboard shortcuts:\nAlt + </>, Alt+P/N - advance forward and back\nAlt+[1..4] - place window to a corner (clockwise from top left)\n"
       "Keyboard mouse shortcuts:\nShift + scroll clicks - advance forward and back faster\n\n"
       "Command line arguments:\n";
 for(i = 0; i < sizeof(CSWPCommandLineInfo::m_pOperCmdText) / sizeof(const char *); i ++) {
   sprintf(tstr, "-%s %s\n", CSWPCommandLineInfo::m_pOperCmdText[i], CSWPCommandLineInfo::m_pOperDesc[i]);
   Str += tstr;
   }
 MessageBox(Str, "Help");
}
