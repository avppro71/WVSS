#include "stdafx.h"
#include "Airplanes2.h"
#include "assert.h"
#pragma warning(disable : 4996)
bool CAPM::CAP3T::operator < (const CAPM::CAP3T &p_Airplane3Type) const
{
 return(strcmp(m_TN, p_Airplane3Type.m_TN) < 0);
}
CAPM::CAPM()
{
}
CAPM::~CAPM()
{
}
void CAPM::AddAirplane(RAPL3 *p_pAirplane)
{
 CAP3T Type;
 strcpy(Type.m_TN, p_pAirplane->m_TN);
 m_APMA[Type] = *p_pAirplane;
}
bool CAPM::LoadFromFile(const char *p_pFileName, bool p_bBinary)
{
 FILE *fFile;
 char *pWork, tstr[256];
 unsigned long li, ulCount = 0;
 int nVal;
 RAPL3 Airplane3;
 m_APMA.clear();
 fFile = fopen(p_pFileName, "rb");
 if(!fFile) {
   m_ErrorList.AddTail(CString("Airplane file ") + p_pFileName + " failed top load");
   return(false); }
 if(p_bBinary) {
   nVal = fread(tstr, strlen(AVP_AIRPLANE_FILE_BIN_PREFIX), 1, fFile);
   if(nVal != 1) { m_ErrorList.AddTail(CString("Airplane file ") + p_pFileName + ": unable to read marker"); return(false); }
   if(strnicmp(tstr, AVP_AIRPLANE_FILE_BIN_PREFIX, strlen(AVP_AIRPLANE_FILE_BIN_PREFIX))) {
     m_ErrorList.AddTail(CString("Airplane file ") + p_pFileName + ": wrong file format");
     return(false); }
   nVal = fread(&ulCount, sizeof(ulCount), 1, fFile);
   if(nVal != 1) { m_ErrorList.AddTail(CString("Airplane file ") + p_pFileName + ": unable to read count"); return(false); }
   }
 else
   fgets(tstr, sizeof(tstr), fFile); 
 li = 0;
 while(true) {
   if(p_bBinary) {
     nVal = fread(&Airplane3, sizeof(Airplane3), 1, fFile);
     if(nVal != 1) { m_ErrorList.AddTail(CString("Airplane file ") + p_pFileName + ": unable to read data structure"); return(false); }
     }
   else {
     if(!fgets(tstr, sizeof(tstr), fFile)) break;
     if((pWork = strrchr(tstr, '\n'))) *pWork = '\0';
     if((pWork = strrchr(tstr, '\r'))) *pWork = '\0';
     if(!PTALe(tstr, &Airplane3)) { m_ErrorList.AddTail(CString("Airplane file ") + p_pFileName + ": parse error"); return(false); }
     }
   AddAirplane(&Airplane3);
   li ++;
   if(p_bBinary && li == ulCount) break;
   }
 fclose(fFile);
 return(true);
}
bool CAPM::PTALe(char *p_pStr, RAPL3 *p_pAirplane3)
{
 char *pWork;
 if(!p_pAirplane3) { m_ErrorList.AddTail("Internal: p_pAirplane3 is NULL"); return(false); }
 memset(p_pAirplane3, 0, sizeof(RAPL3));
 pWork = strtok(p_pStr, " ,");
 if(!pWork) { m_ErrorList.AddTail("Internal: parse error 1"); return(false); }
 strcpy(p_pAirplane3->m_TN, pWork);
 pWork = strtok(NULL, " ,");
 if(!pWork) { m_ErrorList.AddTail("Internal: parse error 2"); return(false); }
 p_pAirplane3->m_dMTMK = atof(pWork);
 if(p_pAirplane3->m_dMTMK >= 136000)
   p_pAirplane3->m_ICAOCat = ICAO_HEAVY;
 else if(p_pAirplane3->m_dMTMK >= 7000)
   p_pAirplane3->m_ICAOCat = ICAO_MEDIUM;
 else   
   p_pAirplane3->m_ICAOCat = ICAO_LIGHT;
 if(!strnicmp(p_pAirplane3->m_TN, "A38", 3))
   p_pAirplane3->m_FAACat = FAA_SUPER;
 else if(p_pAirplane3->m_dMTMK >= 140000)
   p_pAirplane3->m_FAACat = FAA_HEAVY;
 else if(p_pAirplane3->m_dMTMK >= 19000)
   p_pAirplane3->m_FAACat = FAA_LARGE;
 else   
   p_pAirplane3->m_FAACat = FAA_SMALL;
 pWork = strtok(NULL, " ,");
 if(!pWork) { m_ErrorList.AddTail("Internal: parse error 3"); return(false); }
 p_pAirplane3->m_dMLMK = atof(pWork);
 pWork = strtok(NULL, " ,");
 if(!pWork) { m_ErrorList.AddTail("Internal: parse error 4"); return(false); }
 p_pAirplane3->m_dWSM = atof(pWork);
 pWork = strtok(NULL, " ,");
 if(!pWork) { m_ErrorList.AddTail("Internal: parse error 5"); return(false); }
 p_pAirplane3->m_dLSTK = atof(pWork);
 pWork = strtok(NULL, " ,");
 if(!pWork) { m_ErrorList.AddTail("Internal: parse error 6"); return(false); }
 p_pAirplane3->m_dWAM = atof(pWork);
 pWork = strtok(NULL, " ,");
 if(!pWork) { m_ErrorList.AddTail("Internal: parse error 7"); return(false); }
 p_pAirplane3->m_dWAS = atof(pWork);
 pWork = strtok(NULL, " ,");
 if(!pWork) { m_ErrorList.AddTail("Internal: parse error 8"); return(false); }
 p_pAirplane3->m_dWRT = atof(pWork);
 pWork = strtok(NULL, " ,");
 if(!pWork) { m_ErrorList.AddTail("Internal: parse error 9"); return(false); }
 p_pAirplane3->m_dMWXX = atof(pWork);
 pWork = strtok(NULL, " ,");
 if(!pWork) { m_ErrorList.AddTail("Internal: parse error 10"); return(false); }
 p_pAirplane3->m_sXXJI = atof(pWork);
 pWork = strtok(NULL, " ,");
 if(!pWork) { m_ErrorList.AddTail("Internal: parse error 11"); return(false); }
 p_pAirplane3->m_dWPXOM = atof(pWork);
 return(true);
}
bool CAPM::SaveToFile(const char *p_pFileName, bool p_bBinary)
{
 FILE *fFile;
 int nVal;
 unsigned long ulVal;
 std::map<CAP3T, RAPL3>::iterator It;
 fFile = fopen(p_pFileName, "wb");
 if(!fFile) return(false);
 if(p_bBinary) {
   nVal = fwrite(AVP_AIRPLANE_FILE_BIN_PREFIX, strlen(AVP_AIRPLANE_FILE_BIN_PREFIX), 1, fFile);
   assert(nVal == 1);
   ulVal = m_APMA.size();
   fwrite(&ulVal, sizeof(ulVal), 1, fFile);
   assert(nVal == 1);
   }
 else
   fprintf(fFile, "TypeName, MaxTakeoffMassKG, MaxLandingMassKG, WingSpanM, LandingSpeedKT, WingAreaM, WingAspectRatio, WingTaper, mx_wx, Jxx, VortexPointXOffsetM\r\n");
 for(It = m_APMA.begin(); It != m_APMA.end(); It ++) {
   if(p_bBinary)
      fwrite(&It->second, sizeof(RAPL3), 1, fFile);
   else {
     fprintf(fFile, "%s,%lf,%lf,%lf,%lf,%lf,%lf,%lf,%lf,%lf,%lf\r\n",
       It->second.m_TN, 
       It->second.m_dMTMK,
       It->second.m_dMLMK,
       It->second.m_dWSM,
       It->second.m_dLSTK,
       It->second.m_dWAM,
       It->second.m_dWAS,
       It->second.m_dWRT,
       It->second.m_dMWXX,
       It->second.m_sXXJI,
       It->second.m_dWPXOM);
     }
   }
 fclose(fFile);
 return(true);
}
const CAPM::RAPL3 *CAPM::GALBNa(LPCSTR p_pTypeName)
{
 CAP3T Type;
 std::map<CAP3T, RAPL3>::iterator It;
 strcpy(Type.m_TN, p_pTypeName);
 It = m_APMA.find(Type);
 if(It == m_APMA.end()) return(NULL);
 return(&It->second);
}
const char *CAPM::GetICAOCatName(eICAOCat p_ICAOCat)
{
 switch(p_ICAOCat) {
   case ICAO_LIGHT: return("light");
   case ICAO_MEDIUM: return("medium");
   case ICAO_HEAVY: return("heavy");
   default: return("unknown"); }
}
const char *CAPM::GetFAACatName(eFAACat p_FAACat)
{
 switch(p_FAACat) {
   case FAA_SMALL: return("small");
   case FAA_LARGE: return("large");
   case FAA_HEAVY: return("heavy");
   case FAA_SUPER: return("super");
   default: return("unknown"); }
}
