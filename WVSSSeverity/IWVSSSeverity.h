#pragma once
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
#include <vector>
#include <map>
namespace WVSS
{
#define AVP_BAD_WIND_VALUE     -11111
#define AVP_BAD_WX_WAKE_VALUE  -11111
#define AVP_BAD_VW_Y_VALUE     -11111
#define AVP_BAD_VW_Z_VALUE     -11111
#define AVP_BAD_VW_VALUE       -11111
#define AVP_BAD_DEGREE_VALUE   -11111
#define AVP_BAD_DIST_VALUE     -11111
#define AVP_BAD_ALTITUDE_VALUE -11111
class WVSSAPI CSOC
{
public:
 char m_GeneratorCallSign[32], m_FollowerCallSign[32]; 
 double m_dGeneratorLatDeg, m_dGeneratorLongDeg, m_dFollowerLatDeg, m_dFollowerLongDeg;
 double m_dGeneratorAltitudeM, m_dFollowerAltitudeM;
 double m_dGFDM;
 SYSTEMTIME m_EntryTime;
 double m_dTimeInWake;
 double m_dMaxBankRad;
 double m_dMaxAngularSpeedOfBankRadPerS;
 double m_dMaxAltitudeLossM;
 double m_dTimeFromGeneratorS;
 double m_dmx_wake, m_dvw_y_max, m_dvw_z_max;
 double m_dWindSpeedMS, m_dWindDirectionDegTo;
 unsigned long m_lPlayerPos;
 double m_dPlaneToWakeHorz, m_dPlaneToWakeVert;
public:
 CSOC();
};
struct WVSSAPI CWindServerOutputRecord
{
private:
protected:
public:
 CString m_strRequest;
 bool m_bSuccess;
 bool m_bHTTPRequestSuccess;
 DWORD m_dwRequestHTTPStatusCode;
 unsigned short m_nWeatherRecords;
public:
 CWindServerOutputRecord() { Init(); }
 void Init() { m_strRequest.Empty(); m_bSuccess =  m_bHTTPRequestSuccess = false; m_dwRequestHTTPStatusCode = 0; m_nWeatherRecords = 0; }
};
struct WVSSAPI rAtmosphereFlags 
{
	double eta; 
	double c1; 
	double c2; 
	double m_dStratification; 
};
struct WVSSAPI rWeatherFlags 
{
 char m_WeatherUndergroundID[32]; 
 char m_WeatherUndergroundHTTP[64]; 
 char m_WeatherUndergroundRequestMask[64]; 
};
class WVSSAPI CWEAIF
{
public:
 bool m_bCS;
 short m_nPR;
 bool m_bEF;
 double m_dIPWCD;
 double m_dWWT;
 bool m_bGE;
 short m_nFS;
 double m_dFT;
 bool m_bFAS;
 bool m_bCOF;
 double m_dWDRT;
 double m_dWS;
 bool m_bNWE;
 bool m_bOverrideProximityValues;
 bool m_bStartPlaneHOverride, m_bStartPlaneZOverride, m_bClosingAngleRadOverride; 
 double m_dStartPlaneHOverride, m_dStartPlaneZOverride, m_dCARdOverride;
 bool m_bOverrideGamma;
 double m_dWakeGammaOverrideValue;
 bool m_bDebugMessages;
 bool m_bRW;
 short m_nDefaultYear, m_nDefaultMonth, m_nDefaultDay;
 rAtmosphereFlags m_AFs;
 rWeatherFlags m_WFs;
 char m_GeoidName[16];
 char m_GeoidDir[MAX_PATH];
 char m_EDD[MAX_PATH];
 float m_fMG;
 bool m_bInitGeoid;
 bool m_bUSE;
 bool m_bUCOU;
protected:
 const char *TrueFalseStr(bool p_bVar);
public:
 CWEAIF() { Reset(); }
 ~CWEAIF() { }
 void WriteToVarJSON(char *p_pDest, unsigned long p_lDestSize);
 void Reset();
};
class WVSSAPI IWVSSSeverity
{
private:
 short m_nMajorVersion;
 short m_nMinorVersion;
 char m_BuildDate[32];
 char m_BuildTime[32];
protected:
public:
 char m_ThisDir[MAX_PATH];
 enum eFileType { FILETYPE_NONE, FILETYPE_ISA, FILETYPE_FDR, FILETYPE_WVSS_JSON, FILETYPE_MULTI, FILETYPE_MITRE, FILETYPE_ASAIC_JSON };
 std::vector<CSOC> m_FullSeverityOutput;
 CStringList m_ErrorList;
 std::vector<CWindServerOutputRecord> m_WindServerOutput;
 CWEAIF m_FSG;
 enum eObtainWeatherFlag { WVSS_OWF_NO, WVSS_OWF_YES, WVSS_OWF_DEFAULT }; 
private:
protected:
public:
 IWVSSSeverity() { m_nMajorVersion = 2; m_nMinorVersion = 0; strcpy_s(m_BuildDate, sizeof(m_BuildDate), __DATE__); strcpy_s(m_BuildTime, sizeof(m_BuildTime), __TIME__); }
 virtual ~IWVSSSeverity() { }
 static IWVSSSeverity *Create();
 virtual void GetSettings(const char *p_pINIFileName) = 0;
 static void Destroy(IWVSSSeverity *p_pObject);
 virtual bool Init() = 0;
 virtual bool LoadFile(LPCSTR p_pFileName, IWVSSSeverity::eFileType p_FileType, bool p_bFirst, bool p_bLast) = 0;
 virtual bool ProcessTrackBuffer(const char *p_pDataBuff, DWORD p_dwBuffSize, eFileType p_FileType) = 0;
 virtual void WriteToLog(const char *p_pFormat, ...) = 0;
 virtual void GetWeatherStationInfo(char *p_pStation, double *p_pdElevation) = 0;
 virtual std::map<CString, bool>GetMissingElevationDataAreas() = 0;
 short GetMajorVersion() { return(m_nMajorVersion); }
 short GetMinorVersion() { return(m_nMinorVersion); }
 const char *GetBuildDate() { return(m_BuildDate); }
 const char *GetBuildTime() { return(m_BuildTime); }
};
} 
