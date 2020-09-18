#pragma once
#include "resource.h"
#include "AdvancedListCtrl.h"
#include <vector>
namespace WVSS {
class CPageFrame : public CPropertyPage
{
private:
 CAdvancedListCtrl m_ListPlane;
 CAdvancedListCtrl m_ListWake;
public:
	enum { IDD = IDD_PAGE_FRAME };
 class CFrameData {
   public:
    bool m_bWindSet, m_bWakeSet;
    char m_CallSign[32];
    char m_AircraftType[32];
    double m_dWindDirectionFrom_deg;
    double m_dWindSpeed_kts;
    double m_dSpeedMS;
    double m_nAltitudeM;
    long m_lStartWakeSection, m_lEndWakeSection;
    double m_dLatDeg, m_dLongDeg;
    CFrameData() { Reset(); }
    void Reset();
   };
private:
 std::vector<CFrameData> m_FrameData;
protected:
 virtual BOOL OnInitDialog();
public:
 CPageFrame(CWnd* pParent);
 ~CPageFrame();
 void ResetFrameData() { m_FrameData.clear(); }
 void AddFrameData(CFrameData *p_pFrameData) { m_FrameData.push_back(*p_pFrameData); }
 void UpdateFrameData();
protected:
DECLARE_MESSAGE_MAP()
};
} 
