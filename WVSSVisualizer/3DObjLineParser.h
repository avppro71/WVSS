#pragma once

#include "LineParser.h"
#include <QTOpenGL>

class CPathParts
{
public:
 char m_Path[256];
 char m_FileName[128];

public:
 CPathParts() { Reset(); }
 ~CPathParts() { }
 void Reset() { memset(this, 0, sizeof(CPathParts)); }
 bool ParseFileName(const char *p_pFileName);
 QString FullName() { return((QString)m_Path + "/" + m_FileName); }
};

/************************************************************/

class C3DObjLineParser : public CLineParser
{
private:
protected:
 struct rf3 { GLfloat x, y, z; };
 struct ri3 { GLuint VertexIndex, TextureIndex, NormalIndex; unsigned long lOrdinal; };

public:

private:
protected:
 bool ParseToR3(char *p_pLine, rf3 *p_pR3); // strtok changes the line
 bool ParseTo3I3(char *p_pLine, ri3 *p_pR3); // strtok changes the line
 bool ParseToStr(char *p_pLine, char *p_pDest, size_t p_nDestSize, bool p_bLast = false); // strtok changes the line
 // p_nDestCount strings of p_nDestSize each
 bool ParseToStrList(char *p_pLine, char *p_pDest, short p_nDestCount, size_t p_nDestSize); // strtok changes the line

public:
 C3DObjLineParser() { }
 virtual ~C3DObjLineParser() { }
};
