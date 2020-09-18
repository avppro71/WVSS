#include "3DObjLineParser.h"
#include "qfileinfo.h"

#pragma warning (disable: 4996)

/***************************************/

bool CPathParts::ParseFileName(const char *p_pFileName)
{
 QFileInfo FileInfo;

 Reset();
 FileInfo.setFile(p_pFileName);
 FileInfo.makeAbsolute();
 strcpy(m_Path, FileInfo.absoluteDir().path().toLocal8Bit());
 strcpy(m_FileName, FileInfo.fileName().toLocal8Bit());
 return(true);
}

/***************************************/

bool C3DObjLineParser::ParseToR3(char *p_pLine, rf3 *p_pR3)
// strtok changes the line
{
 char *pWork;
 short TokenNum;

 pWork = strtok(p_pLine, " \t");
 TokenNum = 0;
 while(pWork) {
   switch(TokenNum) {
     case 0: break;
     case 1: if(p_pR3) p_pR3->x = atof(pWork); break;
     case 2: if(p_pR3) p_pR3->y = atof(pWork); break;
     case 3: if(p_pR3) p_pR3->z = atof(pWork); break;
     }
   TokenNum ++;
   pWork = strtok(NULL, " \t");
   }
 return(true);
}

bool C3DObjLineParser::ParseTo3I3(char *p_pLine, ri3 *p_pR3)
{
 char *pWork, *pWork2;
 short TokenNum;

 pWork = strtok(p_pLine, " \t");
 TokenNum = 0;
 while(pWork) {
   switch(TokenNum) {
     case 0: break;
     case 1:
     case 2:
     case 3:
       (p_pR3 + TokenNum - 1)->VertexIndex = atoi(pWork);
       pWork2 = strchr(pWork, '/');
       if(!pWork2) return(false);
       (p_pR3 + TokenNum - 1)->TextureIndex = atoi(pWork2 + 1);
       pWork2 = strchr(pWork2 + 1, '/');
       if(!pWork2) return(false);
       (p_pR3 + TokenNum - 1)->NormalIndex = atoi(pWork2 + 1);
       break;
     }
   TokenNum ++;
   pWork = strtok(NULL, " \t");
   }
 return(true);
}

bool C3DObjLineParser::ParseToStr(char *p_pLine, char *p_pDest, size_t p_nDestSize, bool p_bLast)
{
 char *pWork;
 short TokenNum;

 memset(p_pDest, 0, p_nDestSize);
 pWork = strtok(p_pLine, " \t");
 TokenNum = 0;
 while(pWork) {
   memset(p_pDest, 0, p_nDestSize);
   strncpy(p_pDest, pWork, p_nDestSize - 1);
   if(!p_bLast) break;
   TokenNum ++;
   pWork = strtok(NULL, " \t"); }
 return(true);
}

bool C3DObjLineParser::ParseToStrList(char *p_pLine, char *p_pDest, short p_nDestCount, size_t p_nDestSize)
// p_nDestCount strings of p_nDestSize each
{
 char *pWork;
 short TokenNum;

 memset(p_pDest, 0, p_nDestSize * p_nDestCount);
 pWork = strtok(p_pLine, " \t");
 TokenNum = 0;
 while(pWork) {
   strncpy(p_pDest + TokenNum * p_nDestSize, pWork, p_nDestSize - 1);
   TokenNum ++;
   if(TokenNum == p_nDestCount) break;
   pWork = strtok(NULL, " \t");
   }
 return(true);
}
