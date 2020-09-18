#pragma once
#include "interpolation.h"
#define AVP_BAD_WIND_VALUE -11111
#define GEOGRAPHICLIB_SHARED_LIB 1
namespace GeographicLib {
 class Geodesic;
}
class C2D
{
public:
 double dLat, dLong;
 double m_dWDRT;
 double m_dWindSpeedMS;
public:
 C2D() { m_dWDRT = AVP_BAD_WIND_VALUE; m_dWindSpeedMS = AVP_BAD_WIND_VALUE; }
};
class CWindServer
{
private:
 bool m_bReady;
 char m_LastLoadFileName[256];
public:
 C2D *m_p2dArray;
 unsigned long m_l2dArraySize;
 GeographicLib::Geodesic *m_pGeo;
public:
 CWindServer();
 ~CWindServer();
 bool Set2dArraySize(unsigned long p_lSize);
 void SetValue(unsigned long p_lIndex, C2D *p_p2D) { m_p2dArray[p_lIndex] = *p_p2D; }
 bool SetDone();
 bool GetValue(C2D *p_p2D);
 bool GetValueExact(unsigned long p_lIndex, C2D *p_p2D);
 bool SaveToFile(const char *p_pFileName = NULL);
 bool LoadFromFile(const char *p_pFileName);
 bool IsReady() { return(m_bReady); }
 const char *GetLastLoadFileName() { return(m_LastLoadFileName); }
};
