#pragma once
#include "Airplanes2.h"
#include "IWVSSSeverity.h"
#include "Matrix.h"
#include "WakeClass2.h"
namespace GeographicLib {
class Geodesic;
class LocalCartesian;
class Geoid;
}
namespace WVSS
{
class CSensitivityAnalysisSetup;
class CADAA;
class CTrackPoint;
class CWC2
{
public:
 LONGLONG m_llTTt;
 CADAA *m_pGA;
 CADAA *m_pEE;
 double m_dEALD, m_dEALLD, m_dEAMH;
 double m_dAGLD, m_dAGLlD, m_dGAHM;
 double m_dGFDM;
 double m_dEAHD;
 double m_dEtAfLCX, m_dEtAfLCZ, m_dEtAfLCH;
 double m_dEtAfLWtpiLCX, m_dEtAfLWtpiLCZ, m_dEtAfLWtpiLCH;
 double m_dEtAfRWtpiLCX, m_dEtAfRWtpiLCZ, m_dEtAfRWtpiLCH;
 double m_dEtAfLWtpiLatDeg, m_dEtAfLWtpiLongDeg, m_dEtAfLWtpiGeoH_m;
 double m_dEtAfRWtpiLatDeg, m_dEtAfRWtpiLongDeg, m_dEtAfRWtpiGeoH_m;
 CWS2 m_WSNWE[3];
 bool m_bWSOKP[3]; 
 double m_dDTWoa[2]; 
 double m_dCSAloa; 
 DWORD m_dwSN; 
 bool m_bOK; 
 bool m_bRWakeOK; 
 double m_dRX, m_dRZ, m_dRH; 
 double m_dRLat, m_dRLong, m_dRGeoH; 
 double m_dRVertExtent, m_dRHorzExtent, m_dRHeadingDeg, m_dRDistance; 
 bool m_bLWakeOK; 
 double m_dLX, m_dLZ, m_dLH; 
 double m_dLLat, m_dLLong, m_dLGeoH; 
 double m_dLVertExtent, m_dLHorzExtent, m_dLHeadingDeg, m_dLDistance; 
 double m_dCX, m_dCZ, m_dCH, m_dCLat, m_dCLong, m_dCGeoH;
 double m_dDBWC;
 double m_dPTCWCD;
 double m_dCARd;
 double m_dTFLS;
 double m_dRGt, m_dLtG;
 double m_dPVCSRoeRad, m_dPVPHCtiRad, m_dPVBankRad;
 double m_dPVXOffset, m_dPVZOffset, m_dPVHOffset;
 double m_dVertOffsetUp, m_dVertOffsetDown;
 double m_dLWakeUSRad, m_dRWakeUSRad;
public:
 CWC2() { m_bOK = m_bRWakeOK = m_bLWakeOK = false; }
 ~CWC2() { }
};
struct rAircraftPosition
{
 double x_m, z_m, h_m;
 double m_dPDict;
 double m_dBdK;
 double m_dHd;
 double m_sWSMig;
};
class CWESC
{
private:
 GeographicLib::LocalCartesian *m_pLC;
protected:
public:
 CWEAIF m_FSG;
 CWC2 m_WCSS;
 CSensitivityAnalysisSetup *m_pSensitivityAnalysisSetup;
 std::vector<CADAA *>(*m_pAircraft);
 std::vector<CSOC>m_CSSO;
 void (*m_pfMessage)(const char *);
 void (*m_pfUAP)(rAircraftPosition *p_pLAData, short p_nAircraftOrdinal, CTrackPoint *p_pTP);
 char m_FilePrefixWithExt[MAX_PATH]; 
 DWORD m_lFrame; 
private:
 void camcxyfw(double *p_mx_wake, double *p_cy_wake, double p_Bank, double p_DeltaZ, double p_DeltaH, double p_dCurAngle, double p_dGamma, const CAPM::RAPL3 *p_pEnteringPlaneParams, double p_dSpeedMS, double *p_pd_vw_y, double *p_pd_vw_z);
 void CTmatf(CMatrix *p_pViewMatrix, CWC2 *p_pWakeCross);
 void TTRSNF(double *p_pdX, double *p_pdZ, double *p_pdH, CMatrix *p_pMatrix);
 void MultMatrix(CMatrix *p_pM1, CMatrix *p_pM2, CMatrix *p_pMDest);
 void FCWP3(CTrackPoint *p_pTP);
 bool FPTWA2(const CAPM::RAPL3 *p_pPlaneParams);
 bool FillPlaneToWakeAttitude3(const CAPM::RAPL3 *p_pPlaneParams);
 void CSAELV(CTrackPoint *p_pTP, short p_nGeneratorAircraftOrdinal, short p_nEtAfOrdinal, const CAPM::RAPL3 *p_pEnteringPlaneParams);
protected:
public:
 CWESC();
 ~CWESC();
 void RSOtp();
 void Message(const char *p_pFormat, ...);
 void UAPuos(rAircraftPosition *p_pAircraftPosition, short p_nAircraftOrdinal, CTrackPoint *p_pTP);
 bool PWC3(CTrackPoint *p_pTP, short p_nGeneratorAircraftOrdinal, short p_nEtAfOrdinal);
 CWESC MakeSafeCopy(); 
};
} 
