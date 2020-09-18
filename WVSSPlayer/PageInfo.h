#pragma once
#include "resource.h"
namespace WVSS {
class CMainDialog;
class CPageInfo : public CPropertyPage
{
private:
 CListBox m_StatusList;
 bool m_bPrintMessages;
public:
	enum { IDD = IDD_PAGE_INFO };
 CMainDialog *m_pMainDialog;
private:
protected:
 virtual BOOL OnInitDialog();
public:
	CPageInfo(CWnd* pParent);
	~CPageInfo();
	void SetPrintMessages(bool p_bPrintMessages) { m_bPrintMessages = p_bPrintMessages; }
	void AddToMessageList(const char *p_pFormat, ...);
protected:
DECLARE_MESSAGE_MAP()
 afx_msg void OnClearHistoryButtonClicked();
 afx_msg void OnSaveMessagesButtonClicked();
 afx_msg void OnHelpButtonClicked();
};
} 
