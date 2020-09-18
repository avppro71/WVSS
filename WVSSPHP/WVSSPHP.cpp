#include "stdafx.h"
#include <math.h>
#include <wchar.h>
#include "IWVSSSeverity.h"
#include "php.h"
#include "zend_config.w32.h"
#include "Global.h"
#pragma warning(disable : 4996)
#define WVSS_PHP_EXTNAME "WVSSPHP"
#define WVSS_PHP_VERSION "2.02 Date:" __DATE__ " Time: " __TIME__
bool ReallocateWriteBuffer(char **p_ppWriteBuff, DWORD *p_pdwWriteBuffLen);
char *WriteToOuput(char **p_ppWriteBuff, DWORD *p_pdwWriteBuffLen, char *pWork, char *pStr);
bool WriteJSON(char **p_ppWriteBuff, DWORD p_dwWriteBuffLen, bool p_bLoadSuccess, WVSS::IWVSSSeverity *p_pSeverity);
class CTwoPlaneKeyValue
{
public:
 struct rTwoPlaneKey {
   char m_Generator[32];
   char m_Follower[32];
   bool rTwoPlaneKey::operator < (const rTwoPlaneKey &p_Other) const;
   };
 struct rTwoPlaneValues {
   SYSTEMTIME m_EntryTime;
   double m_dVal; };
 rTwoPlaneKey m_Key;
 rTwoPlaneValues m_Values;
public:
 CTwoPlaneKeyValue() { memset(this, 0, sizeof(CTwoPlaneKeyValue)); }
};
bool CTwoPlaneKeyValue::rTwoPlaneKey::operator < (const CTwoPlaneKeyValue::rTwoPlaneKey &p_Other) const
{
 int ret;
 ret = strcmp(m_Follower, p_Other.m_Follower);
 if(ret) return(ret < 0);
 ret = strcmp(m_Generator, p_Other.m_Generator);
 return(ret < 0);
}
void WVSS_CalcSeverityCommon(INTERNAL_FUNCTION_PARAMETERS, WVSS::IWVSSSeverity::eFileType p_FileType);
PHP_FUNCTION(WVSS_CalcSeverityText);
PHP_FUNCTION(WVSS_CalcSeverityWVSSJSON);
PHP_FUNCTION(WVSS_CalcSeverityASAICJSON);
zend_function_entry wvssphp_functions[] = {
 PHP_FE(WVSS_CalcSeverityText, NULL)
 PHP_FE(WVSS_CalcSeverityWVSSJSON, NULL)
 PHP_FE(WVSS_CalcSeverityASAICJSON, NULL)
 {NULL, NULL, NULL }
};
zend_module_entry wvssphp_module_entry = {
 #if ZEND_MODULE_API_NO >= 20010901
   STANDARD_MODULE_HEADER,
 #endif
 WVSS_PHP_EXTNAME,
 wvssphp_functions,
 NULL,
 NULL,
 NULL,
 NULL,
 NULL,
 #if ZEND_MODULE_API_NO >= 20010901
   WVSS_PHP_VERSION,
 #endif
 STANDARD_MODULE_PROPERTIES
};
ZEND_GET_MODULE(wvssphp)
using namespace WVSS;
PHP_FUNCTION(WVSS_CalcSeverityText) { WVSS_CalcSeverityCommon(INTERNAL_FUNCTION_PARAM_PASSTHRU, IWVSSSeverity::FILETYPE_ISA); }
PHP_FUNCTION(WVSS_CalcSeverityWVSSJSON) { WVSS_CalcSeverityCommon(INTERNAL_FUNCTION_PARAM_PASSTHRU, IWVSSSeverity::FILETYPE_WVSS_JSON); }
PHP_FUNCTION(WVSS_CalcSeverityASAICJSON) { WVSS_CalcSeverityCommon(INTERNAL_FUNCTION_PARAM_PASSTHRU, IWVSSSeverity::FILETYPE_ASAIC_JSON); }
void WVSS_CalcSeverityCommon(INTERNAL_FUNCTION_PARAMETERS, WVSS::IWVSSSeverity::eFileType p_FileType)
{
 const char *pEndStr = "<br>\n";
 char *pWork, *pReadBuff, *pWriteBuff, FileName[256];
 char tstr[512];
 DWORD dwReadBuffLen, dwWriteBuffLen, dwTime;
 int Len;
 IWVSSSeverity *pSeverity;
 FILE *fFile;
 bool bVal, bProcOK;
 bool bOutputFlags = false;
 bool bDeleteFile = true;
 std::vector<CSOC>::iterator It;
 std::map<CTwoPlaneKeyValue::rTwoPlaneKey, CTwoPlaneKeyValue::rTwoPlaneValues>ClosestPlaneToWakeMap;
 std::map<CTwoPlaneKeyValue::rTwoPlaneKey, CTwoPlaneKeyValue::rTwoPlaneValues>::iterator ClosestPlaneToWakeMapIt;
 CTwoPlaneKeyValue TwoPlaneKeyValue;
 std::map<CString, bool>CombinedMissingAreasList;
 std::map<CString, bool>::const_iterator CombinedMissingAreasListIt;
 bProcOK = false;
 pReadBuff = NULL;
 pWriteBuff = NULL;
 fFile = NULL;
 pSeverity = NULL;
 for(;;) {
   if(zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &pWork, &Len) == FAILURE) {
     sprintf_s(tstr, sizeof(tstr), "{\"WVSSSuccessFlag\":\"0\", \"WVSSError\":\"HTTP parameter parse failure\"}%s", pEndStr);
     break; }
   dwReadBuffLen = Len;
   pReadBuff = new char [dwReadBuffLen + 1];
   if(!pReadBuff) {
     sprintf_s(tstr, sizeof(tstr), "{\"WVSSSuccessFlag\":\"0\", \"WVSSError\":\"No memory, needed %ld\"}%s", dwReadBuffLen, pEndStr);
     break; }
   memset(pReadBuff, 0, dwReadBuffLen + 1);
   dwWriteBuffLen = 64 * 1024;
   pWriteBuff = new char [dwWriteBuffLen + 1];
   if(!pWriteBuff) {
     sprintf_s(tstr, sizeof(tstr), "{\"WVSSSuccessFlag\":\"0\", \"WVSSError\":\"No memory, needed %ld\"}%s", dwWriteBuffLen, pEndStr);
     break; }
   memset(pWriteBuff, 0, dwWriteBuffLen + 1);
   memcpy(pReadBuff, pWork, Len);
   strrplInPlace(pReadBuff, "&quot;", "\"");
   Len = strlen(pReadBuff);
   GetTempPath(sizeof(tstr), tstr);
   GetTempFileName(tstr, "WVS", 0, FileName);
   fFile = fopen(FileName, "wb");
   if(!fFile) {
     sprintf_s(pReadBuff, dwReadBuffLen, "{\"WVSSSuccessFlag\":\"0\", \"WVSSError\":\"temp file creation failed\"}%s", pEndStr);
     break; }
   if(fwrite(pReadBuff, 1, Len, fFile) != Len) {
     sprintf_s(tstr, sizeof(tstr), "{\"WVSSSuccessFlag\":\"0\", \"WVSSError\":\"temp file write failed\"}%s", pEndStr);
     break; }
   pSeverity = IWVSSSeverity::Create();
   if(!pSeverity) {
     sprintf_s(tstr, sizeof(tstr), "{\"WVSSSuccessFlag\":\"0\", \"WVSSError\":\"Severity module unavailable\"}%s", pEndStr);
     break; }
   bProcOK = true;
   break; }
 if(fFile) fclose(fFile);
 if(!bProcOK) {
   if(pReadBuff) delete [] pReadBuff;
   if(pWriteBuff) delete [] pWriteBuff;
   if(pSeverity)
     IWVSSSeverity::Destroy(pSeverity);
   RETVAL_STRING(tstr, true);
   return; }
 pWork = pWriteBuff;
 dwTime = GetTickCount();
 pSeverity->WriteToLog("Processing %s of type %d", FileName, (int)p_FileType);
 bVal = pSeverity->LoadFile(FileName, p_FileType, true, true);
 pSeverity->WriteToLog("Done Processing in %.1lf", (double)(GetTickCount() - dwTime) / 1000);
 WriteJSON(&pWriteBuff, dwWriteBuffLen, bVal, pSeverity);
 if(bDeleteFile)
   DeleteFile(FileName);
 IWVSSSeverity::Destroy(pSeverity);
 RETVAL_STRING(pWriteBuff, true);
 if(pReadBuff) delete[] pReadBuff; 
 if(pWriteBuff) delete[] pWriteBuff; 
}
bool ReallocateWriteBuffer(char **p_ppWriteBuff, DWORD *p_pdwWriteBuffLen)
{
 DWORD dwAdd = (64 * 1024);
 char *pBuff = new char[(*p_pdwWriteBuffLen) + dwAdd];
 if(!pBuff) return(false);
 memset(pBuff, 0, (*p_pdwWriteBuffLen) + dwAdd);
 if(*p_ppWriteBuff) {
   memcpy(pBuff, *p_ppWriteBuff, *p_pdwWriteBuffLen);
   delete [] (*p_ppWriteBuff); }
 *p_ppWriteBuff = pBuff;
 *p_pdwWriteBuffLen += dwAdd;
 return(true);
}
char *WriteToOuput(char **p_ppWriteBuff, DWORD *p_pdwWriteBuffLen, char *pWork, char *pStr)
{
 size_t Len, nWorkOffset;
 if(!pStr || !p_ppWriteBuff || !p_pdwWriteBuffLen || !pWork) return(NULL);
 Len = strlen(pStr);
 nWorkOffset = pWork - *p_ppWriteBuff;
 while(nWorkOffset + Len > *p_pdwWriteBuffLen) {
   if(!ReallocateWriteBuffer(p_ppWriteBuff, p_pdwWriteBuffLen))
     return(NULL);
   pWork = *p_ppWriteBuff + nWorkOffset;
   }
 strcpy(pWork, pStr);
 pWork += Len;
 return(pWork);
}
bool WriteJSON(char **p_ppWriteBuff, DWORD p_dwWriteBuffLen, bool p_bLoadSuccess, WVSS::IWVSSSeverity *p_pSeverity)
{
 const char *pEndStr = "<br>\n";
 char *pWork, tstr[512], tstr2[256];
 DWORD dwWriteBuffLen, dwCount;
 double dVal;
 POSITION Pos;
 std::vector<CSOC>::iterator It;
 int i;
 std::map<CTwoPlaneKeyValue::rTwoPlaneKey, CTwoPlaneKeyValue::rTwoPlaneValues>ClosestPlaneToWakeMap;
 std::map<CTwoPlaneKeyValue::rTwoPlaneKey, CTwoPlaneKeyValue::rTwoPlaneValues>::iterator ClosestPlaneToWakeMapIt;
 CTwoPlaneKeyValue TwoPlaneKeyValue;
 const CTwoPlaneKeyValue::rTwoPlaneKey *pTwoPlaneKey;
 const CTwoPlaneKeyValue::rTwoPlaneValues *pTwoPlaneValues;
 std::map<CString, bool>::const_iterator CombinedMissingAreasListIt;
 std::map<CString, bool>CombinedMissingAreasList;
 const char *p_pWUID = "";
 p_pWUID = p_pSeverity->m_FSG.m_WFs.m_WeatherUndergroundID;
 dwWriteBuffLen = p_dwWriteBuffLen;
 pWork = *p_ppWriteBuff;
 sprintf_s(tstr, sizeof(tstr), "{%s", pEndStr);
 pWork = WriteToOuput(p_ppWriteBuff, &dwWriteBuffLen, pWork, tstr);
 sprintf_s(tstr, sizeof(tstr), "\"WVSSSuccessFlag\":\"%d\",%s", (p_bLoadSuccess ? 1 : 0), pEndStr);
 pWork = WriteToOuput(p_ppWriteBuff, &dwWriteBuffLen, pWork, tstr);
 sprintf_s(tstr, sizeof(tstr), "\"WVSSErrorCount\":\"%d\",%s", p_pSeverity->m_ErrorList.GetCount(), pEndStr);
 pWork = WriteToOuput(p_ppWriteBuff, &dwWriteBuffLen, pWork, tstr);
 sprintf_s(tstr, sizeof(tstr), "\"WVSSErrors\":[%s", pEndStr);
 pWork = WriteToOuput(p_ppWriteBuff, &dwWriteBuffLen, pWork, tstr);
 Pos =p_pSeverity->m_ErrorList.GetHeadPosition();
 dwCount = 0;
 while(Pos) {
   sprintf_s(tstr, sizeof(tstr), "\"%s\"%s%s", (LPCSTR)p_pSeverity->m_ErrorList.GetNext(Pos), (((int)dwCount < (int)p_pSeverity->m_ErrorList.GetCount() - 1) ? ", " : ""), pEndStr);
   pWork = WriteToOuput(p_ppWriteBuff, &dwWriteBuffLen, pWork, tstr);
   dwCount ++; }
 sprintf_s(tstr, sizeof(tstr), "],%s", pEndStr);
 pWork = WriteToOuput(p_ppWriteBuff, &dwWriteBuffLen, pWork, tstr);
 sprintf_s(tstr, sizeof(tstr), "\"WindServer\": {%s", pEndStr);
 pWork = WriteToOuput(p_ppWriteBuff, &dwWriteBuffLen, pWork, tstr);
 p_pSeverity->GetWeatherStationInfo(tstr2, &dVal); 
 sprintf_s(tstr, sizeof(tstr), "\"WeatherStationName\": \"%s\", \"WeatherStationElevation\" : \"%.1lf\",%s", tstr2, dVal, pEndStr);
 pWork = WriteToOuput(p_ppWriteBuff, &dwWriteBuffLen, pWork, tstr);
 sprintf_s(tstr, sizeof(tstr), "\"Records\": [%s", pEndStr);
 pWork = WriteToOuput(p_ppWriteBuff, &dwWriteBuffLen, pWork, tstr);
 for(i = 0; (size_t)i <p_pSeverity->m_WindServerOutput.size(); i ++) {
   strcpy(tstr2, p_pSeverity->m_WindServerOutput[i].m_strRequest);
   strrplInPlace(tstr2, p_pWUID, "$WUID$");
   sprintf_s(tstr, sizeof(tstr), "{ \"Request\":\"%s\", \"Success\":\"%d\", \"HTTPSuccess\":\"%d\", \"HTTPRequestStatusCode\":\"%d\", \"RecordCount\":\"%d\" }%s%s", 
     tstr2,
     (int)p_pSeverity->m_WindServerOutput[i].m_bSuccess,
     (int)p_pSeverity->m_WindServerOutput[i].m_bHTTPRequestSuccess,
     (int)p_pSeverity->m_WindServerOutput[i].m_dwRequestHTTPStatusCode,
     (int)p_pSeverity->m_WindServerOutput[i].m_nWeatherRecords,
     ((size_t)i <p_pSeverity->m_WindServerOutput.size() - 1 ? "," : ""),
     pEndStr);
   pWork = WriteToOuput(p_ppWriteBuff, &dwWriteBuffLen, pWork, tstr);
   }
 sprintf_s(tstr, sizeof(tstr), "]%s", pEndStr);
 pWork = WriteToOuput(p_ppWriteBuff, &dwWriteBuffLen, pWork, tstr);
 sprintf_s(tstr, sizeof(tstr), "},%s", pEndStr);
 pWork = WriteToOuput(p_ppWriteBuff, &dwWriteBuffLen, pWork, tstr);
 sprintf_s(tstr, sizeof(tstr), "\"WakeEncounters\": {%s", pEndStr);
 pWork = WriteToOuput(p_ppWriteBuff, &dwWriteBuffLen, pWork, tstr);
 sprintf_s(tstr, sizeof(tstr), "\"EncounterCount\":\"%d\",%s", p_pSeverity->m_FullSeverityOutput.size(), pEndStr);
 pWork = WriteToOuput(p_ppWriteBuff, &dwWriteBuffLen, pWork, tstr);
 sprintf_s(tstr, sizeof(tstr), "\"EncounterData\": [%s", pEndStr);
 pWork = WriteToOuput(p_ppWriteBuff, &dwWriteBuffLen, pWork, tstr);
 dwCount = 0;
 ClosestPlaneToWakeMap.clear();
 for(It = p_pSeverity->m_FullSeverityOutput.begin(); It !=p_pSeverity->m_FullSeverityOutput.end(); It ++) {
   strcpy(TwoPlaneKeyValue.m_Key.m_Follower, It->m_FollowerCallSign);
   strcpy(TwoPlaneKeyValue.m_Key.m_Generator, It->m_GeneratorCallSign);
   dVal = sqrt(pow(It->m_dPlaneToWakeHorz, 2) + pow(It->m_dPlaneToWakeVert, 2));
   TwoPlaneKeyValue.m_Values.m_dVal = dVal;
   TwoPlaneKeyValue.m_Values.m_EntryTime = It->m_EntryTime;
   if((ClosestPlaneToWakeMapIt = ClosestPlaneToWakeMap.find(TwoPlaneKeyValue.m_Key)) == ClosestPlaneToWakeMap.end())
     ClosestPlaneToWakeMap.insert(std::pair<CTwoPlaneKeyValue::rTwoPlaneKey, CTwoPlaneKeyValue::rTwoPlaneValues>(TwoPlaneKeyValue.m_Key, TwoPlaneKeyValue.m_Values));
   else {
     if(dVal < ClosestPlaneToWakeMapIt->second.m_dVal) {
       ClosestPlaneToWakeMapIt->second.m_EntryTime = It->m_EntryTime;
       ClosestPlaneToWakeMapIt->second.m_dVal = dVal; }
     }
   dVal = It->m_dWindDirectionDegTo - 180;
   if(dVal < 0) dVal += 360;
   sprintf_s(tstr, sizeof(tstr),
     "{ \"GeneratorCallSign\":\"%s\", \"FollowerCallSign\":\"%s\", \"DateTime\":\"%.4d-%.2d-%.2d %.2d:%.2d:%.2d\", \"TimeFromLeader_s\":\"%.1lf\", "
     "\"MaxBank_deg\":\"%.2lf\", \"MaxAngularSpeedOfBank_deg_per_s\":\"%.2lf\", \"MaxAltitudeLoss_ft\":\"%.1lf\", "
     "\"WindDirectionFrom_deg\":\"%.1f\", \"WindSpeed_kts\":\"%.1f\", "
     "\"PlaneToWakeDistanceHorz_m\":\"%.1lf\", \"PlaneToWakeDistanceVert_m\":\"%.1lf\" }%s%s",
     It->m_GeneratorCallSign,
     It->m_FollowerCallSign, 
     It->m_EntryTime.wYear, It->m_EntryTime.wMonth, It->m_EntryTime.wDay,
     It->m_EntryTime.wHour, It->m_EntryTime.wMinute, It->m_EntryTime.wSecond,
     It->m_dTimeFromGeneratorS,
     It->m_dMaxBankRad * 180 / M_PI, 
     It->m_dMaxAngularSpeedOfBankRadPerS * 180 / M_PI, 
     It->m_dMaxAltitudeLossM / 0.3048,
     dVal, 
     It->m_dWindSpeedMS * 3.6 / 1.852,
     It->m_dPlaneToWakeHorz, It->m_dPlaneToWakeVert,
     (dwCount <p_pSeverity->m_FullSeverityOutput.size() - 1 ? "," : ""),
     pEndStr);
   pWork = WriteToOuput(p_ppWriteBuff, &dwWriteBuffLen, pWork, tstr);
   dwCount ++;
   }
 sprintf_s(tstr, sizeof(tstr), "],%s", pEndStr); 
 pWork = WriteToOuput(p_ppWriteBuff, &dwWriteBuffLen, pWork, tstr);
 sprintf_s(tstr, sizeof(tstr), "\"PlaneToWakeClosestDistances\": [%s", pEndStr); 
 pWork = WriteToOuput(p_ppWriteBuff, &dwWriteBuffLen, pWork, tstr);
 dwCount = 0;
 for(ClosestPlaneToWakeMapIt = ClosestPlaneToWakeMap.begin(); ClosestPlaneToWakeMapIt != ClosestPlaneToWakeMap.end(); ClosestPlaneToWakeMapIt ++) {
   pTwoPlaneKey = &ClosestPlaneToWakeMapIt->first;
   pTwoPlaneValues = &ClosestPlaneToWakeMapIt->second;
   sprintf_s(tstr, sizeof(tstr), "{ \"GeneratorCallSign\":\"%s\", \"FollowerCallSign\":\"%s\", \"DateTime\":\"%.4d-%.2d-%.2d %.2d:%.2d:%.2d\", \"Distance_m\":\"%.1lf\" }%s%s",
             pTwoPlaneKey->m_Generator, pTwoPlaneKey->m_Follower, 
             pTwoPlaneValues->m_EntryTime.wYear, pTwoPlaneValues->m_EntryTime.wMonth, pTwoPlaneValues->m_EntryTime.wDay, 
             pTwoPlaneValues->m_EntryTime.wHour, pTwoPlaneValues->m_EntryTime.wMinute, pTwoPlaneValues->m_EntryTime.wSecond,
             pTwoPlaneValues->m_dVal,
             (dwCount < ClosestPlaneToWakeMap.size() - 1 ? "," : ""),
             pEndStr);
   pWork = WriteToOuput(p_ppWriteBuff, &dwWriteBuffLen, pWork, tstr);
   dwCount ++;
   }
 sprintf_s(tstr, sizeof(tstr), "]%s", pEndStr); 
 pWork = WriteToOuput(p_ppWriteBuff, &dwWriteBuffLen, pWork, tstr);
 sprintf_s(tstr, sizeof(tstr), "},%s", pEndStr); 
 pWork = WriteToOuput(p_ppWriteBuff, &dwWriteBuffLen, pWork, tstr);
 sprintf_s(tstr, sizeof(tstr), "\"MissingElevationAreas\":[%s", pEndStr); 
 pWork = WriteToOuput(p_ppWriteBuff, &dwWriteBuffLen, pWork, tstr);
 CombinedMissingAreasList =p_pSeverity->GetMissingElevationDataAreas();
 dwCount = 0;
 for(CombinedMissingAreasListIt = CombinedMissingAreasList.begin(); CombinedMissingAreasListIt != CombinedMissingAreasList.end(); CombinedMissingAreasListIt++) {
   sprintf_s(tstr, sizeof(tstr), "\"%s\"%s%s", CombinedMissingAreasListIt->first, ((dwCount < CombinedMissingAreasList.size() - 1) ? "," : ""), pEndStr);
   pWork = WriteToOuput(p_ppWriteBuff, &dwWriteBuffLen, pWork, tstr);
   dwCount ++; }
 sprintf_s(tstr, sizeof(tstr), "]%s", pEndStr); 
 pWork = WriteToOuput(p_ppWriteBuff, &dwWriteBuffLen, pWork, tstr);
 sprintf_s(tstr, sizeof(tstr), "}%s", pEndStr); 
 pWork = WriteToOuput(p_ppWriteBuff, &dwWriteBuffLen, pWork, tstr);
 return(true);
}
