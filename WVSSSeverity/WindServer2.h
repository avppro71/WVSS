#pragma once
#include "WVSSInternetConnection.h"
#include <vector>
#include <map>
#define WVSS_BAD_WIND_VALUE -11111
namespace WVSS {
#define WVSS_WIND_FLAG_COORD     1L
#define WVSS_WIND_FLAG_TEXT      2L
#define WVSS_WIND_FLAG_DATE      4L
#define WVSS_WIND_FLAG_TIME      8L
#define WVSS_WIND_FLAG_DATETIME 12L
class CWindPoint3D
{
public:
 DWORD m_ldwFlags;
 double m_dLat, m_dLong;
 double m_dAltitude; 
 double m_dSurfaceElevation; 
 char m_LocationText[32];
 double m_dWindDirectionDegTo;
 double m_dWindSpeedMS;
 SYSTEMTIME m_MeasurementDT;
 LARGE_INTEGER m_MeasurementDTLI;
public:
 CWindPoint3D() { Init(); }
 virtual void Init();
};
class CWindServer2RequestCacheRecord
{
private:
protected:
public:
 CString m_strRequest;
 bool m_bSuccess;
 bool m_bHTTPRequestSuccess;
 DWORD m_dwRequestHTTPStatusCode;
 unsigned short m_nWeatherRecords;
private:
protected:
public:
 CWindServer2RequestCacheRecord() { Init(); }
 ~CWindServer2RequestCacheRecord() { }
 void Init() { m_strRequest.Empty(); m_bSuccess = m_bHTTPRequestSuccess = false; m_dwRequestHTTPStatusCode = 0;  m_nWeatherRecords = 0; };
};
class CWindServer2
{
private:
protected:
 CWVSSInternetSession *m_pInternetSession;
 CHttpConnection *m_pHTTP;
 CHttpFile *m_pHttpFile;
 char *m_pDataBuff; 
 DWORD m_dwDataBuffSize; 
 DWORD m_dwDataBuffLen; 
public:
 CStringList m_ErrorList;
 std::map<CString, CWindServer2RequestCacheRecord> m_RequestCache;
private:
protected:
 void Message(const char *p_pFormat, ...);
 virtual bool Process(CWindServer2RequestCacheRecord *p_pRequestRecord) = 0;
 bool ReallocateDataBuff(); 
 bool ReadIntoDataBuff(); 
public:
 CWindServer2();
 virtual ~CWindServer2();
 virtual bool Load(const char *p_pSite, const char *p_pRequest);
 void (* m_pfMessage)(CWindServer2 *p_pSender, const char *);
 virtual bool GetValue(CWindPoint3D *p_pWindPoint3D) = 0;
 static bool SystemTimeToLI(SYSTEMTIME *p_pST, LARGE_INTEGER *p_pllLI);
 static bool LIToSystemTime(LARGE_INTEGER *p_pllLI, SYSTEMTIME *p_pST);
 virtual DWORD GetValueCount() = 0;
 virtual const std::vector<CWindPoint3D> *GetWindPointVector() = 0;
};
} 
