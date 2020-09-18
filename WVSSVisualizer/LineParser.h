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
 virtual bool OnBeforeLoad(const char *p_pFileName) = 0;
 virtual bool OnLoaded() = 0;
};
