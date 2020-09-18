#include "LineParser.h"
#include "stdio.h"
#include "string.h"

#pragma warning (disable: 4996)

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
 bool DoRead;
 unsigned long lReadBufUsed, lRead, lTotalRead, lCurLine;
 char *pStartWork, *pEndWork;

 if(!OnBeforeLoad(p_pFileName)) return(false);

 fFile = fopen(p_pFileName, "rb");
 if(!fFile || !m_pReadBuff) return(false);

 //FILE *fOutFile = fopen("e:\\temp\\3dobj.txt", "wb");

 lTotalRead = 0;
 lReadBufUsed = 0;
 lCurLine = 0;
 DoRead = true;
 while(DoRead) {
   lRead = fread(m_pReadBuff + lReadBufUsed, 1, m_lReadBufLen - lReadBufUsed, fFile);
   if(!lRead)
     lRead = lRead;
   *(m_pReadBuff + lReadBufUsed + lRead) = '\0';
   lTotalRead += lRead;
   lReadBufUsed += lRead;
   pStartWork = m_pReadBuff;
   while(pStartWork) {
     pEndWork = strchr(pStartWork, '\n');
     if(!pEndWork && lRead) break; // let's read more
     if(pEndWork) {
       if(pEndWork != pStartWork) {
         if(*(pEndWork - 1) == '\r') // we are not at the very beginning of the line with \n and previous symbol is \r
           *(pEndWork - 1) = '\0'; }
       *pEndWork = '\0'; }
     if(*pStartWork)
       ProcessLine(pStartWork);
     lCurLine ++;
     if(lCurLine == 26688)
       lCurLine = lCurLine;
     //fprintf(fOutFile, "%s%s", pStartWork, pEndWork ? "\n" : "");
     if(!pEndWork) break;
     pStartWork = pEndWork + 1;
     }
   if(!lRead) // last read
     break;
   lReadBufUsed = m_pReadBuff + lReadBufUsed - pStartWork;
   memmove(m_pReadBuff, pStartWork, lReadBufUsed);
   }

 //fclose(fOutFile);
 fclose(fFile);
 if(!OnLoaded()) return(false);
 return(true);
}
