#pragma once
#include "resource.h"
#include <vector>
#include <map>
#include "wake_shared.h"
namespace WVSS {
class CMainDialog;
class CADAA;
class CPageDisplay : public CPropertyPage
{
public:
 class CDisplayFlags {
   public:
    short m_nPlaneSizeCorrection; 
    short m_nWakeSectionSizeCorrection; 
    short m_nRingAirplaneNum;
    bool m_bShowAirBlocks;
    int m_ShowSections[2];
    bool m_bShowConeOfUncertainty;
    float m_DistanceRingsNM[10];
    bool m_bShowLinesFromAircraftCenter;
    bool m_bShowLinesFromWtpis;
    CDisplayFlags();
   };
private:
 CDisplayFlags m_DisplayFlags;
 CComboBox m_ScenarioCombo;
 CComboBox m_RingAircraftCombo;
 CFont m_ScenarioFont;
 std::map<short, CWVSSScenario>m_Scenarios;
protected:
public:
	enum { IDD = IDD_PAGE_DISPLAY };
 CMainDialog *m_pMainDialog;
private:
protected:
 virtual BOOL OnInitDialog();
public:
	CPageDisplay(CWnd* pParent);
	~CPageDisplay();
 void SetFlags(CDisplayFlags *p_pFlags) { m_DisplayFlags = *p_pFlags; }
 void FillFlagsFromControls(CDisplayFlags *p_pFlags);
 void OnScenarioChanged();
 void FillRingAircraftList(std::vector <CADAA *>*p_pAircraft = NULL);
protected:
DECLARE_MESSAGE_MAP()
 afx_msg void OnSelChangeScenario();
};
} 
