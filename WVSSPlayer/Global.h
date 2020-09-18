#pragma once
#ifndef M_PI
#define M_PI   3.1415926535897932384626433832795
#endif
#define Degrees(x) ((x) * 57.29577951308232)	
#define Radians(x) ((x) / 57.29577951308232)	
void GetIniFileName(char *p_pDest);
void strrplInPlace(char *p_pSource, LPCSTR p_pToken, LPCSTR p_pReplacement);
bool GetFilePartsFromName(char *p_pFileName, char *p_pPath, char *p_pTitle, char *p_pExt);
