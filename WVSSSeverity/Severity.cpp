#include "stdafx.h"
#include "Severity.h"
#include "GeographicLib/geodesic.hpp"
#include "GeographicLib/LocalCartesian.hpp"
#include "GeographicLib/Geoid.hpp"
#include "Global.h"
#include "GeometricVector.h"
#include "WindServer.h"
#include "yajl/yajl_tree.h"
#include "yajl/yajl_parse.h"
#include "ATLComTime.h" 
#pragma warning(disable : 4996)
#define STRING(arg) #arg
#define STRING2(arg) STRING(arg)
#define PPMSG(arg) STRING2(__FILE__ STRING2(__LINE__) arg)
using namespace WVSS;
CSOC::CSOC()
{
 memset(this, 0, sizeof(CSOC));
 m_dmx_wake = AVP_BAD_WX_WAKE_VALUE;
 m_dvw_y_max = AVP_BAD_VW_Y_VALUE;
 m_dvw_z_max = AVP_BAD_VW_Z_VALUE;
 m_lPlayerPos = (unsigned long)-1L;
}
IWVSSSeverity *IWVSSSeverity::Create()
{
 char tstr[256];
 IWVSSSeverity *pSeverity = new CSeverity;
 if(!pSeverity) return(NULL);
 sprintf(tstr, "%s/WVSSSettings.ini", pSeverity->m_ThisDir);
 pSeverity->GetSettings(tstr);
 pSeverity->Init();
 return(pSeverity);
}
void IWVSSSeverity::Destroy(IWVSSSeverity *p_pObject)
{
 if(p_pObject) delete p_pObject;
}
void CWEAIF::Reset()
{
 m_bCS = true;
 m_nPR = 0;
 m_bEF = false;
 m_dIPWCD = 300;
 m_dWWT = 50;
 m_bGE = true;
 m_nFS = 1;
 m_dFT = 1;
 m_bFAS = false;
 m_bCOF = true;
 m_dWDRT = 90.0 * M_PI / 180;
 m_dWS = 0;
 m_bNWE = false;
 m_bOverrideProximityValues = false;
 m_bStartPlaneHOverride = m_bStartPlaneZOverride = m_bClosingAngleRadOverride = false;
 m_dStartPlaneHOverride = m_dStartPlaneZOverride = m_dCARdOverride = 0;
 m_bOverrideGamma = false;
 m_dWakeGammaOverrideValue = 0;
 m_bDebugMessages = false;
 m_bRW = true;
 m_nDefaultYear = 2016;
 m_nDefaultMonth = 1;
 m_nDefaultDay = 1;
 m_AFs.c1 = 0.3;
 m_AFs.c2 = 0.6;
 m_AFs.eta = 0.3;
 m_AFs.m_dStratification = 0;
 memset(m_GeoidName, 0, sizeof(m_GeoidName)); 
 strcpy(m_GeoidName, "EGM96");
 memset(m_GeoidDir, 0, sizeof(m_GeoidDir)); 
 strcpy(m_GeoidDir, "GeoidData");
 memset(m_EDD, 0, sizeof(m_EDD)); 
 strcpy(m_EDD, "ElevationData");
 m_fMG = 150;
 m_bInitGeoid = false;
 m_bUSE = true;
 m_bUCOU = false;
 memset(m_WFs.m_WeatherUndergroundID, 0, sizeof(m_WFs.m_WeatherUndergroundID));
 memset(m_WFs.m_WeatherUndergroundHTTP, 0, sizeof(m_WFs.m_WeatherUndergroundHTTP));
 memset(m_WFs.m_WeatherUndergroundRequestMask, 0, sizeof(m_WFs.m_WeatherUndergroundRequestMask));
}
void CWEAIF::WriteToVarJSON(char *p_pDest, unsigned long p_lDestSize)
{
 char *pDestWork, CurPairStr[128];
 int Len, CurPairSize;
 bool ProcOK;
 unsigned long lDestSize;
 const char *pEndStr = "<br>\n";
 double dVal;
 if(!p_pDest || p_lDestSize < 32) return; 
 lDestSize = p_lDestSize - 32;
 CurPairSize = sizeof(CurPairStr);
 ProcOK = false;
 pDestWork = p_pDest;
 for(;;) {
   Len = sprintf_s(CurPairStr, CurPairSize, "\"Flags\":{%s", pEndStr);
   if(lDestSize < (unsigned int)Len) break; strcpy_s(pDestWork, lDestSize, CurPairStr); pDestWork += Len; lDestSize -= Len;
   Len = sprintf_s(CurPairStr, CurPairSize, "\"CSAELV\":\"%s\"%s", TrueFalseStr(m_bCS), pEndStr);
   if(lDestSize < (unsigned int)Len) break; strcpy_s(pDestWork, lDestSize, CurPairStr); pDestWork += Len; lDestSize -= Len;
   Len = sprintf_s(CurPairStr, CurPairSize, ", \"PilotResponse\":\"%d\"%s", m_nPR, pEndStr);
   if(lDestSize < (unsigned int)Len) break; strcpy_s(pDestWork, lDestSize, CurPairStr); pDestWork += Len; lDestSize -= Len;
   Len = sprintf_s(CurPairStr, CurPairSize, ", \"EmulateFlight\":\"%s\"%s", TrueFalseStr(m_bEF), pEndStr);
   if(lDestSize < (unsigned int)Len) break; strcpy_s(pDestWork, lDestSize, CurPairStr); pDestWork += Len; lDestSize -= Len;
   Len = sprintf_s(CurPairStr, CurPairSize, ", \"InitialPlaneToWakeCrossDistance_m\":\"%.0lf\"%s", m_dIPWCD, pEndStr);
   if(lDestSize < (unsigned int)Len) break; strcpy_s(pDestWork, lDestSize, CurPairStr); pDestWork += Len; lDestSize -= Len;
   Len = sprintf_s(CurPairStr, CurPairSize, ", \"WakeSectionToWtpiTolerance_m\":\"%.0lf\"%s", m_dWWT, pEndStr);
   if(lDestSize < (unsigned int)Len) break; strcpy_s(pDestWork, lDestSize, CurPairStr); pDestWork += Len; lDestSize -= Len;
   Len = sprintf_s(CurPairStr, CurPairSize, ", \"UseGroundEffect\":\"%s\"%s", TrueFalseStr(m_bGE), pEndStr);
   if(lDestSize < (unsigned int)Len) break; strcpy_s(pDestWork, lDestSize, CurPairStr); pDestWork += Len; lDestSize -= Len;
   Len = sprintf_s(CurPairStr, CurPairSize, ", \"FlightSpeed\":\"%d\"%s", m_nFS, pEndStr);
   if(lDestSize < (unsigned int)Len) break; strcpy_s(pDestWork, lDestSize, CurPairStr); pDestWork += Len; lDestSize -= Len;
   Len = sprintf_s(CurPairStr, CurPairSize, ", \"FlightTime\":\"%.0lf\"%s", m_dFT, pEndStr);
   if(lDestSize < (unsigned int)Len) break; strcpy_s(pDestWork, lDestSize, CurPairStr); pDestWork += Len; lDestSize -= Len;
   Len = sprintf_s(CurPairStr, CurPairSize, ", \"FlightAutoStop\":\"%s\"%s", TrueFalseStr(m_bFAS), pEndStr);
   if(lDestSize < (unsigned int)Len) break; strcpy_s(pDestWork, lDestSize, CurPairStr); pDestWork += Len; lDestSize -= Len;
   Len = sprintf_s(CurPairStr, CurPairSize, ", \"CreateOutputFiles\":\"%s\"%s", TrueFalseStr(m_bCOF), pEndStr);
   if(lDestSize < (unsigned int)Len) break; strcpy_s(pDestWork, lDestSize, CurPairStr); pDestWork += Len; lDestSize -= Len;
   dVal = m_dWDRT * 180 / M_PI - 180;
   if(dVal < 0) dVal += 360;
   Len = sprintf_s(CurPairStr, CurPairSize, ", \"WindDirectionDegFrom\":\"%.0lf\"%s", dVal, pEndStr);
   if(lDestSize < (unsigned int)Len) break; strcpy_s(pDestWork, lDestSize, CurPairStr); pDestWork += Len; lDestSize -= Len;
   Len = sprintf_s(CurPairStr, CurPairSize, ", \"WindSpeed_kts\":\"%.0lf\"%s", m_dWS * 3.6 / 1.852, pEndStr);
   if(lDestSize < (unsigned int)Len) break; strcpy_s(pDestWork, lDestSize, CurPairStr); pDestWork += Len; lDestSize -= Len;
   Len = sprintf_s(CurPairStr, CurPairSize, ", \"OverrideProximityValues\":\"%s\"%s", TrueFalseStr(m_bOverrideProximityValues), pEndStr);
   if(lDestSize < (unsigned int)Len) break; strcpy_s(pDestWork, lDestSize, CurPairStr); pDestWork += Len; lDestSize -= Len;
   Len = sprintf_s(CurPairStr, CurPairSize, ", \"StartPlaneHOverride\":\"%s\"%s", TrueFalseStr(m_bStartPlaneHOverride), pEndStr);
   if(lDestSize < (unsigned int)Len) break; strcpy_s(pDestWork, lDestSize, CurPairStr); pDestWork += Len; lDestSize -= Len;
   Len = sprintf_s(CurPairStr, CurPairSize, ", \"StartPlaneZOverride\":\"%s\"%s", TrueFalseStr(m_bStartPlaneZOverride), pEndStr);
   if(lDestSize < (unsigned int)Len) break; strcpy_s(pDestWork, lDestSize, CurPairStr); pDestWork += Len; lDestSize -= Len;
   Len = sprintf_s(CurPairStr, CurPairSize, ", \"ClosingAngleRadOverride\":\"%s\"%s", TrueFalseStr(m_bClosingAngleRadOverride), pEndStr);
   if(lDestSize < (unsigned int)Len) break; strcpy_s(pDestWork, lDestSize, CurPairStr); pDestWork += Len; lDestSize -= Len;
   Len = sprintf_s(CurPairStr, CurPairSize, ", \"StartPlaneHOverride\":\"%.0lf\"%s", m_dStartPlaneHOverride, pEndStr);
   if(lDestSize < (unsigned int)Len) break; strcpy_s(pDestWork, lDestSize, CurPairStr); pDestWork += Len; lDestSize -= Len;
   Len = sprintf_s(CurPairStr, CurPairSize, ", \"StartPlaneZOverride\":\"%.0lf\"%s", m_dStartPlaneZOverride, pEndStr);
   if(lDestSize < (unsigned int)Len) break; strcpy_s(pDestWork, lDestSize, CurPairStr); pDestWork += Len; lDestSize -= Len;
   Len = sprintf_s(CurPairStr, CurPairSize, ", \"ClosingAngleRadOverride\":\"%.0lf\"%s", m_dCARdOverride, pEndStr);
   if(lDestSize < (unsigned int)Len) break; strcpy_s(pDestWork, lDestSize, CurPairStr); pDestWork += Len; lDestSize -= Len;
   Len = sprintf_s(CurPairStr, CurPairSize, ", \"OverrideGamma\":\"%s\"%s", TrueFalseStr(m_bOverrideGamma), pEndStr);
   if(lDestSize < (unsigned int)Len) break; strcpy_s(pDestWork, lDestSize, CurPairStr); pDestWork += Len; lDestSize -= Len;
   Len = sprintf_s(CurPairStr, CurPairSize, ", \"WakeGammaOverrideValue\":\"%.0lf\"%s", m_dWakeGammaOverrideValue, pEndStr);
   if(lDestSize < (unsigned int)Len) break; strcpy_s(pDestWork, lDestSize, CurPairStr); pDestWork += Len; lDestSize -= Len;
   Len = sprintf_s(CurPairStr, CurPairSize, ", \"DebugMessages\":\"%s\"%s", TrueFalseStr(m_bDebugMessages), pEndStr);
   if(lDestSize < (unsigned int)Len) break; strcpy_s(pDestWork, lDestSize, CurPairStr); pDestWork += Len; lDestSize -= Len;
   Len = sprintf_s(CurPairStr, CurPairSize, ", \"RealWeather\":\"%s\"%s", TrueFalseStr(m_bRW), pEndStr);
   if(lDestSize < (unsigned int)Len) break; strcpy_s(pDestWork, lDestSize, CurPairStr); pDestWork += Len; lDestSize -= Len;
   Len = sprintf_s(CurPairStr, CurPairSize, ", \"DefaultYear\":\"%d\"%s", m_nDefaultYear, pEndStr);
   if(lDestSize < (unsigned int)Len) break; strcpy_s(pDestWork, lDestSize, CurPairStr); pDestWork += Len; lDestSize -= Len;
   Len = sprintf_s(CurPairStr, CurPairSize, ", \"DefaultMonth\":\"%d\"%s", m_nDefaultMonth, pEndStr);
   if(lDestSize < (unsigned int)Len) break; strcpy_s(pDestWork, lDestSize, CurPairStr); pDestWork += Len; lDestSize -= Len;
   Len = sprintf_s(CurPairStr, CurPairSize, ", \"DefaultDay\":\"%d\"%s", m_nDefaultDay, pEndStr);
   if(lDestSize < (unsigned int)Len) break; strcpy_s(pDestWork, lDestSize, CurPairStr); pDestWork += Len; lDestSize -= Len;
   Len = sprintf_s(CurPairStr, CurPairSize, ", \"Atmosphere_c1\":\"%.3lf\"%s", m_AFs.c1, pEndStr);
   if(lDestSize < (unsigned int)Len) break; strcpy_s(pDestWork, lDestSize, CurPairStr); pDestWork += Len; lDestSize -= Len;
   Len = sprintf_s(CurPairStr, CurPairSize, ", \"Atmosphere_c2\":\"%.3lf\"%s", m_AFs.c2, pEndStr);
   if(lDestSize < (unsigned int)Len) break; strcpy_s(pDestWork, lDestSize, CurPairStr); pDestWork += Len; lDestSize -= Len;
   Len = sprintf_s(CurPairStr, CurPairSize, ", \"EDR\":\"%.3lf\"%s", m_AFs.eta, pEndStr);
   if(lDestSize < (unsigned int)Len) break; strcpy_s(pDestWork, lDestSize, CurPairStr); pDestWork += Len; lDestSize -= Len;
   Len = sprintf_s(CurPairStr, CurPairSize, ", \"GeoidName\":\"%s\"%s", m_GeoidName, pEndStr);
   if(lDestSize < (unsigned int)Len) break; strcpy_s(pDestWork, lDestSize, CurPairStr); pDestWork += Len; lDestSize -= Len;
   Len = sprintf_s(CurPairStr, CurPairSize, ", \"GeoidDir\":\"%s\"%s", m_GeoidDir, pEndStr);
   if(lDestSize < (unsigned int)Len) break; strcpy_s(pDestWork, lDestSize, CurPairStr); pDestWork += Len; lDestSize -= Len;
   Len = sprintf_s(CurPairStr, CurPairSize, ", \"ElevationDataDir\":\"%s\"%s", m_EDD, pEndStr);
   if(lDestSize < (unsigned int)Len) break; strcpy_s(pDestWork, lDestSize, CurPairStr); pDestWork += Len; lDestSize -= Len;
   Len = sprintf_s(CurPairStr, CurPairSize, ", \"MinGamma\":\"%.0f\"%s", m_fMG, pEndStr);
   if(lDestSize < (unsigned int)Len) break; strcpy_s(pDestWork, lDestSize, CurPairStr); pDestWork += Len; lDestSize -= Len;
   Len = sprintf_s(CurPairStr, CurPairSize, ", \"InitGeoid\":\"%s\"%s", TrueFalseStr(m_bInitGeoid), pEndStr);
   if(lDestSize < (unsigned int)Len) break; strcpy_s(pDestWork, lDestSize, CurPairStr); pDestWork += Len; lDestSize -= Len;
   Len = sprintf_s(CurPairStr, CurPairSize, ", \"UseStratificationEffect\":\"%s\"%s", TrueFalseStr(m_bUSE), pEndStr);
   if(lDestSize < (unsigned int)Len) break; strcpy_s(pDestWork, lDestSize, CurPairStr); pDestWork += Len; lDestSize -= Len;
   Len = sprintf_s(CurPairStr, CurPairSize, ", \"UseConeOfUncertaintyForSeverity\":\"%s\"%s", TrueFalseStr(m_bUCOU), pEndStr);
   if(lDestSize < (unsigned int)Len) break; strcpy_s(pDestWork, lDestSize, CurPairStr); pDestWork += Len; lDestSize -= Len;
   Len = sprintf_s(CurPairStr, CurPairSize, ", \"WeatherUndergroundID\":\"%s\"%s", m_WFs.m_WeatherUndergroundID, pEndStr);
   if(lDestSize < (unsigned int)Len) break; strcpy_s(pDestWork, lDestSize, CurPairStr); pDestWork += Len; lDestSize -= Len;
   Len = sprintf_s(CurPairStr, CurPairSize, ", \"WeatherUndergroundHTTP\":\"%s\"%s", m_WFs.m_WeatherUndergroundHTTP, pEndStr);
   if(lDestSize < (unsigned int)Len) break; strcpy_s(pDestWork, lDestSize, CurPairStr); pDestWork += Len; lDestSize -= Len;
   Len = sprintf_s(CurPairStr, CurPairSize, ", \"WeatherUndergroundRequestMask\":\"%s\"%s", m_WFs.m_WeatherUndergroundRequestMask, pEndStr);
   if(lDestSize < (unsigned int)Len) break; strcpy_s(pDestWork, lDestSize, CurPairStr); pDestWork += Len; lDestSize -= Len;
   ProcOK = true;
   break; }
 Len = 0;
 if(!ProcOK)
   Len = sprintf_s(CurPairStr, CurPairSize, "\"More\":\"true\"\n");
 Len += sprintf_s(CurPairStr + Len, CurPairSize - Len, "}\n"); 
 strcpy_s(pDestWork, Len + 1, CurPairStr); 
}
const char *CWEAIF::TrueFalseStr(bool p_bVar)
{
 return(p_bVar ? "true" : "false");
}
bool CTrackPoint::operator < (CTrackPoint &p_Pt)
{
 int ret;
 ret = strcmp(m_CallSign, p_Pt.m_CallSign);
 if(ret == -1) return(true);
 else if(ret == 1) return(false);
 return(m_CTLE.QuadPart < p_Pt.m_CTLE.QuadPart);
}
CSeverity::CSeverity()
{
 AFX_MANAGE_STATE(AfxGetStaticModuleState());
 GetModuleFileName(AfxGetInstanceHandle(), m_ThisDir, sizeof(m_ThisDir)); 
 *strrchr(m_ThisDir, '\\') = '\0';
 m_hLogFile = INVALID_HANDLE_VALUE;
 WriteToLog("WVSS Severity module start"); 
 m_pIntTS = NULL;
 m_pDataBuff = NULL;
 m_dwDataBuffSize = 0;
 m_dwBreakInterval = 10000000; 
 memset(m_LastLoadedFileName, 0, sizeof(m_LastLoadedFileName));
 m_pfMessage = NULL;
 m_pfOnFileLoaded = NULL;
 m_pfUAP = NULL;
 m_dWeatherStationElevation = AVP_BAD_ALTITUDE_VALUE;
 m_fProcessed = 0;
 m_pCurAircraftForWakeProgress = NULL;
 m_pGeoid = NULL;
 m_CurFileType = FILETYPE_NONE;
 m_dwCurInputValueOrdinal = 0;
 m_bCurInputInArray = false;
 m_ObtainWeatherFlag = WVSS_OWF_DEFAULT; 
 m_pGeo = new GeographicLib::Geodesic(GeographicLib::Constants::WGS84_a(), GeographicLib::Constants::WGS84_f());
 m_pLC = new GeographicLib::LocalCartesian(0, 0, 0);
 m_pEncounterLC = new GeographicLib::LocalCartesian(0, 0, 0);
 LoadAirplaneList();
 LoadEDRList();
 m_pScenarios = NULL;
 m_nScenarios = 0;
 m_pCurInputScenario = NULL;
}
CSeverity::~CSeverity()
{
 size_t i;
 if(m_pIntTS) delete [] m_pIntTS;
 for(i = 0; i < m_Aircraft.size(); i ++)
   delete m_Aircraft[i];
 if(m_pGeo) delete m_pGeo;
 if(m_pLC) delete m_pLC;
 if(m_pEncounterLC) delete m_pEncounterLC;
 if(m_pDataBuff) delete m_pDataBuff;
 if(m_pGeoid) delete m_pGeoid;
 if(m_pScenarios) delete [] m_pScenarios;
 if(m_hLogFile != INVALID_HANDLE_VALUE) CloseHandle(m_hLogFile);
}
bool CSeverity::Init()
{
 bool ProcOK = false;
 for(;;) {
   if(m_FSG.m_bInitGeoid)
     if(!InitGeoid()) break;
   ProcOK = true;
   break; }
 return(ProcOK);   
}
void CSeverity::GetSettings(const char *p_pINIFileName)
{
 DWORD dwRetVal;
 short Len;
 char tstr[256];
 float fVal;
 Len = sizeof(tstr) - 1;
 dwRetVal = GetPrivateProfileString("SETTINGS", "WakeSectionToWtpiTolerance", "50", tstr, Len, p_pINIFileName);
 m_FSG.m_dWWT = atof(tstr);
 m_FSG.m_bGE = !!GetPrivateProfileInt("SETTINGS", "UseGroundEffect", 1, p_pINIFileName);
 m_FSG.m_bCS = !!GetPrivateProfileInt("SETTINGS", "CSAELV", 1, p_pINIFileName);
 m_FSG.m_bCOF = !!GetPrivateProfileInt("SETTINGS", "GenerateOutputFiles", 0, p_pINIFileName);
 m_FSG.m_dFT = (short)GetPrivateProfileInt("SETTINGS", "EmulatedFlightTime", 5, p_pINIFileName);
 m_FSG.m_bFAS = !!GetPrivateProfileInt("SETTINGS", "EmulatedFlightAutoStop", 0, p_pINIFileName);
 m_FSG.m_nDefaultYear = GetPrivateProfileInt("SETTINGS", "DefaultYear", 2000, p_pINIFileName);
 m_FSG.m_nDefaultMonth = GetPrivateProfileInt("SETTINGS", "DefaultMonth", 1, p_pINIFileName);
 m_FSG.m_nDefaultDay = GetPrivateProfileInt("SETTINGS", "DefaultDay", 1, p_pINIFileName);
 m_FSG.m_bDebugMessages = !!GetPrivateProfileInt("SETTINGS", "DebugMessages", 1, p_pINIFileName);
 dwRetVal = GetPrivateProfileString("ATMOSPHERE", "DefaultWindDirection_deg_from", "270", tstr, Len, p_pINIFileName);
 fVal = (float)Radians((float)atof(tstr) - 180); 
 if(fVal < 0) fVal += (float)(2 * M_PI);
 m_FSG.m_dWDRT = fVal;
 dwRetVal = GetPrivateProfileString("ATMOSPHERE", "DefaultWindSpeed_kts", "0", tstr, Len, p_pINIFileName);
 m_FSG.m_dWS = (float)atof(tstr) * 1.852 / 3.6; 
 m_FSG.m_fMG = (float)GetPrivateProfileInt("SETTINGS", "MinGamma", 150, p_pINIFileName);
 sprintf(tstr, "%s\\%s", m_ThisDir, "WVSSSensitivityAnalysis.ini");
#pragma message("don't do it like this. Have a local structure and assign when needed")
 LoadSensitivityAnalysisFromFile(tstr);
 m_FSG.m_bRW = (GetPrivateProfileInt("SETTINGS", "RealWeather", 1, p_pINIFileName) == 1);
 m_FSG.m_bUSE = (GetPrivateProfileInt("SETTINGS", "UseStratificationEffect", 0, p_pINIFileName) == 1);
 m_FSG.m_bUCOU = (GetPrivateProfileInt("SETTINGS", "UseConeOfUncertaintyForSeverity", 0, p_pINIFileName) == 1);
 dwRetVal = GetPrivateProfileString("ATMOSPHERE", "c1", "0.31", tstr, Len, p_pINIFileName);
 m_FSG.m_AFs.c1 = (float)atof(tstr);
 dwRetVal = GetPrivateProfileString("ATMOSPHERE", "c2", "0.61", tstr, Len, p_pINIFileName);
 m_FSG.m_AFs.c2 = (float)atof(tstr);
 dwRetVal = GetPrivateProfileString("ATMOSPHERE", "EDR", "0.1", tstr, Len, p_pINIFileName);
 m_FSG.m_AFs.eta = (float)atof(tstr);
 dwRetVal = GetPrivateProfileString("ATMOSPHERE", "StratificationValue", "0", tstr, Len, p_pINIFileName);
 m_FSG.m_AFs.m_dStratification = (float)atof(tstr);
 dwRetVal = GetPrivateProfileString("ELEVATION", "GeoidName", "EGM96", m_FSG.m_GeoidName, sizeof(m_FSG.m_GeoidName), p_pINIFileName);
 sprintf(m_FSG.m_GeoidDir, "%s\\", m_ThisDir);
 dwRetVal = GetPrivateProfileString("ELEVATION", "GeoidDataDir", "GeoidData", m_FSG.m_GeoidDir + strlen(m_FSG.m_GeoidDir), Len - strlen(m_FSG.m_GeoidDir), p_pINIFileName);
 sprintf(m_FSG.m_EDD, "%s\\", m_ThisDir);
 dwRetVal = GetPrivateProfileString("ELEVATION", "ElevationDataDir", "ElevationData", m_FSG.m_EDD + strlen(m_FSG.m_EDD), Len - strlen(m_FSG.m_EDD), p_pINIFileName);
 dwRetVal = GetPrivateProfileString("WEATHER", "WeatherUndergroundID", "0", m_FSG.m_WFs.m_WeatherUndergroundID, sizeof(m_FSG.m_WFs.m_WeatherUndergroundID) - 1, p_pINIFileName);
 dwRetVal = GetPrivateProfileString("WEATHER", "WeatherUndergroundHTTP", "$WeatherUndergroundHTTP$", m_FSG.m_WFs.m_WeatherUndergroundHTTP, sizeof(m_FSG.m_WFs.m_WeatherUndergroundHTTP) - 1, p_pINIFileName);
 dwRetVal = GetPrivateProfileString("WEATHER", "WeatherUndergroundRequestMask", "$WeatherUndergroundRequestMask$", m_FSG.m_WFs.m_WeatherUndergroundRequestMask, sizeof(m_FSG.m_WFs.m_WeatherUndergroundRequestMask) - 1, p_pINIFileName);
 LoadScenarios(p_pINIFileName);
}
void CSeverity::LoadScenarios(const char *p_pINIFileName)
{
 DWORD dwRetVal;
 char tstr[256], EntryName[64];
 short i;
 size_t Len[3];
 if(m_pScenarios) delete [] m_pScenarios;
 m_nScenarios = 0;
 Len[0] = sizeof(((CWVSSScenario *)0)->m_ScenarioName) - 1;
 Len[1] = sizeof(((CWVSSScenario *)0)->m_AirportName) - 1;
 Len[2] = sizeof(tstr) - 1;
 dwRetVal = GetPrivateProfileString("SCENARIO", "ScenarioCount", "0", tstr, Len[2], p_pINIFileName);
 m_nScenarios = atoi(tstr);
 m_pScenarios = new CWVSSScenario[m_nScenarios];
 if(!m_pScenarios) { m_nScenarios = 0; return; }
 for(i = 0; i < m_nScenarios; i ++) {
   sprintf(EntryName, "SCENARIO%d", i + 1);
   dwRetVal = GetPrivateProfileString("SCENARIO", EntryName, "", m_pScenarios[i].m_ScenarioName, Len[0] - 1, p_pINIFileName);
   if(dwRetVal == Len[0] - 1 || dwRetVal == Len[0] - 2 || !*m_pScenarios[i].m_ScenarioName) break;
   sprintf(EntryName, "SCENARIO_%s", m_pScenarios[i].m_ScenarioName);
   dwRetVal = GetPrivateProfileString(EntryName, "Name", "", m_pScenarios[i].m_AirportName, Len[1] - 1, p_pINIFileName);
   dwRetVal = GetPrivateProfileString(EntryName, "CenterLat", "0", tstr, Len[2] - 1, p_pINIFileName);
   m_pScenarios[i].m_dCenterLatDeg = atof(tstr);
   dwRetVal = GetPrivateProfileString(EntryName, "CenterLong", "0", tstr, Len[2] - 1, p_pINIFileName);
   m_pScenarios[i].m_dCenterLongDeg = atof(tstr);
   }
}
const CWVSSScenario *CSeverity::ScenarioByName(const char *p_pName)
{
 short i;
 for(i = 0; i < m_nScenarios; i ++) {
   if(!stricmp(m_pScenarios[i].m_ScenarioName, p_pName))
     return(m_pScenarios + i); }
 return(NULL);
}
bool CSeverity::OpenLogFile(char *p_pDT )
{
 char FileTitle[64], FileName[MAX_PATH];
 COleDateTime DT = COleDateTime::GetCurrentTime();
 if(p_pDT)
   sprintf(p_pDT, "%.4d-%.2d-%.2d %.2d:%.2d:%.2d", DT.GetYear(), DT.GetMonth(), DT.GetDay(), DT.GetHour(), DT.GetMinute(), DT.GetSecond());
 sprintf(FileTitle, "WVSSSeverity_%.4d%.2d%.2d", DT.GetYear(), DT.GetMonth(), DT.GetDay());
 if(m_hLogFile != INVALID_HANDLE_VALUE && !_stricmp(FileTitle, m_LogFileTitle)) 
   return(true);
 if(m_hLogFile != INVALID_HANDLE_VALUE)
   CloseHandle(m_hLogFile);
 strcpy(m_LogFileTitle, FileTitle);
 sprintf(FileName, "%s\\%s.log", m_ThisDir, m_LogFileTitle);
 m_hLogFile = CreateFile(FileName, GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
 return(m_hLogFile != INVALID_HANDLE_VALUE);
}
bool CSeverity::LoadAirplaneList()
{
 char tstr[256];
 bool bVal1, bVal2;
 sprintf(tstr, "%s\\WVSSAirplaneList.bin", m_ThisDir);
 bVal1 = m_AirplaneMgr.LoadFromFile(tstr, true);
 if(!bVal1) {
   sprintf(tstr, "%s\\WVSSAirplaneList.txt", m_ThisDir);
   bVal2 = m_AirplaneMgr.LoadFromFile(tstr, false);
   sprintf(tstr, "%s\\WVSSAirplaneList.bin", m_ThisDir);
   m_AirplaneMgr.SaveToFile(tstr, true);
   }
 if(bVal1 || bVal2) {
   Message("Aircraft list loaded from %s", tstr);
   return(true); }
 return(true);
}
bool CSeverity::LoadFile(LPCSTR p_pFileName, IWVSSSeverity::eFileType p_FileType, bool p_bFirst, bool p_bLast)
{
 FILE *fFile;
 char ErrStr[256];
 CTrackPoint TP;
 DWORD li, dwFileSize, dwStartTPIntSize, dwTime;
 std::list<CTrackPoint>::iterator It, PrevIt;
 bool bInterpolation, bProcessFirstPoint;
 C2D Wind2D;
 eFileType FileType;
 SYSTEMTIME DataDateST;
 LONGLONG LLVal;
 dwTime = GetTickCount();
 strcpy(m_LastLoadedFileName, p_pFileName);
 FileType = p_FileType;
 GetFileTypeDateAndSizeFromName(p_pFileName, &FileType, &DataDateST, &dwFileSize); 
 switch(FileType) {
   case FILETYPE_ISA: bInterpolation = true; break;
   case FILETYPE_FDR: bInterpolation = false; break;
   case FILETYPE_WVSS_JSON: bInterpolation = true; break;
   case FILETYPE_ASAIC_JSON: bInterpolation = true; break;
   case FILETYPE_MITRE: bInterpolation = true; break;
   default:
     sprintf(ErrStr, "Unsupported file type %d", (int)FileType);
     AddToErrorList(ErrStr);
     return(false); }
 Message("Track file %s", p_pFileName);
 m_ErrorList.RemoveAll();
 m_TrackPoints.clear();
 m_TrackPointsV.clear();
 if(p_bFirst) {
   m_TrackPointsInt.clear();
   for(li = 0; li < m_Aircraft.size(); li ++)
     delete m_Aircraft[li];
   m_Aircraft.clear();
   m_dwTSCount = 0; }
 fFile = fopen(p_pFileName, "rb");
 if(!fFile) { AddToErrorList((CString)"File " + p_pFileName + " failed to open"); return(false); }
 FillTrackMetaInfo(p_pFileName); 
 if(!ReadFile(fFile, FileType, &DataDateST, dwFileSize)) return(false);
 fclose(fFile);
 if(!m_TrackPoints.size()) {
   AddToErrorList("The file produced an empty data set");
   return(false); }
 m_TrackPoints.sort();  
 SetStartAndEndPointFlag(); 
#pragma message ("We should generate WVSS JSON from ASAIC, but the input file is also JSON. Original file will be overwritten.")
 if(FileType == FILETYPE_ISA || FileType == FILETYPE_MITRE )
   WriteJSONFromSource(p_pFileName); 
 LoadWeatherData(p_pFileName); 
 for(It = m_TrackPoints.begin(); It != m_TrackPoints.end(); It ++) {
   if(It->m_bFirstPoint) {
     bProcessFirstPoint = true;
     PrevIt = It;
     continue; 
     }
   LLVal = It->m_CTLE.QuadPart - PrevIt->m_CTLE.QuadPart;
   if(LLVal < (DWORD)((double)m_dwBreakInterval * 2)) { 
     if(PrevIt->m_bFirstPoint && bProcessFirstPoint) { 
       PrevIt->m_CreationTimeOrig = PrevIt->m_CreationTime; 
       AdjustToBoundary(&PrevIt->m_CTLE.QuadPart, m_dwBreakInterval); 
       LIToSystemTime(&PrevIt->m_CTLE, &PrevIt->m_CreationTime);
       m_TrackPointsV.push_back(*PrevIt); 
       bProcessFirstPoint = false;
       }
     It->m_CreationTimeOrig = It->m_CreationTime; 
     AdjustToBoundary(&It->m_CTLE.QuadPart, m_dwBreakInterval); 
     LIToSystemTime(&It->m_CTLE, &It->m_CreationTime);
     if(It->m_CTLE.QuadPart - PrevIt->m_CTLE.QuadPart >= m_dwBreakInterval) {
       m_TrackPointsV.push_back(*It);
       PrevIt = It;
       }
     }
   else { 
     if(PrevIt->m_bFirstPoint) {
       m_TrackPointsV.push_back(*PrevIt); 
       bProcessFirstPoint = false; }
     m_TrackPointsV.push_back(*It); 
     PrevIt = It;
     }
   }
 m_TrackPoints.clear();
 SetStartAndEndPointV();
 WriteOBTFile(p_pFileName, false, false);
 dwStartTPIntSize = m_TrackPointsInt.size(); 
 if(bInterpolation) {
   if(!Interpolate2(p_pFileName)) {
     sprintf(ErrStr, "Track interpolation failed.");
     AddToErrorList(ErrStr);
     return(false); } }
 else { 
   for(li = 0; li < m_TrackPointsV.size(); li ++)
     m_TrackPointsInt.push_back(m_TrackPointsV[li]); }
 m_TrackPointsV.clear(); 
 FillInWeatherData(dwStartTPIntSize);
 if(p_bLast) {
   SetStartEndPointAndAssignAircraft();
   CreateIntTP();
   if(!LoadFileContinued2(p_pFileName)) return(false);
   Message("%ld aircraft, %ld interpolated points", m_Aircraft.size(), m_dwTSCount);
   Message("FILE LOADED SUCCESSFULLY in %.1lf sec", (double)(GetTickCount() - dwTime) / 1000); }
 return(true);
}
bool CSeverity::LoadFileContinued2(LPCSTR p_pFileName)
{
 DWORD li, lj, lVal, dwTime;
 CTrackPoint TP, *pTP, *pTPP;
 char tstr[256];
 bool bVal;
 float fProcessedStart; 
 const CAPM::RAPL3 *pCurAirplane;
 dwTime = GetTickCount();
 fProcessedStart = m_fProcessed; 
 m_fCurProcessedTotal = (float)0.4; 
 *TP.m_CallSign = '\0';
 for(li = 0; li < m_TrackPointsInt.size(); li ++) {
   LIToSystemTime(&m_TrackPointsInt[li].m_CTLE, &m_TrackPointsInt[li].m_CreationTime); 
   if(m_TrackPointsInt[li].m_CreationTime.wYear == 0) { Message("Internal datetime error 386"); ASSERT(0); return(false); }
   if(!stricmp(m_TrackPointsInt[li].m_CallSign, TP.m_CallSign)) 
     TP.m_lPoints ++;
   else { 
     if(li) {
       m_TrackPointsInt[li - 1].m_bLastPoint = true;
       m_Aircraft[m_TrackPointsInt[li - 1].m_nAircraftOrdinal]->m_lTrackPointCount = TP.m_lPoints;
       }
     TP.m_lStartIndex = li;
     TP.m_lPoints = 1;
     strcpy(TP.m_CallSign, m_TrackPointsInt[li].m_CallSign);
     m_Aircraft[m_TrackPointsInt[li].m_nAircraftOrdinal]->m_lStartTrackPointIndex = li;
     m_Aircraft[m_TrackPointsInt[li].m_nAircraftOrdinal]->m_lStartPos = (DWORD)((m_TrackPointsInt[li].m_CTLE.QuadPart - m_llMinTime) / m_dwBreakInterval);
     }
   m_TrackPointsInt[li].m_lStartIndex = TP.m_lStartIndex;
   m_TrackPointsInt[m_TrackPointsInt[li].m_lStartIndex].m_lPoints = TP.m_lPoints; }
 m_TrackPointsInt[m_TrackPointsInt.size() - 1].m_bLastPoint = true;
 m_Aircraft[m_TrackPointsInt[m_TrackPointsInt.size() - 1].m_nAircraftOrdinal]->m_lTrackPointCount = TP.m_lPoints;
 m_fProcessed = fProcessedStart + (float)0.5 * m_fCurProcessedTotal;
 WriteOBTFile(p_pFileName, true, true);
 WriteCesiumFile(p_pFileName);
 WriteInterpolatedJSON(p_pFileName);
 for(li = 0; li < m_TrackPointsInt.size(); li ++) {
   if(li)
     m_TrackPointsInt[li].m_dLifetime = (double)(m_TrackPointsInt[li].m_CTLE.QuadPart - m_TrackPointsInt[m_TrackPointsInt[li].m_lStartIndex].m_CTLE.QuadPart) / 10000000;
   }
 m_SA.AdjustValue(&m_FSG.m_AFs.c1, CSensitivityAnalysisSetup::AVP_SAVT_C1);
 m_SA.AdjustValue(&m_FSG.m_AFs.c2, CSensitivityAnalysisSetup::AVP_SAVT_C2);
 m_SA.AdjustValue(&m_FSG.m_AFs.eta, CSensitivityAnalysisSetup::AVP_SAVT_EDR);
 for(li = 0; li < m_Aircraft.size(); li ++) {
   m_Aircraft[li]->m_Wake.Clear();
   m_Aircraft[li]->m_Wake.SetFlags(&m_FSG);
   m_Aircraft[li]->m_Wake.SetBreakInterval(m_dwBreakInterval);
   pCurAirplane = m_AirplaneMgr.GALBNa(m_Aircraft[li]->m_ACModel);
   m_Aircraft[li]->m_Wake.SATe(pCurAirplane);
   bVal = (pCurAirplane != NULL);
   sprintf(tstr, "%s [%s-%s]: Aircraft type %s [ICAO Cat %s, FAA Cat %s] is %s", 
           m_Aircraft[li]->m_CallSign, m_Aircraft[li]->m_DepartureAirport, m_Aircraft[li]->m_ArrivalAirport, m_Aircraft[li]->m_ACModel, 
           bVal ? CAPM::GetICAOCatName(m_Aircraft[li]->m_pAircraft->m_ICAOCat) : "unknown",
           bVal ? CAPM::GetFAACatName(m_Aircraft[li]->m_pAircraft->m_FAACat) : "unknown",
           bVal ? "OK" : "unknown");
   Message(tstr); }
 for(li = 0; li < m_dwTSCount; li ++) {
   for(lj = 0; lj < m_pIntTS[li].m_TP.size(); lj ++) {
     lVal = m_pIntTS[li].m_TP[lj];
     pTP = &m_TrackPointsInt[lVal];
     pTPP = (li > m_Aircraft[pTP->m_nAircraftOrdinal]->m_lStartPos) ? &m_TrackPointsInt[lVal - 1] : pTP; 
     m_Aircraft[pTP->m_nAircraftOrdinal]->m_Wake.ADSEDC(pTP, false);
     }
   }
 Message("Wake setup completed in %.1lf sec", (double)(GetTickCount() - dwTime) / 1000);
 LogMissingElevationDataAreas(p_pFileName);
 WriteInterpolatedFile(p_pFileName);
 m_fProcessed = fProcessedStart + (float)0.6 * m_fCurProcessedTotal;
 m_pCurAircraftForWakeProgress = NULL;
 for(li = 0; li < m_Aircraft.size(); li ++) {
   m_pCurAircraftForWakeProgress = m_Aircraft[li];
   m_Aircraft[li]->m_Wake.m_fProcessedTotal = (float)0.3 * m_fCurProcessedTotal / m_Aircraft.size();
   if(!m_Aircraft[li]->m_Wake.OSAeD(p_pFileName)) {
     Message("Wake engine returned an error. Error message follows.");
     Message(m_Aircraft[li]->m_Wake.m_ErrorStr);
     return(false); }
   m_fProcessed = GetProcessedPercentage();
   }
 m_pCurAircraftForWakeProgress = NULL;
 m_fProcessed = fProcessedStart + (float)0.9 * m_fCurProcessedTotal;
 for(li = 0; li < m_Aircraft.size(); li ++)
   Message("%s [%s]: %ld points, %ld reallocations, wake memory size %.1lfMB", m_Aircraft[li]->m_CallSign, m_Aircraft[li]->m_ACModel, m_Aircraft[li]->m_Wake.GNOS(), m_Aircraft[li]->m_Wake.GetSectionHistoryManager()->m_lMemReallocCount, m_Aircraft[li]->m_Wake.GetMemoryUsedMB());
 for(li = 0; li < m_Aircraft.size(); li ++) {
   m_Aircraft[li]->m_bCurOn = false;
   m_Aircraft[li]->m_bLastOn = false; }
 m_fProcessed = fProcessedStart + (float)1 * m_fCurProcessedTotal;
 m_fCurProcessedTotal = (float)0.2;
 OnFileLoaded(p_pFileName);
 return(true);
}
bool CSeverity::ReadFile(FILE *p_fFile, eFileType p_FileType, SYSTEMTIME *p_pST, DWORD p_dwFileSize)
{
 char ErrStr[256];
 bool ProcOK = true;
 m_ObtainWeatherFlag = WVSS_OWF_DEFAULT; 
 *m_UseWeatherStation = '\0'; 
 m_dWeatherStationElevation = 0; 
 m_pCurInputListStartingPointIt = m_TrackPoints.end();
 m_nCurUnnamedCallsignNum = 1;
 m_pCurInputScenario = NULL;
 m_CurTrackPointToAdd.Init(); 
 switch(p_FileType) { 
   case FILETYPE_ISA: if(!ReadFileISA2(p_fFile, p_pST)) ProcOK = false; break;
   case FILETYPE_FDR: if(!ReadFileFDR(p_fFile, p_FileType)) ProcOK = false; break;
   case FILETYPE_WVSS_JSON: m_CurFileType = FILETYPE_WVSS_JSON; if(!ReadFileWVSSJSON(p_fFile, p_dwFileSize)) ProcOK = false; break;
   case FILETYPE_ASAIC_JSON: m_CurFileType = FILETYPE_ASAIC_JSON; if(!ReadFileASAICJSON(p_fFile, p_dwFileSize)) ProcOK = false; break;
   case FILETYPE_MITRE: if(!ReadFileMITRE(p_fFile)) ProcOK = false; break;
   default: sprintf(ErrStr, "Unsupported file type %d", (int)p_FileType); AddToErrorList(ErrStr); ProcOK = false; }
 if(!ProcOK) m_CurFileType = FILETYPE_NONE;
 return(ProcOK);
}
bool CSeverity::ReadFileISA2(FILE *p_fFile, SYSTEMTIME *p_pST)
{
 char tstr[4096], tstr2[128], *pWork, WhiteSpaceStr[] = "\t ";
 CTrackPoint TP;
 const char *pHeaderItems[] = { "ACMODEL", "ALTITUDE", "CALLSIGN", "ADEP", "ADES", "DATETIME", "LAT", "LONG", "SPEED", "WINDSPEED", "UWIND", "VWIND", "WINDDIRECTION" };
 const char *pAllowedModifierWindSpeed[] = { "KTS", "MS" };
 const char *pAllowedModifierDatetime[] = { "UTC" };  
 const char *pAllowedModifierAltitude[] = { "100FEET", "FEET", "METERS" }; 
 const char *pAllowedModifierSpeed[] = { "KTS", "KPH", "MS" };
 const char *pAllowedModifierLatLong[] = { "DEGREES" };
 const char *pAllowedWindDirectionModifiers[] = { "FROMDEG", "TODEG" };
 const short HeaderCount = sizeof(pHeaderItems) / sizeof(const char *);
 char ValueModifier[HeaderCount][32];
 short ValueModifierIndex[HeaderCount];
 const short MaxColumns = 1000; 
 short HeaderIndex[MaxColumns]; 
 short i, j, HeaderLength[HeaderCount], CurColumn;
 time_t Timer;
 tm *pTM;
 double dUWindSpeedMS, dVWindSpeedMS;
 bool m_bUWindSupplied, m_bVWindSupplied; 
 if(!fgets(tstr, sizeof(tstr), p_fFile)) { AddToErrorList("Not a single line in the file to read"); return(false); }
 if(pWork = strrchr(tstr, '\r')) *pWork = '\0';
 else if(pWork = strrchr(tstr, '\n')) *pWork = '\0';
 memset(ValueModifier, 0, sizeof(ValueModifier));
 for(i = 0; i < HeaderCount; i ++) {
   HeaderLength[i] = strlen(pHeaderItems[i]);
   ValueModifierIndex[i] = -1; }
 for(i = 0; i < MaxColumns; i ++)
   HeaderIndex[i] = -1;
 pWork = strtok(tstr, WhiteSpaceStr);
 if(!pWork) { AddToErrorList("Header is not available. Items must be separated by tabs or spaces."); return(false); }
 CurColumn = 0;
 m_bUWindSupplied = m_bVWindSupplied = false;
 while(pWork) {
   for(i = 0; i < HeaderCount; i ++) {
     if(!strnicmp(pWork, pHeaderItems[i], HeaderLength[i])) {
       strcpy(ValueModifier[i], pWork + HeaderLength[i]);
       HeaderIndex[CurColumn] = i;
       if(!*ValueModifier[i]) break; 
       switch(i) {
         case 1: 
           for(j = 0; j < sizeof(pAllowedModifierAltitude) / sizeof(const char *); j ++) {
             if(!stricmp(ValueModifier[i], pAllowedModifierAltitude[j])) {
               ValueModifierIndex[i] = j;
               break; } }
           break;
         case 5: 
           for(j = 0; j < sizeof(pAllowedModifierDatetime) / sizeof(const char *); j ++) {
             if(!stricmp(ValueModifier[i], pAllowedModifierDatetime[j])) {
               ValueModifierIndex[i] = j;
               break; } }
           break;
         case 6:
         case 7: 
           for(j = 0; j < sizeof(pAllowedModifierLatLong) / sizeof(const char *); j ++) {
             if(!stricmp(ValueModifier[i], pAllowedModifierLatLong[j])) {
               ValueModifierIndex[i] = j;
               break; } }
           break;
         case 8: 
           for(j = 0; j < sizeof(pAllowedModifierSpeed) / sizeof(const char *); j ++) {
             if(!stricmp(ValueModifier[i], pAllowedModifierSpeed[j])) {
               ValueModifierIndex[i] = j;
               break; } }
           break;
         case 9: 
           for(j = 0; j < sizeof(pAllowedModifierWindSpeed) / sizeof(const char *); j ++) {
             if(!stricmp(ValueModifier[i], pAllowedModifierWindSpeed[j])) {
               ValueModifierIndex[i] = j;
               break; } }
           break;
         case 10: 
           m_bUWindSupplied = true;
           for(j = 0; j < sizeof(pAllowedModifierWindSpeed) / sizeof(const char *); j ++) {
             if(!stricmp(ValueModifier[i], pAllowedModifierWindSpeed[j])) {
               ValueModifierIndex[i] = j;
               break; } }
           break;
         case 11: 
           m_bVWindSupplied = true;
           for(j = 0; j < sizeof(pAllowedModifierWindSpeed) / sizeof(const char *); j ++) {
             if(!stricmp(ValueModifier[i], pAllowedModifierWindSpeed[j])) {
               ValueModifierIndex[i] = j;
               break; } }
           break;
         case 12: 
           for(j = 0; j < sizeof(pAllowedWindDirectionModifiers) / sizeof(const char *); j ++) {
             if(!stricmp(ValueModifier[i], pAllowedWindDirectionModifiers[j])) {
               ValueModifierIndex[i] = j;
               break; } }
           break;
         } 
       break; } }
   pWork = strtok(NULL, WhiteSpaceStr);
   CurColumn ++; }
 while(fgets(tstr, sizeof(tstr), p_fFile)) {
   if(pWork = strrchr(tstr, '\r')) *pWork = '\0';
   else if(pWork = strrchr(tstr, '\n')) *pWork = '\0';
   if(!*tstr) continue;
   TP.Init();
   CurColumn = 0;
   pWork = strtok(tstr, WhiteSpaceStr);
   while(pWork) {
     if(HeaderIndex[CurColumn] == -1) {
       pWork = strtok(NULL, WhiteSpaceStr);
       CurColumn ++;
       continue; }
     switch(HeaderIndex[CurColumn]) {
       case 0: 
         strcpy_s(TP.m_ACModel, sizeof(TP.m_ACModel), pWork);
         break;
       case 1: 
         switch(ValueModifierIndex[HeaderIndex[CurColumn]]) {
           case 0: 
             TP.m_dAltitude100Ft = atof(pWork);
             break;
           case 1: 
             TP.m_dAltitude100Ft = atof(pWork) / 100;
             break;
           case 2: 
           default:
             TP.m_dAltitude100Ft = atof(pWork) / 0.3048 / 100;
             break; }
         break;
       case 2: 
         strcpy_s(TP.m_CallSign, sizeof(TP.m_CallSign), pWork);
         break;
       case 3: 
         strcpy_s(TP.m_DepartureAirport, sizeof(TP.m_DepartureAirport), pWork);
         break;
       case 4: 
         strcpy_s(TP.m_ArrivalAirport, sizeof(TP.m_ArrivalAirport), pWork);
         break;
       case 5: 
         switch(ValueModifierIndex[HeaderIndex[CurColumn]]) {
         case 0: 
           Timer = atol(pWork);
           pTM = gmtime(&Timer);
           TP.m_CreationTime.wYear = pTM->tm_year + 1900;
           TP.m_CreationTime.wMonth = pTM->tm_mon + 1;
           TP.m_CreationTime.wDay = pTM->tm_mday;
           TP.m_CreationTime.wHour = pTM->tm_hour;
           TP.m_CreationTime.wMinute = pTM->tm_min;
           TP.m_CreationTime.wSecond = pTM->tm_sec;
           TP.m_CreationTime.wMilliseconds = 0;
           break;
         default: 
           TP.m_CreationTime.wYear = p_pST->wYear;
           TP.m_CreationTime.wMonth = p_pST->wMonth;
           TP.m_CreationTime.wDay = p_pST->wDay;
           if(!DateTimeFromStr(pWork, ValueModifier[HeaderIndex[CurColumn]], &TP.m_CreationTime)) {
             sprintf(tstr2, "Datetime %s is invalid for format %s", pWork, ValueModifier[HeaderIndex[CurColumn]]);
             AddToErrorList(tstr2);
             return(false); }
           break; }
         TP.m_CreationTimeOrig = TP.m_CreationTime;
         if(!SystemTimeToLI(&TP.m_CreationTime, &TP.m_CTLE)) {
           sprintf(tstr2, "Time conversion error, time value %s is invalid", pWork);
           AddToErrorList(tstr2);
           return(false); }
         break;
       case 6: 
         switch(ValueModifierIndex[HeaderIndex[CurColumn]]) {
           case 0: 
           default:
             TP.m_dLatDeg = atof(pWork);
             break; }
         break;
       case 7: 
         switch(ValueModifierIndex[HeaderIndex[CurColumn]]) {
           case 0: 
           default:
             TP.m_dLongDeg = atof(pWork);
             break; }
         break;
       case 8: 
         switch(ValueModifierIndex[HeaderIndex[CurColumn]]) {
           case 1: 
             TP.m_dInitialLegSpeedMS = atof(pWork) / 3.6;
             break;
           case 2: 
             TP.m_dInitialLegSpeedMS = atof(pWork);
             break;
           case 0: 
           default:
             TP.m_dInitialLegSpeedMS = atof(pWork) * 1.852 / 3.6;
             break; }
         break;
       case 9: 
         switch(ValueModifierIndex[HeaderIndex[CurColumn]]) {
           case 0: 
             TP.m_dWindSpeedMS = atoi(pWork) * 1.852 / 3.6;
             break;
           case 1: 
             TP.m_dWindSpeedMS = atoi(pWork);
             break;
           }
         break;
       case 10: 
         switch(ValueModifierIndex[HeaderIndex[CurColumn]]) {
           case 0: 
             dUWindSpeedMS = atof(pWork) * 1.852 / 3.6;
             break;
           case 1: 
             dUWindSpeedMS = atof(pWork);
             break;
           }
         break;
       case 11: 
         switch(ValueModifierIndex[HeaderIndex[CurColumn]]) {
           case 0: 
             dVWindSpeedMS = atof(pWork) * 1.852 / 3.6;
             break;
           case 1: 
             dVWindSpeedMS = atof(pWork);
             break;
           }
         break;
       case 12: 
         switch(ValueModifierIndex[HeaderIndex[CurColumn]]) {
           case 0: 
             TP.m_dWDRT = (atof(pWork) + 180) * M_PI / 180;
             if(TP.m_dWDRT < -M_PI) TP.m_dWDRT += 2 * M_PI;
             if(TP.m_dWDRT > M_PI) TP.m_dWDRT -= 2 * M_PI;
             break;
           case 1: 
             TP.m_dWDRT = atof(pWork) * M_PI / 180;
             if(TP.m_dWDRT < -M_PI) TP.m_dWDRT += 2 * M_PI;
             if(TP.m_dWDRT > M_PI) TP.m_dWDRT -= 2 * M_PI;
             break;
           }
         break;
       }
     pWork = strtok(NULL, WhiteSpaceStr);
     CurColumn ++; }
   if(m_bUWindSupplied && m_bVWindSupplied) { 
     TP.m_dWindSpeedMS = sqrt(dUWindSpeedMS * dUWindSpeedMS + dVWindSpeedMS * dVWindSpeedMS);
     TP.m_dWDRT = M_PI / 2 - atan2(dVWindSpeedMS, dUWindSpeedMS);
     if(TP.m_dWDRT < - M_PI) TP.m_dWDRT += 2 * M_PI;
     if(TP.m_dWDRT > M_PI) TP.m_dWDRT -= 2 * M_PI; }
   m_TrackPoints.push_back(TP); }
 return(true);
}
bool CSeverity::ReadFileMITRE(FILE *p_fFile)
{
 bool bVal;
 CWVSSLoaderLineParserMITRE LineParser;
 LineParser.m_pSeverity = this;
 LineParser.m_pTrackPoints = &m_TrackPoints;
 bVal = LineParser.Load(p_fFile);
 if(!LineParser.GetProcessedLineCount())
   AddToErrorList("Not a single line in the file to read");
 return(bVal);
}
bool CSeverity::ReadFileFDR(FILE *p_fFile, eFileType p_FileType)
{
 char tstr[4096], tstr2[64]; 
 char *pStartWork, *pEndWork;
 CTrackPoint TP, LastTP;
 short i, j, Count;
 short Type2Cols[] = { 1, 2, 3, 8, 9, 13, 14, 17, 18, 59 };
 bool bVal;
 C2D Wind2D;
 unsigned long li, lStartIndex, lEndIndex;
 std::list <CTrackPoint>::iterator TPI;
 std::list <CTrackPoint>::iterator TPI1;
 LARGE_INTEGER LI;
 DWORD lLines = 0, lLastTimedLine; 
 LARGE_INTEGER liLastTime, liCurTime;
 double dVal;
 liLastTime.QuadPart = 0;
 liCurTime.QuadPart = 0;
 lLastTimedLine = 0xFFFFFFFF;
 Count = sizeof(Type2Cols) / sizeof(short);
 if(!fgets(tstr, sizeof(tstr), p_fFile)) { AddToErrorList("Not a single line in the file to read"); return(false); }
 while(fgets(tstr, sizeof(tstr), p_fFile)) {
   memset(&TP, 0, sizeof(TP));
   pStartWork = tstr;
   i = j = 0;
   while(true) {
     pEndWork = strchr(pStartWork, ',');
     if(pEndWork)
       *pEndWork = '\0';
     if(j == Type2Cols[i]) { 
       strcpy(tstr2, pStartWork);
       switch(Type2Cols[i]) {
         case 1:
           bVal = FromChar(tstr2, &TP.m_CTLE, p_FileType, Type2Cols[i]);
           if(liCurTime.QuadPart)
             liCurTime.QuadPart += 10000000;
           if(TP.m_CTLE.QuadPart) {
             if(!liCurTime.QuadPart)
               liCurTime = TP.m_CTLE;
             if(liCurTime.QuadPart != TP.m_CTLE.QuadPart) {
               TP.m_CTLE.QuadPart = liCurTime.QuadPart;
               } }
           LIToSystemTime(&TP.m_CTLE, &TP.m_CreationTime);
           TP.m_CreationTimeOrig = TP.m_CreationTime;
           break;
         case 2: bVal = FromChar(tstr2, &TP.m_dLatDeg, p_FileType, Type2Cols[i]); break;
         case 3: bVal = FromChar(tstr2, &TP.m_dLongDeg, p_FileType, Type2Cols[i]); break;
         case 8: bVal = FromChar(tstr2, &TP.m_dBankDeg, p_FileType, Type2Cols[i]); break;
         case 9: bVal = FromChar(tstr2, &TP.m_dSpeedMS, p_FileType, Type2Cols[i]); break;
         case 13: bVal = FromChar(tstr2, &TP.m_dWindSpeedMS, p_FileType, Type2Cols[i]); break;
         case 14: bVal = FromChar(tstr2, &TP.m_dWDRT, p_FileType, Type2Cols[i]); break;
         case 17: bVal = FromChar(tstr2, &TP.m_nAltitudeM, p_FileType, Type2Cols[i]); break;
         case 18: bVal = FromChar(tstr2, &TP.m_dAzimuthDeg, p_FileType, Type2Cols[i]); TP.m_dAzimuth = TP.m_dAzimuthDeg * M_PI / 180; break;
         case 59: bVal = FromChar(tstr2, &TP.m_dWeightKg, p_FileType, Type2Cols[i]); break;
         }
       if(!bVal) { AddToErrorList("Text to value parser returned an error"); return(false); }
       i ++;
       if(i == Count) break;
       }
     if(pEndWork) *pEndWork = ',';
     else break;
     pStartWork = pEndWork + 1;
     j ++;
     }
   m_TrackPoints.push_back(TP);
   LastTP = TP;
   lLines ++;
   } 
 if(!m_TrackPoints.size()) { AddToErrorList("The file contains no data"); return(false); }
 TPI1 = m_TrackPoints.end();
 TPI = m_TrackPoints.begin();
 LI.QuadPart = 0;
 lLines = 0;
 while(TPI != m_TrackPoints.end()) {
   if(TPI->m_CTLE.QuadPart) {
     LI = TPI->m_CTLE;
     if(TPI1 == m_TrackPoints.end())
       TPI1 = TPI; 
     }
   else {
     if(LI.QuadPart) {
       LI.QuadPart += 10000000;
       TPI->m_CTLE = LI;
       }
     }
   TPI ++;
   lLines ++;
   }
 if(TPI1 != m_TrackPoints.end() && TPI1 != m_TrackPoints.begin()) { 
   LI = TPI1->m_CTLE;
   TPI = --TPI1;
   while(true) {
     LI.QuadPart -= 10000000;
     TPI->m_CTLE = LI;
     if(TPI == m_TrackPoints.begin()) break;
     TPI --;
     } }
 dVal = 0;
 TPI1 = m_TrackPoints.end();
 TPI = m_TrackPoints.begin();
 lLines = 0;
 while(TPI != m_TrackPoints.end()) {
   if(TPI->m_dWeightKg) {
     dVal = TPI->m_dWeightKg;
     if(TPI1 == m_TrackPoints.end())
       TPI1 = TPI; 
     }
   else {
     if(dVal) {
       TPI->m_dWeightKg = dVal;
       }
     }
   TPI ++;
   lLines ++;
   }
 if(TPI1 != m_TrackPoints.end() && TPI1 != m_TrackPoints.begin()) { 
   dVal = TPI1->m_dWeightKg;
   TPI = --TPI1;
   while(true) {
     TPI->m_dWeightKg = dVal;
     if(TPI == m_TrackPoints.begin()) break;
     TPI --;
     } }
 dVal = AVP_BAD_WIND_VALUE;
 TPI1 = m_TrackPoints.end();
 TPI = m_TrackPoints.begin();
 lLines = 0;
 while(TPI != m_TrackPoints.end()) {
   if(TPI->m_dWDRT != AVP_BAD_WIND_VALUE) {
     dVal = TPI->m_dWDRT;
     if(TPI1 == m_TrackPoints.end())
       TPI1 = TPI; 
     }
   else {
     if(dVal != AVP_BAD_WIND_VALUE)
       TPI->m_dWDRT = dVal;
     }
   TPI ++;
   lLines ++;
   }
 if(TPI1 != m_TrackPoints.end() && TPI1 != m_TrackPoints.begin()) { 
   dVal = TPI1->m_dWDRT;
   TPI = --TPI1;
   while(true) {
     TPI->m_dWDRT = dVal;
     if(TPI == m_TrackPoints.begin()) break;
     TPI --;
     } }
 dVal = AVP_BAD_WIND_VALUE;
 TPI1 = m_TrackPoints.end();
 TPI = m_TrackPoints.begin();
 lLines = 0;
 while(TPI != m_TrackPoints.end()) {
   if(TPI->m_dWindSpeedMS != AVP_BAD_WIND_VALUE) {
     dVal = TPI->m_dWindSpeedMS;
     if(TPI1 == m_TrackPoints.end())
       TPI1 = TPI; 
     }
   else {
     if(dVal != AVP_BAD_WIND_VALUE) {
       TPI->m_dWindSpeedMS = dVal;
       }
     }
   TPI ++;
   lLines ++;
   }
 if(TPI1 != m_TrackPoints.end() && TPI1 != m_TrackPoints.begin()) { 
   dVal = TPI1->m_dWindSpeedMS;
   TPI = --TPI1;
   while(true) {
     TPI->m_dWindSpeedMS = dVal;
     if(TPI == m_TrackPoints.begin()) break;
     TPI --;
     } }
 lLines = 0;
 TPI = m_TrackPoints.begin();
 LI = TPI->m_CTLE;
 while(TPI != m_TrackPoints.end()) {
   if(!*TPI->m_CallSign)
     sprintf_s(TPI->m_CallSign, sizeof(TPI->m_CallSign), m_TrackMetaInfo.m_CallSign);
   if(!*TPI->m_ACModel)
     sprintf_s(TPI->m_ACModel, sizeof(TPI->m_ACModel), m_TrackMetaInfo.m_ACModel);
   if(!*TPI->m_DepartureAirport)
     sprintf_s(TPI->m_DepartureAirport, sizeof(TPI->m_DepartureAirport), m_TrackMetaInfo.m_DepartureAirport);
   if(!*TPI->m_ArrivalAirport)
     sprintf_s(TPI->m_ArrivalAirport, sizeof(TPI->m_ArrivalAirport), m_TrackMetaInfo.m_ArrivalAirport);
   if(TPI->m_CTLE.QuadPart != LI.QuadPart) {
     AddToErrorList("The time line is broken");
     return(false); }
   LI.QuadPart += 10000000;
   if(!TPI->m_dWeightKg) {
     AddToErrorList("Aircraft weight value is not set");
     return(false); }
   if(TPI->m_dWDRT == AVP_BAD_WIND_VALUE) {
     TPI->m_dWDRT = m_TrackMetaInfo.m_dWindDirectionToRad;
     }
   if(TPI->m_dWindSpeedMS == AVP_BAD_WIND_VALUE) {
     TPI->m_dWindSpeedMS = m_TrackMetaInfo.m_dWindSpeedMS;
     }
   TPI ++;
   lLines ++;
   }
 if(!m_AuxWindServer.IsReady()) {
   lStartIndex = 0; 
   lEndIndex = m_TrackPoints.size() - 1;
   bVal = m_AuxWindServer.Set2dArraySize(lEndIndex - lStartIndex + 1);
   if(!bVal) { Message("No memory for the wind server"); return(false); }
   TPI = m_TrackPoints.begin();
   for(li = 0; li < lStartIndex; li ++) TPI ++;
   lLines = 0;
   while(lLines < lEndIndex - lStartIndex + 1) {
     Wind2D.dLat = TPI->m_dLatDeg;
     Wind2D.dLong = TPI->m_dLongDeg;
     Wind2D.m_dWDRT = TPI->m_dWDRT;
     Wind2D.m_dWindSpeedMS = TPI->m_dWindSpeedMS;
     m_AuxWindServer.SetValue(lLines, &Wind2D);
     lLines ++;
     TPI ++;
     }
   bVal = m_AuxWindServer.SetDone();
   bVal = m_AuxWindServer.SaveToFile();
   Message("Wind cache file %s created", m_AuxWindServer.GetLastLoadFileName());
   }
 return(true);
}
bool CSeverity::ReadFileIntoMemory(FILE *p_fFile, DWORD p_dwFileSize)
{
 if(!m_pDataBuff || m_dwDataBuffSize < p_dwFileSize) {
   if(!m_pDataBuff) delete [] m_pDataBuff;
   m_dwDataBuffSize = 0;
   m_pDataBuff = new char [p_dwFileSize + 1];
   if(!m_pDataBuff) return(false);
   m_dwDataBuffSize = p_dwFileSize + 1;
   memset(m_pDataBuff, 0, m_dwDataBuffSize); }
 return(fread(m_pDataBuff, 1, p_dwFileSize, p_fFile) == p_dwFileSize);
}
bool CSeverity::ReadFileWVSSJSON(FILE *p_fFile, DWORD p_dwFileSize)
{
 if(!ReadFileIntoMemory(p_fFile, p_dwFileSize)) return(false);
 return(ProcessTrackBuffer(m_pDataBuff, p_dwFileSize, FILETYPE_WVSS_JSON));
}
bool CSeverity::ReadFileASAICJSON(FILE *p_fFile, DWORD p_dwFileSize)
{
 if(!ReadFileIntoMemory(p_fFile, p_dwFileSize)) return(false);
 return(ProcessTrackBuffer(m_pDataBuff, p_dwFileSize, FILETYPE_ASAIC_JSON));
}
bool CSeverity::ProcessTrackBuffer(const char *p_pDataBuff, DWORD p_dwBuffSize, eFileType p_FileType)
{
 yajl_handle YAJLHandle;
 yajl_callbacks YAJLCallbacks;
 yajl_status YAJLStatus;
 if(!p_pDataBuff || (p_FileType != FILETYPE_WVSS_JSON && p_FileType != FILETYPE_ASAIC_JSON)) return(false);
 m_strlCurInputDataPath.RemoveAll();
 YAJLCallbacks.yajl_null = NULL;
 YAJLCallbacks.yajl_boolean = NULL;
 YAJLCallbacks.yajl_integer = NULL;
 YAJLCallbacks.yajl_double = NULL;
 YAJLCallbacks.yajl_number = static_yajl_number;
 YAJLCallbacks.yajl_string = static_yajl_string;
 YAJLCallbacks.yajl_start_map = static_yajl_start_map;
 YAJLCallbacks.yajl_map_key = static_yajl_map_key;
 YAJLCallbacks.yajl_end_map = static_yajl_end_map;
 YAJLCallbacks.yajl_start_array = static_yajl_start_array;
 YAJLCallbacks.yajl_end_array = static_yajl_end_array;
 YAJLHandle = yajl_alloc(&YAJLCallbacks, NULL, this);
 YAJLStatus = yajl_parse(YAJLHandle, (const unsigned char *)p_pDataBuff, p_dwBuffSize); 
 yajl_free(YAJLHandle);
 return(true);
}
void CSeverity::Recalc2(int p_nCurPos, bool p_bAddToWakeEncounterList)
{
 char tstr[256];
 int Pos;
 unsigned short i, j, AircraftOrdinal;
 long lVal, lGeneratorTPIndex, lj, lStartIndex, lEndIndex;
 LONGLONG llCurTime;
 LARGE_INTEGER CurTimeLI;
 GeographicLib::Math::real DistM, DistMwithH;
 CWSNEC CurWakeSectionOrig; 
 CWS2 CurWakeSection2;
 double dClosestSectionGamma, dTimeFromLeaderS;
 DWORD dwWakeSectionsAnalysed;
 CWESC WakeInteractionGeo;
 if(!m_pIntTS) return;
 WakeInteractionGeo.m_FSG = m_FSG;
 WakeInteractionGeo.m_pAircraft = &m_Aircraft;
 WakeInteractionGeo.m_pSensitivityAnalysisSetup = &m_SA;
 WakeInteractionGeo.m_pfUAP = m_pfUAP;
 WakeInteractionGeo.m_pfMessage = m_pfMessage;
 WakeInteractionGeo.RSOtp();
 strcpy(WakeInteractionGeo.m_FilePrefixWithExt, m_LastLoadedFileName);
 WakeInteractionGeo.m_lFrame = p_nCurPos;
 Pos = p_nCurPos;
 llCurTime = m_llMinTime + (LONGLONG)Pos * m_dwBreakInterval;
 WakeInteractionGeo.m_WCSS.m_llTTt = llCurTime;
 CurTimeLI.QuadPart = llCurTime;
 m_pIntTS[Pos].m_WakeInteractions.clear(); 
 SetAircraftFlags(p_nCurPos); 
 for(i = 0; i < m_Aircraft.size(); i ++) {
   AircraftOrdinal = i;
   if(!m_Aircraft[AircraftOrdinal]->m_pAircraft)
     continue; 
   lGeneratorTPIndex = GetTPIntIndexFromAircraftOrdinal(Pos, i);
   lStartIndex = Pos - m_Aircraft[AircraftOrdinal]->m_lStartPos;
   if(!m_Aircraft[AircraftOrdinal]->m_Wake.CSRae(CurTimeLI, &lStartIndex, &m_Aircraft[AircraftOrdinal]->m_lCurSectionCount))
     return;
   if(!m_Aircraft[AircraftOrdinal]->m_lCurSectionCount) continue; 
   WakeInteractionGeo.m_WCSS.m_pGA = m_Aircraft[AircraftOrdinal];
   for(j = 0; j < m_Aircraft.size(); j ++) {
     if(!m_Aircraft[j]->m_pAircraft)
       continue; 
     if(j == AircraftOrdinal || !m_Aircraft[j]->m_bCurOn) continue; 
     lVal = GetTPIntIndexFromAircraftOrdinal(Pos, j);
     ASSERT(lVal != -1);
     WakeInteractionGeo.m_WCSS.m_bOK = false;
     WakeInteractionGeo.m_WCSS.m_dDTWoa[0] = 40000000; 
     WakeInteractionGeo.m_WCSS.m_pEE = m_Aircraft[j]; 
     WakeInteractionGeo.m_WCSS.m_dAGLD = lGeneratorTPIndex != -1 ? m_TrackPointsInt[lGeneratorTPIndex].m_dLatDeg : AVP_BAD_DEGREE_VALUE;
     WakeInteractionGeo.m_WCSS.m_dAGLlD = lGeneratorTPIndex != -1 ? m_TrackPointsInt[lGeneratorTPIndex].m_dLongDeg : AVP_BAD_DEGREE_VALUE;
     WakeInteractionGeo.m_WCSS.m_dGAHM = lGeneratorTPIndex != -1 ? m_TrackPointsInt[lGeneratorTPIndex].m_nAltitudeM : AVP_BAD_ALTITUDE_VALUE;
     WakeInteractionGeo.m_WCSS.m_dEALD = m_TrackPointsInt[lVal].m_dLatDeg;
     WakeInteractionGeo.m_WCSS.m_dEALLD = m_TrackPointsInt[lVal].m_dLongDeg;
     WakeInteractionGeo.m_WCSS.m_dEAMH = m_TrackPointsInt[lVal].m_nAltitudeM;
     WakeInteractionGeo.m_WCSS.m_dEAHD = m_TrackPointsInt[lVal].m_dAzimuthDeg;
     WakeInteractionGeo.m_WCSS.m_dwSN =  (DWORD)-1L;
     m_pEncounterLC->Reset(m_TrackPointsInt[lVal].m_dLatDeg, m_TrackPointsInt[lVal].m_dLongDeg, m_TrackPointsInt[lVal].m_nAltitudeM);
     WakeInteractionGeo.m_WCSS.m_dGFDM = lGeneratorTPIndex != -1 ? GetDistanceTo(m_pGeo, m_pEncounterLC, 
                                               WakeInteractionGeo.m_WCSS.m_dAGLD, WakeInteractionGeo.m_WCSS.m_dAGLlD, 
                                               WakeInteractionGeo.m_WCSS.m_dGAHM) : AVP_BAD_DIST_VALUE;
     lStartIndex = Pos - m_Aircraft[AircraftOrdinal]->m_lStartPos;
     if(lStartIndex >= m_Aircraft[AircraftOrdinal]->m_Wake.GNOS())
       lStartIndex = m_Aircraft[AircraftOrdinal]->m_Wake.GNOS() - 1;
     lEndIndex = lStartIndex - m_Aircraft[AircraftOrdinal]->m_lCurSectionCount;
     if(m_FSG.m_bDebugMessages) {
       sprintf(tstr, "%.4d-%.2d-%.2d %.2d:%.2d:%.2d: %s to %s",
         m_TrackPointsInt[lVal].m_CreationTime.wYear, m_TrackPointsInt[lVal].m_CreationTime.wMonth, m_TrackPointsInt[lVal].m_CreationTime.wDay, m_TrackPointsInt[lVal].m_CreationTime.wHour, m_TrackPointsInt[lVal].m_CreationTime.wMinute, m_TrackPointsInt[lVal].m_CreationTime.wSecond, 
         m_Aircraft[j]->m_CallSign, m_Aircraft[AircraftOrdinal]->m_CallSign);
       if(WakeInteractionGeo.m_WCSS.m_dGFDM != AVP_BAD_DIST_VALUE)
         sprintf(tstr + strlen(tstr), ", distance %.0f m (%.1f nm)", WakeInteractionGeo.m_WCSS.m_dGFDM, WakeInteractionGeo.m_WCSS.m_dGFDM / 1852);
       else
         sprintf(tstr + strlen(tstr), ", distance not available");
       Message(tstr); }
     dwWakeSectionsAnalysed = 0;
     dTimeFromLeaderS = 0;
     for(lj = lStartIndex; lj > lEndIndex; lj--) { 
       if(!m_Aircraft[AircraftOrdinal]->m_Wake.GS2EC(lj, CurTimeLI, &CurWakeSection2, &CurWakeSectionOrig)) 
         continue;
       if(!WakeSectionRelevantICAO(m_Aircraft[j]->m_pAircraft, &CurWakeSection2))
         continue;
       dwWakeSectionsAnalysed ++;
       DistM = GetDistanceTo(m_pGeo, m_pEncounterLC, CurWakeSection2.m_dLT1d, CurWakeSection2.m_sLL1DT, CurWakeSection2.m_dAltitude1, &DistMwithH);
       if(DistM < WakeInteractionGeo.m_WCSS.m_dDTWoa[0]) { 
         WakeInteractionGeo.m_WCSS.m_dDTWoa[0] = DistM;
         WakeInteractionGeo.m_WCSS.m_dDTWoa[1] = DistMwithH;
         WakeInteractionGeo.m_WCSS.m_dCSAloa = CurWakeSection2.m_dAltitude1;
         WakeInteractionGeo.m_WCSS.m_dwSN = lj;
         dClosestSectionGamma = CurWakeSection2.m_dCiCu;
         dTimeFromLeaderS = (double)(CurTimeLI.QuadPart - CurWakeSectionOrig.m_CTLE.QuadPart) / 10000000;
         }
       } 
     if(m_FSG.m_bDebugMessages) {
       sprintf(tstr, "%s: %ld wake sections analysed", m_Aircraft[AircraftOrdinal]->m_CallSign, dwWakeSectionsAnalysed);
       Message(tstr); }
     if(m_FSG.m_bCS && dwWakeSectionsAnalysed && WakeInteractionGeo.m_WCSS.m_dDTWoa[1] <= m_FSG.m_dIPWCD) { 
       sprintf(tstr, "%s to %s, distance(alt/no alt) to section %ld: %.1f/%.1f m , dH=%.1f m (%.1lf ft), m_dga=%.1lf", 
               m_Aircraft[j]->m_CallSign, m_Aircraft[AircraftOrdinal]->m_CallSign, 
               WakeInteractionGeo.m_WCSS.m_dwSN,
               WakeInteractionGeo.m_WCSS.m_dDTWoa[1], WakeInteractionGeo.m_WCSS.m_dDTWoa[0], 
               WakeInteractionGeo.m_WCSS.m_dEAMH - WakeInteractionGeo.m_WCSS.m_dCSAloa,
               (WakeInteractionGeo.m_WCSS.m_dEAMH - WakeInteractionGeo.m_WCSS.m_dCSAloa) / 0.3048,
               dClosestSectionGamma);
       Message(tstr);
       if(WakeInteractionGeo.m_WCSS.m_dDTWoa[1] < m_FSG.m_dIPWCD) { 
         WakeInteractionGeo.m_WCSS.m_dEtAfLCX = WakeInteractionGeo.m_WCSS.m_dEtAfLCZ = WakeInteractionGeo.m_WCSS.m_dEtAfLCH = 0;
         memset(WakeInteractionGeo.m_WCSS.m_bWSOKP, 0, sizeof(WakeInteractionGeo.m_WCSS.m_bWSOKP));
         if(WakeInteractionGeo.m_WCSS.m_dwSN < (DWORD)lStartIndex) { 
           if(m_Aircraft[AircraftOrdinal]->m_Wake.GS2EC(WakeInteractionGeo.m_WCSS.m_dwSN + 1, CurTimeLI, &WakeInteractionGeo.m_WCSS.m_WSNWE[0]))
             WakeInteractionGeo.m_WCSS.m_bWSOKP[0] = true; }
         m_Aircraft[AircraftOrdinal]->m_Wake.GS2EC(WakeInteractionGeo.m_WCSS.m_dwSN, CurTimeLI, &WakeInteractionGeo.m_WCSS.m_WSNWE[1]);
         WakeInteractionGeo.m_WCSS.m_bWSOKP[1] = true;
         if(WakeInteractionGeo.m_WCSS.m_dwSN > (DWORD)lEndIndex + 1) { 
           if(m_Aircraft[AircraftOrdinal]->m_Wake.GS2EC(WakeInteractionGeo.m_WCSS.m_dwSN - 1, CurTimeLI, &WakeInteractionGeo.m_WCSS.m_WSNWE[2]))
             WakeInteractionGeo.m_WCSS.m_bWSOKP[2] = true; }
         WakeInteractionGeo.m_WCSS.m_dTFLS = dTimeFromLeaderS;
         sprintf(tstr, "Entering plane course %.1f, time from the leader %.0lf sec", m_TrackPointsInt[lVal].m_dAzimuthDeg, WakeInteractionGeo.m_WCSS.m_dTFLS);
         Message(tstr);
         if(WakeInteractionGeo.PWC3(&m_TrackPointsInt[lVal], AircraftOrdinal, j)) {
           if(WakeInteractionGeo.m_WCSS.m_bLWakeOK || WakeInteractionGeo.m_WCSS.m_bRWakeOK || WakeInteractionGeo.m_FSG.m_bOverrideProximityValues)
             m_pIntTS[Pos].m_WakeInteractions.push_back(WakeInteractionGeo.MakeSafeCopy()); }
         }
       }  
     } 
   }
 if(p_bAddToWakeEncounterList) {
   for(i = 0; i < m_pIntTS[Pos].m_WakeInteractions.size(); i ++) {
     for(j = 0; j < m_pIntTS[Pos].m_WakeInteractions[i].m_CSSO.size(); j ++) {
       m_FullSeverityOutput.push_back(WakeInteractionGeo.m_CSSO[j]); } } }
}
long CSeverity::GetTPIntIndexFromAircraftOrdinal(DWORD p_dwTrackPoint, short p_nAircraftOrdinal)
{
 unsigned short i;
 size_t Size;
 Size = m_pIntTS[p_dwTrackPoint].m_TP.size();
 for(i = 0; i < Size; i ++)
   if(m_TrackPointsInt[m_pIntTS[p_dwTrackPoint].m_TP[i]].m_nAircraftOrdinal == p_nAircraftOrdinal)
     return(m_pIntTS[p_dwTrackPoint].m_TP[i]);
 return(-1);  
}
void CSeverity::WriteToLog(const char *p_pFormat, ...)
{
 int Len;
 char tstr[512];
 va_list ArgList;
 DWORD dwWritten;
 if(!OpenLogFile(tstr)) return; 
 strcat(tstr, " ");
 WriteFile(m_hLogFile, tstr, strlen(tstr), &dwWritten, NULL); 
 if(!p_pFormat) return;
 va_start(ArgList, p_pFormat);
 vsprintf_s(tstr, sizeof(tstr), p_pFormat, ArgList);
 va_end(ArgList);
 Len = strlen(tstr);
 strcpy(tstr + Len, "\r\n");
 Len += strlen(tstr + Len);
 WriteFile(m_hLogFile, tstr, Len, &dwWritten, NULL);
}
void CSeverity::Message(const char *p_pFormat, ...)
{
 char tstr[256];
 va_list ArgList;
 if(!p_pFormat) return;
 va_start(ArgList, p_pFormat);
 vsprintf_s(tstr, sizeof(tstr), p_pFormat, ArgList);
 va_end(ArgList);
 if(m_pfMessage) 
   m_pfMessage(tstr);
 WriteToLog(tstr);
}
void CSeverity::OnFileLoaded(const char *p_pFileName)
{
 if(m_pfOnFileLoaded) 
   m_pfOnFileLoaded(p_pFileName);
 DoOnFileLoaded(p_pFileName);
}
BOOL CSeverity::DoOnFileLoaded(const char *p_pFileName)
{
 DWORD i, dwTime;
 float fProcessed;
 bool bEmulateFlight, bCSAELV;
 std::vector<CSOC>::const_iterator It;
 DWORD lStartWakeRangeFrame, lEndWakeRangeFrame;
 void (* pfMessage)(const char *);
 if(!m_FSG.m_bCS) return(TRUE);
 Message("Calculating severity...");
 fProcessed = m_fProcessed;
 bEmulateFlight = m_FSG.m_bEF;
 bCSAELV = m_FSG.m_bCS;
 dwTime = GetTickCount();
 m_FullSeverityOutput.clear();
 m_FSG.m_bEF = false;
 m_FSG.m_bCS = true;
 pfMessage = m_pfMessage;
 m_pfMessage = NULL; 
 for(i = 0; i < m_dwTSCount - 1; i ++) {
   if(i == 4922)
     i = i;
   Recalc2(i, true);
   m_fProcessed = fProcessed + ((float)(i + 1) / (m_dwTSCount - 1)) * m_fCurProcessedTotal;
   }
 m_FSG.m_bEF = bEmulateFlight;
 m_FSG.m_bCS = bCSAELV;
 m_pfMessage = pfMessage;
 WriteSeverityFile(p_pFileName, " new");
 WriteWakeJSON(p_pFileName);
 Message("Done calculating severity in %.1lf seconds, %ld entries", ((double)GetTickCount() - dwTime) / 1000, (unsigned long)m_FullSeverityOutput.size());
 if(m_FullSeverityOutput.size() > 0) {
   Message("Wake entry frame ranges:");
   lStartWakeRangeFrame = lEndWakeRangeFrame = 0;
   for(It = m_FullSeverityOutput.begin(); It != m_FullSeverityOutput.end(); It++) {
     if(!lStartWakeRangeFrame || It->m_lPlayerPos > lEndWakeRangeFrame + 1) {
       if(lStartWakeRangeFrame)
         Message("%ld-%ld (%ld periods)", lStartWakeRangeFrame, lEndWakeRangeFrame, lEndWakeRangeFrame - lStartWakeRangeFrame + 1);
       lStartWakeRangeFrame = It->m_lPlayerPos;
       lEndWakeRangeFrame = lStartWakeRangeFrame; }
     else
       lEndWakeRangeFrame = It->m_lPlayerPos; } 
   Message("%ld-%ld (%ld periods)", lStartWakeRangeFrame, lEndWakeRangeFrame, lEndWakeRangeFrame - lStartWakeRangeFrame + 1); }
 return(TRUE);
}
void CSeverity::UAPuos(rAircraftPosition *p_pAircraftPosition, short p_nAircraftOrdinal, CTrackPoint *p_pTP)
{
 if(!m_pfUAP) return;
 m_pfUAP(p_pAircraftPosition, p_nAircraftOrdinal, p_pTP);
}
void CSeverity::SetAircraftFlags(int p_nCurPos)
{
 unsigned short i, TSCount;
 for(i = 0; i < m_Aircraft.size(); i ++)
   m_Aircraft[i]->m_bCurOn = false; 
 TSCount = m_pIntTS[p_nCurPos].m_TP.size();
 for(i = 0; i < TSCount; i ++) 
   m_Aircraft[m_TrackPointsInt[m_pIntTS[p_nCurPos].m_TP[i]].m_nAircraftOrdinal]->m_bCurOn = 1;
}
float CSeverity::GetProcessedPercentage()
{
 float fVal = m_fProcessed;
 if(m_pCurAircraftForWakeProgress) {
   fVal += m_pCurAircraftForWakeProgress->m_Wake.GetProcessedPercentage();
   }
 return(fVal);
}
void CSeverity::FillTrackMetaInfo(const char *p_pDataFileName)
{
 char *pWork, FileName[256], tstr[128];
 short Len;
 DWORD dwRetVal;
 SYSTEMTIME ST;
 FILETIME FT;
 strcpy(FileName, p_pDataFileName);
 if(pWork = strrchr(FileName, '.')) *pWork = '\0'; 
 strcat(FileName, ".meta");
 Len = sizeof(tstr) - 1;
 dwRetVal = GetPrivateProfileString("MAIN", "AircraftType", "???", tstr, 256, FileName);
 if(dwRetVal != Len - 1 && dwRetVal != Len - 2)
   strcpy_s(m_TrackMetaInfo.m_ACModel, sizeof(m_TrackMetaInfo.m_ACModel), tstr);
 dwRetVal = GetPrivateProfileString("MAIN", "CallSign", "???", tstr, Len, FileName);
 if(dwRetVal != Len - 1 && dwRetVal != Len - 2)
   strcpy_s(m_TrackMetaInfo.m_CallSign, sizeof(m_TrackMetaInfo.m_CallSign), tstr);
 dwRetVal = GetPrivateProfileString("MAIN", "DepartureAirport", "???", tstr, Len, FileName);
 if(dwRetVal != Len - 1 && dwRetVal != Len - 2)
   strcpy_s(m_TrackMetaInfo.m_DepartureAirport, sizeof(m_TrackMetaInfo.m_DepartureAirport), tstr);
 dwRetVal = GetPrivateProfileString("MAIN", "ArrivalAirport", "???", tstr, Len, FileName);
 if(dwRetVal != Len - 1 && dwRetVal != Len - 2)
   strcpy_s(m_TrackMetaInfo.m_ArrivalAirport, sizeof(m_TrackMetaInfo.m_ArrivalAirport), tstr);
 dwRetVal = GetPrivateProfileString("MAIN", "WindDirectionDegFrom", "", tstr, Len, FileName);
 if(dwRetVal != Len - 1 && dwRetVal != Len - 2 && *tstr) {
   m_TrackMetaInfo.m_dWindDirectionToRad = atof(tstr) * M_PI / 180;
   m_TrackMetaInfo.m_dWindDirectionToRad -= M_PI;
   if(m_TrackMetaInfo.m_dWindDirectionToRad < 0) m_TrackMetaInfo.m_dWindDirectionToRad += 2 * M_PI; }
 else m_TrackMetaInfo.m_dWindDirectionToRad = AVP_BAD_WIND_VALUE;
 dwRetVal = GetPrivateProfileString("MAIN", "WindSpeedKts", "", tstr, Len, FileName);
 if(dwRetVal != Len - 1 && dwRetVal != Len - 2 && *tstr)
   m_TrackMetaInfo.m_dWindSpeedMS = atof(tstr) * 1.852 / 3.6; 
 else
   m_TrackMetaInfo.m_dWindSpeedMS = AVP_BAD_WIND_VALUE;
 memset(&ST, 0, sizeof(ST));
 dwRetVal = GetPrivateProfileString("MAIN", "Year", "2000", tstr, Len, FileName);
 if(dwRetVal != Len - 1 && dwRetVal != Len - 2)
   ST.wYear = atoi(tstr);
 dwRetVal = GetPrivateProfileString("MAIN", "Month", "2000", tstr, Len, FileName);
 if(dwRetVal != Len - 1 && dwRetVal != Len - 2)
   ST.wMonth = atoi(tstr);
 dwRetVal = GetPrivateProfileString("MAIN", "Day", "2000", tstr, Len, FileName);
 if(dwRetVal != Len - 1 && dwRetVal != Len - 2)
   ST.wDay = atoi(tstr);
 SystemTimeToFileTime(&ST, &FT);
 m_TrackMetaInfo.m_Date.HighPart = FT.dwHighDateTime;
 m_TrackMetaInfo.m_Date.LowPart = FT.dwLowDateTime;
}
template <class T> bool CSeverity::FromChar(const char *p_pCharData, T *p_pRes, eFileType p_FileType, short p_nFileColOrd )
{
 bool bRes = false, bTimeErrorMsgGiven = false;
 char tstr[128];
 SYSTEMTIME ST;
 LARGE_INTEGER LI;
 FILETIME FT;
 LONGLONG llVal;
 if(!p_pCharData || !p_pRes) { AddToErrorList("CSeverity::FromChar internal error"); return(false); }
 if(typeid(p_pRes) == typeid(LARGE_INTEGER *)) { 
   if(p_FileType == FILETYPE_FDR && p_nFileColOrd == 1) {
     bRes = true;
     ((LARGE_INTEGER *)p_pRes)->QuadPart = 0;
     if(*p_pCharData) {
       strcpy(tstr, p_pCharData); 
       strrplInPlace(tstr, ",", ".");
       memset(&ST, 0, sizeof(ST));
       LI.QuadPart = m_TrackMetaInfo.m_Date.QuadPart;
       ((LARGE_INTEGER *)p_pRes)->QuadPart = LI.QuadPart + (LONGLONG)(atof(tstr) * 3600000) * 10000; 
       llVal = (((LARGE_INTEGER *)p_pRes)->QuadPart / 10000) % 1000;
       if(llVal > 10 && llVal < 990) {
         AddToErrorList((CString)"CSeverity::FromChar WARNING: times aren't aligned at full second intervals. Example: " + tstr);
         bTimeErrorMsgGiven = true; }
       ((LARGE_INTEGER *)p_pRes)->QuadPart = (((LARGE_INTEGER *)p_pRes)->QuadPart / 10000000 + (llVal > 500 ? 1 : 0)) * 10000000;
       FT.dwHighDateTime = ((LARGE_INTEGER *)p_pRes)->HighPart;
       FT.dwLowDateTime = ((LARGE_INTEGER *)p_pRes)->LowPart;
       FileTimeToSystemTime(&FT, &ST);
       }
     }
   } 
 else if(typeid(p_pRes) == typeid(double *)) { 
   *((double *)p_pRes) = atof(p_pCharData);  
   bRes = true;
   if(p_FileType == FILETYPE_FDR && (p_nFileColOrd == 18 || p_nFileColOrd == 59)) {
     #pragma message ("fix magnetic deviation kludge")
     if(p_nFileColOrd == 18) 
       *((double *)p_pRes) -= 10;
     if(p_nFileColOrd == 59) 
      *((double *)p_pRes) *= 0.45359;
     }
   if(p_FileType == FILETYPE_FDR && p_nFileColOrd == 9)
     *((double *)p_pRes) = atof(p_pCharData) * 1.852 / 3.6; 
   if(p_FileType == FILETYPE_FDR && p_nFileColOrd == 13) {
     *((double *)p_pRes) = AVP_BAD_WIND_VALUE;
     if(*p_pCharData && *p_pCharData != '0')
       *((double *)p_pRes) = atof(p_pCharData) * 1.852 / 3.6; 
     }
   if(p_FileType == FILETYPE_FDR && p_nFileColOrd == 14) {
     *((double *)p_pRes) = AVP_BAD_WIND_VALUE;
     if(*p_pCharData && *p_pCharData != '0') {
       *((double *)p_pRes) = atof(p_pCharData) * M_PI / 180;
       (*((double *)p_pRes)) -= M_PI; 
       if(*((double *)p_pRes) < 0) (*((double *)p_pRes)) += 2 * M_PI;
       }
     }
   } 
 else if(typeid(p_pRes) == typeid(short *)) { 
   *((short *)p_pRes) = (short)atoi(p_pCharData); 
   bRes = true;
   if(p_FileType == FILETYPE_FDR && p_nFileColOrd == 17) {
     *((short *)p_pRes) = (short)(atof(p_pCharData) * 0.3048); 
     }
   } 
 else if(typeid(p_pRes) == typeid(SYSTEMTIME *)) { 
   if(p_FileType == FILETYPE_WVSS_JSON)
     bRes = TimeFromString(p_pCharData, reinterpret_cast<SYSTEMTIME *>(p_pRes), true);
   }
 else {
   sprintf(tstr, "Value type %s is not supported", typeid(p_pRes).name());
   AddToErrorList(tstr); }
 if(!bRes) {
   sprintf(tstr, "Value type %s not supported for file type %d and file column(1-based) %d", typeid(p_pRes).name(), (int)p_FileType, p_nFileColOrd + 1);
   AddToErrorList(tstr); }
 return(bRes);
}
bool CSeverity::Interpolate(const char *p_pFileName)
{
 char ErrStr[256];
 DWORD li, dwStartTPIntSize;
 bool bVal;
 GeographicLib::Math::real Azimut1Deg, Azimut2Deg, DistM, m12, M12, M21, S12, dCurAzimuth;
 CTrackPoint TP;
 double pt1[2], pt2[2], dVal, dVal2;
 double dMaxAccelPos, dMaxAccelNeg;
 double dMinimumSpeed_ms;
 LONGLONG llCurTime;
 dMaxAccelPos = 2.5 * 9.81; 
 dMaxAccelNeg = -0.5 * 9.81; 
 dwStartTPIntSize = m_TrackPointsInt.size();
 for(li = 0; li < m_TrackPointsV.size(); li ++) {
   bVal = (li != m_TrackPointsV.size() - 1);
   if(bVal)
     bVal = (!stricmp(m_TrackPointsV[li].m_CallSign, m_TrackPointsV[li + 1].m_CallSign) != 0);
   if(bVal) { 
     m_pGeo->Inverse(m_TrackPointsV[li].m_dLatDeg, m_TrackPointsV[li].m_dLongDeg, m_TrackPointsV[li + 1].m_dLatDeg, m_TrackPointsV[li + 1].m_dLongDeg, DistM, Azimut1Deg, Azimut2Deg, m12, M12, M21, S12);
     m_TrackPointsV[li].m_dAzimuthDeg = (Azimut1Deg + Azimut2Deg) / 2; 
     if(m_TrackPointsV[li].m_dAzimuthDeg > 2 * 360) m_TrackPointsV[li].m_dAzimuthDeg -= 360;
     if(m_TrackPointsV[li].m_dAzimuthDeg < 0) m_TrackPointsV[li].m_dAzimuthDeg += 360;
     m_TrackPointsV[li].m_dAzimuth = m_TrackPointsV[li].m_dAzimuthDeg * M_PI / 180;
     m_TrackPointsV[li].m_dDistKm = DistM / 1000;
     m_pGeo->Inverse(m_TrackPointsV[li + 1].m_dLatDeg, m_TrackPointsV[li + 1].m_dLongDeg, m_TrackPointsV[li].m_dLatDeg, m_TrackPointsV[li].m_dLongDeg, DistM, Azimut1Deg, Azimut2Deg, m12, M12, M21, S12);
     m_TrackPointsV[li].m_dAzimuthBackDeg = Azimut1Deg;
     if(m_TrackPointsV[li].m_dAzimuthBackDeg > 2 * 360) m_TrackPointsV[li].m_dAzimuthBackDeg -= 360;
     if(m_TrackPointsV[li].m_dAzimuthBackDeg < 0) m_TrackPointsV[li].m_dAzimuthBackDeg += 360;
     m_TrackPointsV[li].m_dAzimuthBack = m_TrackPointsV[li].m_dAzimuthBackDeg * M_PI / 180;
     m_TrackPointsV[li].m_dTimeSpanS = (double)(m_TrackPointsV[li + 1].m_CTLE.QuadPart - m_TrackPointsV[li].m_CTLE.QuadPart) / 10000000;
     m_TrackPointsV[li].m_dAveSpeedMS = m_TrackPointsV[li].m_dDistKm * 1000 / m_TrackPointsV[li].m_dTimeSpanS;
     m_TrackPointsV[li].m_dVSpeedMS = (m_TrackPointsV[li + 1].m_dAltitude100Ft - m_TrackPointsV[li].m_dAltitude100Ft) * 100 * 0.3048 / m_TrackPointsV[li].m_dTimeSpanS;
     if(m_TrackPointsV[li].m_dDistKm > 0) {
       dVal = (m_TrackPointsV[li + 1].m_dAltitude100Ft - m_TrackPointsV[li].m_dAltitude100Ft) * 100 * 0.3048 / (m_TrackPointsV[li].m_dDistKm * 1000);
       m_TrackPointsV[li].m_dAttitudeAngleDeg = atan(dVal) * 180 / M_PI; }
     else
       m_TrackPointsV[li + 1].m_dAttitudeAngleDeg = m_TrackPointsV[li].m_dAttitudeAngleDeg;
     if(m_TrackPointsV[li + 1].m_dAttitudeAngleDeg != m_TrackPointsV[li + 1].m_dAttitudeAngleDeg)
       dVal = dVal;
     }
   else { 
     m_TrackPointsV[li].m_dAzimuth = m_TrackPointsV[li - 1].m_dAzimuth;
     m_TrackPointsV[li].m_dAzimuthDeg = m_TrackPointsV[li - 1].m_dAzimuthDeg;
     m_TrackPointsV[li].m_dAzimuthBack = m_TrackPointsV[li - 1].m_dAzimuthBack;
     m_TrackPointsV[li].m_dAzimuthBackDeg = m_TrackPointsV[li - 1].m_dAzimuthBackDeg;
     m_TrackPointsV[li].m_dTimeSpanS = 0;
     m_TrackPointsV[li].m_dAveSpeedMS = m_TrackPointsV[li - 1].m_dAveSpeedMS;
     m_TrackPointsV[li].m_dVSpeedMS = m_TrackPointsV[li - 1].m_dVSpeedMS;
     m_TrackPointsV[li].m_dAttitudeAngleDeg = m_TrackPointsV[li - 1].m_dAttitudeAngleDeg;
     dVal = dVal;
     if(dVal != dVal)
       dVal = dVal;
     }
   }
 SmoothOutAverageSpeed();
 for(li = 0; li < m_TrackPointsV.size(); li ++) {
   bVal = (li != m_TrackPointsV.size() - 1);
   if(bVal) 
     bVal = (!stricmp(m_TrackPointsV[li].m_CallSign, m_TrackPointsV[li + 1].m_CallSign) != 0);
   if(li == m_TrackPointsV[li].m_lStartIndex) { 
     if(!bVal) continue; } 
   if(bVal) {
     m_TrackPointsV[li].m_dInitialLegSpeedMS = 2 * m_TrackPointsV[li].m_dAveSpeedMS - m_TrackPointsV[li + 1].m_dAveSpeedMS;
     m_TrackPointsV[li].m_dAccel = (m_TrackPointsV[li + 1].m_dAveSpeedMS - m_TrackPointsV[li].m_dInitialLegSpeedMS) / m_TrackPointsV[li].m_dTimeSpanS;
     }
   else {
     m_TrackPointsV[li].m_dInitialLegSpeedMS = m_TrackPointsV[li].m_dAveSpeedMS;
     m_TrackPointsV[li].m_dAccel = 0; }
   }
 WriteAccelerationFile(p_pFileName);
 dMinimumSpeed_ms = 20; 
 for(li = 0; li < m_TrackPointsV.size(); li ++) {
   if(m_TrackPointsV[li].m_dInitialLegSpeedMS < dMinimumSpeed_ms) {
     sprintf(ErrStr, "Computed leg starting speed of %.1lf kts is less than allowed minimum of %.1lf kts.", m_TrackPointsV[li].m_dInitialLegSpeedMS, dMinimumSpeed_ms);
     if(m_FSG.m_bDebugMessages)
       Message(ErrStr);
     AddToErrorList(ErrStr);
     return(false);
     }
   if(m_TrackPointsV[li].m_dAccel > dMaxAccelPos || m_TrackPointsV[li].m_dAccel < dMaxAccelNeg) { 
     sprintf(ErrStr, "%s: track point (1-based) %d excessive acceleration %.1lf (g-force %.1lf) detected!", m_TrackPointsV[li].m_CallSign, li + 1, m_TrackPointsV[li].m_dAccel, m_TrackPointsV[li].m_dAccel / 9.81);
     AddToErrorList(ErrStr);
     if(m_FSG.m_bDebugMessages)
       Message(ErrStr);
     sprintf(ErrStr, "Leg from point %ld. Leg data (initial speed/distance/time/average speed): %.1lf/%.1lf/%.1lf/%.1lf", li,
       m_TrackPointsV[li].m_dInitialLegSpeedMS,
       m_TrackPointsV[li].m_dDistKm * 1000,
       m_TrackPointsV[li].m_dTimeSpanS,
       m_TrackPointsV[li].m_dAveSpeedMS);
     AddToErrorList(ErrStr);
     if(m_FSG.m_bDebugMessages)
       Message(ErrStr);
     }
   }
 WriteAccelerationFile(p_pFileName);
 for(li = 0; li < m_TrackPointsV.size(); li ++) {
   TP = m_TrackPointsV[li];
   dCurAzimuth = m_TrackPointsV[li].m_dAzimuthDeg; 
   if(li == m_TrackPointsV[li].m_lStartIndex) { 
     llCurTime = TP.m_CTLE.QuadPart;
     if(TP.m_CTLE.QuadPart % m_dwBreakInterval) { 
       TP.m_CTLE.QuadPart = TP.m_CTLE.QuadPart / m_dwBreakInterval * m_dwBreakInterval;
       LIToSystemTime(&TP.m_CTLE, &TP.m_CreationTime);
       llCurTime = TP.m_CTLE.QuadPart; 
       pt1[0] = Radians(m_TrackPointsV[li].m_dLatDeg);
       pt1[1] = Radians(m_TrackPointsV[li].m_dLongDeg);
       dVal2 = (double)(m_TrackPointsV[li].m_CTLE.QuadPart - TP.m_CTLE.QuadPart) / 10000000;
       dVal = m_TrackPointsV[li].m_dInitialLegSpeedMS * dVal2 + m_TrackPointsV[li].m_dAccel * (dVal2 * dVal2) / 2;
       TP.m_dSpeedMS = SetPointSpeed(&m_TrackPointsV[li], dVal2);
       m_pGeo->Direct(m_TrackPointsV[li].m_dLatDeg, m_TrackPointsV[li].m_dLongDeg, m_TrackPointsV[li].m_dAzimuthBackDeg, dVal, pt2[0], pt2[1], Azimut2Deg, m12, M12, M21, S12);
       TP.m_dLatDeg = pt2[0];
       TP.m_dLongDeg = pt2[1];
       TP.m_nAltitudeM = (short)(m_TrackPointsV[li].m_dAltitude100Ft * 100 * 0.3048 + m_TrackPointsV[li].m_dVSpeedMS * ((double)(m_TrackPointsV[li].m_CTLE.QuadPart - TP.m_CTLE.QuadPart) / 10000000));
       m_TrackPointsInt.push_back(TP);
       llCurTime += m_dwBreakInterval; 
       }
     else { 
       TP.m_dSpeedMS = SetPointSpeed(&m_TrackPointsV[li], 0);  
       TP.m_nAltitudeM = (short)(m_TrackPointsV[li].m_dAltitude100Ft * 100 * 0.3048); 
       }
     } 
   else if(m_TrackPointsV[li].m_bLastPoint) { 
     if(TP.m_CTLE.QuadPart % m_dwBreakInterval) { 
       TP.m_CTLE.QuadPart = (TP.m_CTLE.QuadPart / m_dwBreakInterval + 1) * m_dwBreakInterval;
       LIToSystemTime(&TP.m_CTLE, &TP.m_CreationTime);
       pt1[0] = Radians(m_TrackPointsV[li].m_dLatDeg);
       pt1[1] = Radians(m_TrackPointsV[li].m_dLongDeg);
       dVal2 = (double)(TP.m_CTLE.QuadPart - m_TrackPointsV[li].m_CTLE.QuadPart) / 10000000;
       dVal = m_TrackPointsV[li].m_dInitialLegSpeedMS * dVal2 + m_TrackPointsV[li].m_dAccel * (dVal2 * dVal2) / 2;
       TP.m_dSpeedMS = SetPointSpeed(&m_TrackPointsV[li], dVal2); 
       m_pGeo->Direct(m_TrackPointsV[li].m_dLatDeg, m_TrackPointsV[li].m_dLongDeg, dCurAzimuth, dVal, pt2[0], pt2[1], Azimut2Deg, m12, M12, M21, S12);
       TP.m_dLatDeg = pt2[0];
       TP.m_dLongDeg = pt2[1];
       TP.m_nAltitudeM = (short)(m_TrackPointsV[li].m_dAltitude100Ft * 100 * 0.3048 + m_TrackPointsV[li].m_dVSpeedMS * ((double)(TP.m_CTLE.QuadPart - m_TrackPointsV[li].m_CTLE.QuadPart) / 10000000));
       }
     else { 
#pragma message("why are we changing altitude here and speed above?")
       TP.m_dSpeedMS = SetPointSpeed(&m_TrackPointsV[li], 0); 
       TP.m_nAltitudeM = (short)(m_TrackPointsV[li].m_dAltitude100Ft * 100 * 0.3048);
       }
     m_TrackPointsInt.push_back(TP);
     continue; 
     } 
   while(true) { 
     if(llCurTime >= m_TrackPointsV[li + 1].m_CTLE.QuadPart) 
       break;
     dVal2 = (double)(llCurTime - m_TrackPointsV[li].m_CTLE.QuadPart) / 10000000;
     if(llCurTime > m_TrackPointsV[li].m_CTLE.QuadPart) { 
       pt1[0] = Radians(m_TrackPointsV[li].m_dLatDeg);
       pt1[1] = Radians(m_TrackPointsV[li].m_dLongDeg);
       dVal2 = (double)(llCurTime - m_TrackPointsV[li].m_CTLE.QuadPart) / 10000000;
       dVal = m_TrackPointsV[li].m_dInitialLegSpeedMS * dVal2 + m_TrackPointsV[li].m_dAccel * (dVal2 * dVal2) / 2;
       m_pGeo->Direct(m_TrackPointsV[li].m_dLatDeg, m_TrackPointsV[li].m_dLongDeg, dCurAzimuth, dVal, pt2[0], pt2[1], Azimut2Deg, m12, M12, M21, S12);
       TP.m_dLatDeg = pt2[0];
       TP.m_dLongDeg = pt2[1];
       TP.m_CTLE.QuadPart = llCurTime;
       LIToSystemTime(&TP.m_CTLE, &TP.m_CreationTime);
       TP.m_nAltitudeM = (short)(m_TrackPointsV[li].m_dAltitude100Ft * 100 * 0.3048 + m_TrackPointsV[li].m_dVSpeedMS * ((double)(llCurTime - m_TrackPointsV[li].m_CTLE.QuadPart) / 10000000));
       }
     else
       TP.m_nAltitudeM = (short)(m_TrackPointsV[li].m_dAltitude100Ft * 100 * 0.3048);
     TP.m_dSpeedMS = SetPointSpeed(&m_TrackPointsV[li], dVal2);
     m_TrackPointsInt.push_back(TP);
     llCurTime += m_dwBreakInterval; 
     }
   }
 return(true);
}
bool CSeverity::Interpolate2(const char *p_pFileName)
{
 char ErrStr[256];
 DWORD li, dwStartTPIntSize;
 bool bVal;
 GeographicLib::Math::real Azimut1Deg, Azimut2Deg, DistM, m12, M12, M21, S12, dCurAzimuth;
 CTrackPoint TP;
 double pt1[2], pt2[2], dVal, dVal2;
 double dMaxAccelPos, dMaxAccelNeg;
 double dMinimumSpeed_ms;
 LONGLONG llCurTime;
 dMaxAccelPos = 2.5 * 9.81; 
 dMaxAccelNeg = -0.5 * 9.81; 
 dwStartTPIntSize = m_TrackPointsInt.size();
 if(m_TrackPointsV.size() == 1) {
   strcpy(ErrStr, "Source data results in only one track point.");
   AddToErrorList(ErrStr);
   if(m_FSG.m_bDebugMessages)
     Message(ErrStr);
   return(false); }
 for(li = 0; li < m_TrackPointsV.size(); li ++) {
   bVal = (li != m_TrackPointsV.size() - 1);
   if(bVal)
     bVal = (!stricmp(m_TrackPointsV[li].m_CallSign, m_TrackPointsV[li + 1].m_CallSign) != 0);
   if(!bVal && !li) {
     sprintf(ErrStr, "Flight %s has only one track point. Bad data formatting?", m_TrackPointsV[0].m_CallSign);
     AddToErrorList(ErrStr);
     if(m_FSG.m_bDebugMessages) Message(ErrStr);
     return(false); }
   if(bVal) { 
     m_pGeo->Inverse(m_TrackPointsV[li].m_dLatDeg, m_TrackPointsV[li].m_dLongDeg, m_TrackPointsV[li + 1].m_dLatDeg, m_TrackPointsV[li + 1].m_dLongDeg, DistM, Azimut1Deg, Azimut2Deg, m12, M12, M21, S12);
     m_TrackPointsV[li].m_dAzimuthDeg = (Azimut1Deg + Azimut2Deg) / 2; 
     if(m_TrackPointsV[li].m_dAzimuthDeg > 2 * 360) m_TrackPointsV[li].m_dAzimuthDeg -= 360;
     if(m_TrackPointsV[li].m_dAzimuthDeg < 0) m_TrackPointsV[li].m_dAzimuthDeg += 360;
     m_TrackPointsV[li].m_dAzimuth = m_TrackPointsV[li].m_dAzimuthDeg * M_PI / 180;
     m_TrackPointsV[li].m_dDistKm = DistM / 1000;
     m_pGeo->Inverse(m_TrackPointsV[li + 1].m_dLatDeg, m_TrackPointsV[li + 1].m_dLongDeg, m_TrackPointsV[li].m_dLatDeg, m_TrackPointsV[li].m_dLongDeg, DistM, Azimut1Deg, Azimut2Deg, m12, M12, M21, S12);
     m_TrackPointsV[li].m_dAzimuthBackDeg = Azimut1Deg;
     if(m_TrackPointsV[li].m_dAzimuthBackDeg > 2 * 360) m_TrackPointsV[li].m_dAzimuthBackDeg -= 360;
     if(m_TrackPointsV[li].m_dAzimuthBackDeg < 0) m_TrackPointsV[li].m_dAzimuthBackDeg += 360;
     m_TrackPointsV[li].m_dAzimuthBack = m_TrackPointsV[li].m_dAzimuthBackDeg * M_PI / 180;
     m_TrackPointsV[li].m_dTimeSpanS = (double)(m_TrackPointsV[li + 1].m_CTLE.QuadPart - m_TrackPointsV[li].m_CTLE.QuadPart) / 10000000;
     if(m_TrackPointsV[li].m_dInitialLegSpeedMS) 
       m_TrackPointsV[li].m_dAveSpeedMS = (m_TrackPointsV[li].m_dInitialLegSpeedMS + m_TrackPointsV[li + 1].m_dInitialLegSpeedMS) / 2;
     else
       m_TrackPointsV[li].m_dAveSpeedMS = m_TrackPointsV[li].m_dDistKm * 1000 / m_TrackPointsV[li].m_dTimeSpanS;
     m_TrackPointsV[li].m_dVSpeedMS = (m_TrackPointsV[li + 1].m_dAltitude100Ft - m_TrackPointsV[li].m_dAltitude100Ft) * 100 * 0.3048 / m_TrackPointsV[li].m_dTimeSpanS;
     if(m_TrackPointsV[li].m_dDistKm > 0) {
       dVal = (m_TrackPointsV[li + 1].m_dAltitude100Ft - m_TrackPointsV[li].m_dAltitude100Ft) * 100 * 0.3048 / (m_TrackPointsV[li].m_dDistKm * 1000);
       m_TrackPointsV[li].m_dAttitudeAngleDeg = atan(dVal) * 180 / M_PI; }
     else
       m_TrackPointsV[li + 1].m_dAttitudeAngleDeg = m_TrackPointsV[li].m_dAttitudeAngleDeg;
     if(m_TrackPointsV[li + 1].m_dAttitudeAngleDeg != m_TrackPointsV[li + 1].m_dAttitudeAngleDeg)
       dVal = dVal;
     }
   else { 
     m_TrackPointsV[li].m_dAzimuth = m_TrackPointsV[li - 1].m_dAzimuth;
     m_TrackPointsV[li].m_dAzimuthDeg = m_TrackPointsV[li - 1].m_dAzimuthDeg;
     m_TrackPointsV[li].m_dAzimuthBack = m_TrackPointsV[li - 1].m_dAzimuthBack;
     m_TrackPointsV[li].m_dAzimuthBackDeg = m_TrackPointsV[li - 1].m_dAzimuthBackDeg;
     m_TrackPointsV[li].m_dTimeSpanS = 0;
     m_TrackPointsV[li].m_dAveSpeedMS = m_TrackPointsV[li - 1].m_dAveSpeedMS;
     m_TrackPointsV[li].m_dVSpeedMS = m_TrackPointsV[li - 1].m_dVSpeedMS;
     m_TrackPointsV[li].m_dAttitudeAngleDeg = m_TrackPointsV[li - 1].m_dAttitudeAngleDeg;
     dVal = dVal;
     if(dVal != dVal)
       dVal = dVal;
     }
   }
 if(!m_TrackPointsV[0].m_dInitialLegSpeedMS) 
   SmoothOutAverageSpeed(); 
 for(li = 0; li < m_TrackPointsV.size(); li ++) {
   bVal = (li != m_TrackPointsV.size() - 1);
   if(bVal) 
     bVal = (!stricmp(m_TrackPointsV[li].m_CallSign, m_TrackPointsV[li + 1].m_CallSign) != 0);
   if(li == m_TrackPointsV[li].m_lStartIndex) { 
     if(!bVal) continue; } 
   if(bVal) {
     if(!m_TrackPointsV[li].m_dInitialLegSpeedMS) 
       m_TrackPointsV[li].m_dInitialLegSpeedMS = 2 * m_TrackPointsV[li].m_dAveSpeedMS - m_TrackPointsV[li + 1].m_dAveSpeedMS;
     if(m_TrackPointsV[li].m_dInitialLegSpeedMS < 0)
       m_TrackPointsV[li].m_dInitialLegSpeedMS = 0; 
     m_TrackPointsV[li].m_dAccel = (m_TrackPointsV[li + 1].m_dAveSpeedMS - m_TrackPointsV[li].m_dInitialLegSpeedMS) / m_TrackPointsV[li].m_dTimeSpanS;
     }
   else {
     if(!m_TrackPointsV[li].m_dInitialLegSpeedMS) 
       m_TrackPointsV[li].m_dInitialLegSpeedMS = m_TrackPointsV[li].m_dAveSpeedMS;
     m_TrackPointsV[li].m_dAccel = 0; }
   }
 WriteAccelerationFile(p_pFileName);
 dMinimumSpeed_ms = 0;
 for(li = 0; li < m_TrackPointsV.size(); li ++) {
   if(m_TrackPointsV[li].m_dInitialLegSpeedMS < dMinimumSpeed_ms) {
     sprintf(ErrStr, "Source data error. Callsign %s, Datetime: %.4d-%.2d-%.2d %.2d:%.2d:%.2d, Distance=%.1lf m, Initial leg speed=%.3lf km/h", m_TrackPointsV[li].m_CallSign,
             m_TrackPointsV[li].m_CreationTime.wYear, m_TrackPointsV[li].m_CreationTime.wMonth, m_TrackPointsV[li].m_CreationTime.wDay,
             m_TrackPointsV[li].m_CreationTime.wHour, m_TrackPointsV[li].m_CreationTime.wMinute, m_TrackPointsV[li].m_CreationTime.wSecond,
             m_TrackPointsV[li].m_dDistKm * 1000, m_TrackPointsV[li].m_dInitialLegSpeedMS * 3.6);
     if(m_FSG.m_bDebugMessages) Message(ErrStr);
     AddToErrorList(ErrStr);
     sprintf(ErrStr, "Source data error: computed/supplied leg starting speed of %.1lf kts is less than allowed minimum of %.1lf kts.", m_TrackPointsV[li].m_dInitialLegSpeedMS, dMinimumSpeed_ms);
     if(m_FSG.m_bDebugMessages) Message(ErrStr);
     AddToErrorList(ErrStr);
     return(false);
     }
   if(m_TrackPointsV[li].m_dAccel > dMaxAccelPos || m_TrackPointsV[li].m_dAccel < dMaxAccelNeg) { 
     sprintf(ErrStr, "Source data line %ld: %s: excessive acceleration %.1lf (g-force %.1lf) detected!", li + 1, m_TrackPointsV[li].m_CallSign, m_TrackPointsV[li].m_dAccel, m_TrackPointsV[li].m_dAccel / 9.81);
     AddToErrorList(ErrStr);
     if(m_FSG.m_bDebugMessages)
       Message(ErrStr);
     sprintf(ErrStr, "Source data line %ld: leg data (initial speed/distance/time/average speed): %.1lf/%.1lf/%.1lf/%.1lf", li + 1,
       m_TrackPointsV[li].m_dInitialLegSpeedMS,
       m_TrackPointsV[li].m_dDistKm * 1000,
       m_TrackPointsV[li].m_dTimeSpanS,
       m_TrackPointsV[li].m_dAveSpeedMS);
     AddToErrorList(ErrStr);
     if(m_FSG.m_bDebugMessages)
       Message(ErrStr);
     }
   }
 WriteAccelerationFile(p_pFileName);
 for(li = 0; li < m_TrackPointsV.size(); li ++) {
   TP = m_TrackPointsV[li];
   dCurAzimuth = m_TrackPointsV[li].m_dAzimuthDeg; 
   if(li == m_TrackPointsV[li].m_lStartIndex) { 
     llCurTime = TP.m_CTLE.QuadPart;
     if(TP.m_CTLE.QuadPart % m_dwBreakInterval) { 
       TP.m_CTLE.QuadPart = TP.m_CTLE.QuadPart / m_dwBreakInterval * m_dwBreakInterval;
       LIToSystemTime(&TP.m_CTLE, &TP.m_CreationTime);
       llCurTime = TP.m_CTLE.QuadPart; 
       pt1[0] = Radians(m_TrackPointsV[li].m_dLatDeg);
       pt1[1] = Radians(m_TrackPointsV[li].m_dLongDeg);
       dVal2 = (double)(m_TrackPointsV[li].m_CTLE.QuadPart - TP.m_CTLE.QuadPart) / 10000000;
       dVal = m_TrackPointsV[li].m_dInitialLegSpeedMS * dVal2 + m_TrackPointsV[li].m_dAccel * (dVal2 * dVal2) / 2;
       TP.m_dSpeedMS = SetPointSpeed(&m_TrackPointsV[li], dVal2);
       m_pGeo->Direct(m_TrackPointsV[li].m_dLatDeg, m_TrackPointsV[li].m_dLongDeg, m_TrackPointsV[li].m_dAzimuthBackDeg, dVal, pt2[0], pt2[1], Azimut2Deg, m12, M12, M21, S12);
       TP.m_dLatDeg = pt2[0];
       TP.m_dLongDeg = pt2[1];
       TP.m_nAltitudeM = (short)(m_TrackPointsV[li].m_dAltitude100Ft * 100 * 0.3048 + m_TrackPointsV[li].m_dVSpeedMS * ((double)(m_TrackPointsV[li].m_CTLE.QuadPart - TP.m_CTLE.QuadPart) / 10000000));
       m_TrackPointsInt.push_back(TP);
       llCurTime += m_dwBreakInterval; 
       }
     else { 
       TP.m_dSpeedMS = SetPointSpeed(&m_TrackPointsV[li], 0);  
       TP.m_nAltitudeM = (short)(m_TrackPointsV[li].m_dAltitude100Ft * 100 * 0.3048); 
       }
     } 
   else if(m_TrackPointsV[li].m_bLastPoint) { 
     if(TP.m_CTLE.QuadPart % m_dwBreakInterval) { 
       TP.m_CTLE.QuadPart = (TP.m_CTLE.QuadPart / m_dwBreakInterval + 1) * m_dwBreakInterval;
       LIToSystemTime(&TP.m_CTLE, &TP.m_CreationTime);
       pt1[0] = Radians(m_TrackPointsV[li].m_dLatDeg);
       pt1[1] = Radians(m_TrackPointsV[li].m_dLongDeg);
       dVal2 = (double)(TP.m_CTLE.QuadPart - m_TrackPointsV[li].m_CTLE.QuadPart) / 10000000;
       dVal = m_TrackPointsV[li].m_dInitialLegSpeedMS * dVal2 + m_TrackPointsV[li].m_dAccel * (dVal2 * dVal2) / 2;
       TP.m_dSpeedMS = SetPointSpeed(&m_TrackPointsV[li], dVal2); 
       m_pGeo->Direct(m_TrackPointsV[li].m_dLatDeg, m_TrackPointsV[li].m_dLongDeg, dCurAzimuth, dVal, pt2[0], pt2[1], Azimut2Deg, m12, M12, M21, S12);
       TP.m_dLatDeg = pt2[0];
       TP.m_dLongDeg = pt2[1];
       TP.m_nAltitudeM = (short)(m_TrackPointsV[li].m_dAltitude100Ft * 100 * 0.3048 + m_TrackPointsV[li].m_dVSpeedMS * ((double)(TP.m_CTLE.QuadPart - m_TrackPointsV[li].m_CTLE.QuadPart) / 10000000));
       }
     else { 
#pragma message("why are we changing altitude here and speed above?")
       TP.m_dSpeedMS = SetPointSpeed(&m_TrackPointsV[li], 0); 
       TP.m_nAltitudeM = (short)(m_TrackPointsV[li].m_dAltitude100Ft * 100 * 0.3048);
       }
     m_TrackPointsInt.push_back(TP);
     continue; 
     } 
   while(true) { 
     if(llCurTime >= m_TrackPointsV[li + 1].m_CTLE.QuadPart) 
       break;
     dVal2 = (double)(llCurTime - m_TrackPointsV[li].m_CTLE.QuadPart) / 10000000;
     if(llCurTime > m_TrackPointsV[li].m_CTLE.QuadPart) { 
       pt1[0] = Radians(m_TrackPointsV[li].m_dLatDeg);
       pt1[1] = Radians(m_TrackPointsV[li].m_dLongDeg);
       dVal2 = (double)(llCurTime - m_TrackPointsV[li].m_CTLE.QuadPart) / 10000000;
       dVal = m_TrackPointsV[li].m_dInitialLegSpeedMS * dVal2 + m_TrackPointsV[li].m_dAccel * (dVal2 * dVal2) / 2;
       m_pGeo->Direct(m_TrackPointsV[li].m_dLatDeg, m_TrackPointsV[li].m_dLongDeg, dCurAzimuth, dVal, pt2[0], pt2[1], Azimut2Deg, m12, M12, M21, S12);
       TP.m_dLatDeg = pt2[0];
       TP.m_dLongDeg = pt2[1];
       TP.m_CTLE.QuadPart = llCurTime;
       LIToSystemTime(&TP.m_CTLE, &TP.m_CreationTime);
       TP.m_nAltitudeM = (short)(m_TrackPointsV[li].m_dAltitude100Ft * 100 * 0.3048 + m_TrackPointsV[li].m_dVSpeedMS * ((double)(llCurTime - m_TrackPointsV[li].m_CTLE.QuadPart) / 10000000));
       }
     else
       TP.m_nAltitudeM = (short)(m_TrackPointsV[li].m_dAltitude100Ft * 100 * 0.3048);
     TP.m_dSpeedMS = SetPointSpeed(&m_TrackPointsV[li], dVal2);
     m_TrackPointsInt.push_back(TP);
     llCurTime += m_dwBreakInterval; 
     }
   }
 return(true);
}
bool CSeverity::SetStartEndPointAndAssignAircraft()
{
 CTrackPoint TP;
 short nVal;
 DWORD li;
 CADAA *pAircraft;
 *TP.m_CallSign = '\0';
 nVal = -1;
 for(li = 0; li < m_TrackPointsInt.size(); li ++) {
   if(!stricmp(m_TrackPointsInt[li].m_CallSign, TP.m_CallSign)) { 
     TP.m_lPoints ++;
     TP.m_bFirstPoint = false; 
     }
   else { 
     TP.m_lStartIndex = li;
     TP.m_lPoints = 1;
     nVal ++;
     strcpy(TP.m_CallSign, m_TrackPointsInt[li].m_CallSign);
     strcpy(TP.m_ACModel, m_TrackPointsInt[li].m_ACModel);
     strcpy(TP.m_DepartureAirport, m_TrackPointsInt[li].m_DepartureAirport);
     strcpy(TP.m_ArrivalAirport, m_TrackPointsInt[li].m_ArrivalAirport);
     TP.m_bFirstPoint = true; 
     if(li) m_TrackPointsInt[li - 1].m_bLastPoint = true;
     pAircraft = new CADAA;
     if(!pAircraft) {
       AddToErrorList("Unable to allocate memory for a CADAA instance");
       return(false); }
     strcpy(pAircraft->m_CallSign, TP.m_CallSign);
     strcpy(pAircraft->m_ACModel, TP.m_ACModel);
     pAircraft->m_pAircraft = m_AirplaneMgr.GALBNa(pAircraft->m_ACModel);
     if(!*pAircraft->m_DepartureAirport)
       strcpy(pAircraft->m_DepartureAirport, TP.m_DepartureAirport);
     if(!*pAircraft->m_ArrivalAirport)
       strcpy(pAircraft->m_ArrivalAirport, TP.m_ArrivalAirport);
     m_Aircraft.push_back(pAircraft);
     pAircraft->m_Wake.m_pSensitivityAnalysisSetup = &m_SA;
     }
   m_TrackPointsInt[li].m_bFirstPoint = TP.m_bFirstPoint; 
   m_TrackPointsInt[li].m_lStartIndex = TP.m_lStartIndex;
   m_TrackPointsInt[li].m_nAircraftOrdinal = nVal;
   m_TrackPointsInt[m_TrackPointsInt[li].m_lStartIndex].m_lPoints = TP.m_lPoints; }
 m_TrackPointsInt[m_TrackPointsInt.size() - 1].m_bLastPoint = true;
 return(true);
}
bool CSeverity::CreateIntTP()
{
 LONGLONG llMaxTime;
 std::vector<CTrackPoint>::iterator It;
 DWORD dwVal, dwCurIndex;
 m_llMinTime = 0;
 llMaxTime = 0;
 for(It = m_TrackPointsInt.begin(); It != m_TrackPointsInt.end(); It ++) {
   if(!m_llMinTime || m_llMinTime > It->m_CTLE.QuadPart)
     m_llMinTime = It->m_CTLE.QuadPart;
   if(!llMaxTime || llMaxTime < It->m_CTLE.QuadPart)
     llMaxTime = It->m_CTLE.QuadPart;
   }
 if(m_llMinTime % m_dwBreakInterval)
   m_llMinTime = m_llMinTime / m_dwBreakInterval * m_dwBreakInterval;
 if(llMaxTime % m_dwBreakInterval)
   llMaxTime = (llMaxTime / m_dwBreakInterval + 1) * m_dwBreakInterval;
 if(m_pIntTS) delete [] m_pIntTS;
 m_pIntTS = NULL;
 m_dwTSCount = 0;
 dwVal = (DWORD)((llMaxTime - m_llMinTime) / m_dwBreakInterval);
 m_pIntTS = new rIntervalTrackPoint[dwVal + 1];
 if(!m_pIntTS) {
   AddToErrorList("Unable to allocate memory for rIntervalTrackPoint instances");
   return(false); }
 m_dwTSCount = dwVal;
 dwCurIndex = 0;
 for(It = m_TrackPointsInt.begin(); It != m_TrackPointsInt.end(); It ++) {
   dwVal = (DWORD)((It->m_CTLE.QuadPart - m_llMinTime) / m_dwBreakInterval);
   m_pIntTS[dwVal].m_TP.push_back(dwCurIndex);
   dwCurIndex ++;
   }
 return(true);
}
void CSeverity::SetStartAndEndPointFlag()
{
 char *pCallSign = "";
 std::list <CTrackPoint>::iterator It, PrevIt;
 if(!m_TrackPoints.size()) return;
 for(It = m_TrackPoints.begin(); It != m_TrackPoints.end(); It ++) {
   if(stricmp(It->m_CallSign, pCallSign)) {
     It->m_bFirstPoint = true; 
     if(It != m_TrackPoints.begin())
       PrevIt->m_bLastPoint = true;
     pCallSign = It->m_CallSign; }
   PrevIt = It; }
 PrevIt->m_bLastPoint = true;
}
void CSeverity::SetStartAndEndPointV()
{
 DWORD li, dwStartIndex = 0;
 char *pCallSign = "";
 for(li = 0; li < m_TrackPointsV.size(); li ++) {
   if(stricmp(m_TrackPointsV[li].m_CallSign, pCallSign)) {
     m_TrackPointsV[li].m_bFirstPoint = true;
     dwStartIndex = li;
     m_TrackPointsV[dwStartIndex].m_lPoints = 0;
     if(li) m_TrackPointsV[li - 1].m_bLastPoint = true;
     pCallSign = m_TrackPointsV[li].m_CallSign;
     }
   m_TrackPointsV[li].m_lStartIndex = dwStartIndex;
   m_TrackPointsV[dwStartIndex].m_lPoints ++;
   }
 m_TrackPointsV[m_TrackPointsV.size() - 1].m_bLastPoint = true;
}
bool CSeverity::GetFileTypeDateAndSizeFromName(const char *p_pFileName, eFileType *p_peFileType, SYSTEMTIME *p_pST, DWORD *p_pdwFileSize)
{
 const char *pWork, *pStartWork;
 char ch, DateStr[9];
 short nDigits;
 HANDLE hFile;
 short Offsets[2], YearAdd;
 if(!p_pFileName || !p_peFileType) return(false);
 if(*p_peFileType == FILETYPE_NONE) { 
   *p_peFileType = FILETYPE_ISA;
   pWork = strrchr(p_pFileName, '.');
   if(pWork) {
     if(!stricmp(pWork + 1, "isa") || !stricmp(pWork + 1, "dat")) *p_peFileType = FILETYPE_ISA;
     else if(!stricmp(pWork + 1, "fdr")) *p_peFileType = FILETYPE_FDR;
     else if(!stricmp(pWork + 1, "json")) *p_peFileType = FILETYPE_WVSS_JSON;
     else if(!stricmp(pWork + 1, "json")) *p_peFileType = FILETYPE_ASAIC_JSON; 
     else if(!stricmp(pWork + 1, "mitre")) *p_peFileType = FILETYPE_MITRE;
     else if(!stricmp(pWork + 1, "mdf")) *p_peFileType = FILETYPE_MULTI;
     } }
 if(p_pdwFileSize) {
   *p_pdwFileSize = 0;
   hFile = CreateFile(p_pFileName, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
   if(hFile == INVALID_HANDLE_VALUE) return(false);
   if((*p_pdwFileSize = GetFileSize(hFile, NULL)) == INVALID_FILE_SIZE)
     return(false);
   CloseHandle(hFile);
   }
 if(!p_pST) return(true);
 memset(p_pST, 0, sizeof(SYSTEMTIME));
 p_pST->wYear = m_FSG.m_nDefaultYear;
 p_pST->wMonth = m_FSG.m_nDefaultMonth;
 p_pST->wDay = m_FSG.m_nDefaultDay;
 pWork = p_pFileName + strlen(p_pFileName) - 1;
 nDigits = 0;
 while(pWork != p_pFileName) {
   if(AVP_IsDigit(*pWork))
     nDigits ++;
   else {
     if(nDigits == 6 || nDigits == 8) break; 
     else nDigits = 0; }
   -- pWork; }
 if(nDigits == 6 || nDigits == 8) {
   if(nDigits == 6) {
     Offsets[0] = 2;
     Offsets[1] = 4;
     YearAdd = 2000; }
   else if(nDigits == 8) {
     Offsets[0] = 4;
     Offsets[1] = 6;
     YearAdd = 0; }
   else
     return(true);
   memset(DateStr, 0, sizeof(DateStr));
   if(!AVP_IsDigit(*pWork))
     pWork ++;
   memcpy(DateStr, pWork, nDigits);
   pStartWork = DateStr;
   ch = *(pStartWork + Offsets[0]);
   *((char *)pStartWork + Offsets[0]) = '\0';
   p_pST->wYear = atoi(pStartWork) + YearAdd;
   *((char *)pStartWork + Offsets[0]) = ch;
   ch = *(pStartWork + Offsets[1]);
   *((char *)pStartWork + Offsets[1]) = '\0';
   p_pST->wMonth = atoi(pStartWork + Offsets[0]);
   *((char *)pStartWork + Offsets[1]) = ch;
   p_pST->wDay = atoi(pStartWork + Offsets[1]);
   }
 return(true);
}
bool CSeverity::LoadWindServer()
{
 char tstr[256];
 bool bVal;
 sprintf(tstr, "%s\\WVSSWindServer.bin", m_ThisDir);
 bVal = m_WindServer.LoadFromFile(tstr);
 if(bVal)
   Message("Wind server input file %s loaded", tstr);
 return(bVal);
}
void CSeverity::SetThisDir(const char *p_pStr)
{
 strcpy_s(m_ThisDir, sizeof(m_ThisDir) - 1, p_pStr);
}
void CSeverity::AdjustToBoundary(LONGLONG *p_pllVal, DWORD p_dwBoundary)
{
 *p_pllVal = (*p_pllVal) / p_dwBoundary * p_dwBoundary + ((((*p_pllVal) % p_dwBoundary) < p_dwBoundary / 2) ? 0 : p_dwBoundary);
}
double CSeverity::GetDistance2(GeographicLib::Geodesic *p_pGeo, GeographicLib::LocalCartesian *p_pLC, bool p_bLCSetToPoint1, double p_dLatDeg1, double p_dLongDeg1, double p_dhM1, double p_dLatDeg2, double p_dLongDeg2, double p_dhM2, double *p_pDistMWithAltitude )
{
 GeographicLib::Math::real LCX, LCZ, LCH;
 GeographicLib::Math::real Azimut1Deg, Azimut2Deg, DistM, m12, M12, M21, S12;
 GeographicLib::Math::real LCOrigLat, LCOrigLong, LCOrigH;
 if(p_bLCSetToPoint1) {
   LCOrigLat = p_pLC->LatitudeOrigin();
   LCOrigLong = p_pLC->LongitudeOrigin();
   LCOrigH = p_pLC->HeightOrigin();
   p_pLC->Reset(p_dLatDeg1, p_dLongDeg1, p_dhM1); }
 p_pGeo->Inverse(p_dLatDeg1, p_dLongDeg1, p_dLatDeg2, p_dLongDeg2, DistM, Azimut1Deg, Azimut2Deg, m12, M12, M21, S12);
 if(p_pDistMWithAltitude) *p_pDistMWithAltitude = DistM;
 if(DistM < 5000) {
   p_pLC->Forward(p_dLatDeg2, p_dLongDeg2, p_dhM2, LCX, LCZ, LCH);
   DistM = sqrt(pow(LCX, 2) + pow(LCZ, 2));
   if(p_pDistMWithAltitude)
     *p_pDistMWithAltitude = sqrt(pow(LCX, 2) + pow(LCZ, 2) + pow(LCH, 2));
   }
 if(p_bLCSetToPoint1)
   p_pLC->Reset(LCOrigLat, LCOrigLong, LCOrigH);
 return(DistM);
}
double CSeverity::GetDistanceTo(GeographicLib::Geodesic *p_pGeo, GeographicLib::LocalCartesian *p_pLC, double p_dLatDeg2, double p_dLongDeg2, double p_dhM2, double *p_pDistMWithAltitude )
{
 return(GetDistance2(p_pGeo, p_pLC, false, p_pLC->LatitudeOrigin(), p_pLC->LongitudeOrigin(), p_pLC->HeightOrigin(), p_dLatDeg2, p_dLongDeg2, p_dhM2, p_pDistMWithAltitude));
}
double CSeverity::GetExtrapolatedWind(double p_dRefPointWindSpeed_ms, double p_dRefPointElevation_m, double p_dAltitude_m)
{
#pragma message("change wind speed extrapolation at altitude from power law to something else")
 double dVal, dRefPointElevation_m;
 dRefPointElevation_m = (p_dRefPointElevation_m <= 10) ? 10 : p_dRefPointElevation_m; 
 dVal = p_dAltitude_m / dRefPointElevation_m;
 if(dVal < 1) dVal = 1;
 return(p_dRefPointWindSpeed_ms * pow(dVal, 0.143));
}
double CSeverity::SetPointSpeed(CTrackPoint *p_pTP, double p_dTimeS)
{
 if(p_pTP->m_dTimeSpanS < 10)
   return(p_pTP->m_dAveSpeedMS);
 return(p_pTP->m_dInitialLegSpeedMS + p_pTP->m_dAccel * p_dTimeS);
}
bool CSeverity::WakeSectionRelevantICAO(const CAPM::RAPL3 * , const CWSNEC * )
{
#pragma message("WakeSectionRelevantICAO is disabled. Need to rework and reenable. Suggestion: compute danger level RCR per aircraft type")
 return(true); 
}
bool CSeverity::WakeSectionRelevantICAO(const CAPM::RAPL3 *p_pAircraft, const CWS2 *p_pWakeSection)
{
#pragma message("WakeSectionRelevantICAO is disabled. Need to rework and reenable. Suggestion: compute danger level RCR per aircraft type")
 return(true); 
}
int CSeverity::static_yajl_start_map(void *p_pContext)
{
 CSeverity *pSeverity = reinterpret_cast<CSeverity *>(p_pContext);
 if(!pSeverity) return(0);
 return pSeverity->yajl_start_map();
}
int CSeverity::static_yajl_map_key(void *p_pContext, const unsigned char *p_pKey, size_t p_StringLen)
{
 CSeverity *pSeverity = reinterpret_cast<CSeverity *>(p_pContext);
 if(!pSeverity) return(0);
 return pSeverity->yajl_map_key(p_pKey, p_StringLen);
}
int CSeverity::static_yajl_end_map(void *p_pContext)
{
 CSeverity *pSeverity = reinterpret_cast<CSeverity *>(p_pContext);
 if(!pSeverity) return(0);
 return pSeverity->yajl_end_map();
}
int CSeverity::static_yajl_string(void *p_pContext, const unsigned char *p_pStringVal, size_t p_StringLen)
{
 CSeverity *pSeverity = reinterpret_cast<CSeverity *>(p_pContext);
 if(!pSeverity) return(0);
 return pSeverity->yajl_string(p_pStringVal, p_StringLen);
}
int CSeverity::static_yajl_start_array(void *p_pContext)
{
 CSeverity *pSeverity = reinterpret_cast<CSeverity *>(p_pContext);
 if(!pSeverity) return(0);
 return pSeverity->yajl_start_array();
}
int CSeverity::static_yajl_end_array(void *p_pContext)
{
 CSeverity *pSeverity = reinterpret_cast<CSeverity *>(p_pContext);
 return pSeverity->yajl_end_array();
}
int CSeverity::static_yajl_number(void *p_pContext, const char *p_pNumberVal, size_t p_nNumberLen)
{
 CSeverity *pSeverity = reinterpret_cast<CSeverity *>(p_pContext);
 return pSeverity->yajl_number(p_pContext, p_pNumberVal, p_nNumberLen);
}
int CSeverity::yajl_start_map()
{
 m_strlCurInputDataPath.AddTail("{");
 if(m_CurFileType == FILETYPE_ASAIC_JSON) {
   if(CWildcard::Match(m_strlCurInputDataPath.GetPath(), "[/[/{")) { 
     m_pCurInputListStartingPointIt = m_TrackPoints.end();
     if(m_TrackPoints.size() > 0)
       -- m_pCurInputListStartingPointIt;
     m_pCurInputListCurPointIt = m_pCurInputListStartingPointIt; } }
 return(1);
}
int CSeverity::yajl_map_key(const unsigned char *p_pKey, size_t p_StringLen)
{
 char tstr[128];
 CString Str;
 Str = m_strlCurInputDataPath.GetTail();
 if(Str != "{") 
   m_strlCurInputDataPath.RemoveTail(); 
 memset(tstr, 0, sizeof(tstr));
 strncpy(reinterpret_cast<char *>(tstr), reinterpret_cast<const char *>(p_pKey), p_StringLen);
 m_strlCurInputDataPath.AddTail(tstr);
 return(1);
}
int CSeverity::yajl_string(const unsigned char *p_pStringVal, size_t p_StringLen)
{
 char tstr[128];
 LARGE_INTEGER liTime, liTimeToAdd;
 if(m_CurFileType == FILETYPE_WVSS_JSON) {
   if(CWildcard::Match(m_strlCurInputDataPath.GetPath(), "{/Weather/{/ObtainWeatherData", TRUE)) { 
     strncpy(tstr, (const char *)p_pStringVal, p_StringLen);
     m_ObtainWeatherFlag = WVSS_OWF_DEFAULT;
     if(!stricmp(tstr, "Yes") || !strcmp(tstr, "1")) 
       m_ObtainWeatherFlag = WVSS_OWF_YES;
     else if(!stricmp(tstr, "No") || !strcmp(tstr, "0")) 
       m_ObtainWeatherFlag = WVSS_OWF_NO;
     }
   else if(CWildcard::Match(m_strlCurInputDataPath.GetPath(), "{/Weather/{/UseStation", TRUE)) {
     memset(m_UseWeatherStation, 0, sizeof(m_UseWeatherStation));
     strncpy(m_UseWeatherStation, (const char *)p_pStringVal, p_StringLen);
     }
   else if(CWildcard::Match(m_strlCurInputDataPath.GetPath(), "{/Weather/{/StationElevation_meters", TRUE)) {
     m_dWeatherStationElevation = atof((const char *)p_pStringVal);
     }
   if(CWildcard::Match(m_strlCurInputDataPath.GetPath(), "{/Track*/{/Callsign", TRUE)) {
     memset(m_CurTrackPointToAdd.m_CallSign, 0, sizeof(m_CurTrackPointToAdd.m_CallSign));
     strncpy(m_CurTrackPointToAdd.m_CallSign, (const char *)p_pStringVal, p_StringLen);
     }
   else if(CWildcard::Match(m_strlCurInputDataPath.GetPath(), "{/Track*/{/AircraftType", TRUE)) {
     memset(m_CurTrackPointToAdd.m_ACModel, 0, sizeof(m_CurTrackPointToAdd.m_ACModel));
     strncpy(m_CurTrackPointToAdd.m_ACModel, (const char *)p_pStringVal, p_StringLen);
     }
   else if(CWildcard::Match(m_strlCurInputDataPath.GetPath(), "{/Track*/{/ADEP", TRUE)) {
     memset(m_CurTrackPointToAdd.m_DepartureAirport, 0, sizeof(m_CurTrackPointToAdd.m_DepartureAirport));
     strncpy(m_CurTrackPointToAdd.m_DepartureAirport, (const char *)p_pStringVal, p_StringLen);
     }
   else if(CWildcard::Match(m_strlCurInputDataPath.GetPath(), "{/Track*/{/ADES", TRUE)) {
     memset(m_CurTrackPointToAdd.m_ArrivalAirport, 0, sizeof(m_CurTrackPointToAdd.m_ArrivalAirport));
     strncpy(m_CurTrackPointToAdd.m_ArrivalAirport, (const char *)p_pStringVal, p_StringLen);
     }
   else if(CWildcard::Match(m_strlCurInputDataPath.GetPath(), "{/Track*/{/TimeShift_s", TRUE)) {
     m_CurTrackPointToAdd.m_lTimeShift_s = 0;
     m_CurTrackPointToAdd.m_lTimeShift_s = atol((const char *)p_pStringVal);
     }
   else if(CWildcard::Match(m_strlCurInputDataPath.GetPath(), "{/Track*/{/Points/[/{/DateTime", TRUE)) {
     if(!FromChar((const char *)p_pStringVal, &m_CurTrackPointToAdd.m_CreationTime, FILETYPE_WVSS_JSON, 0)) {
       sprintf(tstr, "DateTime value %s parse error", p_pStringVal);
       AddToErrorList(tstr);
       return(0); }
     m_CurTrackPointToAdd.m_CreationTimeOrig = m_CurTrackPointToAdd.m_CreationTime;
     if(!SystemTimeToLI(&m_CurTrackPointToAdd.m_CreationTime, &m_CurTrackPointToAdd.m_CTLE)) {
       sprintf(tstr, "DateTime value %s is invalid", (const char *)p_pStringVal);
       AddToErrorList(tstr);
       return(0); }
     if(m_CurTrackPointToAdd.m_lTimeShift_s) {
       liTime = m_CurTrackPointToAdd.m_CTLE;
       liTimeToAdd.QuadPart = m_CurTrackPointToAdd.m_lTimeShift_s;
       liTimeToAdd.QuadPart *= 10000000;
       liTime.QuadPart += liTimeToAdd.QuadPart;
       if(!LIToSystemTime(&liTime, &m_CurTrackPointToAdd.m_CreationTime)) {
         sprintf(tstr, "DateTime value %s after time shift %ld is invalid", (const char *)p_pStringVal, m_CurTrackPointToAdd.m_lTimeShift_s);
         AddToErrorList(tstr);
         return(0); }
       m_CurTrackPointToAdd.m_CTLE = liTime;
       m_CurTrackPointToAdd.m_CreationTimeOrig = m_CurTrackPointToAdd.m_CreationTime; }
     }
   else if(CWildcard::Match(m_strlCurInputDataPath.GetPath(), "{/Track*/{/Points/[/{/Lat", TRUE)) {
     if(!FromChar((const char *)p_pStringVal, &m_CurTrackPointToAdd.m_dLatDeg, FILETYPE_WVSS_JSON, 0)) {
       sprintf(tstr, "Lat value %s is invalid", (const char *)p_pStringVal);
       AddToErrorList(tstr);
       return(0); }
     }
   else if(CWildcard::Match(m_strlCurInputDataPath.GetPath(), "{/Track*/{/Points/[/{/Long", TRUE)) {
     if(!FromChar((const char *)p_pStringVal, &m_CurTrackPointToAdd.m_dLongDeg, FILETYPE_WVSS_JSON, 0)) {
       sprintf(tstr, "Long value %s is invalid", (const char *)p_pStringVal);
       AddToErrorList(tstr);
       return(0); }
     }
   else if(CWildcard::Match(m_strlCurInputDataPath.GetPath(), "{/Track*/{/Points/[/{/Altitude", TRUE)) {
     if(!FromChar((const char *)p_pStringVal, &m_CurTrackPointToAdd.m_dAltitude100Ft, FILETYPE_WVSS_JSON, 0)) {
       sprintf(tstr, "Altitude value %s is invalid", (const char *)p_pStringVal);
       AddToErrorList(tstr);
       return(0); }
     }
   else if(CWildcard::Match(m_strlCurInputDataPath.GetPath(), "{/Track*/{/Points/[/{/WindFromDeg", TRUE)) { 
     if(!FromChar((const char *)p_pStringVal, &m_CurTrackPointToAdd.m_dWDRT, FILETYPE_WVSS_JSON, 0)) {
       sprintf(tstr, "Wind direction value %s is invalid", (const char *)p_pStringVal);
       AddToErrorList(tstr);
       return(0); }
     m_CurTrackPointToAdd.m_dWDRT = Radians(m_CurTrackPointToAdd.m_dWDRT + 180);
     if(m_CurTrackPointToAdd.m_dWDRT < -M_PI) m_CurTrackPointToAdd.m_dWDRT += 2 * M_PI;
     if(m_CurTrackPointToAdd.m_dWDRT > M_PI) m_CurTrackPointToAdd.m_dWDRT -= 2 * M_PI;
     }
   else if(CWildcard::Match(m_strlCurInputDataPath.GetPath(), "{/Track*/{/Points/[/{/WindSpeedKts", TRUE)) { 
     if(!FromChar((const char *)p_pStringVal, &m_CurTrackPointToAdd.m_dWindSpeedMS, FILETYPE_WVSS_JSON, 0)) {
       sprintf(tstr, "Wind speed value %s is invalid", (const char *)p_pStringVal);
       AddToErrorList(tstr);
       return(0); }
     m_CurTrackPointToAdd.m_dWindSpeedMS = m_CurTrackPointToAdd.m_dWindSpeedMS * 1.852 / 3.6;
     }
   } 
 else if(m_CurFileType == FILETYPE_ASAIC_JSON) {
   if(CWildcard::Match(m_strlCurInputDataPath.GetPath(), "[") && !m_dwCurInputValueOrdinal) {
     strncpy(m_CurTrackPointToAdd.m_DepartureAirport, (const char *)p_pStringVal, min(p_StringLen, sizeof(m_CurTrackPointToAdd.m_DepartureAirport) - 1));
     strncpy(m_CurTrackPointToAdd.m_ArrivalAirport, (const char *)p_pStringVal, min(p_StringLen, sizeof(m_CurTrackPointToAdd.m_ArrivalAirport) - 1));
     m_pCurInputScenario = ScenarioByName(m_CurTrackPointToAdd.m_DepartureAirport);
     m_pLC->Reset(m_pCurInputScenario ? m_pCurInputScenario->m_dCenterLatDeg : 0, m_pCurInputScenario ? m_pCurInputScenario->m_dCenterLongDeg : 0);
     }
   else if(CWildcard::Match(m_strlCurInputDataPath.GetPath(), "[/[/{/callsign", TRUE)) {
     memset(m_CurTrackPointToAdd.m_CallSign, 0, sizeof(m_CurTrackPointToAdd.m_CallSign));
     strncpy(m_CurTrackPointToAdd.m_CallSign, (const char *)p_pStringVal, min(p_StringLen, sizeof(m_CurTrackPointToAdd.m_CallSign) - 1));
     TrimStr(m_CurTrackPointToAdd.m_CallSign);
     if(!*m_CurTrackPointToAdd.m_CallSign) {
       sprintf(m_CurTrackPointToAdd.m_CallSign, "Callsign%d", m_nCurUnnamedCallsignNum);
       m_nCurUnnamedCallsignNum ++; }
     }
   else if(CWildcard::Match(m_strlCurInputDataPath.GetPath(), "[/[/{/acType", TRUE)) {
     memset(m_CurTrackPointToAdd.m_ACModel, 0, sizeof(m_CurTrackPointToAdd.m_ACModel));
     strncpy(m_CurTrackPointToAdd.m_ACModel, (const char *)p_pStringVal, min(p_StringLen, sizeof(m_CurTrackPointToAdd.m_ACModel) - 1));
     TrimStr(m_CurTrackPointToAdd.m_ACModel); }
   }
 if(m_bCurInputInArray)
   m_dwCurInputValueOrdinal ++;
 return(1);
}
int CSeverity::yajl_number(void *p_pContext, const char *p_pNumberVal, size_t p_nNumberLen)
{
 char tstr[128], DataStr[32];
 const CTrackPoint *pCurTrackPoint = NULL;
 double dVal[3];
 if(m_CurFileType == FILETYPE_ASAIC_JSON) {
   if(CWildcard::Match(m_strlCurInputDataPath.GetPath(), "[/[/{/coords/[", TRUE)) { 
     if(!(m_dwCurInputValueOrdinal % 2)) {
       if(!FromChar((const char *)p_pNumberVal, &m_CurTrackPointToAdd.m_dLatDeg, FILETYPE_ASAIC_JSON, 0)) {
         sprintf(tstr, "Latitude value %s is invalid", (const char *)p_pNumberVal);
         AddToErrorList(tstr);
         return(0); } }
     else {
       if(!FromChar((const char *)p_pNumberVal, &m_CurTrackPointToAdd.m_dLongDeg, FILETYPE_ASAIC_JSON, 0)) {
         sprintf(tstr, "Longitude value %s is invalid", (const char *)p_pNumberVal);
         AddToErrorList(tstr);
         return(0); }
       dVal[0] = m_CurTrackPointToAdd.m_dLatDeg; 
       dVal[1] = m_CurTrackPointToAdd.m_dLongDeg; 
       m_pLC->Reverse(dVal[0], dVal[1], 0, m_CurTrackPointToAdd.m_dLatDeg, m_CurTrackPointToAdd.m_dLongDeg, dVal[2]); 
       m_TrackPoints.push_back(m_CurTrackPointToAdd); }
     }
   else if(CWildcard::Match(m_strlCurInputDataPath.GetPath(), "[/[/{/times/[", TRUE)) { 
     if(m_pCurInputListCurPointIt == m_TrackPoints.end()) {
       sprintf(tstr, "Encountered \"times\" with no records to update. Make sure \"coords\" preceedes \"times\"");
       AddToErrorList(tstr);
       return(0); }
     if(p_nNumberLen > sizeof(DataStr) - 1) {
       sprintf(tstr, "The length (%d) of a supposed datetime value is more than %d", p_nNumberLen, sizeof(DataStr) - 1);
       AddToErrorList(tstr);
       return(0); }
     memset(DataStr, 0, sizeof(DataStr));
     strncpy(DataStr, p_pNumberVal, p_nNumberLen);
     if(!DateTimeFromStr(DataStr, "UnixTimeMs", &m_pCurInputListCurPointIt->m_CreationTime)) {
       sprintf(tstr, "The datetime value %s is not a UnixTime with milliseconds", p_pNumberVal);
       AddToErrorList(tstr);
       return(0); }
     m_pCurInputListCurPointIt->m_CreationTimeOrig = m_pCurInputListCurPointIt->m_CreationTime;
     if(!SystemTimeToLI(&m_pCurInputListCurPointIt->m_CreationTime, &m_pCurInputListCurPointIt->m_CTLE)) {
       sprintf(tstr, "Time conversion error, time value %s is invalid", p_pNumberVal);
       AddToErrorList(tstr);
       return(false); }
     ++ m_pCurInputListCurPointIt;
     }
   else if(CWildcard::Match(m_strlCurInputDataPath.GetPath(), "[/[/{/pz/[", TRUE)) { 
     if(m_pCurInputListCurPointIt == m_TrackPoints.end()) {
       sprintf(tstr, "Encountered \"pz\" with no records to update. Make sure \"coords\" preceedes \"pz\"");
       AddToErrorList(tstr);
       return(0); }
     if(!FromChar((const char *)p_pNumberVal, &m_pCurInputListCurPointIt->m_dAltitude100Ft, FILETYPE_WVSS_JSON, 0)) {
       sprintf(tstr, "Altitude value %s is invalid", (const char *)p_pNumberVal);
       AddToErrorList(tstr);
       return(0); }
     m_pCurInputListCurPointIt->m_dAltitude100Ft = m_pCurInputListCurPointIt->m_dAltitude100Ft / 0.3048 / 100; 
     ++ m_pCurInputListCurPointIt;
     }
   else if(CWildcard::Match(m_strlCurInputDataPath.GetPath(), "[/[/{/vg/[", TRUE)) { 
     if(m_pCurInputListCurPointIt == m_TrackPoints.end()) {
       sprintf(tstr, "Encountered \"vg\" with no records to update. Make sure \"coords\" preceedes \"vg\"");
       AddToErrorList(tstr);
       return(0); }
     if(!FromChar((const char *)p_pNumberVal, &m_pCurInputListCurPointIt->m_dSpeedMS, FILETYPE_WVSS_JSON, 0)) {
       sprintf(tstr, "Speed value %s is invalid", (const char *)p_pNumberVal);
       AddToErrorList(tstr);
       return(0); }
     ++ m_pCurInputListCurPointIt;
     }
   } 
 if(m_bCurInputInArray)
   m_dwCurInputValueOrdinal ++;
 return(1);
}
int CSeverity::yajl_end_map()
{
 CString Str;
 Str = m_strlCurInputDataPath.GetTail();
 if(Str != "{") 
   m_strlCurInputDataPath.RemoveTail(); 
 Str = m_strlCurInputDataPath.GetTail();
 ASSERT(Str == "{"); 
 m_strlCurInputDataPath.RemoveTail(); 
 if(m_strlCurInputDataPath.IsEmpty())
   return(1); 
 if(m_CurFileType == FILETYPE_WVSS_JSON) {
   if(CWildcard::Match(m_strlCurInputDataPath.GetPath(), "{/Track*/{/Points/[", TRUE)) {
     m_TrackPoints.push_back(m_CurTrackPointToAdd);
     } }
 Str = m_strlCurInputDataPath.GetTail();
 if(Str != "[") 
   m_strlCurInputDataPath.RemoveTail();
 return(1);
}
int CSeverity::yajl_start_array()
{
 std::list <CTrackPoint>::iterator It;
 m_strlCurInputDataPath.AddTail("[");
 m_bCurInputInArray = true;
 m_dwCurInputValueOrdinal = 0;
 if(m_CurFileType == FILETYPE_ASAIC_JSON) {
   m_pCurInputListCurPointIt = m_pCurInputListStartingPointIt;
   if(m_pCurInputListCurPointIt == m_TrackPoints.end() && m_TrackPoints.size() > 0)
     m_pCurInputListCurPointIt = m_TrackPoints.begin();
   else if(m_pCurInputListCurPointIt != m_TrackPoints.end()) {
     It = m_pCurInputListCurPointIt;
     ++ It;
     if(It != m_TrackPoints.end())
       ++ m_pCurInputListCurPointIt; }
   }
 return(1);
}
int CSeverity::yajl_end_array()
{
 CString Str;
 Str = m_strlCurInputDataPath.GetTail();
 ASSERT(Str == "["); 
 m_strlCurInputDataPath.RemoveTail(); 
 m_bCurInputInArray = false;
 if(m_strlCurInputDataPath.IsEmpty())
   return(1); 
 Str = m_strlCurInputDataPath.GetTail();
 if(Str != "[")
   m_strlCurInputDataPath.RemoveTail();
 return(1);
}
void CSeverity::FillInWeatherData(DWORD p_dwStartTPIndex)
{
 DWORD li;
 C2D Wind2D;
 CWindPoint3D WindPoint3D;
 if(m_AuxWindServer.IsReady()) { 
   for(li = p_dwStartTPIndex; li < m_TrackPointsInt.size(); li ++) {
     m_AuxWindServer.GetValueExact(li - p_dwStartTPIndex, &Wind2D);
     m_TrackPointsInt[li].m_dWDRT = Wind2D.m_dWDRT;
     m_TrackPointsInt[li].m_dWindSpeedMS = Wind2D.m_dWindSpeedMS; } }
 if(m_WindServer.IsReady()) {
   m_AuxWindServer.Set2dArraySize(m_TrackPointsInt.size() - p_dwStartTPIndex);
   for(li = p_dwStartTPIndex; li < m_TrackPointsInt.size(); li ++) {
     if(m_TrackPointsInt[li].m_dWDRT != AVP_BAD_WIND_VALUE && m_TrackPointsInt[li].m_dWindSpeedMS != AVP_BAD_WIND_VALUE)
       continue; 
     Wind2D.dLat = m_TrackPointsInt[li].m_dLatDeg;
     Wind2D.dLong = m_TrackPointsInt[li].m_dLongDeg;
     Wind2D.m_dWDRT = m_TrackPointsInt[li].m_dWDRT; 
     Wind2D.m_dWindSpeedMS = m_TrackPointsInt[li].m_dWindSpeedMS; 
     if(m_WindServer.GetValue(&Wind2D)) {
       m_TrackPointsInt[li].m_dWDRT = Wind2D.m_dWDRT;
       m_TrackPointsInt[li].m_dWindSpeedMS = Wind2D.m_dWindSpeedMS;
       }
     m_AuxWindServer.SetValue(li - p_dwStartTPIndex, &Wind2D);
     if(!((li - p_dwStartTPIndex) % 100))
       Message("Wind cache creation %.1lf%%", (double)(li - p_dwStartTPIndex) * 100 / (m_TrackPointsInt.size() - p_dwStartTPIndex));
     }
   if(m_AuxWindServer.SaveToFile())
     Message("Wind cache file %s created", m_AuxWindServer.GetLastLoadFileName());
   }
 if(m_WindServerWU.GetValueCount()) {
   WindPoint3D.Init();
   WindPoint3D.m_ldwFlags |= (WVSS_WIND_FLAG_DATETIME | WVSS_WIND_FLAG_TEXT);
   for(li = p_dwStartTPIndex; li < m_TrackPointsInt.size(); li ++) {
     if(m_TrackPointsInt[li].m_dWDRT != AVP_BAD_WIND_VALUE && m_TrackPointsInt[li].m_dWindSpeedMS != AVP_BAD_WIND_VALUE)
       continue; 
     if(*m_UseWeatherStation)
       strcpy(WindPoint3D.m_LocationText, m_UseWeatherStation);
     else {
       continue;
       }
     WindPoint3D.m_MeasurementDTLI = m_TrackPointsInt[li].m_CTLE;
     if(m_WindServerWU.GetValue(&WindPoint3D)) {
       m_TrackPointsInt[li].m_dWDRT = WindPoint3D.m_dWindDirectionDegTo * M_PI / 180;
       if(m_FSG.m_bNWE)
         m_TrackPointsInt[li].m_dWindSpeedMS = WindPoint3D.m_dWindSpeedMS;
       else
         m_TrackPointsInt[li].m_dWindSpeedMS = GetExtrapolatedWind(WindPoint3D.m_dWindSpeedMS, m_dWeatherStationElevation, m_TrackPointsInt[li].m_nAltitudeM);
       }
     }   
   }
 for(li = p_dwStartTPIndex; li < m_TrackPointsInt.size(); li ++) {
   if(m_TrackPointsInt[li].m_dWDRT != AVP_BAD_WIND_VALUE && m_TrackPointsInt[li].m_dWindSpeedMS != AVP_BAD_WIND_VALUE)
     continue; 
   m_TrackPointsInt[li].m_dWDRT = m_FSG.m_dWDRT;
   if(m_FSG.m_bNWE)
     m_TrackPointsInt[li].m_dWindSpeedMS = m_FSG.m_dWS;
   else
     m_TrackPointsInt[li].m_dWindSpeedMS = GetExtrapolatedWind(m_FSG.m_dWS, m_dWeatherStationElevation, m_TrackPointsInt[li].m_nAltitudeM);
   }
}
void CSeverity::LoadWeatherData(const char *p_pFileName)
{
 char tstr[MAX_PATH + 128];
 bool bVal;
 std::list<CTrackPoint>::iterator It, PrevIt;
 std::map<CString, CWindServer2RequestCacheRecord>::iterator WS2WURRequestCacheIt;
 CWindServerOutputRecord WSOR;
 strcpy(tstr, p_pFileName);
 ReplaceFileExtension(tstr, ".wind");
 bVal = m_AuxWindServer.LoadFromFile(tstr);
 if(bVal) Message("Wind cache file %s loaded", tstr);
 if(!m_TrackPoints.size())
   return;
 if(m_ObtainWeatherFlag == WVSS_OWF_NO || (m_ObtainWeatherFlag == WVSS_OWF_DEFAULT && !m_FSG.m_bRW))
   return;
 It = m_TrackPoints.begin();
 if(!*m_UseWeatherStation && strlen(It->m_DepartureAirport) == 3)
   sprintf(m_UseWeatherStation, "K%s", It->m_DepartureAirport);
 if(!*m_UseWeatherStation && strlen(It->m_ArrivalAirport) == 3)
   sprintf(m_UseWeatherStation, "K%s", It->m_ArrivalAirport);
 if(!*m_UseWeatherStation && strlen(It->m_DepartureAirport) == 4 && tolower(*It->m_DepartureAirport) == 'k')
   strcpy(m_UseWeatherStation, It->m_DepartureAirport);
 if(!*m_UseWeatherStation && strlen(It->m_ArrivalAirport) == 4 && tolower(*It->m_ArrivalAirport) == 'k')
   strcpy(m_UseWeatherStation, It->m_ArrivalAirport);
 if(*m_UseWeatherStation)
   Message("Weather station %s at evelation %.1lf", m_UseWeatherStation, m_dWeatherStationElevation);
 if(!*m_UseWeatherStation) {
   Message("Weather cannot be obtained - no weather station explicitly specified, cannot be deduced from the track.");
   return; }
 if(*tstr) {
   m_WindServerWU.m_FSG = m_FSG;
   bVal = m_WindServerWU.MakeRequest(m_UseWeatherStation, &It->m_CreationTime, m_dWeatherStationElevation);
   if(!bVal) Message("ERROR OBTAINING WEATHER"); }
 m_WindServerOutput.clear();
 if(m_WindServerWU.m_RequestCache.size()) {
   for(WS2WURRequestCacheIt = m_WindServerWU.m_RequestCache.begin(); WS2WURRequestCacheIt != m_WindServerWU.m_RequestCache.end(); ++WS2WURRequestCacheIt) {
     WSOR.Init();
     WSOR.m_strRequest = WS2WURRequestCacheIt->second.m_strRequest;
     WSOR.m_bSuccess = WS2WURRequestCacheIt->second.m_bSuccess;
     WSOR.m_bHTTPRequestSuccess = WS2WURRequestCacheIt->second.m_bHTTPRequestSuccess;
     WSOR.m_dwRequestHTTPStatusCode = WS2WURRequestCacheIt->second.m_dwRequestHTTPStatusCode;
     WSOR.m_nWeatherRecords = WS2WURRequestCacheIt->second.m_nWeatherRecords;
     m_WindServerOutput.push_back(WSOR);
     }
   }
}
bool CSeverity::LoadSensitivityAnalysisFromFile(const char *p_pFile)
{
 return(m_SA.LoadFromFile(p_pFile));
}
bool CSeverity::WriteOBTFile(const char *p_pFileName, bool p_bAddToFile, bool p_bFinalVector)
{
 FILE *fFile;
 std::vector <CTrackPoint>::iterator It, PrevIt;
 double dLongitude; 
 fFile = OpenOutputFile2(p_pFileName, ".obt", (p_bAddToFile ? "ab" : "wb"), m_FSG.m_bCOF);
 if(!fFile) return(false);
 if(p_bAddToFile)
   fseek(fFile, 0, SEEK_END);
 for(It = (p_bFinalVector ? m_TrackPointsInt.begin() : m_TrackPointsV.begin()); It != (p_bFinalVector ? m_TrackPointsInt.end() : m_TrackPointsV.end()); ++ It) {
   if(It->m_bFirstPoint)
     fprintf(fFile, "Name: %s\r\nClassifier: 193 145\r\nSortOrder: 1\r\nPointCount: %ld\r\n", It->m_CallSign, It->m_lPoints);
   dLongitude = It->m_dLongDeg < 0 ? It->m_dLongDeg + 360 : It->m_dLongDeg;
   if(p_bFinalVector)
     dLongitude += 0.01;
   fprintf(fFile, "%lf;%lf;%d;%.4d-%.2d-%.2d %.2d:%.2d:%.2d.%.3d;%.4d-%.2d-%.2d %.2d:%.2d:%.2d.%.3d;%ld;%.0f;%.1f;\r\n", 
           It->m_dLatDeg, 
           dLongitude,
           It->m_nAltitudeM,
           It->m_CreationTimeOrig.wYear, It->m_CreationTimeOrig.wMonth, It->m_CreationTimeOrig.wDay, It->m_CreationTimeOrig.wHour, It->m_CreationTimeOrig.wMinute, It->m_CreationTimeOrig.wSecond, It->m_CreationTimeOrig.wMilliseconds,
           It->m_CreationTime.wYear, It->m_CreationTime.wMonth, It->m_CreationTime.wDay, It->m_CreationTime.wHour, It->m_CreationTime.wMinute, It->m_CreationTime.wSecond, It->m_CreationTime.wMilliseconds,
           (DWORD)(It->m_bFirstPoint ? 0 : ((It->m_CTLE.QuadPart - PrevIt->m_CTLE.QuadPart) / 10000)),
           (It->m_dWDRT == AVP_BAD_WIND_VALUE ? AVP_BAD_WIND_VALUE : It->m_dWDRT * 180 / M_PI),
           It->m_dWindSpeedMS);
   PrevIt = It;
   }
 if(fFile) fclose(fFile);
 return(true);
}
bool CSeverity::WriteCesiumFile(const char *p_pFileName)
{
 FILE *fFile;
 std::vector <CTrackPoint>::iterator It, PrevIt;
 if(!(fFile = OpenOutputFile2(p_pFileName, ".js", "wb", m_FSG.m_bCOF))) return(false);
 for(It = m_TrackPointsInt.begin(); It != m_TrackPointsInt.end(); ++ It) {
   if(It->m_bFirstPoint) {
     if(It != m_TrackPointsInt.begin())
       fprintf(fFile, "]\n\n");
     fprintf(fFile, "[");
     }
   fprintf(fFile, "[%lf,%lf,%d],\n", It->m_dLatDeg, It->m_dLongDeg, It->m_nAltitudeM);
   PrevIt = It;
   }
 fclose(fFile);
 return(true);
}
bool CSeverity::WriteInterpolatedJSON(const char *p_pFileName)
{
 FILE *fFile;
 std::vector <CTrackPoint>::iterator It, PrevIt;
 short TrackOrdinal = 1;
 if(!(fFile = OpenOutputFile2(p_pFileName, " interpolated.json", "wb", m_FSG.m_bCOF))) return(false);
 fprintf(fFile, "{\r\n\"Meta\" : { \"DataType\" : \"ExternalTraffic\" },\r\n");
 for(It = m_TrackPointsInt.begin(); It != m_TrackPointsInt.end(); ++ It) {
   if(!It->m_bFirstPoint)
     fprintf(fFile, ",\r\n");
   else {
     if(It != m_TrackPointsInt.begin()) fprintf(fFile, "\r\n]\r\n},\r\n");
     fprintf(fFile, "\"Track%d\" : { \"Callsign\" : \"%s\", \"AircraftType\" : \"%s\", \"ADEP\" : \"%s\", \"ADES\" : \"%s\",\r\n\"Points\":\r\n[\r\n",
       TrackOrdinal, It->m_CallSign, It->m_ACModel, It->m_DepartureAirport, It->m_ArrivalAirport);
     TrackOrdinal ++;
     }
   fprintf(fFile, "{ \"DateTime\" : \"%.4d-%.2d-%.2d %.2d:%.2d:%.2d.%.3d\", \"Lat\" : \"%lf\", \"Long\" : \"%lf\", \"Altitude_m\" : \"%d\", \"Speed_ms\" : \"%lf\" }",
           It->m_CreationTime.wYear, It->m_CreationTime.wMonth, It->m_CreationTime.wDay, It->m_CreationTime.wHour, It->m_CreationTime.wMinute, It->m_CreationTime.wSecond, It->m_CreationTime.wMilliseconds,
           It->m_dLatDeg, It->m_dLongDeg, It->m_nAltitudeM, It->m_dSpeedMS);
   PrevIt = It;
   }
 fprintf(fFile, "\r\n]\r\n}\r\n}\r\n");
 fclose(fFile);
 return(true);
}
void CSeverity::WriteJSONFromSource(const char *p_pFileName)
{
 FILE *fFile;
 std::list <CTrackPoint>::iterator It;
 short TrackOrdinal;
 if(!m_TrackPoints.size()) return;
 fFile = OpenOutputFile2(p_pFileName, ".json", "wb", m_FSG.m_bCOF);
 if(!fFile) return;
 fprintf(fFile, "{\r\n");
 fprintf(fFile, "\"Meta\" : { \"DataType\" : \"ExternalTraffic\" },\r\n");
 TrackOrdinal = 1;
 for(It = m_TrackPoints.begin(); It != m_TrackPoints.end(); ++It) {
   if(It->m_bFirstPoint) {
     fprintf(fFile, "\"Track%d\" : { \"Callsign\" : \"%s\", \"AircraftType\" : \"%s\", \"ADEP\" : \"%s\", \"ADES\" : \"%s\",\r\n\"Points\":\r\n[\r\n", TrackOrdinal, It->m_CallSign, It->m_ACModel, It->m_DepartureAirport, It->m_ArrivalAirport);
     }
   fprintf(fFile, "{ \"DateTime\" : \"%.4d-%.2d-%.2d %.2d:%.2d:%.2d.%.3d\", \"Lat\" : \"%lf\", \"Long\" : \"%lf\", \"Altitude\" : \"%.0lf\" }%s\r\n", 
                  It->m_CreationTimeOrig.wYear, It->m_CreationTimeOrig.wMonth, It->m_CreationTimeOrig.wDay, It->m_CreationTimeOrig.wHour, It->m_CreationTimeOrig.wMinute, It->m_CreationTimeOrig.wSecond, It->m_CreationTimeOrig.wMilliseconds,
                  It->m_dLatDeg, It->m_dLongDeg, It->m_dAltitude100Ft, It->m_bLastPoint ? "" : ",");
   if(It->m_bLastPoint) {
     fprintf(fFile, "]\r\n},\r\n");
     TrackOrdinal ++;
     }
   }
 fprintf(fFile, "}\r\n");
 fclose(fFile);
}
bool CSeverity::WriteInterpolatedFile(const char *p_pFileName)
{
 FILE *fFile;
 DWORD li, lj, lk, lVal, lCurWakeSectionIndex;
 CTrackPoint *pTP, *pTPP;
 CWS2 WakeSection2;
 if(!(fFile = OpenOutputFile2(p_pFileName, " Interpolated.csv", "wb", m_FSG.m_bCOF))) return(false);
 fprintf(fFile, "Ord;CallSign;AveSpeedKmH;AltitudeM;m_dg0a;PtSpeedKmH;dt;x-x0;y-y0;Accel;InitialSpeedKmH;\r\n");
 for(lk = 0; lk < m_Aircraft.size(); lk ++) {
   lCurWakeSectionIndex = 0;
   for(li = 0; li < m_dwTSCount; li ++) {
     for(lj = 0; lj < m_pIntTS[li].m_TP.size(); lj ++) {
       lVal = m_pIntTS[li].m_TP[lj];
       pTP = &m_TrackPointsInt[lVal];
       if(pTP->m_nAircraftOrdinal != lk) continue; 
       pTPP = (li > m_Aircraft[pTP->m_nAircraftOrdinal]->m_lStartPos) ? &m_TrackPointsInt[lVal - 1] : pTP; 
       if(!m_Aircraft[pTP->m_nAircraftOrdinal]->m_Wake.GS2EC(lCurWakeSectionIndex, pTP->m_CTLE, &WakeSection2))
         continue;
       if(fFile) {
         fprintf(fFile, "%d;%s;%.1f;%d;%.1f;%.1f;%.1f;%.3f;%.1f\r\n",
           lCurWakeSectionIndex + 1,
           m_Aircraft[pTP->m_nAircraftOrdinal]->m_CallSign,
           pTP->m_dAveSpeedMS * 3.6,
           pTP->m_nAltitudeM,
           WakeSection2.m_dCiCu,
           pTP->m_dSpeedMS * 3.6,
           (double)(pTP->m_CTLE.QuadPart - pTPP->m_CTLE.QuadPart) / 10000000,
           pTP->m_dAccel,
           pTP->m_dInitialLegSpeedMS * 3.6
           );
         }
       lCurWakeSectionIndex ++;
       }
     }
   }
 fclose(fFile);
 return(true);
}
bool CSeverity::WriteAccelerationFile(const char *p_pFileName)
{
 FILE *fFile;
 DWORD li;
 if(!(fFile = OpenOutputFile2(p_pFileName, " Acceleration.csv", "wb", m_FSG.m_bCOF))) return(false);
 fprintf(fFile, "Ord;Callsign;DistM;TimeS;AveSpeedMS;InitialSpeedMS;Accel;\r\n");
 for(li = 0; li < m_TrackPointsV.size(); li ++) {
   fprintf(fFile, "%ld;%s;%.1lf;%.1lf;%.1lf;%.1lf;%.1lf;\r\n", li, m_TrackPointsV[li].m_CallSign, 
       m_TrackPointsV[li].m_dDistKm * 1000, m_TrackPointsV[li].m_dTimeSpanS, m_TrackPointsV[li].m_dAveSpeedMS, m_TrackPointsV[li].m_dInitialLegSpeedMS, m_TrackPointsV[li].m_dAccel);
     }
 fclose(fFile);
 return(true);
}
bool CSeverity::WriteSeverityFile(const char *p_pFileName, const char *p_pNameSuffix)
{
 FILE *fFile;
 DWORD li;
 double dWindDirFrom;
 char tstr[64];
 sprintf(tstr, " Severity%s.csv", (p_pNameSuffix ? p_pNameSuffix : ""));
 if(!(fFile = OpenOutputFile2(p_pFileName, tstr, "wb", m_FSG.m_bCOF))) return(false);
 fprintf(fFile, "Leader;Follower;Time;TimeFromLeader;MaxBankDeg;MaxAngularSpeedOfBankDegPerS;MaxAltitudeLossFt;AC2ACDistNM;WindDirectionDegFrom;WindSpeedKTS;PlaneToWakeDistanceHorz_m;PlaneToWakeDistanceVert_m;PlayerPos\r\n");
 for(li = 0; li < m_FullSeverityOutput.size(); li ++) {
   dWindDirFrom = m_FullSeverityOutput[li].m_dWindDirectionDegTo - 180;
   if(dWindDirFrom < 0) dWindDirFrom += 360;
   fprintf(fFile, "%s;%s;%.2d:%.2d:%.2d;%.1lf;%.2lf;%.2lf;%.1lf;%.1f;%.1lf;%.1lf;%.1lf;%.1lf;%ld\r\n",
     m_FullSeverityOutput[li].m_GeneratorCallSign, m_FullSeverityOutput[li].m_FollowerCallSign, 
     m_FullSeverityOutput[li].m_EntryTime.wHour, m_FullSeverityOutput[li].m_EntryTime.wMinute, m_FullSeverityOutput[li].m_EntryTime.wSecond,
     m_FullSeverityOutput[li].m_dTimeFromGeneratorS,
     m_FullSeverityOutput[li].m_dMaxBankRad * 180 / M_PI, 
     m_FullSeverityOutput[li].m_dMaxAngularSpeedOfBankRadPerS * 180 / M_PI, 
     m_FullSeverityOutput[li].m_dMaxAltitudeLossM / 0.3048,
     m_FullSeverityOutput[li].m_dGFDM / 1852,
     dWindDirFrom,
     m_FullSeverityOutput[li].m_dWindSpeedMS * 3.6 / 1.852, 
     m_FullSeverityOutput[li].m_dPlaneToWakeHorz,
     m_FullSeverityOutput[li].m_dPlaneToWakeVert,
     m_FullSeverityOutput[li].m_lPlayerPos);
     }
 fclose(fFile);
 return(true);
}
bool CSeverity::InitGeoid()
{
 DWORD dwTime = GetTickCount();
 try {
   m_pGeoid = new GeographicLib::Geoid(m_FSG.m_GeoidName, m_FSG.m_GeoidDir, true, true);
   }
 catch(GeographicLib::GeographicErr Err) {
   std::string str;
   str = Err.what();
   if(m_pGeoid) { delete m_pGeoid; m_pGeoid = NULL; }
   }
 dwTime = GetTickCount() - dwTime;
 Message("Geoid named %s from dir %s: load %s in %.1lf sec", m_FSG.m_GeoidName, m_FSG.m_GeoidDir, m_pGeoid != NULL ? "SUCCESS" : "FAILURE", (double)dwTime / 1000);
 return(m_pGeoid != NULL);
}
void CSeverity::AddToErrorList(const char *p_pErrStr)
{
 m_ErrorList.AddTail(p_pErrStr);
 Message(p_pErrStr);
}
std::map<CString, bool>CSeverity::GetMissingElevationDataAreas()
{
 DWORD li;
 std::map<CString, bool>CombinedMissingAreasList;
 std::map<CString, bool>::const_iterator It;
 const std::map<CString, bool>*pMissingAreas;
 for(li = 0; li < m_Aircraft.size(); li ++) {
   pMissingAreas = m_Aircraft[li]->m_Wake.GetMissingElevationAreas();
   for(It = pMissingAreas->begin(); It != pMissingAreas->end(); It++) {
     if(CombinedMissingAreasList.find(It->first) == CombinedMissingAreasList.end())
       CombinedMissingAreasList.insert(std::pair<CString, bool>(It->first, It->second));
     } }
 return(CombinedMissingAreasList);
}
void CSeverity::LogMissingElevationDataAreas(const char *p_pFileName)
{
 bool bFirst = true;
 std::map<CString, bool>CombinedMissingAreasList;
 std::map<CString, bool>::const_iterator It;
 CombinedMissingAreasList = GetMissingElevationDataAreas();
 for(It = CombinedMissingAreasList.begin(); It != CombinedMissingAreasList.end(); It++) {
   if(bFirst) {
     Message("Track %s: missing elevation data tiles:", p_pFileName);
     bFirst = false; }
   Message(It->first);
   }
}
void CSeverity::GetWeatherStationInfo(char *p_pStation, double *p_pdElevation)
{
 if(!p_pStation || !p_pdElevation) return;
 strcpy(p_pStation, m_UseWeatherStation);
 *p_pdElevation = m_dWeatherStationElevation; 
}
void CSeverity::LoadEDRList()
{
 char FileName[256], tstr[32], ValueStr[64], *pWork;
 unsigned long li, Count;
 CEDRManager::rEDRRecord EDRRec;
 sprintf(FileName, "%s\\EDR.ini", m_ThisDir);
 Count = GetPrivateProfileInt("EDR", "Count", 0, FileName);
 for(li = 0; li < Count; li ++) {
   sprintf(tstr, "Line%d", li + 1);
   GetPrivateProfileString("EDR", tstr, "", ValueStr, sizeof(ValueStr) - 1, FileName);
   if(!*ValueStr) break;
   pWork = strtok(ValueStr, " ,");
   if(!pWork) break;
   EDRRec.m_dAltitude_m = atof(pWork);
   pWork = strtok(NULL, " ,");
   if(!pWork) break;
   EDRRec.m_dEDR = atof(pWork);
   if(!CWDRR::m_EDRManager.AddRecord(&EDRRec))
     break; }
 CWDRR::m_EDRManager.Sort();
}
bool CSeverity::WriteWakeJSON(const char *p_pFileName)
{
 FILE *fFile;
 unsigned long li, lj, lTrackOrdinal, lTrackPointOrdinal, lSectionCount, lBreakInterval, lSectionsFound;
 CWSNEC WakeSectionOrig;
 CWS2 WakeSection2;
 LARGE_INTEGER liTime;
 SYSTEMTIME ST;
 if(!(fFile = OpenOutputFile2(p_pFileName, " wake.json", "wb", m_FSG.m_bCOF))) return(false);
 fprintf(fFile, "{\r\n");
 lTrackOrdinal = 1;
 for(li = 0; li < m_Aircraft.size(); li ++) {
   if(lTrackOrdinal != 1)
     fprintf(fFile, ",\r\n");
   fprintf(fFile, "\"Track%ld\" : { \"Callsign\" : \"%s\", \"AircraftType\" : \"%s\", \"ADEP\" : \"%s\", \"ADES\" : \"%s\",\r\n\"TrackPoints\": [\r\n",
     lTrackOrdinal, m_Aircraft[li]->m_CallSign, m_Aircraft[li]->m_ACModel, m_Aircraft[li]->m_DepartureAirport, m_Aircraft[li]->m_ArrivalAirport);
   lTrackOrdinal ++;
   lSectionCount = m_Aircraft[li]->m_Wake.GNOS();
   lBreakInterval = m_Aircraft[li]->m_Wake.GetBreakInterval();
   m_Aircraft[li]->m_Wake.GetOriginalSection(0, &WakeSectionOrig);
   liTime = WakeSectionOrig.m_CTLE;
   lTrackPointOrdinal = 1;
   for(;;) {
     LIToSystemTime(&liTime, &ST);
     if(lTrackPointOrdinal > 1)
       fprintf(fFile, ",\r\n");
     fprintf(fFile, "{ \"Ordinal\" : \"%ld\", \"Time\" : \"%.4d%.2d%.2d%.2d%.2d%.2d%.3d\", \"WakeSections\": [\r\n", lTrackPointOrdinal,
       ST.wYear, ST.wMonth, ST.wDay, ST.wHour, ST.wMinute, ST.wSecond, ST.wMilliseconds);
     lSectionsFound = 0;
     for(lj = 0; lj < lSectionCount; lj ++) {
       if(m_Aircraft[li]->m_Wake.GS2EC(lj, liTime, &WakeSection2)) {
         if(lSectionsFound)
           fprintf(fFile, ",\r\n");
         fprintf(fFile, "{ \"Ordinal\" : \"%ld\", "
                        "\"Lat1\" : \"%lf\", \"Long1\" : \"%lf\", \"Altitude1\" : \"%.2lf\", "
                        "\"Lat2\" : \"%lf\", \"Long2\" : \"%lf\", \"Altitude2\" : \"%.2lf\", "
                        "\"HorzSize\" : \"%.2lf\", \"VertSize\" : \"%.2lf\" }", 
           lSectionsFound + 1, 
           WakeSection2.m_dLT1d, WakeSection2.m_sLL1DT, WakeSection2.m_dAltitude1,
           WakeSection2.m_dL2TD, WakeSection2.m_dLLD2, WakeSection2.m_dAltitude2,
           WakeSection2.m_dHSe, WakeSection2.m_deVSe);
         lSectionsFound ++; }
       }
     fprintf(fFile, "]}");
     if(!lSectionsFound) break;
     liTime.QuadPart += lBreakInterval;
     lTrackPointOrdinal ++;
     }
   fprintf(fFile, "\r\n]\r\n}");
   }
 fprintf(fFile, "\r\n}\r\n"); 
 fclose(fFile);
 return(true);
}
bool CSeverity::SmoothOutAverageSpeed()
{
 unsigned long li;
 const char *pCallSign;
 double dVal;
 short nVal, nAircraftOrdinal;
 m_TrackPointsV[0].m_nAircraftOrdinal = 100; 
 pCallSign = m_TrackPointsV[0].m_CallSign;
 for(li = 1; li < m_TrackPointsV.size(); li ++) {
   m_TrackPointsV[li].m_nAircraftOrdinal = m_TrackPointsV[li - 1].m_nAircraftOrdinal;
   if(stricmp(m_TrackPointsV[li].m_CallSign, pCallSign)) { 
     m_TrackPointsV[li].m_nAircraftOrdinal ++;
     pCallSign = m_TrackPointsV[li].m_CallSign; } }
 nAircraftOrdinal = m_TrackPointsV[0].m_nAircraftOrdinal;
 for(li = 0; li < m_TrackPointsV.size(); li ++) {
   dVal = 0;
   nVal = 0;
   if((long)li - 2 >= 0) { 
     if(m_TrackPointsV[li - 2].m_nAircraftOrdinal == nAircraftOrdinal) { 
       dVal += m_TrackPointsV[li - 2].m_dAveSpeedMS; nVal ++; } }
   if((long)li - 1 >= 0) { 
     if(m_TrackPointsV[li - 1].m_nAircraftOrdinal == nAircraftOrdinal) { 
       dVal += m_TrackPointsV[li - 1].m_dAveSpeedMS; nVal ++; } }
   if(m_TrackPointsV[li].m_nAircraftOrdinal != nAircraftOrdinal) { 
     dVal = 0;
     nVal = 0;
     nAircraftOrdinal = m_TrackPointsV[li].m_nAircraftOrdinal; } 
   dVal += m_TrackPointsV[li].m_dAveSpeedMS;
   nVal ++;
   if(li + 1 < m_TrackPointsV.size()) { 
     if(m_TrackPointsV[li + 1].m_nAircraftOrdinal == nAircraftOrdinal) { 
       dVal += m_TrackPointsV[li + 1].m_dAveSpeedMS; nVal ++; } }
   if(li + 2 < m_TrackPointsV.size()) { 
     if(m_TrackPointsV[li + 2].m_nAircraftOrdinal == nAircraftOrdinal) { 
       dVal += m_TrackPointsV[li + 2].m_dAveSpeedMS; nVal ++; } }
   dVal /= nVal;
   m_TrackPointsV[li].m_dSpeedMS = dVal; 
   }
 for(li = 0; li < m_TrackPointsV.size(); li ++) {
   m_TrackPointsV[li].m_dAveSpeedMS = m_TrackPointsV[li].m_dSpeedMS;
   m_TrackPointsV[li].m_dSpeedMS = 0; }
 return(true);
}
