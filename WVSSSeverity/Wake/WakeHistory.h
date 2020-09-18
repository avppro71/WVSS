#pragma once
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
namespace WVSS
{
class WVSSAPIEX CWS2
{
public:
 LARGE_INTEGER m_liTime;
 double m_dCiCu; 
 double m_dX1, m_dZ1, m_dX2, m_dZ2; 
 double m_dLT1d, m_dL2TD, m_sLL1DT, m_dLLD2; 
 double m_dLT1dUpper, m_dL2TDUpper, m_sLL1DTUpper, m_dLLD2Upper; 
 double m_dLT1dLower, m_dL2TDLower, m_sLL1DTLower, m_dLLD2Lower;
 double m_dAltitude1, m_dAltitude2;
 double m_dHSe, m_deVSe; 
 double m_dHd; 
 double m_dBdK; 
 unsigned char m_cDangerLevel;
 double m_dEDR_star; 
 double m_dEDR; 
 bool m_bUNC;
 double m_dHSeCoU; 
 double m_dCiCuUpper;
 double m_dAltitude1Upper, m_dAltitude2Upper;
 double m_dCiCuLower;
 double m_dAltitude1Lower, m_dAltitude2Lower;
 double m_dX1Lefter, m_dX2Lefter, m_dZ1Lefter, m_dZ2Lefter; 
 double m_dX1Righter, m_dX2Righter, m_dZ1Righter, m_dZ2Righter; 
 double m_dLT1dLefter, m_dL2TDLefter, m_sLL1DTLefter, m_dLLD2Lefter; 
 double m_dLT1dRighter, m_dL2TDRighter, m_sLL1DTRighter, m_dLLD2Righter;
public:
 CWS2() { Reset(); }
 void Reset();
};
class CWSHM
{
public:
 CWS2 *m_pRecords;
 unsigned long m_lRecords, m_lAvlRecords;
 unsigned long m_lMemReallocCount;
 char m_ErrorStr[256];
private:
 void Reset();
public:
 CWSHM();
 ~CWSHM();
 bool AddRecord(CWS2 *p_pRec, float p_fFraction);
 double GetMemoryUsedKB();
};
} 