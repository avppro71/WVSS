#include "stdafx.h"
#include "WindServerWU.h"
#include "yajl/yajl_tree.h"
#include "yajl/yajl_parse.h"
#include "Wildcard.h"
using namespace WVSS;
CWindServerWU::CWindServerWU()
{
 m_pDataBuff = NULL;
}
CWindServerWU::~CWindServerWU()
{
 if(!m_pDataBuff) delete [] m_pDataBuff;
}
bool CWindServerWU::Process(CWindServer2RequestCacheRecord *p_pRequestRecord)
{
 bool bVal;
 size_t WindPointCount = m_WindPoints3D.size();
 if(!ReadIntoDataBuff()) {
   Message("CWindServerWU::Process: Could not read into the data buffer");
   return(false); }
 bVal = ProcessResponseBuffer(m_pDataBuff, m_dwDataBuffLen);
 p_pRequestRecord->m_nWeatherRecords = (unsigned short)(m_WindPoints3D.size() - WindPointCount);
 return(bVal);
}
bool CWindServerWU::ProcessResponseBuffer(const char *p_pDataBuff, DWORD p_dwBuffSize)
{
 yajl_handle YAJLHandle;
 yajl_callbacks YAJLCallbacks;
 yajl_status YAJLStatus;
 if(!p_pDataBuff) {
   Message("CWindServerWU: Internal Error 53");
   return(false); }
 YAJLCallbacks.yajl_null = NULL;
 YAJLCallbacks.yajl_boolean = NULL;
 YAJLCallbacks.yajl_integer = NULL;
 YAJLCallbacks.yajl_double = NULL;
 YAJLCallbacks.yajl_number = NULL;
 YAJLCallbacks.yajl_string = static_yajl_string;
 YAJLCallbacks.yajl_start_map = static_yajl_start_map;
 YAJLCallbacks.yajl_map_key = static_yajl_map_key;
 YAJLCallbacks.yajl_end_map = static_yajl_end_map;
 YAJLCallbacks.yajl_start_array = static_yajl_start_array;
 YAJLCallbacks.yajl_end_array = static_yajl_end_array;
 YAJLHandle = yajl_alloc(&YAJLCallbacks, NULL, this);
 YAJLStatus = yajl_parse(YAJLHandle, (const unsigned char *)p_pDataBuff, p_dwBuffSize); 
 yajl_free(YAJLHandle);
 GetValue(&m_CurWindPoint3D);
 return(true);
}
int CWindServerWU::static_yajl_start_map(void *p_pContext)
{
 CWindServerWU *pWindServerWU = reinterpret_cast<CWindServerWU *>(p_pContext);
 if(!pWindServerWU) return(0);
 return pWindServerWU->yajl_start_map();
}
int CWindServerWU::static_yajl_map_key(void *p_pContext, const unsigned char *p_pKey, size_t p_StringLen)
{
 CWindServerWU *pWindServerWU = reinterpret_cast<CWindServerWU *>(p_pContext);
 if(!pWindServerWU) return(0);
 return pWindServerWU->yajl_map_key(p_pKey, p_StringLen);
}
int CWindServerWU::static_yajl_end_map(void *p_pContext)
{
 CWindServerWU *pWindServerWU = reinterpret_cast<CWindServerWU *>(p_pContext);
 if(!pWindServerWU) return(0);
 return pWindServerWU->yajl_end_map();
}
int CWindServerWU::static_yajl_string(void *p_pContext, const unsigned char *p_pStringVal, size_t p_StringLen)
{
 CWindServerWU *pWindServerWU = reinterpret_cast<CWindServerWU *>(p_pContext);
 if(!pWindServerWU) return(0);
 return pWindServerWU->yajl_string(p_pStringVal, p_StringLen);
}
int CWindServerWU::static_yajl_start_array(void *p_pContext)
{
 CWindServerWU *pWindServerWU = reinterpret_cast<CWindServerWU *>(p_pContext);
 if(!pWindServerWU) return(0);
 return pWindServerWU->yajl_start_array();
}
int CWindServerWU::static_yajl_end_array(void *p_pContext)
{
 CWindServerWU *pWindServerWU = reinterpret_cast<CWindServerWU *>(p_pContext);
 if(!pWindServerWU) return(0);
 return pWindServerWU->yajl_end_array();
}
int CWindServerWU::yajl_map_key(const unsigned char *p_pKey, size_t p_StringLen)
{
 char tstr[128];
 CString Str;
 Str = m_strlCurInputDataPath.GetTail();
 if(Str != "{") 
   m_strlCurInputDataPath.RemoveTail(); 
 memset(tstr, 0, sizeof(tstr));
 strncpy_s(reinterpret_cast<char *>(tstr), _countof(tstr), reinterpret_cast<const char *>(p_pKey), p_StringLen);
 m_strlCurInputDataPath.AddTail(tstr);
 return(1);
}
int CWindServerWU::yajl_start_map()
{
 m_strlCurInputDataPath.AddTail("{");
 return(1);
}
int CWindServerWU::yajl_end_map()
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
 if(CWildcard::Match(m_strlCurInputDataPath.GetPath(), "{/History/{/Observations/[", TRUE)) {
   SystemTimeToLI(&m_CurWindPoint3D.m_MeasurementDT, &m_CurWindPoint3D.m_MeasurementDTLI);
   m_WindPoints3D.push_back(m_CurWindPoint3D);
   Message("Track point added");
   }
 Str = m_strlCurInputDataPath.GetTail();
 if(Str != "[") 
   m_strlCurInputDataPath.RemoveTail();
 return(1);
}
int CWindServerWU::yajl_start_array()
{
 m_strlCurInputDataPath.AddTail("[");
 return(1);
}
int CWindServerWU::yajl_end_array()
{
 CString Str;
 Str = m_strlCurInputDataPath.GetTail();
 ASSERT(Str == "["); 
 m_strlCurInputDataPath.RemoveTail(); 
 Str = m_strlCurInputDataPath.GetTail();
 ASSERT(Str != "["); 
 m_strlCurInputDataPath.RemoveTail(); 
 return(1);
}
int CWindServerWU::yajl_string(const unsigned char *p_pStringVal, size_t p_StringLen)
{
 char tstr[32], *pWork;
 if(CWildcard::Match(m_strlCurInputDataPath.GetPath(), "{/History/{/Observations/[/{/utcdate/{/year", TRUE)) {
   m_CurWindPoint3D.Init();
   m_CurWindPoint3D.m_MeasurementDT.wYear = (WORD)atoi((const char *)p_pStringVal);
   }
 else if(CWildcard::Match(m_strlCurInputDataPath.GetPath(), "{/History/{/Observations/[/{/utcdate/{/mon", TRUE)) {
   m_CurWindPoint3D.m_MeasurementDT.wMonth = (WORD)atoi((const char *)p_pStringVal);
   }
 else if(CWildcard::Match(m_strlCurInputDataPath.GetPath(), "{/History/{/Observations/[/{/utcdate/{/mday", TRUE)) {
   m_CurWindPoint3D.m_MeasurementDT.wDay = (WORD)atoi((const char *)p_pStringVal);
   m_CurWindPoint3D.m_ldwFlags |= WVSS_WIND_FLAG_DATE;
   }
 else if(CWildcard::Match(m_strlCurInputDataPath.GetPath(), "{/History/{/Observations/[/{/utcdate/{/hour", TRUE)) {
   m_CurWindPoint3D.m_MeasurementDT.wHour = (WORD)atoi((const char *)p_pStringVal);
   }
 else if(CWildcard::Match(m_strlCurInputDataPath.GetPath(), "{/History/{/Observations/[/{/utcdate/{/min", TRUE)) {
   m_CurWindPoint3D.m_MeasurementDT.wMinute = (WORD)atoi((const char *)p_pStringVal);
   m_CurWindPoint3D.m_ldwFlags |= WVSS_WIND_FLAG_TIME;
   }
 else if(CWildcard::Match(m_strlCurInputDataPath.GetPath(), "{/History/{/Observations/[/{/wspdm", TRUE)) {
   m_CurWindPoint3D.m_dWindSpeedMS = atof((const char *)p_pStringVal) / 3.6;
   m_CurWindPoint3D.m_dSurfaceElevation = m_dCurRequest_WeatherStationElevation;
   }
 else if(CWildcard::Match(m_strlCurInputDataPath.GetPath(), "{/History/{/Observations/[/{/wdird", TRUE)) {
   m_CurWindPoint3D.m_dWindDirectionDegTo = atof((const char *)p_pStringVal);
   m_CurWindPoint3D.m_dWindDirectionDegTo -= 180;
   if(m_CurWindPoint3D.m_dWindDirectionDegTo < 0)
     m_CurWindPoint3D.m_dWindDirectionDegTo += 360;
   }
 else if(CWildcard::Match(m_strlCurInputDataPath.GetPath(), "{/History/{/Observations/[/{/metar", TRUE)) {
   memset(tstr, 0, sizeof(tstr));
   strncpy_s(tstr, _countof(tstr), (const char *)p_pStringVal, min(sizeof(tstr) - 1, p_StringLen));
   pWork = strchr(tstr + 6, ' ');
   if(pWork) *pWork = '\0';
   strcpy_s(m_CurWindPoint3D.m_LocationText, _countof(m_CurWindPoint3D.m_LocationText), tstr + 6);
   m_CurWindPoint3D.m_ldwFlags |= WVSS_WIND_FLAG_TEXT;
   }
 return(1);
}
bool CWindServerWU::GetValue(CWindPoint3D *p_pWindPoint3D)
{
 std::vector<CWindPoint3D>::iterator It, It2;
 LONGLONG llCurTimeDiff = (LONGLONG)10000000 * 60 * 60 * 24 * 365; 
 LONGLONG llVal;
 if(!p_pWindPoint3D->m_MeasurementDTLI.QuadPart) 
   SystemTimeToLI(&p_pWindPoint3D->m_MeasurementDT, &p_pWindPoint3D->m_MeasurementDTLI);
 if(!(p_pWindPoint3D->m_ldwFlags & WVSS_WIND_FLAG_DATETIME) || !(p_pWindPoint3D->m_ldwFlags & WVSS_WIND_FLAG_TEXT))
   return(false); 
 It2 = m_WindPoints3D.end();
 for(It = m_WindPoints3D.begin(); It != m_WindPoints3D.end(); ++It) {
   if(!(It->m_ldwFlags & WVSS_WIND_FLAG_DATETIME) || !(It->m_ldwFlags & WVSS_WIND_FLAG_TEXT))
     continue;
   if(_stricmp(It->m_LocationText, p_pWindPoint3D->m_LocationText))
     continue; 
   llVal = It->m_MeasurementDTLI.QuadPart - p_pWindPoint3D->m_MeasurementDTLI.QuadPart;
   if(llVal < 0) llVal *= -1;
   if(llVal < llCurTimeDiff) {
     llCurTimeDiff = llVal;
     It2 = It; }
   }
 if(It2 == m_WindPoints3D.end()) 
   return(false);
 if(llCurTimeDiff >= (LONGLONG)10000000 * 60 * 60)
   return(false);
 *p_pWindPoint3D = *It2;
 return(true);
}
bool CWindServerWU::MakeRequest(const char *p_pAirportCode, SYSTEMTIME *p_pST, double p_dWeatherStationElevation)
{
 char DateStr[16], tstr[128];
 bool bVal;
 LARGE_INTEGER llVal;
 SYSTEMTIME ST;
 if(!p_pAirportCode || !p_pST) return(false);
 SystemTimeToLI(p_pST, &llVal);
 LIToSystemTime(&llVal, &ST);
 m_dCurRequest_WeatherStationElevation = p_dWeatherStationElevation;
 sprintf_s(DateStr, _countof(DateStr), "%.4d%.2d%.2d", p_pST->wYear, p_pST->wMonth, p_pST->wDay);
 sprintf_s(tstr, _countof(tstr), m_FSG.m_WFs.m_WeatherUndergroundRequestMask, m_FSG.m_WFs.m_WeatherUndergroundID, DateStr, p_pAirportCode);
 bVal = Load(m_FSG.m_WFs.m_WeatherUndergroundHTTP, tstr);
 if(!bVal) return(false);
 llVal.QuadPart -= (LONGLONG)10000000 * 60 * 60 * 24; 
 LIToSystemTime(&llVal, &ST);
 sprintf_s(DateStr, _countof(DateStr), "%.4d%.2d%.2d", ST.wYear, ST.wMonth, ST.wDay);
 sprintf_s(tstr, _countof(tstr), m_FSG.m_WFs.m_WeatherUndergroundRequestMask, m_FSG.m_WFs.m_WeatherUndergroundID, DateStr, p_pAirportCode);
 bVal = Load(m_FSG.m_WFs.m_WeatherUndergroundHTTP, tstr);
 if(!bVal) return(false);
 return(true);
}
