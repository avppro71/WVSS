#include "stdafx.h"
#include "SensitivityAnalysis.h"
#define _USE_MATH_DEFINES
#include "math.h"
#include "WakeClass2.h"
#include "Global.h"
#pragma warning(disable:4996)
using namespace WVSS;
const char *CSensitivityAnalysisSetup::m_ValueAdjTypeNames[3] = { "None", "Assign", "Multiply" };
CSensitivityAnalysisSetup::CSensitivityAnalysisSetup()
{
 Reset();
}
void CSensitivityAnalysisSetup::Reset()
{
 m_bSensitivityAnalysis = false;
 m_lSectionNum = 0;
 m_SectionNumAdjType = AVP_SAVAT_NONE;
 memset(m_PlaneType, 0, sizeof(m_PlaneType));
 m_PlaneTypeAdjType = AVP_SAVAT_NONE;
 m_dC1 = 0;
 m_C1AdjType = AVP_SAVAT_NONE;
 m_dC2 = 0;
 m_C2AdjType = AVP_SAVAT_NONE;
 m_dEDR = 0;
 m_EDRAdjType = AVP_SAVAT_NONE;
 m_dCurrentWakeGamma = 0;
 m_CurrentWakeGammaAdjType = AVP_SAVAT_NONE;
 m_dWingSpan_m = 0;
 m_WingSpanAdjType = AVP_SAVAT_NONE;
 m_dPlaneAltitude_m = 0;
 m_PlaneAltitudeAdjType = AVP_SAVAT_NONE;
 m_dPlaneMass_kg = 0;
 m_PlaneMassAdjType = AVP_SAVAT_NONE;
 m_dWindDirection_rad_to = 0;
 m_WindDirectionAdjType = AVP_SAVAT_NONE;
	m_dWS = 0;
	m_WindSpeedAdjType = AVP_SAVAT_NONE;
 m_dASMS = 0;
 m_AircraftSpeedAdjType = AVP_SAVAT_NONE;
 m_dInitialWakeGamma = 0;
 m_InitialWakeGammaAdjType = AVP_SAVAT_NONE;
}
bool CSensitivityAnalysisSetup::LoadFromFile(const char *p_pFileName)
{
 DWORD dwRetVal;
 bool ProcOK = false; 
 char tstr[128];
 double dVal;
 long lVal;
 Reset();
 for(;;) {
   m_bSensitivityAnalysis = !!GetPrivateProfileInt("SENSITIVITY_ANALYSIS", "On", false, p_pFileName);
   dwRetVal = GetPrivateProfileString("SENSITIVITY_ANALYSIS", "PlaneTypeAdjType", "None", tstr, sizeof(tstr), p_pFileName);
   m_PlaneTypeAdjType = AdjTypeFromStr(tstr);
     dwRetVal = GetPrivateProfileString("SENSITIVITY_ANALYSIS", "PlaneTypeValue", "None", m_PlaneType, sizeof(m_PlaneType), p_pFileName);
   dwRetVal = GetPrivateProfileString("SENSITIVITY_ANALYSIS", "SectionNumAdjType", "None", tstr, sizeof(tstr), p_pFileName);
   m_SectionNumAdjType = AdjTypeFromStr(tstr);
     lVal = GetPrivateProfileInt("SENSITIVITY_ANALYSIS", "SectionNumValue", 0, p_pFileName);
       m_lSectionNum = lVal;
   dwRetVal = GetPrivateProfileString("SENSITIVITY_ANALYSIS", "C1AdjType", "None", tstr, sizeof(tstr), p_pFileName);
   m_C1AdjType = AdjTypeFromStr(tstr);
     dwRetVal = GetPrivateProfileString("SENSITIVITY_ANALYSIS", "C1Value", "0", tstr, sizeof(tstr), p_pFileName);
     dVal = atof(tstr);
       m_dC1 = dVal;
   dwRetVal = GetPrivateProfileString("SENSITIVITY_ANALYSIS", "C2AdjType", "None", tstr, sizeof(tstr), p_pFileName);
   m_C2AdjType = AdjTypeFromStr(tstr);
     dwRetVal = GetPrivateProfileString("SENSITIVITY_ANALYSIS", "C2Value", "0", tstr, sizeof(tstr), p_pFileName);
     dVal = atof(tstr);
       m_dC2 = dVal;
   dwRetVal = GetPrivateProfileString("SENSITIVITY_ANALYSIS", "EDRAdjType", "None", tstr, sizeof(tstr), p_pFileName);
   m_EDRAdjType = AdjTypeFromStr(tstr);
     dwRetVal = GetPrivateProfileString("SENSITIVITY_ANALYSIS", "EDRValue", "0", tstr, sizeof(tstr), p_pFileName);
     dVal = atof(tstr);
       m_dEDR = dVal;
   dwRetVal = GetPrivateProfileString("SENSITIVITY_ANALYSIS", "CurrentWakeGammaAdjType", "None", tstr, sizeof(tstr), p_pFileName);
   m_CurrentWakeGammaAdjType = AdjTypeFromStr(tstr);
     dwRetVal = GetPrivateProfileString("SENSITIVITY_ANALYSIS", "CurrentWakeGammaValue", "0", tstr, sizeof(tstr), p_pFileName);
     dVal = atof(tstr);
       m_dCurrentWakeGamma = dVal;
   dwRetVal = GetPrivateProfileString("SENSITIVITY_ANALYSIS", "WingSpanAdjType", "None", tstr, sizeof(tstr), p_pFileName);
   m_WingSpanAdjType = AdjTypeFromStr(tstr);
     dwRetVal = GetPrivateProfileString("SENSITIVITY_ANALYSIS", "WingSpanValue_m", "0", tstr, sizeof(tstr), p_pFileName);
     dVal = atof(tstr);
       m_dWingSpan_m = dVal;
   dwRetVal = GetPrivateProfileString("SENSITIVITY_ANALYSIS", "AltitudeAdjType", "None", tstr, sizeof(tstr), p_pFileName);
   m_PlaneAltitudeAdjType = AdjTypeFromStr(tstr);
     dwRetVal = GetPrivateProfileString("SENSITIVITY_ANALYSIS", "AltitudeValue_m", "0", tstr, sizeof(tstr), p_pFileName);
     dVal = atof(tstr);
       m_dPlaneAltitude_m = dVal;
   dwRetVal = GetPrivateProfileString("SENSITIVITY_ANALYSIS", "PlaneMassAdjType", "None", tstr, sizeof(tstr), p_pFileName);
   m_PlaneMassAdjType = AdjTypeFromStr(tstr);
     dwRetVal = GetPrivateProfileString("SENSITIVITY_ANALYSIS", "PlaneMassValue_kg", "0", tstr, sizeof(tstr), p_pFileName);
     dVal = atof(tstr);
       m_dPlaneMass_kg = dVal;
   dwRetVal = GetPrivateProfileString("SENSITIVITY_ANALYSIS", "WindDirectionAdjType", "None", tstr, sizeof(tstr), p_pFileName);
   m_WindDirectionAdjType = AdjTypeFromStr(tstr);
     dwRetVal = GetPrivateProfileString("SENSITIVITY_ANALYSIS", "WindDirectionValue_deg_from", "0", tstr, sizeof(tstr), p_pFileName);
     dVal = atof(tstr);
       m_dWindDirection_rad_to = Radians(dVal - 180); 
       if(m_dWindDirection_rad_to < 0) m_dWindDirection_rad_to += 2 * M_PI; 
   dwRetVal = GetPrivateProfileString("SENSITIVITY_ANALYSIS", "WindSpeedAdjType", "None", tstr, sizeof(tstr), p_pFileName);
   m_WindSpeedAdjType = AdjTypeFromStr(tstr);
     dwRetVal = GetPrivateProfileString("SENSITIVITY_ANALYSIS", "WindSpeedValue_kts", "0", tstr, sizeof(tstr), p_pFileName);
     dVal = atof(tstr);
       m_dWS = dVal * 1.852 / 3.6; 
   dwRetVal = GetPrivateProfileString("SENSITIVITY_ANALYSIS", "SpeedAdjType", "None", tstr, sizeof(tstr), p_pFileName);
   m_AircraftSpeedAdjType = AdjTypeFromStr(tstr);
     dwRetVal = GetPrivateProfileString("SENSITIVITY_ANALYSIS", "SpeedValue_kts", "0", tstr, sizeof(tstr), p_pFileName);
     dVal = atof(tstr);
       m_dASMS = dVal * 1.852 / 3.6; 
   dwRetVal = GetPrivateProfileString("SENSITIVITY_ANALYSIS", "InitialWakeGammaAdjType", "None", tstr, sizeof(tstr), p_pFileName);
   m_InitialWakeGammaAdjType = AdjTypeFromStr(tstr);
     dwRetVal = GetPrivateProfileString("SENSITIVITY_ANALYSIS", "InitialWakeGammaValue", "0", tstr, sizeof(tstr), p_pFileName);
     dVal = atof(tstr);
       m_dInitialWakeGamma = dVal;
   ProcOK = true;
   break; }
 if(!ProcOK) Reset();
 return(ProcOK);
}
CSensitivityAnalysisSetup::eValueAdjType CSensitivityAnalysisSetup::AdjTypeFromStr(const char *p_pStr)
{
 if(!_stricmp(p_pStr, "None")) return(AVP_SAVAT_NONE);
 if(!_stricmp(p_pStr, "Override")) return(AVP_SAVAT_ASSIGN);
 if(!_stricmp(p_pStr, "Multiply")) return(AVP_SAVAT_MULTIPLY);
 return(AVP_SAVAT_NONE);
}
void CSensitivityAnalysisSetup::AdjustValue(char *p_pStrValue, eValueType p_ValueType)
{
 if(!m_bSensitivityAnalysis) return;
 switch(p_ValueType) {
   case AVP_SAVT_PLANE_TYPE: if(m_PlaneTypeAdjType == AVP_SAVAT_ASSIGN) strcpy(p_pStrValue, m_PlaneType); break;
   default: break;
   }
}
void CSensitivityAnalysisSetup::AdjustValue(double *p_pdValue, eValueType p_ValueType)
{
 eValueAdjType ValueAdjType;
 double dValue;
 if(!m_bSensitivityAnalysis) return;
 switch(p_ValueType) {
   case AVP_SAVT_C1: ValueAdjType = m_C1AdjType; dValue = m_dC1; break;
   case AVP_SAVT_C2: ValueAdjType = m_C2AdjType; dValue = m_dC2; break;
   case AVP_SAVT_EDR: ValueAdjType = m_EDRAdjType; dValue = m_dEDR; break;
   case AVP_SAVT_CURRENT_GAMMA: ValueAdjType = m_CurrentWakeGammaAdjType; dValue = m_dCurrentWakeGamma; break;
   case AVP_SAVT_WINGSPAN: ValueAdjType = m_WingSpanAdjType; dValue = m_dWingSpan_m; break;
   case AVP_SAVT_ALTITUDE: ValueAdjType = m_PlaneAltitudeAdjType; dValue = m_dPlaneAltitude_m; break;
   case AVP_SAVT_MASS: ValueAdjType = m_PlaneMassAdjType; dValue = m_dPlaneMass_kg; break;
   case AVP_SAVT_WIND_DIRECTION: ValueAdjType = m_WindDirectionAdjType; dValue = m_dWindDirection_rad_to; break;
   case AVP_SAVT_WIND_SPEED: ValueAdjType = m_WindSpeedAdjType; dValue = m_dWS; break;
   case AVP_SAVT_SPEED: ValueAdjType = m_AircraftSpeedAdjType; dValue = m_dASMS; break;
   case AVP_SAVT_INITIAL_GAMMA : ValueAdjType = m_InitialWakeGammaAdjType; dValue = m_dInitialWakeGamma; break;
   default: return;
   }
 if(ValueAdjType == AVP_SAVAT_ASSIGN) *p_pdValue = dValue;
 else if(ValueAdjType == AVP_SAVAT_MULTIPLY) (*p_pdValue) *= dValue;
}
void CSensitivityAnalysisSetup::AdjustValue(unsigned long *p_pulValue, eValueType p_ValueType)
{
 eValueAdjType ValueAdjType;
 unsigned long ulValue;
 if(!m_bSensitivityAnalysis) return;
 switch(p_ValueType) {
   case AVP_SAVT_SECTION_NUM: ValueAdjType = m_SectionNumAdjType; ulValue = m_lSectionNum; break;
   default: return;
   }
 if(ValueAdjType == AVP_SAVAT_ASSIGN) *p_pulValue = ulValue;
 else if(ValueAdjType == AVP_SAVAT_MULTIPLY) (*p_pulValue) *= ulValue;
}
bool CSensitivityAnalysisSetup::ProperAC(const char *p_pAircraftType)
{
 if(!m_bSensitivityAnalysis) return(false);
 return(m_PlaneTypeAdjType == AVP_SAVAT_NONE || (m_PlaneTypeAdjType == AVP_SAVAT_ASSIGN && !_stricmp(m_PlaneType, p_pAircraftType)));
}
bool CSensitivityAnalysisSetup::ProperSection(unsigned long p_lSectionNum)
{
 if(!m_bSensitivityAnalysis) return(false);
 return(m_SectionNumAdjType == AVP_SAVAT_NONE || (m_SectionNumAdjType == AVP_SAVAT_ASSIGN && m_lSectionNum == p_lSectionNum));
}
bool CSensitivityAnalysisSetup::ProperACAndSection(const char *p_pAircraftType, unsigned long p_lSectionNum)
{
 if(!m_bSensitivityAnalysis) return(false);
 return(ProperSection(p_lSectionNum) && ProperAC(p_pAircraftType));
}
bool CSensitivityAnalysisSetup::GetAssignedSectionNumber(unsigned long *p_plSectionNum)
{
 if(m_SectionNumAdjType != AVP_SAVAT_ASSIGN) return(false);
 if(p_plSectionNum)
   *p_plSectionNum = m_lSectionNum;
 return(true);
}
void CTest_ModelSensitivityOutput::Write(FILE *p_fFile)
{
 size_t i;
 char cSep = ';';
 if(!p_fFile) return;
 fprintf(p_fFile, "SectionNum%c"
                  "CreationDate%cCreationTime%c"
                  "CurDate%cCurTime%c"
                  "dt(sec)%c"
                  "EDR_star%c"
                  "EDR%c"
                  "x(m)%cm_dga%chLeft(m)%chRight(m)%c"
                  "ConeOfUncertainty%c"
                  "m_dgaUpper%chLeftUpper(m)%chRightUpper(m)%c"
                  "m_dgaLower%chLeftLower(m)%chRightLower(m)%c"
                  "rz(m)%crh(m)%cdz(m)%cdhLeft(m)%cddz(m)%cddhLeft(m)\r\n",
         cSep, cSep, cSep, cSep, cSep, cSep, cSep, cSep, cSep, cSep, cSep, cSep,
         cSep, cSep, cSep, cSep, cSep, cSep, cSep, cSep, cSep, cSep, cSep, cSep);
 for(i = 0; i < m_Data.size(); i ++) {
   LIToSystemTime(&m_Data[i].m_CTLE, &m_Data[i].m_CTST);
   LIToSystemTime(&m_Data[i].m_CurrentTimeLI, &m_Data[i].m_CurrentTimeST);
   fprintf(p_fFile, "%ld%c" 
                    "%.4d-%.2d-%.2d%c" 
                    "%.2d:%2d:%.2d%c" 
                    "%.4d-%.2d-%.2d%c" 
                    "%.2d:%.2d:%.2d%c" 
                    "%ld%c" 
                    "%lf%c" 
                    "%lf%c" 
                    "%.1lf%c" 
                    "%.4lf%c" 
                    "%.4lf%c" 
                    "%.4lf%c" 
                    "%s%c" 
                    "%.4lf%c" 
                    "%.4lf%c" 
                    "%.4lf%c" 
                    "%.4lf%c" 
                    "%.4lf%c" 
                    "%.4lf%c" 
                    "%.1lf%c" 
                    "%.1lf%c" 
                    "%.1lf%c" 
                    "%.1lf%c" 
                    "%.1lf%c" 
                    "%.1lf%c" 
                    "\r\n",
                    m_Data[i].m_lSectionNum, cSep,
                    m_Data[i].m_CTST.wYear, m_Data[i].m_CTST.wMonth, m_Data[i].m_CTST.wDay, cSep,
                    m_Data[i].m_CTST.wHour, m_Data[i].m_CTST.wMinute, m_Data[i].m_CTST.wSecond, cSep,
                    m_Data[i].m_CurrentTimeST.wYear, m_Data[i].m_CurrentTimeST.wMonth, m_Data[i].m_CurrentTimeST.wDay,  cSep,
                    m_Data[i].m_CurrentTimeST.wHour, m_Data[i].m_CurrentTimeST.wMinute, m_Data[i].m_CurrentTimeST.wSecond, cSep,
                    (DWORD)((m_Data[i].m_CurrentTimeLI.QuadPart - m_Data[i].m_CTLE.QuadPart) / 10000000), cSep,
                    m_Data[i].m_dEDR_star, cSep,
                    m_Data[i].m_dEDR, cSep,
                    m_Data[i].m_dX, cSep,
                    m_Data[i].m_dGamma, cSep,
                    m_Data[i].m_dLeftH, cSep,
                    m_Data[i].m_dRightH, cSep,
                    (m_Data[i].m_bUNC ? "yes" : "no"), cSep,
                    m_Data[i].m_dGammaUpper, cSep,
                    m_Data[i].m_dLeftHUpper, cSep,
                    m_Data[i].m_dRightHUpper, cSep,
                    m_Data[i].m_dGammaLower, cSep,
                    m_Data[i].m_dLeftHLower, cSep,
                    m_Data[i].m_dRightHLower, cSep,
                    m_Data[i].m_dZSize, cSep,
                    m_Data[i].m_dHSize, cSep,
                    m_Data[i].m_dX - m_Data[0].m_dX, cSep,
                    m_Data[i].m_dLeftH - m_Data[0].m_dLeftH, cSep,
                    (!i ? 0 : m_Data[i].m_dX - m_Data[i - 1].m_dX), cSep,
                    (!i ? 0 : m_Data[i].m_dLeftH - m_Data[i - 1].m_dLeftH), cSep
                    );
   }
}
