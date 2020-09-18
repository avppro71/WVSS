#include "stdafx.h"
#include "SITARWVSSPlayer.h"
#include "ProgressDlg.h"
BEGIN_MESSAGE_MAP(CProgressDlg, CDialog)
 ON_WM_TIMER()
END_MESSAGE_MAP()
IMPLEMENT_DYNAMIC(CProgressDlg, CDialog)
UINT GlobalProgressDlgRunProc(LPVOID p_lParam);
CProgressDlg::CProgressDlg(CWnd* pParent, LPCSTR p_pFileName) : CDialog(CProgressDlg::IDD, pParent)
{
 m_pFileName = p_pFileName;
 m_bProcRes = false;
 m_Mode = NONE;
 m_pfProgress = NULL;
 m_RunFunctions[NONE] = NULL;
 m_RunFunctions[LOAD] = NULL;
}
CProgressDlg::~CProgressDlg()
{
 short a;
 a = 10;
}
void CProgressDlg::OnCancel()
{
 CDialog::EndDialog(IDOK);
}
BOOL CProgressDlg::OnInitDialog()
{
 m_ProgressBar.SubclassDlgItem(IDC_PROGRESS, this);
 m_ProgressBar.SetRange(0, 100); 
 m_ProgressBar.SetBarColor(RGB(98, 205, 208));
 AfxBeginThread(GlobalProgressDlgRunProc, this, THREAD_PRIORITY_NORMAL, 0, 0, NULL);
 SetDlgItemText(IDS_TEXT1, CString("Processing ") + m_pFileName);
 return(TRUE);
}
UINT GlobalProgressDlgRunProc(LPVOID p_lParam)
{
 CProgressDlg *pDlg = (CProgressDlg *)p_lParam;
 if(!pDlg) { ASSERT(0); return(0); }
 pDlg->RunProc();
 return(0);
}
void CProgressDlg::RunProc()
{
 std::map<ProgressDlgMode, pfProgressDlgRunFunction>::iterator It;
 BOOL bRet = FALSE;
 m_bProcRes = false;
 m_TimerID = SetTimer(1, 200, NULL);
 It = m_RunFunctions.find(m_Mode);
 if(It != m_RunFunctions.end())
   if(It->second != NULL)
     m_bProcRes = bRet = It->second(m_pFileName);
 KillTimer(m_TimerID);
 PostMessage(WM_CLOSE);
}
void CProgressDlg::OnTimer(UINT p_nIDEvent)
{
 if(!m_pfProgress) return;
 m_ProgressBar.SetPos((int)(m_pfProgress() * 100));
}
