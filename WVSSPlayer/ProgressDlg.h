#pragma once
#include<map>
typedef bool(* pfProgressDlgRunFunction)(const char *);
class CProgressDlg : public CDialog
{
	DECLARE_DYNAMIC(CProgressDlg)
private:
 UINT m_TimerID;
 bool m_bProcRes;
protected:
public:
 enum ProgressDlgMode { NONE, LOAD };
 enum { IDD = IDD_PROGRESS };
 CProgressCtrl m_ProgressBar;
 LPCSTR m_pFileName;
 ProgressDlgMode m_Mode;
 std::map<ProgressDlgMode, pfProgressDlgRunFunction>m_RunFunctions;
 float(* m_pfProgress)(); 
private:
protected:
public:
	CProgressDlg(CWnd* pParent, LPCSTR p_pFileName = NULL);
	virtual ~CProgressDlg();
 virtual BOOL OnInitDialog();
 void RunProc();
 virtual void OnCancel();
 bool GetProcRes() { return(m_bProcRes); }
protected:
	DECLARE_MESSAGE_MAP()
	void OnTimer(UINT p_nIDEvent);
};
