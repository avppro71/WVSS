#pragma once
#include "resource.h"
namespace WVSS {
class CPageComms : public CPropertyPage
{
private:
 CListBox m_InfoList;
public:
	enum { IDD = IDD_PAGE_COMMS };
protected:
 virtual BOOL OnInitDialog();
public:
	CPageComms(CWnd* pParent);
	~CPageComms();
	void AddToMessageList(const char *p_pFormat, ...);
protected:
DECLARE_MESSAGE_MAP()
 afx_msg void OnClearHistoryButtonClicked();
};
} 
