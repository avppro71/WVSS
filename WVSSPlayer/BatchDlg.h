#pragma once
class CBatchDlg : public CDialog
{
	DECLARE_DYNAMIC(CBatchDlg)
public:
 enum Mode { OVERRIDE, FRAME };
private:
protected:
public:
 enum { IDD = IDD_BATCH };
 Mode m_Mode;
 double m_dStart_dZ, m_dEnd_dZ, m_dStart_dH, m_dEnd_dH, m_dStart_dAngle, m_dEnd_dAngle;
 DWORD m_lStartFrame, m_lEndFrame;
public:
	CBatchDlg(CWnd* pParent = NULL);
	virtual ~CBatchDlg();
 virtual void OnOK();
 virtual BOOL OnInitDialog();
protected:
	DECLARE_MESSAGE_MAP()
public:
 afx_msg void OnBnClickedByOverride();
};
