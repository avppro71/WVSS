#pragma once
#include "resource.h"
#include "IWVSSSeverity.h"
namespace WVSS {
class CPageWakeOptions : public CPropertyPage
{
private:
 CWEAIF m_FSG;
 CComboBox m_FlightSpeedCombo, m_PilotResponseCombo;
 CEdit m_FlightTimeEdit;
public:
	enum { IDD = IDD_PAGE_WAKE_OPTIONS };
private:
protected:
 virtual BOOL OnInitDialog();
public:
	CPageWakeOptions(CWnd* pParent);
	~CPageWakeOptions();
 void SetFlags(CWEAIF *p_pFlags) { m_FSG = *p_pFlags; }
 void FillFlagsFromControls(CWEAIF *p_pFlags);
protected:
DECLARE_MESSAGE_MAP()
 afx_msg void OnCalcSeverityButtonClicked();
};
} 
