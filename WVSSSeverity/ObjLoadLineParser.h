#pragma once
#include "LineParser.h"
#include "IWVSSSeverity.h"
#include "TrackPoint.h"
#include <list>
namespace WVSS
{
class CSeverity;
class CWVSSLoaderLineParserMITRE : public CLineParser
{
private:
protected:
 CTrackPoint m_TP;
 DWORD m_dwLinesProcessed;
public:
 CSeverity *m_pSeverity;
 std::list <CTrackPoint> *m_pTrackPoints;
private:
protected:
 virtual bool ProcessLine(char *p_pLine);
public:
 CWVSSLoaderLineParserMITRE();
 virtual ~CWVSSLoaderLineParserMITRE();
 virtual bool OnBeforeFileOpen(const char *p_pFileName);
 virtual bool OnBeforeLoad(FILE *p_pFile);
 virtual bool OnLoaded();
 DWORD GetProcessedLineCount() { return(m_dwLinesProcessed); }
};
} 