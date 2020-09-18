#pragma once
#define Degrees(x) ((x) * 57.29577951308232)	
#define Radians(x) ((x) / 57.29577951308232)	
void strrplInPlace(char *p_pSource, LPCSTR p_pToken, LPCSTR p_pReplacement);
char *TrimStr(char *p_pStr);
char *ReplaceFileExtension(char *p_pFileName, const char *p_pNewExt = NULL);
FILE *OpenOutputFile2(const char *p_pOrigFileName, const char *p_pNewExt, char *p_Mode, bool p_bCheckFlag, bool *p_pbFileExists = NULL);
bool SystemTimeToLI(SYSTEMTIME *p_pST, LARGE_INTEGER *p_pllLI);
bool LIToSystemTime(LARGE_INTEGER *p_pllLI, SYSTEMTIME *p_pST);
bool AVP_IsDigit(char p_cC);
double Trapz(double *p_pdValues, unsigned long p_lStartIndex, unsigned long p_lEndIndex, double p_dStep);
double TrapzVarStep(double *p_pdX, double *p_pdY, unsigned long p_lStartIndex, unsigned long p_lEndIndex);
double TrapzTwoVal(double p_dY1, double p_dY2, double p_dX);
double DistanceFromPointToPlane(double p_dX, double p_dY, double p_dZ, double p_dX1, double p_dY1, double p_dZ1, double p_dX2, double p_dY2, double p_dZ2, double p_dX3, double p_dY3, double p_dZ3);
bool IsLeapYear(short p_Year);
bool DateTimeFromStr(const char *p_pStr, const char *p_pFormat, SYSTEMTIME *p_pST);
bool TimeFromString(const char *p_pStr, SYSTEMTIME *p_pDest, bool p_bDatePresent); 
