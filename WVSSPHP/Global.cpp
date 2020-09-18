#include "stdafx.h"
#include "Global.h"
void strrplInPlace(char *p_pSource, const char *p_pToken, const char *p_pReplacement)
{
 size_t lSourceLen, lTokenLen, lReplacementLen, lLenDiff, lAdvanced;
 char *pStartWork, *pEndWork; 
 if(!p_pSource || !p_pToken || !p_pReplacement) return;
 if(!*p_pSource || !*p_pToken) return; 
 lSourceLen = strlen(p_pSource);
 lTokenLen = strlen(p_pToken);
 lReplacementLen = strlen(p_pReplacement);
 lLenDiff = (lTokenLen - lReplacementLen);
 if(lReplacementLen > lTokenLen) return; 
 pStartWork = p_pSource;
 lAdvanced = 0;
 while(pEndWork = strstr(pStartWork, p_pToken)) {
   lAdvanced += (pEndWork - pStartWork) + lReplacementLen;
   memcpy(pEndWork, p_pReplacement, lReplacementLen);
   pEndWork += lReplacementLen;
   if(lReplacementLen != lTokenLen) { 
     memmove(pEndWork, pEndWork + lLenDiff, lSourceLen - lAdvanced - lLenDiff);
     lSourceLen -= lLenDiff;
     *(p_pSource + lSourceLen) = '\0'; }
   pStartWork = pEndWork;
   }
}
