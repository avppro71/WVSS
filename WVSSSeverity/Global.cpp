#include "stdafx.h"
#include "Global.h"
#include "math.h"
#pragma warning(disable : 4996)
void strrplInPlace(char *p_pSource, LPCSTR p_pToken, LPCSTR p_pReplacement)
{
 size_t Len;
 char *pWork;
 Len = strlen(p_pToken);
 if(Len != strlen(p_pReplacement)) return;
 pWork = p_pSource;
 while(pWork) {
   pWork = strstr(pWork, p_pToken);
   if(pWork) {
     memcpy(pWork, p_pReplacement, Len);
     pWork += Len; }
   }
}
char *ReplaceFileExtension(char *p_pFileName, const char *p_pNewExt )
{
 char *pWork;
 if(!p_pFileName) return(p_pFileName);
 pWork = strrchr(p_pFileName, '.');
 if(pWork) *pWork = '\0';
 if(p_pNewExt)
   strcat(p_pFileName, p_pNewExt);
 return(p_pFileName);
}
FILE *OpenOutputFile2(const char *p_pOrigFileName, const char *p_pNewExt, char *p_Mode, bool p_bCheckFlag, bool *p_pbFileExists)
{
 FILE *fFile = NULL;
 char tstr[MAX_PATH];
 if(!p_bCheckFlag) return(NULL);
 strcpy(tstr, p_pOrigFileName);
 ReplaceFileExtension(tstr, p_pNewExt);
 if(p_pbFileExists)
   *p_pbFileExists = (GetFileAttributes(tstr) != INVALID_FILE_ATTRIBUTES);
 fFile = fopen(tstr, p_Mode);
 return(fFile);
}
bool SystemTimeToLI(SYSTEMTIME *p_pST, LARGE_INTEGER *p_pllLI)
{
 FILETIME FT;
 if(!SystemTimeToFileTime(p_pST, &FT))
   return(false);
 p_pllLI->HighPart = FT.dwHighDateTime;
 p_pllLI->LowPart = FT.dwLowDateTime;
 return(true);
}
bool LIToSystemTime(LARGE_INTEGER *p_pllLI, SYSTEMTIME *p_pST)
{
 FILETIME FT;
 FT.dwHighDateTime = p_pllLI->HighPart;
 FT.dwLowDateTime = p_pllLI->LowPart;
 if(!FileTimeToSystemTime(&FT, p_pST))
   return(false);
 return(true);
}
bool AVP_IsDigit(char p_cC)
{
 short i;
 char *pDigits = {"1234567890"};
 for(i = 0; i < 10; i ++)
   if(pDigits[i] == p_cC) return(true);
 return(false);
}
double Trapz(double *p_pdValues, unsigned long p_lStartIndex, unsigned long p_lEndIndex, double p_dStep)
{
 unsigned long li; 
 double dVal;
 if(p_lEndIndex <= p_lStartIndex)
   return(0);
 dVal = (p_pdValues[p_lStartIndex] + p_pdValues[p_lEndIndex]) / 2;
 for(li = p_lStartIndex + 1; li < p_lEndIndex; li ++)
     dVal += p_pdValues[li];
 dVal *= p_dStep;
 return(dVal);   
}
double TrapzVarStep(double *p_pdX, double *p_pdY, unsigned long p_lStartIndex, unsigned long p_lEndIndex)
{
 unsigned long li; 
 double dVal;
 if(p_lEndIndex <= p_lStartIndex)
   return(0);
 dVal = 0;
 for(li = p_lStartIndex; li < p_lEndIndex; li ++)
   dVal += (p_pdY[li + 1] + p_pdY[li]) * (p_pdX[li + 1] - p_pdX[li]);
 return(dVal / 2);
}
double TrapzTwoVal(double p_dY1, double p_dY2, double p_dX)
{
 double dVal;
 dVal = (p_dY1 + p_dY2) * p_dX / 2;
 return(dVal);
}
double DistanceFromPointToPlane(double p_dX, double p_dY, double p_dZ, double p_dX1, double p_dY1, double p_dZ1, double p_dX2, double p_dY2, double p_dZ2, double p_dX3, double p_dY3, double p_dZ3)
{
 double dA, dB, dC, dD, dDistance;
 dA = (p_dY2 - p_dY1) * (p_dZ3 - p_dZ1) - (p_dZ2 - p_dZ1)* (p_dY3 - p_dY1);
 dB = (p_dZ2 - p_dZ1)* (p_dX3 - p_dX1) - (p_dX2 - p_dX1) * (p_dZ3 - p_dZ1);
 dC = (p_dX2 - p_dX1)* (p_dY3 - p_dY1) - (p_dY2 - p_dY1) * (p_dX3 - p_dX1);
 dD = - (dA * p_dX1 + dB * p_dY1 + dC * p_dZ1);
 dDistance = fabs(dA * p_dX + dB * p_dY + dC * p_dZ + dD) / sqrt(dA * dA + dB * dB + dC * dC);
 return(dDistance);
}
bool IsLeapYear(short p_Year)
{
 return((!(p_Year % 4) && p_Year % 100) || (!(p_Year % 100) && !((p_Year / 100) % 4)));
}
bool DateTimeFromStr(const char *p_pStr, const char *p_pFormat, SYSTEMTIME *p_pST)
{
 char tstr[32];
 char *pCodes[] = { "CC", "YY", "MMM", "MM", "DD", "HH", "mm", "ss", "mmm" };
 const char *pMonthStr[12] = { "JAN", "FEB", "MAR", "APR", "MAY", "JUN", "JUL", "AUG", "SEP", "OCT", "NOV", "DEC" };
 short MonthDays[12] = { 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };
 const short CodeCount = sizeof(pCodes) / sizeof(char *);
 short i, j, pCodeLen[CodeCount], pCodeOffset[CodeCount], MaxDay, DataStrLen;
 const char *pCodeLocation[CodeCount];
 bool bUnixTime = false, bUnixTimeMilliseconds = false;
 time_t Timer;
 tm *pTM;
 if(!p_pST || !p_pFormat) return(false);
 if(!*p_pFormat) return(false);
 DataStrLen = strlen(p_pStr);
 if(!stricmp(p_pFormat, "UNIXTimeSec")) { bUnixTime = true; bUnixTimeMilliseconds = false; }
 else if(!stricmp(p_pFormat, "UNIXTimeMs")) { bUnixTime = true; bUnixTimeMilliseconds = true; }
 if(!bUnixTime) {
   for(i = 0; i < CodeCount; i ++) {
     pCodeLen[i] = strlen(pCodes[i]);
     pCodeOffset[i] = -1;
     pCodeLocation[i] = NULL;
     if(i == 3 && pCodeLocation[2]) continue; 
     pCodeLocation[i] = strstr(p_pFormat, pCodes[i]);
     if(pCodeLocation[i])
       pCodeOffset[i] = pCodeLocation[i] - p_pFormat;
     }
   for(i = 0; i < CodeCount; i ++) {
     if(!pCodeLocation[i]) continue;
     if(pCodeOffset[i] + pCodeLen[i] > DataStrLen) continue;
     strncpy(tstr, p_pStr + pCodeOffset[i], pCodeLen[i]);
     *(tstr + pCodeLen[i]) = '\0';
     switch(i) {
       case 0: p_pST->wYear = atoi(tstr) * 100; break; 
       case 1: p_pST->wYear += atoi(tstr); break; 
       case 2: 
         for(j = 0; j < 12; j ++)
           if(!_stricmp(tstr, pMonthStr[j])) {
             p_pST->wMonth = j + 1;
             break; }
         break;
       case 3: p_pST->wMonth = atoi(tstr); break; 
       case 4: p_pST->wDay = atoi(tstr); break; 
       case 5: p_pST->wHour = atoi(tstr); break; 
       case 6: p_pST->wMinute = atoi(tstr); break; 
       case 7: p_pST->wSecond = atoi(tstr); break; 
       case 8: p_pST->wMilliseconds = atoi(tstr); break; 
       } } }
 else { 
   memset(tstr, 0, sizeof(tstr));
   strncpy(tstr, p_pStr, sizeof(tstr) - 1);
   DataStrLen = strlen(p_pStr);
   if(bUnixTimeMilliseconds) {
     p_pST->wMilliseconds = (WORD)atol(tstr + DataStrLen - 3);
     *(tstr + DataStrLen - 3) = '\0'; }
   Timer = atol(tstr);
   pTM = gmtime(&Timer);
   p_pST->wYear = pTM->tm_year + 1900;
   p_pST->wMonth = pTM->tm_mon + 1;
   p_pST->wDay = pTM->tm_mday;
   p_pST->wHour = pTM->tm_hour;
   p_pST->wMinute = pTM->tm_min;
   p_pST->wSecond = pTM->tm_sec;
   }
 if(p_pST->wYear < 100) p_pST->wYear += 2000;
 if(p_pST->wMonth < 1) p_pST->wMonth = 1;
 if(p_pST->wMonth > 12) p_pST->wMonth = 12;
 if(p_pST->wDay < 1) p_pST->wDay = 1;
 MaxDay = MonthDays[p_pST->wMonth - 1];
 if(p_pST->wMonth == 2 && IsLeapYear(p_pST->wYear))
   MaxDay = 29;
 if(p_pST->wDay > MaxDay) p_pST->wDay = MaxDay;
 if(p_pST->wHour < 0) p_pST->wHour = 0;
 if(p_pST->wHour > 23) p_pST->wHour = 23;
 if(p_pST->wMinute < 0) p_pST->wMinute = 0;
 if(p_pST->wMinute > 59) p_pST->wMinute = 59;
 if(p_pST->wSecond < 0) p_pST->wSecond = 0;
 if(p_pST->wSecond > 59) p_pST->wSecond = 59;
 return(true);
}
bool TimeFromString(const char *p_pStr, SYSTEMTIME *p_pDest, bool p_bDatePresent)
#pragma message("TimeFromString is deprecated. Use DateTimeFromStr")
{
 const char *pWork;
 if(!p_pStr || !p_pDest) return(false);
 if(p_bDatePresent) {
   p_pDest->wYear = atoi(p_pStr);
   p_pDest->wMonth = atoi(p_pStr + 5);
   p_pDest->wDay = atoi(p_pStr + 8); }
 p_pDest->wHour = (p_bDatePresent) ? atoi(p_pStr + 11) : atoi(p_pStr);
 p_pDest->wMinute = (p_bDatePresent) ? atoi(p_pStr + 14) : atoi(p_pStr + 3);
 p_pDest->wSecond = (p_bDatePresent) ? atoi(p_pStr + 17) : atoi(p_pStr + 6);
 pWork = (p_bDatePresent) ? (p_pStr + 20) : (p_pStr + 9);
 p_pDest->wMilliseconds = atoi(pWork); 
 if(!isdigit(*(pWork + 1))) p_pDest->wMilliseconds *= 100; 
 else if(!isdigit(*(pWork + 2))) p_pDest->wMilliseconds *= 10; 
 return(true);
}
char *TrimStr(char *p_pStr)
{
 if(!p_pStr || !*p_pStr) return(p_pStr);
 char *pWork = p_pStr, *pWork1;
 unsigned short Len;
 while(*pWork == ' ') pWork ++;
 pWork1 = p_pStr + strlen(p_pStr) - 1;
 while(*pWork1 == ' ' && pWork1 > pWork) pWork1 --;
 Len = pWork1 - pWork + 1;
 if(!Len) { *p_pStr = 0; return(p_pStr); }
 else if(Len == (unsigned short)strlen(p_pStr)) return(p_pStr);
 else {
   memcpy(p_pStr, pWork, Len);
   *(p_pStr + Len) = 0; }
 return(p_pStr);
}
