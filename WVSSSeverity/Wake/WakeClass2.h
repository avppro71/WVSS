#pragma once
#include <vector>
#include "Airplanes2.h"
#include "IWVSSSeverity.h"
#include "EarthElevation.h"
#include "TrackPoint.h"
#include "Airplanes2.h"
#include "WakeHistory.h"
#include "EDRManager.h"
#if defined(WVSS_LIBRARY)
#define WVSSAPI   __declspec(dllexport)
#define WVSSAPIEX
#elif defined(WVSS_LIBRARYEX)
#define WVSSAPI   __declspec(dllexport)
#define WVSSAPIEX __declspec(dllexport)
#else 
#define WVSSAPI   __declspec(dllimport)
#define WVSSAPIEX __declspec(dllimport)
#endif
#define GEOGRAPHICLIB_SHARED_LIB 1
namespace GeographicLib {
class Geodesic;
class LocalCartesian;
class Geoid;
}
namespace WVSS
{
class CSensitivityAnalysisSetup;
class CWSNEC 
{
public:
	LARGE_INTEGER m_CTLE; 
	SYSTEMTIME m_CTST; 
	double m_dIL, m_dInitialLong; 
	double m_dIA;
	double m_dHd; 
	double m_dABr; 
	double m_dAMk; 
	double m_dIAD; 
	double m_dASMS; 
	double m_dGE; 
	double WindX; 
	double WindZ; 
	double WindH; 
	double x0;  
	double z0;  
	double z10; 
	double z20; 
	double h10; 
	double h20; 
	double rz0; 
	double x1;
	double z1;
	double h1; 
	double x2;
	double z2;
	double h2; 
	double rz, rh; 
	double m_dg0a; 
	double m_dga; 
	double v0; 
	double psi; 
	unsigned char m_cDangerLevel; 
	double m_dCurLifeTime; 
	bool m_bDissipated; 
	LARGE_INTEGER m_DissipationTimeLI; 
	SYSTEMTIME m_DissipationTimeST; 
	double m_dGroundStart_CenterX, m_dGroundStart_CenterZ;
	LARGE_INTEGER m_Ground_CreationTimeLI; 
	double m_dGroundStart_m_dga;
	double m_dGround_c1, m_dGround_c2;
	double m_dCurLat1, m_dCurLat2, m_dCurLong1, m_dCurLong2; 
	unsigned long m_ulHistoryStartOrd, m_ulHistoryRecCount;
};
class CStratificationHelper
{
public:
 double m_dGamma, m_dZ, m_dP;
 double m_dIntegral, m_dN_squared_current_calculated, m_dAddToIntegral, m_dEDR_star, m_dAddToIntegralHelperVal;
public:
 CStratificationHelper() { Reset(); }
 CStratificationHelper(double p_dGamma, double p_dZ, double p_dP) { Reset(); Set(p_dGamma, p_dZ, p_dP); }
 void Set(double p_dGamma, double p_dZ, double p_dP) { m_dGamma = p_dGamma; m_dZ = p_dZ; m_dP = p_dP; }
 ~CStratificationHelper() { }
 void Reset() { m_dGamma = m_dZ = m_dP = m_dIntegral = m_dN_squared_current_calculated = m_dAddToIntegral = m_dEDR_star = m_dAddToIntegralHelperVal = 0; }
};
class WVSSAPIEX CWDRR
{
private:
	std::vector<CWSNEC> m_Wake; 
	LARGE_INTEGER m_liMaxTime;
	double m_b; 
	double m_dt_eta;	
	double Time_of_eta[2004]; 
 float m_fProcessed;
 CEarthElevation m_EarthElevation;
 CWEAIF m_FSG;
 CWSHM m_WHM; 
 DWORD m_dwBreakInterval; 
public:
 enum SectionProcMode { DEFAULT, PREPARATION, NOGROUND, WITHGROUND };
 void (* m_pfProgress)(float);
 float m_fProcessedTotal; 
 const CAPM::RAPL3 *m_pAirplane;
 CSensitivityAnalysisSetup *m_pSensitivityAnalysisSetup;
 GeographicLib::LocalCartesian *m_pLC; 
 const GeographicLib::Geoid *m_pGeoid; 
 static CEDRManager m_EDRManager;
 char m_ErrorStr[256];
private:
	void InitTimeToLink();
	double CDPST(double p_dETA);
	double CDPSTPeter(double p_dETA);
	double GDPST(double p_dETA);
	bool POSF(CWSNEC *p_pSection, float p_fFraction);
 void OGCED(CWSNEC *p_pSection);
 double GDBEW(CWSNEC *p_pSection);
public:
	CWDRR();
	~CWDRR();
	void ADSEDC(CTrackPoint *p_pTP, bool p_bInsertAtFront); 
	int GNOS() { return m_Wake.size(); }
	bool GS2EC(DWORD p_lIndex, LARGE_INTEGER p_liTime, CWS2 *p_pWakeSection, CWSNEC *p_pWakeSectionOrig = NULL);
	bool GetOriginalSection(DWORD p_lIndex, CWSNEC *p_pWakeSectionOrig); 
	void SATe(const CAPM::RAPL3 *p_pAirplane3);
 void Clear();
 const std::map<CString, bool> *GetMissingElevationAreas() { return(m_EarthElevation.GetMissingElevationAreas()); }
 float GetProcessedPercentage();
 bool OSAeD(const char *p_pFileName);
	bool CSRae(LARGE_INTEGER p_liTime, long *p_plStartSection, long *p_pnSectionCount);
	static double GetAirDensity(double p_dAltitudeM);
	void SetFlags(CWEAIF *p_pFlags) { m_FSG = *p_pFlags; m_EarthElevation.SetFlags(p_pFlags); }
	void ClearMissingElevationAreaLists() { m_EarthElevation.ClearMissingElevationAreaLists(); }
	void SetBreakInterval(DWORD p_dwBreakInterval) { m_dwBreakInterval = p_dwBreakInterval; }
	DWORD GetBreakInterval() { return(m_dwBreakInterval); }
	double GetMemoryUsedMB();
	const CWSHM *GetSectionHistoryManager() { return(&m_WHM); }
};
} 
