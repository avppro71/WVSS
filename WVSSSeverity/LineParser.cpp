#include "stdafx.h"
#include "LineParser.h"
#pragma warning(disable : 4996)
CLineParser::CLineParser()
{
 m_lReadBufLen = (1024 * 32 - 1);
 m_pReadBuff = new char[m_lReadBufLen + 1];
}
CLineParser::~CLineParser()
{
 if(m_pReadBuff) delete [] m_pReadBuff;
}
bool CLineParser::Load(const char *p_pFileName)
{
 FILE *fFile;
 bool bVal;
 if(!OnBeforeFileOpen(p_pFileName)) return(false);
 fFile = fopen(p_pFileName, "rb");
 if(!fFile || !m_pReadBuff) return(false);
 bVal = Load(fFile);
 fclose(fFile);
 return(bVal);
}
bool CLineParser::Load(FILE *p_fFile)
{
 bool DoRead;
 unsigned long lReadBufUsed, lRead, lTotalRead, lCurLine;
 char *pStartWork, *pEndWork;
 if(!OnBeforeLoad(p_fFile)) return(false);
 lTotalRead = 0;
 lReadBufUsed = 0;
 lCurLine = 0;
 DoRead = true;
 while(DoRead) {
   lRead = fread(m_pReadBuff + lReadBufUsed, 1, m_lReadBufLen - lReadBufUsed, p_fFile);
   if(!lRead)
     lRead = lRead;
   *(m_pReadBuff + lReadBufUsed + lRead) = '\0';
   lTotalRead += lRead;
   lReadBufUsed += lRead;
   pStartWork = m_pReadBuff;
   while(pStartWork) {
     pEndWork = strchr(pStartWork, '\n');
     if(!pEndWork && lRead) break; 
     if(pEndWork) {
       if(pEndWork != pStartWork) {
         if(*(pEndWork - 1) == '\r') 
           *(pEndWork - 1) = '\0'; }
       *pEndWork = '\0'; }
     if(*pStartWork)
       ProcessLine(pStartWork);
     lCurLine ++;
     if(!pEndWork) break;
     pStartWork = pEndWork + 1;
     }
   if(!lRead) 
     break;
   lReadBufUsed = m_pReadBuff + lReadBufUsed - pStartWork;
   memmove(m_pReadBuff, pStartWork, lReadBufUsed);
   }
 if(!OnLoaded()) return(false);
 return(true);
}
bool CLineParser::ParseToStrList(char *p_pLine, char *p_pDest, short p_nDestCount, size_t p_nDestSize, const char *p_pDelimiters)
{
 char *pWork;
 short TokenNum;
 if(!p_pLine || !p_pDest || !p_pDelimiters) return(false);
 memset(p_pDest, 0, p_nDestSize * p_nDestCount);
 pWork = strtok(p_pLine, p_pDelimiters);
 TokenNum = 0;
 while(pWork) {
   strncpy(p_pDest + TokenNum * p_nDestSize, pWork, p_nDestSize - 1);
   TokenNum ++;
   if(TokenNum == p_nDestCount) break;
   pWork = strtok(NULL, p_pDelimiters);
   }
 return(true);
}
