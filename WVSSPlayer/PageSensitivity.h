#pragma once
#include "resource.h"
#include "AdvancedListCtrl.h"
#include <vector>
namespace WVSS {
class CSensitivityAnalysisSetup;
class CPageSensitivity : public CPropertyPage
{
private:
 CAdvancedListCtrl m_List;
public:
	enum { IDD = IDD_PAGE_SENSITIVITY };
 CSensitivityAnalysisSetup *m_pSA;
private:
protected:
 virtual BOOL OnInitDialog();
public:
	CPageSensitivity(CWnd* pParent);
	~CPageSensitivity();
 void UpdateControls();
protected:
DECLARE_MESSAGE_MAP()
};
} 
