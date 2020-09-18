#pragma once
#include "WindServer2.h"
#include "StringListExt.h"
#include <vector>
#include "IWVSSSeverity.h"
namespace WVSS {
class CWindServerWU : public CWindServer2
{
private:
 double m_dCurRequest_WeatherStationElevation; 
protected:
 CStringListExt m_strlCurInputDataPath; 
 CWindPoint3D m_CurWindPoint3D;
 std::vector<CWindPoint3D> m_WindPoints3D;
public:
 CWEAIF m_FSG; 
private:
 static int static_yajl_start_map(void *p_pContext);
 static int static_yajl_map_key(void *p_pContext, const unsigned char *p_pKey, size_t p_StringLen);
 static int static_yajl_end_map(void *p_pContext);
 static int static_yajl_string(void *p_pContext, const unsigned char *p_pStringVal, size_t p_StringLen);
 static int static_yajl_start_array(void *p_pContext);
 static int static_yajl_end_array(void *p_pContext);
 int yajl_start_map();
 int yajl_map_key(const unsigned char *p_pKey, size_t p_StringLen);
 int yajl_string(const unsigned char *p_pStringVal, size_t p_StringLen);
 int yajl_end_map();
 int yajl_start_array();
 int yajl_end_array();
 bool TimeFromString(const char *p_pStr, SYSTEMTIME *p_pDest, bool p_bDatePresent);
protected:
 bool ProcessResponseBuffer(const char *p_pDataBuff, DWORD p_dwBuffSize);
 virtual bool Process(CWindServer2RequestCacheRecord *p_pRequestRecord);
public:
 CWindServerWU();
 virtual ~CWindServerWU();
 virtual bool GetValue(CWindPoint3D *p_pWindPoint3D);
 bool MakeRequest(const char *p_pAirportCode, SYSTEMTIME *p_pST, double p_dWeatherStationElevation);
 DWORD GetValueCount() { return(m_WindPoints3D.size()); }
 virtual const std::vector<CWindPoint3D> *GetWindPointVector() { return(&m_WindPoints3D); }
};
} 
