#pragma once
#include "AdvancedListCtrl.h"
#pragma warning(disable : 4996)
namespace WVSS {
class CPageWakeFrame : public CPropertyPage
{
	DECLARE_DYNAMIC(CPageWakeFrame)
private:
 CAdvancedListCtrl m_ListWakeInteraction;
protected:
public:
 enum { IDD = IDD_PAGE_WAKE_INTERACTION };
 class CWakeFrameData {
   public:
    char m_CallSign[32];
    char m_AircraftType[32];
    double m_ddZL, m_ddZC, m_ddZR;
    double m_ddHL, m_ddHC, m_ddHR;
    double m_dGammaL, m_dGammaR;
    bool m_bPositionOverride;
    bool m_bGammaOverride;
    double m_ddZOverride, m_ddHOverride, m_dGammaOverride;
    CWakeFrameData() { Reset(); }
    void Reset();
    void SetCallSign(const char *p_pCallSign) { if(!p_pCallSign) strcpy(m_CallSign, "UNKNOWN"); else { memset(m_CallSign, 0, sizeof(m_CallSign)); strncpy(m_CallSign, p_pCallSign, sizeof(m_CallSign) - 1); } }
    void SATe(const char *p_pAircraftType) { if(!p_pAircraftType) strcpy(m_AircraftType, "UNKNOWN"); else { memset(m_AircraftType, 0, sizeof(m_AircraftType)); strncpy(m_AircraftType, p_pAircraftType, sizeof(m_AircraftType) - 1); } }
   };
private:
 std::vector<CWakeFrameData> m_WakeFrameData;
private:
protected:
 virtual BOOL OnInitDialog();
public:
	CPageWakeFrame(CWnd *p_pParent);
	virtual ~CPageWakeFrame();
 void ResetWakeFrameData() { m_WakeFrameData.clear(); }
 void AddWakeFrameData(CWakeFrameData *p_pWakeFrameData) { m_WakeFrameData.push_back(*p_pWakeFrameData); }
 void UpdateWakeFrameData();
protected:
	DECLARE_MESSAGE_MAP()
};
} 
