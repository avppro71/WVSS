#include "stdafx.h"
#include "Global.h"
#include "math.h"
#pragma warning(disable : 4996)
void GetIniFileName(char *p_pDest)
{
 char tstr[MAX_PATH], *pWork;
 if(!p_pDest) return;
 *p_pDest = '\0';
 GetModuleFileName(NULL, tstr, sizeof(tstr));
 pWork = strrchr(tstr, '\\');
 if(pWork) *(pWork + 1) = '\0'; 
 sprintf(p_pDest, "%s%s", tstr, "WVSSSettings.ini");
}
void strrplInPlace(char *p_pSource, LPCSTR p_pToken, LPCSTR p_pReplacement)
{
 short Len;
 char *pWork, *pStrWork = p_pSource;
 Len = strlen(p_pToken);
 if(Len != strlen(p_pReplacement)) return;
 pWork = pStrWork;
 while(pWork) {
   pWork = strstr(pWork, p_pToken);
   if(pWork) {
     memcpy(pWork, p_pReplacement, Len);
     pWork += Len; }
   }
}
bool GetFilePartsFromName(char *p_pFileName, char *p_pPath, char *p_pTitle, char *p_pExt)
{
 char ch, *pWork1, *pWork2;
 if(!p_pFileName) return(false);
 if(p_pPath) *p_pPath = '\0';
 if(p_pTitle) *p_pTitle = '\0';
 if(p_pExt) *p_pExt = '\0';
 pWork1 = strrchr(p_pFileName, '\\');
 pWork2 = strrchr(p_pFileName, '/');
 if(pWork1 && pWork2) {
   if(pWork2 > pWork1) pWork1 = pWork2; }
 else if(pWork2) pWork1 = pWork2;
 if(pWork1) {
   ch = *pWork1;
   *pWork1 = '\0';
   if(p_pPath) strcpy(p_pPath, p_pFileName);
   *pWork1 = ch;
   pWork1 ++;
   }
 else
   pWork1 = p_pFileName;
 pWork2 = strrchr(pWork1, '.');
 if(pWork2) {
   if(p_pExt) strcpy(p_pExt, pWork2 + 1);
   ch = *pWork2;
   *pWork2 = '\0';
   }
 if(p_pTitle) strcpy(p_pTitle, pWork1);
 if(pWork2) *pWork2 = ch;
 return(true);
}
