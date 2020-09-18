#include "stdafx.h"
#include "ObjLoadLineParser.h"
#include "global.h"
#include "Severity.h"
#pragma warning(disable : 4996)
using namespace WVSS;
#define M_PI   3.1415926535897932384626433832795
CWVSSLoaderLineParserMITRE::CWVSSLoaderLineParserMITRE()
{
 m_pTrackPoints = NULL;
 m_dwLinesProcessed = 0;
 m_pSeverity = NULL;
}
CWVSSLoaderLineParserMITRE::~CWVSSLoaderLineParserMITRE()
{
}
bool CWVSSLoaderLineParserMITRE::OnBeforeFileOpen(const char *p_pFileName)
{
 return(true);
}
bool CWVSSLoaderLineParserMITRE::OnBeforeLoad(FILE *p_pFile)
{
 if(m_pTrackPoints)
   m_pTrackPoints->clear();
 m_dwLinesProcessed = 0;
 return(true);
}
bool CWVSSLoaderLineParserMITRE::OnLoaded()
{
 return(true);
}
bool CWVSSLoaderLineParserMITRE::ProcessLine(char *p_pLine)
{
 char tstr[128];
 char ParsedLine[8][32];
 time_t Timer;
 tm *pTM;
 double dWindU, dWindV;
 if(*p_pLine == '#')
   return(true);
 m_TP.Init();
 if(!ParseToStrList(p_pLine, (char *)ParsedLine, 8, 32, ";\t, \r\n"))
   return(false);
 strcpy_s(m_TP.m_CallSign, sizeof(m_TP.m_CallSign), ParsedLine[0]);
 strcpy_s(m_TP.m_ACModel, sizeof(m_TP.m_ACModel), ParsedLine[1]);
 Timer = atol(ParsedLine[2]);
 pTM = gmtime(&Timer);
 m_TP.m_CreationTime.wYear = pTM->tm_year + 1900;
 m_TP.m_CreationTime.wMonth = pTM->tm_mon + 1;
 m_TP.m_CreationTime.wDay = pTM->tm_mday;
 m_TP.m_CreationTime.wHour = pTM->tm_hour;
 m_TP.m_CreationTime.wMinute = pTM->tm_min;
 m_TP.m_CreationTime.wSecond = pTM->tm_sec;
 m_TP.m_CreationTime.wMilliseconds = 0;
 m_TP.m_CreationTimeOrig = m_TP.m_CreationTime;
 if(!SystemTimeToLI(&m_TP.m_CreationTime, &m_TP.m_CTLE)) {
    sprintf(tstr, "Time value %s is invalid", ParsedLine[2]);
    if(m_pSeverity)
      m_pSeverity->AddToErrorList(tstr);
    return(false); }
 strcpy_s(m_TP.m_DepartureAirport, sizeof(m_TP.m_DepartureAirport), "ADEP");
 strcpy_s(m_TP.m_ArrivalAirport, sizeof(m_TP.m_ArrivalAirport), "ADES");
 m_TP.m_dLatDeg = atof(ParsedLine[3]);
 m_TP.m_dLongDeg = atof(ParsedLine[4]);
 m_TP.m_dAltitude100Ft = atof(ParsedLine[5]) / 100;
 dWindU = atof(ParsedLine[6]);
 dWindV = atof(ParsedLine[7]);
 m_TP.m_dWindSpeedMS = sqrt(dWindU * dWindU + dWindV * dWindV);
 m_TP.m_dWDRT = M_PI / 2 - atan2(dWindV, dWindU);
 if(m_TP.m_dWDRT < - M_PI) m_TP.m_dWDRT += 2 * M_PI;
 if(m_TP.m_dWDRT > M_PI) m_TP.m_dWDRT -= 2 * M_PI;
 if(m_pTrackPoints)
   m_pTrackPoints->push_back(m_TP);
 m_dwLinesProcessed ++;
 return(true);
}
