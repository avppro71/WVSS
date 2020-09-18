#pragma once
#include "resource.h"
#include "IWVSSSeverity.h"
#include "AdvancedListCtrl.h"
namespace WVSS {
class CPageSeverityResults : public CPropertyPage
{
private:
 CAdvancedListCtrl m_List;
public:
	enum { IDD = IDD_PAGE_SEVERITY_RESULTS };
protected:
 virtual BOOL OnInitDialog();
public:
	CPageSeverityResults(CWnd* pParent = NULL);
	~CPageSeverityResults();
 void UpdateResults(IWVSSSeverity *p_pSeverity);
protected:
DECLARE_MESSAGE_MAP()
};
} 
