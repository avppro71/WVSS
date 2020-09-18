#pragma once
#include <vector>
#if defined(WVSS_LIBRARY)
#define WVSSAPI   __declspec(dllexport)
#define WVSSAPIEX
#elif defined(WVSS_LIBRARYEX)
#define WVSSAPI   __declspec(dllexport)
#define WVSSAPIEX __declspec(dllexport)
#else 
#define WVSSAPI   __declspec(dllimport)
#define WVSSAPIEX __declspec(dllimport)
#endif
namespace WVSS
{
class WVSSAPIEX CSensitivityAnalysisSetup
{
#pragma message("CSensitivityAnalysisSetup")
public:
 enum eValueType { AVP_SAVT_NONE,
                   AVP_SAVT_SECTION_NUM,
                   AVP_SAVT_PLANE_TYPE,
                   AVP_SAVT_C1,
                   AVP_SAVT_C2,
                   AVP_SAVT_EDR,
                   AVP_SAVT_CURRENT_GAMMA,
                   AVP_SAVT_WINGSPAN, 
                   AVP_SAVT_ALTITUDE, 
                   AVP_SAVT_MASS, 
                   AVP_SAVT_WIND_DIRECTION, 
                   AVP_SAVT_WIND_SPEED, 
                   AVP_SAVT_SPEED, 
                   AVP_SAVT_INITIAL_GAMMA 
                   };
 enum eValueAdjType { AVP_SAVAT_NONE, AVP_SAVAT_ASSIGN, AVP_SAVAT_MULTIPLY };
 static const char *m_ValueAdjTypeNames[3];
public:
 bool m_bSensitivityAnalysis; 
 unsigned long m_lSectionNum; 
 eValueAdjType m_SectionNumAdjType; 
 char m_PlaneType[32]; 
 eValueAdjType m_PlaneTypeAdjType; 
 double m_dC1;
 eValueAdjType m_C1AdjType;
 double m_dC2;
 eValueAdjType m_C2AdjType;
 double m_dEDR; 
 eValueAdjType m_EDRAdjType;
 double m_dCurrentWakeGamma;
 eValueAdjType m_CurrentWakeGammaAdjType;
 double m_dWingSpan_m;
 eValueAdjType m_WingSpanAdjType;
 double m_dPlaneAltitude_m;
 eValueAdjType m_PlaneAltitudeAdjType;
 double m_dPlaneMass_kg;
 eValueAdjType m_PlaneMassAdjType;
 double m_dWindDirection_rad_to;
 eValueAdjType m_WindDirectionAdjType;
 double m_dWS;
 eValueAdjType m_WindSpeedAdjType;
 double m_dASMS;
 eValueAdjType m_AircraftSpeedAdjType;
 double m_dInitialWakeGamma;
 eValueAdjType m_InitialWakeGammaAdjType;
private:
 eValueAdjType AdjTypeFromStr(const char *p_pStr);
public:
 CSensitivityAnalysisSetup();
 ~CSensitivityAnalysisSetup() { }
 void Reset();
 bool LoadFromFile(const char *p_pFileName);
 void AdjustValue(char *p_pStrValue, eValueType p_ValueType);
 void AdjustValue(double *p_pdValue, eValueType p_ValueType);
 void AdjustValue(unsigned long *p_pulValue, eValueType p_ValueType); 
 bool ProperAC(const char *p_pAircraftType);
 bool ProperSection(unsigned long p_lSectionNum);
 bool ProperACAndSection(const char *p_pAircraftType, unsigned long p_lSectionNum); 
 bool GetAssignedSectionNumber(unsigned long *p_plSectionNum);
};
class CTest_ModelSensitivityLine
{
public:
 DWORD m_lSectionNum;
 LARGE_INTEGER m_CTLE, m_CurrentTimeLI;
 SYSTEMTIME m_CTST, m_CurrentTimeST;
 double m_dGamma, m_dGammaUpper, m_dGammaLower;
 double m_dX;
 double m_dLeftH, m_dRightH, m_dLeftHUpper, m_dRightHUpper, m_dLeftHLower, m_dRightHLower; 
 double m_dZSize, m_dHSize;
 double m_dEDR_star, m_dEDR;
 bool m_bUNC;
public:
 CTest_ModelSensitivityLine() { Reset(); }
 void Reset() { memset(this, 0, sizeof(CTest_ModelSensitivityLine)); }
};
class CTest_ModelSensitivityOutput
{
public:
 std::vector<CTest_ModelSensitivityLine>m_Data;
public:
 CTest_ModelSensitivityOutput() { }
 void AddLine(CTest_ModelSensitivityLine *p_pLine) { m_Data.push_back(*p_pLine); }
 void Write(FILE *p_fFile);
};
} 
