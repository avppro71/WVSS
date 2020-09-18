#pragma once
class CLineParser
{
private:
protected:
 unsigned long m_lReadBufLen;
 char *m_pReadBuff;
public:
private:
protected:
 virtual bool ProcessLine(char *p_pLine) = 0;
public:
 CLineParser();
 virtual ~CLineParser();
 bool Load(const char *p_pFileName);
 bool Load(FILE *p_fFile);
 virtual bool OnBeforeFileOpen(const char *p_pFileName) = 0;
 virtual bool OnBeforeLoad(FILE *p_fFile) = 0;
 virtual bool OnLoaded() = 0;
 static bool ParseToStrList(char *p_pLine, char *p_pDest, short p_nDestCount, size_t p_nDestSize, const char *p_pDelimiters);
};
