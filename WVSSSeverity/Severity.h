#pragma once
#include <vector>
#include <list>
#include "IWVSSSeverity.h"
#include "TrackPoint.h"
#include "Matrix.h"
#include "WindServer.h"
#include "WakeClass2.h"
#include "Wildcard.h"
#include "StringListExt.h"
#include "WindServerWU.h"
#include "SensitivityAnalysis.h"
#include "WakeInteraction.h"
#include "ObjLoadLineParser.h"
#include "wake_shared.h"
#define GEOGRAPHICLIB_SHARED_LIB 1
namespace GeographicLib {
class Geodesic;
class LocalCartesian;
class Geoid;
}
namespace WVSS
{
class CTrackMetaInfo
{
public:
 char m_CallSign[32];
 char m_ACModel[16];
 char m_DepartureAirport[16];
 char m_ArrivalAirport[16];
 LARGE_INTEGER m_Date;
 double m_dWindDirectionToRad; 
 double m_dWindSpeedMS; 
public:
 CTrackMetaInfo() { memset(this, 0, sizeof(CTrackMetaInfo)); }
};
class CADAA
{
public:
 char m_CallSign[32];
 char m_ACModel[32];
 bool m_bLastOn, m_bCurOn; 
 DWORD m_lStartPos; 
 CWDRR m_Wake;
 long m_lCurSectionCount; 
 char m_DepartureAirport[16], m_ArrivalAirport[16];
 const CAPM::RAPL3 *m_pAircraft;
 DWORD m_lStartTrackPointIndex, m_lTrackPointCount;
public:
 CADAA() { m_bLastOn = false; m_bCurOn = false; strcpy_s(m_CallSign, sizeof(m_CallSign), "???"); strcpy_s(m_ACModel, sizeof(m_ACModel), "???"); m_lStartPos = 0; 
                   *m_DepartureAirport = *m_ArrivalAirport = '\0'; m_pAircraft = NULL; 
                   m_lStartTrackPointIndex = (DWORD)-1L; m_lTrackPointCount = 0; }
 ~CADAA() { }
};
struct rIntervalTrackPoint
{
 std::vector<DWORD>m_TP;
 std::vector<CWESC>m_WakeInteractions;
};
class WVSSAPIEX CSeverity : public IWVSSSeverity
{
private:
 float m_fProcessed; 
 float m_fCurProcessedTotal; 
 CTrackMetaInfo m_TrackMetaInfo;
 CAPM m_AirplaneMgr;
 char m_LogFileTitle[64]; 
 HANDLE m_hLogFile; 
 eObtainWeatherFlag m_ObtainWeatherFlag; 
 char m_UseWeatherStation[16]; 
 double m_dWeatherStationElevation; 
 char *m_pDataBuff; 
 DWORD m_dwDataBuffSize;
 eFileType m_CurFileType; 
 CTrackPoint m_CurTrackPointToAdd; 
 CStringListExt m_strlCurInputDataPath; 
 bool m_bCurInputInArray; 
 DWORD m_dwCurInputValueOrdinal; 
 std::list <CTrackPoint>::iterator m_pCurInputListStartingPointIt, m_pCurInputListCurPointIt;
 short m_nCurUnnamedCallsignNum; 
 const CWVSSScenario *m_pCurInputScenario;
 CWindServerWU m_WindServerWU;
 char m_LastLoadedFileName[MAX_PATH];
 CWVSSScenario *m_pScenarios;
 short m_nScenarios;
protected:
public:
 std::list <CTrackPoint> m_TrackPoints;
 std::vector <CTrackPoint> m_TrackPointsV;
 std::vector <CTrackPoint> m_TrackPointsInt;
 rIntervalTrackPoint *m_pIntTS; 
 DWORD m_dwTSCount;
 std::vector <CADAA *> m_Aircraft;
 CADAA *m_pCurAircraftForWakeProgress; 
 DWORD m_dwBreakInterval; 
 LONGLONG m_llMinTime; 
 GeographicLib::Geodesic *m_pGeo;
 GeographicLib::LocalCartesian *m_pLC; 
 GeographicLib::LocalCartesian *m_pEncounterLC; 
 GeographicLib::Geoid *m_pGeoid;
 CWindServer m_WindServer, m_AuxWindServer;
 CSensitivityAnalysisSetup m_SA;
public:
 void (* m_pfMessage)(const char *);
 void (* m_pfOnFileLoaded)(const char *p_pFileName);
 void (* m_pfUAP)(rAircraftPosition *p_pAircraftPosition, short p_nAircraftOrdinal, CTrackPoint *p_pTP);
private:
 bool OpenLogFile(char *p_pDT );
 void Message(const char *p_pFormat, ...);
 virtual void WriteToLog(const char *p_pFormat, ...);  
 void OnFileLoaded(const char *p_pFileName);
 void UAPuos(rAircraftPosition *p_pAircraftPosition, short p_nAircraftOrdinal, CTrackPoint *p_pTP);
 bool LoadFileContinued2(LPCSTR p_pFileName);
 bool ReadFile(FILE *p_fFile, eFileType p_FileType, SYSTEMTIME *p_pST, DWORD p_dwFileSize);
 bool ReadFileISA2(FILE *p_fFile, SYSTEMTIME *p_pST); 
 bool ReadFileFDR(FILE *p_fFile, eFileType p_FileType);
 bool ReadFileIntoMemory(FILE *p_fFile, DWORD p_dwFileSize);
 bool ReadFileWVSSJSON(FILE *p_fFile, DWORD p_dwFileSize);
 bool ReadFileASAICJSON(FILE *p_fFile, DWORD p_dwFileSize);
 bool ReadFileMITRE(FILE *p_fFile);
 virtual bool ProcessTrackBuffer(const char *p_pDataBuff, DWORD p_dwBuffSize, eFileType p_FileType); 
 template <class T> bool FromChar(const char *p_pCharData, T *p_pRes, eFileType p_FileType, short p_nFileColOrd);
 void FillTrackMetaInfo(const char *p_pDataFileName);
 bool LoadAirplaneList();
 bool Interpolate(const char *p_pFileName);
 bool Interpolate2(const char *p_pFileName);
 bool SmoothOutAverageSpeed();
 bool SetStartEndPointAndAssignAircraft();
 bool CreateIntTP();
 void SetStartAndEndPointFlag();
 void SetStartAndEndPointV();
 double SetPointSpeed(CTrackPoint *p_pTP, double p_dTimeS);
 bool WakeSectionRelevantICAO(const CAPM::RAPL3 *p_pAircraft, const CWSNEC *p_pWakeSection);
 bool WakeSectionRelevantICAO(const CAPM::RAPL3 *p_pAircraft, const CWS2 *p_pWakeSection);
 bool InitGeoid();
 void LogMissingElevationDataAreas(const char *p_pFileName); 
 void LoadEDRList();
 static int static_yajl_start_map(void *p_pContext);
 static int static_yajl_map_key(void *p_pContext, const unsigned char *p_pKey, size_t p_StringLen);
 static int static_yajl_end_map(void *p_pContext);
 static int static_yajl_string(void *p_pContext, const unsigned char *p_pStringVal, size_t p_StringLen);
 static int static_yajl_number(void *p_pContext, const char *p_pNumberVal, size_t p_nNumberLen);
 static int static_yajl_start_array(void *p_pContext);
 static int static_yajl_end_array(void *p_pContext);
 int yajl_start_map();
 int yajl_map_key(const unsigned char *p_pKey, size_t p_StringLen);
 int yajl_string(const unsigned char *p_pStringVal, size_t p_StringLen);
 int yajl_number(void *p_pContext, const char *p_pNumberVal, size_t p_nNumberLen);
 int yajl_end_map();
 int yajl_start_array();
 int yajl_end_array();
 void LoadWeatherData(const char *p_pFileName);
 void FillInWeatherData(DWORD p_dwStartTPIndex);
 bool WriteOBTFile(const char *p_pFileName, bool p_bAddToFile, bool p_bFinalVector);
 bool WriteCesiumFile(const char *p_pFileName);
 void WriteJSONFromSource(const char *p_pFileName);
 bool WriteInterpolatedFile(const char *p_pFileName);
 bool WriteAccelerationFile(const char *p_pFileName);
 bool WriteSeverityFile(const char *p_pFileName, const char *p_pNameSuffix);
 bool WriteInterpolatedJSON(const char *p_pFileName);
 bool WriteWakeJSON(const char *p_pFileName);
protected:
public:
 CSeverity();
 virtual ~CSeverity();
 virtual bool Init();
 void GetSettings(const char *p_pINIFileName);
 void LoadScenarios(const char *p_pINIFileName);
 const CWVSSScenario *ScenarioByName(const char *p_pName); 
 void AddToErrorList(const char *p_pErrStr);
 virtual bool LoadFile(LPCSTR p_pFileName, IWVSSSeverity::eFileType p_FileType, bool p_bFirst, bool p_bLast);
 void Recalc2(int p_nCurPos, bool p_bAddToWakeEncounterList); 
 long GetTPIntIndexFromAircraftOrdinal(DWORD p_dwTrackPoint, short p_nAircraftOrdinal);
 void SetAircraftFlags(int p_nCurPos);
 BOOL DoOnFileLoaded(const char *p_pFileName);
 float GetProcessedPercentage();
 bool LoadWindServer();
 bool GetFileTypeDateAndSizeFromName(const char *p_pFileName, eFileType *p_peFileType, SYSTEMTIME *p_pST, DWORD *p_pdwFileSize);
 void SetThisDir(const char *p_pStr);
 void AdjustToBoundary(LONGLONG *p_pllVal, DWORD p_dwBoundary);
 double GetDistance2(GeographicLib::Geodesic *p_pGeo, GeographicLib::LocalCartesian *p_pLC, bool p_bLCSetToPoint1, double p_dLatDeg1, double p_dLongDeg1, double p_dhM1, double p_dLatDeg2, double p_dLongDeg2, double p_dhM2, double *p_pDistMWithAltitude = NULL);
 double GetDistanceTo(GeographicLib::Geodesic *p_pGeo, GeographicLib::LocalCartesian *p_pLC, double p_dLatDeg2, double p_dLongDeg2, double p_dhM2, double *p_pDistMWithAltitude = NULL);
 double GetExtrapolatedWind(double p_dRefPointWindSpeed_ms, double p_dRefPointElevation_m, double p_dAltitude_m);
 bool LoadSensitivityAnalysisFromFile(const char *p_pFile);
 virtual void GetWeatherStationInfo(char *p_pStation, double *p_pdElevation);
 const std::vector<CWindPoint3D> *GetWindPointVector() { return(m_WindServerWU.GetWindPointVector()); }
 virtual std::map<CString, bool>GetMissingElevationDataAreas();
};
} 
