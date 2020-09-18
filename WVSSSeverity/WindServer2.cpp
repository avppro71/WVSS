#include "stdafx.h"
#include "WindServer2.h"
#include <utility>
#pragma once
using namespace WVSS;
void CWindPoint3D::Init()
{
 m_ldwFlags = 0;
 memset(&m_MeasurementDT, 0, sizeof(m_MeasurementDT));
 m_MeasurementDTLI.QuadPart = 0;
 m_dLat = 0;
 m_dLong = 0;
 m_dSurfaceElevation = 0;
 m_dAltitude = 10;
 memset(m_LocationText, 0, sizeof(m_LocationText));
 m_dWindDirectionDegTo = WVSS_BAD_WIND_VALUE;
 m_dWindSpeedMS = WVSS_BAD_WIND_VALUE;
}
CWindServer2::CWindServer2()
{
 m_pfMessage = NULL;
 m_pInternetSession = NULL;
 m_pHTTP = NULL;
 m_pHttpFile = NULL;
 m_ErrorList.RemoveAll();
 m_RequestCache.clear(); 
 m_pDataBuff = NULL; 
 m_dwDataBuffSize = 0;
 m_dwDataBuffLen = 0;
}
CWindServer2::~CWindServer2()
{
 if(m_pHttpFile) { ASSERT(false); delete m_pHttpFile; }
 if(m_pHTTP) { ASSERT(false); delete m_pHTTP; }
 if(m_pInternetSession) { m_pInternetSession->Close(); delete m_pInternetSession; }
 if(m_pDataBuff) delete [] m_pDataBuff;
}
void CWindServer2::Message(const char *p_pFormat, ...)
{
 char tstr[256];
 va_list ArgList;
 if(!p_pFormat) return;
 va_start(ArgList, p_pFormat);
 vsprintf_s(tstr, _countof(tstr), p_pFormat, ArgList);
 va_end(ArgList);
 if(m_pfMessage) 
   m_pfMessage(this, tstr);
}
bool CWindServer2::Load(const char *p_pSite, const char *p_pRequest)
{
 INTERNET_PORT Port;
 char tstr[4096];
 DWORD dwVal;
 bool ProcOK = false;
 std::map<CString, CWindServer2RequestCacheRecord>::iterator CachePos;
 CWindServer2RequestCacheRecord RequestRecord;
 Port = (INTERNET_PORT)80;
 RequestRecord.m_strRequest = CString(p_pSite) + "/" + p_pRequest;
 if(m_RequestCache.find(RequestRecord.m_strRequest) != m_RequestCache.end()) {
   return(true); } 
 try {
   for(;;) {
     if(!m_pInternetSession) {
       m_pInternetSession = new CWVSSInternetSession("WVSS Wind Server 2", 1 , PRE_CONFIG_INTERNET_ACCESS, NULL, NULL, 0);
       if(!m_pInternetSession) {
         Message("No memory for an CWVSSInternetSession object");
         break; }
       m_pInternetSession->EnableStatusCallback(); }
     m_pHTTP = m_pInternetSession->GetHttpConnection(p_pSite, Port, NULL, NULL);
     if(!m_pHTTP) {
       Message("GetHttpConnection failed on %d[%d]", p_pSite, Port);
       break; }
     m_pHttpFile = m_pHTTP->OpenRequest(CHttpConnection::HTTP_VERB_GET, p_pRequest); 
     if(!m_pHttpFile) {
       Message("OpenRequest returned NULL");
       break; }
     if(!m_pHttpFile->SendRequest()) {
       ASSERT(0); 
       Message("SendRequest for %s returned FALSE", p_pRequest);
       break; }
     RequestRecord.m_bHTTPRequestSuccess = true;
     dwVal = sizeof(tstr);
     if(!m_pHttpFile->QueryInfoStatusCode(RequestRecord.m_dwRequestHTTPStatusCode)) {
       sprintf_s(tstr, _countof(tstr), "QueryInfoStatusCode failed with %ld: ", GetLastError());
       FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM, 0, GetLastError(), 0, tstr + strlen(tstr), sizeof(tstr) - strlen(tstr), 0);
       Message(tstr);
       break; }
     if(RequestRecord.m_dwRequestHTTPStatusCode != HTTP_STATUS_OK) {
       Message("Request was not successful. HTTP code: %ld", RequestRecord.m_dwRequestHTTPStatusCode);
       break; }
     if(!Process(&RequestRecord)) {
       Message("Request failed WVSS processing.");
       break; }
     RequestRecord.m_bSuccess = true;
     m_RequestCache.insert(std::pair<CString, CWindServer2RequestCacheRecord>(RequestRecord.m_strRequest, RequestRecord));
     ProcOK = true;
     break;
     }
   }
 catch(CInternetException *pException) {
   Message("CInternetException thrown with context ID %ld and error code %ld", pException->m_dwContext, pException->m_dwError);
   }
 if(m_pHttpFile) {
   m_pHttpFile->Close();
   delete m_pHttpFile;
   m_pHttpFile = NULL; }
 if(m_pHTTP) {
   m_pHTTP->Close();
   delete m_pHTTP;
   m_pHTTP = NULL; }
 return(ProcOK);
}
bool CWindServer2::SystemTimeToLI(SYSTEMTIME *p_pST, LARGE_INTEGER *p_pllLI)
{
 FILETIME FT;
 if(!SystemTimeToFileTime(p_pST, &FT))
   return(false);
 p_pllLI->HighPart = FT.dwHighDateTime;
 p_pllLI->LowPart = FT.dwLowDateTime;
 return(true);
}
bool CWindServer2::LIToSystemTime(LARGE_INTEGER *p_pllLI, SYSTEMTIME *p_pST)
{
 FILETIME FT;
 FT.dwHighDateTime = p_pllLI->HighPart;
 FT.dwLowDateTime = p_pllLI->LowPart;
 if(!FileTimeToSystemTime(&FT, p_pST))
   return(false);
 return(true);
}
bool CWindServer2::ReallocateDataBuff()
{
 char *pNewDataBuff;
 DWORD dwNewDataBuffSize;
 dwNewDataBuffSize = m_dwDataBuffSize + 10240;
 pNewDataBuff = new char[dwNewDataBuffSize];
 if(!pNewDataBuff) return(false);
 memset(pNewDataBuff, 0, dwNewDataBuffSize);
 if(m_dwDataBuffLen) {
   ASSERT(m_pDataBuff);
   memcpy(pNewDataBuff, m_pDataBuff, m_dwDataBuffLen); }
 if(m_pDataBuff) delete [] m_pDataBuff;
 m_pDataBuff = pNewDataBuff;
 m_dwDataBuffSize = dwNewDataBuffSize;
 return(true);
}
bool CWindServer2::ReadIntoDataBuff()
{
 UINT uiRead, uiToRead;
 if(!m_pHttpFile) {
   Message("CWindServerWU: Internal Error 192");
   return(false); }
 try { 
   m_dwDataBuffLen = 0;
   if(!m_dwDataBuffSize)
     ReallocateDataBuff();
   for(;;) {
     uiToRead = m_dwDataBuffSize - 1 - m_dwDataBuffLen;
     uiRead = m_pHttpFile->Read(m_pDataBuff + m_dwDataBuffLen, uiToRead);
     m_dwDataBuffLen += uiRead;
     if(!uiRead) break;
     if(uiToRead - uiRead < 1024) 
       ReallocateDataBuff(); 
     }
   } 
 catch(CInternetException *pException) {
   Message("CWindServerWU, CHttpFile->Read(): CInternetException thrown with context ID %ld and error code %ld", pException->m_dwContext, pException->m_dwError);
   }
 *(m_pDataBuff + m_dwDataBuffLen) = '\0';
 return(true);
}
