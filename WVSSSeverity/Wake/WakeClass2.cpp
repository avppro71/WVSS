#include "stdafx.h"
#define _USE_MATH_DEFINES
#include "math.h"
#include "WakeClass2.h"
#include "GeographicLib/geodesic.hpp"
#include "GeographicLib/LocalCartesian.hpp"
#include "SensitivityAnalysis.h"
#include "Global.h"
#pragma warning(disable : 4996)
const double g_g = 9.81;
using namespace WVSS;
CEDRManager CWDRR::m_EDRManager;
CWDRR::CWDRR()
{
 m_pfProgress = NULL;
 m_fProcessedTotal = 1;
 m_fProcessed = 0;
 m_pAirplane = NULL;
 m_pGeoid = NULL; 
 m_dwBreakInterval = 10000000; 
 InitTimeToLink();
 m_pLC = new GeographicLib::LocalCartesian(0, 0, 0);
}
CWDRR::~CWDRR()
{
	Clear();
	if(m_pLC) delete m_pLC;
}
void CWDRR::Clear()
{
 m_Wake.clear();
}
void CWDRR::ADSEDC(CTrackPoint *p_pTP, bool p_bInsertAtFront)
{
	CWSNEC WakeSection;
 double ro, dSpeedMS, dWindDirectionRad, dWindSpeedMS, dWingSpan, dPlaneAltitudeM, dPlaneBankRad, dPlaneMassKG, dGroundElevation;
 bool bSensitivityOK;
 if(!m_pAirplane) return;
 memset(&WakeSection, 0, sizeof(WakeSection));
 dWingSpan = m_pAirplane->m_dWSM;
 dPlaneAltitudeM = p_pTP->m_nAltitudeM;
 dPlaneBankRad = Radians(p_pTP->m_dBankDeg);
 dPlaneMassKG = p_pTP->m_dWeightKg;
 if(!dPlaneMassKG)
   dPlaneMassKG = m_pAirplane->m_dMLMK;
 dWindSpeedMS = p_pTP->m_dWindSpeedMS;
 dWindDirectionRad = p_pTP->m_dWDRT;
 m_EarthElevation.GetElevation(p_pTP->m_dLatDeg, p_pTP->m_dLongDeg, &dGroundElevation);
#pragma message ("inverse filling of track point's evevation. Change so that track point's initial read fills elevation, and CWake only uses it")
 p_pTP->m_dElevation = dGroundElevation;
 bSensitivityOK = (m_pSensitivityAnalysisSetup != NULL);
 if(bSensitivityOK)
   bSensitivityOK = m_pSensitivityAnalysisSetup->ProperACAndSection(m_pAirplane->m_TN, m_Wake.size());
 if(bSensitivityOK) {
   m_pSensitivityAnalysisSetup->AdjustValue(&dWindSpeedMS, CSensitivityAnalysisSetup::AVP_SAVT_WIND_SPEED);
   m_pSensitivityAnalysisSetup->AdjustValue(&dWindDirectionRad, CSensitivityAnalysisSetup::AVP_SAVT_WIND_DIRECTION); }
 dWindDirectionRad -= M_PI / 2; 
 if(dWindDirectionRad < 0) dWindDirectionRad += 2 * M_PI; 
 if(bSensitivityOK) {
   m_pSensitivityAnalysisSetup->AdjustValue(&dWingSpan, CSensitivityAnalysisSetup::AVP_SAVT_WINGSPAN);
   m_pSensitivityAnalysisSetup->AdjustValue(&dPlaneAltitudeM, CSensitivityAnalysisSetup::AVP_SAVT_ALTITUDE);
   m_pSensitivityAnalysisSetup->AdjustValue(&dPlaneMassKG, CSensitivityAnalysisSetup::AVP_SAVT_MASS); }
 WakeSection.m_dIL = p_pTP->m_dLatDeg;
 WakeSection.m_dInitialLong = p_pTP->m_dLongDeg;
 WakeSection.m_dIA = dPlaneAltitudeM;
 WakeSection.m_dABr = dPlaneBankRad;
 WakeSection.m_dHd = p_pTP->m_dAzimuthDeg;
 double dPI = M_PI;
 WakeSection.psi = M_PI / 2 - p_pTP->m_dAzimuth; 
 if(WakeSection.psi < 0) WakeSection.psi += 2 * M_PI;
 WakeSection.m_dAMk = dPlaneMassKG; 
 WakeSection.rz0 = 0.035 * dWingSpan; 
 SystemTimeToLI(&p_pTP->m_CreationTime, &WakeSection.m_CTLE);
 WakeSection.m_CTST = p_pTP->m_CreationTime;
	WakeSection.WindH = 0;
	WakeSection.WindX = dWindSpeedMS * cos(dWindDirectionRad);
	WakeSection.WindZ = dWindSpeedMS * sin(dWindDirectionRad);
 ro = GetAirDensity(dPlaneAltitudeM);
 WakeSection.m_dIAD = ro; 
 dSpeedMS = sqrt(pow(p_pTP->m_dSpeedMS, 2) + pow(p_pTP->m_dVSpeedMS, 2)); 
 if(bSensitivityOK)
   m_pSensitivityAnalysisSetup->AdjustValue(&dSpeedMS, CSensitivityAnalysisSetup::AVP_SAVT_SPEED);
 WakeSection.m_dASMS = dSpeedMS; 
	m_b = M_PI * dWingSpan / 4;
	if(!dSpeedMS) {
   #pragma message("CWDRR: when point speed is 0, set m_dga to 0")
	  
	  WakeSection.m_dg0a = 0;
	  }
	else  
	  WakeSection.m_dg0a = dPlaneMassKG * g_g / (ro * max(dSpeedMS, m_pAirplane->m_dLSTK) * m_b);
 if(WakeSection.m_dg0a > 1000)
   WakeSection.m_dg0a = WakeSection.m_dg0a;
 if(bSensitivityOK)
   m_pSensitivityAnalysisSetup->AdjustValue(&WakeSection.m_dg0a, CSensitivityAnalysisSetup::AVP_SAVT_INITIAL_GAMMA);
	WakeSection.v0 = WakeSection.m_dg0a / (2 * M_PI * m_b);
	WakeSection.x0 = m_pAirplane->m_dWPXOM * cos(WakeSection.psi);
 WakeSection.z0 = m_pAirplane->m_dWPXOM * sin(WakeSection.psi);	
	WakeSection.z10 = m_b * cos(dPlaneBankRad) / 2;
	WakeSection.z20 = - m_b * cos(dPlaneBankRad) / 2;
	WakeSection.h10 = dPlaneAltitudeM + (m_b * sin(dPlaneBankRad)) / 2;
	WakeSection.h20 = dPlaneAltitudeM - (m_b * sin(dPlaneBankRad)) / 2;
#pragma message("T_link computation redundadncy")
 WakeSection.m_dga = WakeSection.m_dg0a;
 OGCED(&WakeSection);
	WakeSection.m_dGE = dGroundElevation;
	if(p_bInsertAtFront) m_Wake.insert(m_Wake.begin(), WakeSection);
	else m_Wake.push_back(WakeSection);
}
bool CWDRR::GS2EC(DWORD p_lIndex, LARGE_INTEGER p_liTime, CWS2 *p_pWakeSection, CWSNEC *p_pWakeSectionOrig )
{
 unsigned long ulOffset;
 if(!m_pAirplane || p_lIndex < 0 || p_lIndex >= m_Wake.size() || !p_pWakeSection) return(false);
 if(p_liTime.QuadPart < m_Wake[p_lIndex].m_CTLE.QuadPart)
   return(false);
 ulOffset = (long)((double)(p_liTime.QuadPart - m_Wake[p_lIndex].m_CTLE.QuadPart) / m_dwBreakInterval); 
 if(ulOffset + 1 > m_Wake[p_lIndex].m_ulHistoryRecCount) 
   return(false); 
	*p_pWakeSection = m_WHM.m_pRecords[m_Wake[p_lIndex].m_ulHistoryStartOrd + ulOffset];
	if(p_pWakeSectionOrig)
	  *p_pWakeSectionOrig = m_Wake[p_lIndex];
	return(true);
}
bool CWDRR::GetOriginalSection(DWORD p_lIndex, CWSNEC *p_pWakeSectionOrig)
{
 if(!m_pAirplane || p_lIndex < 0 || p_lIndex >= m_Wake.size()) return(false);
 *p_pWakeSectionOrig = m_Wake[p_lIndex];
	return(true);
}
void CWDRR::SATe(const CAPM::RAPL3 *p_pAirplane3)
{
 m_pAirplane = p_pAirplane3;
}
void CWDRR::InitTimeToLink()
{
	double eta, time;
	int j;
	memset(Time_of_eta, 0, sizeof(Time_of_eta));
	m_dt_eta = 0.001 * (0.2535 - 0.0121);
	for(j = 0; j <= 1001; j++) {
	  eta = 0.0121 + m_dt_eta * j;
	  time = CDPST(eta);
	  
	  Time_of_eta[j] = eta;
	  Time_of_eta[1002 + j] = time;
	  }
}
double CWDRR::GDPST(double p_dETA)
{
	int n0, m0, mf;
	double T;
	if(p_dETA <= 0.001)
		T = 9.0;
	else {
		 if(p_dETA <= 0.0121)
		 	 T = 9.18f - 180.0f * p_dETA;
		 else
		 	 if(p_dETA <= 0.2535) {
			 	  n0 = (int)((p_dETA - 0.0121 ) / m_dt_eta);
				   m0 = n0 + 1002;
				   mf = m0 + 1;
				   T = Time_of_eta[m0] + (Time_of_eta[mf] - Time_of_eta[m0]) *	(p_dETA - Time_of_eta[n0]) / m_dt_eta;
			    }
		   else
		     T = pow((0.7475 / p_dETA), 0.75);
	  }
	return T;
}
double CWDRR::CDPST(double p_dETA)
{
	int j;
 double T, Tl, Th, F, Fl, Fh, dT, error;
 if(p_dETA <= 0.001 )
   T = 9.0;
 else if(p_dETA <= 0.0121)
	  T = 9.18 - 180.0 * p_dETA;
 else if(p_dETA <= 0.2535) {
	  Th = 2.2;
	  Fh = pow(Th,0.25) * exp(-0.7 * Th) - p_dETA;
	  Tl = 7.1;
	  Fl = pow(Tl,0.25) * exp(-0.7 * Tl) - p_dETA;
	  dT = Th - Tl;
	  for(j = 0; j < 50; j++) {
   		T = Tl + dT * Fl / (Fl - Fh);
		   F = pow(T,0.25) * exp(-0.7 * T) - p_dETA;
     if(F < 0.0) {
   		  error = Tl - T;
		     Tl = T;
		     Fl = F; }
     else {
		     error = Th - T;
		     Th = T;
		     Fh = F; }
		   dT = Th - Tl;
		   if(fabs(error) < 1e-6 || F == 0.0) break;
		   }
	  }
 else
   T = pow(0.7475 / p_dETA, 0.75);
 return T;
}
double CWDRR::CDPSTPeter(double p_dETA)
{
 double T;
 double Th, Tl, Tc, fc;
 Th = 0;
 if(p_dETA <= 0.001 )
   T = 9.0;
 else if(p_dETA <= 0.0121)
   T = 9.18 - 180.0 * p_dETA;
 else if(p_dETA <= 0.2535) {
   Tl = 2.25;
   Th = 7;
   Tc = (Tl + Th) / 2;
   fc = pow(Tc, 0.25) * exp(-0.7 * Tc) - p_dETA;
   while(abs(fc) > 1e-6) {
     if(fc >= 0) {
       Tl = Tc;
       Tc = (Tl + Th) / 2; }
     else {
       Th = Tc;
       Tc = (Tl + Th) / 2; }
     fc = pow(Tc, 0.25) * exp(-0.7 * Tc) - p_dETA;
     }
   T = Tc; }
 else
   T = pow(0.7475 / p_dETA, 0.75);
 return(T);
}
float CWDRR::GetProcessedPercentage()
{
 return(m_fProcessed * m_fProcessedTotal);
}
bool CWDRR::OSAeD(const char *p_pFileName)
{
 unsigned long li, lj;
 DWORD dwSenseSectionNum;
 bool bVal, bThisOverridePlane = false;
 char tstr[256];
 FILE *fFile = NULL;
 CTest_ModelSensitivityOutput ModelSenseOutput;
 CTest_ModelSensitivityLine ModelSenseLine;
 CWS2 *pSection2;
 LARGE_INTEGER liTime;
 if(m_Wake.size() < 2) return(true);
 bThisOverridePlane = (m_pSensitivityAnalysisSetup != NULL);
 if(bThisOverridePlane)
   bThisOverridePlane = m_pSensitivityAnalysisSetup->ProperAC(m_pAirplane->m_TN);
 for(lj = 0; lj < m_Wake.size(); lj ++) {
   if(!(bVal = POSF(&m_Wake[lj], (float)lj / m_Wake.size())))
     return(false); }
 if(bThisOverridePlane && m_pSensitivityAnalysisSetup && m_pSensitivityAnalysisSetup->GetAssignedSectionNumber(&dwSenseSectionNum)) {
     liTime = m_Wake[dwSenseSectionNum].m_CTLE;
     for(lj = m_Wake[dwSenseSectionNum].m_ulHistoryStartOrd; lj < m_Wake[dwSenseSectionNum].m_ulHistoryStartOrd + m_Wake[dwSenseSectionNum].m_ulHistoryRecCount ; lj ++) {
       pSection2 = &m_WHM.m_pRecords[lj];
       ModelSenseLine.Reset();
       ModelSenseLine.m_lSectionNum = dwSenseSectionNum;
       ModelSenseLine.m_CTLE = m_Wake[dwSenseSectionNum].m_CTLE;
       ModelSenseLine.m_CurrentTimeLI = liTime;
       ModelSenseLine.m_dGamma = pSection2->m_dCiCu;
       ModelSenseLine.m_dX = pSection2->m_dX1;
       ModelSenseLine.m_dRightH = pSection2->m_dAltitude1;
       ModelSenseLine.m_dLeftH = pSection2->m_dAltitude2;
       ModelSenseLine.m_dZSize = pSection2->m_dHSe;
       ModelSenseLine.m_dHSize = pSection2->m_deVSe;
       ModelSenseLine.m_dEDR_star = pSection2->m_dEDR_star;
       ModelSenseLine.m_dEDR = pSection2->m_dEDR;
       ModelSenseLine.m_bUNC = pSection2->m_bUNC;
       ModelSenseLine.m_dGammaUpper = pSection2->m_dCiCuUpper;
       ModelSenseLine.m_dGammaLower = pSection2->m_dCiCuLower;
       ModelSenseLine.m_dRightHUpper = pSection2->m_dAltitude1Upper;
       ModelSenseLine.m_dLeftHUpper = pSection2->m_dAltitude2Upper;
       ModelSenseLine.m_dRightHLower = pSection2->m_dAltitude1Lower;
       ModelSenseLine.m_dLeftHLower = pSection2->m_dAltitude2Lower;
       ModelSenseOutput.AddLine(&ModelSenseLine);
       liTime.QuadPart += m_dwBreakInterval;
       }
     }
 if(ModelSenseOutput.m_Data.size()) {
   sprintf(tstr, " Sensitivity Analysis %s.csv", m_pAirplane->m_TN);
   if(fFile = OpenOutputFile2(p_pFileName, tstr, "wb", m_pSensitivityAnalysisSetup->m_bSensitivityAnalysis)) {
     fprintf(fFile, "Aircraft type %s;c1=%.2lf;c2=%.2lf;EDR=%.3lf\r\n", m_pAirplane->m_TN, m_FSG.m_AFs.c1, m_FSG.m_AFs.c2, m_FSG.m_AFs.eta);
     ModelSenseOutput.Write(fFile);
     fclose(fFile); } }
 bVal = true;
 for(li = 0; li < m_Wake.size(); li ++) {
   if(m_Wake[li].m_DissipationTimeLI.QuadPart == 0) {
     ASSERT(0);
     bVal = false; }
   }
 return(bVal);
}
bool CWDRR::CSRae(LARGE_INTEGER p_liTime, long *p_plStartSection, long *p_pnSectionCount)
{
 long li;
 unsigned long ulOffset;
 if(!p_pnSectionCount) return(false); 
	*p_pnSectionCount = 0;
 if(!m_pAirplane || !p_plStartSection) return(false); 
	if(*p_plStartSection >= (long)m_Wake.size()) *p_plStartSection = (long)m_Wake.size() - 1;
	if(*p_plStartSection < 0) return(true); 
 li = *p_plStartSection;
 for(;;) {
   if(p_liTime.QuadPart < m_Wake[li].m_CTLE.QuadPart)
     break; 
   ulOffset = (long)((double)(p_liTime.QuadPart - m_Wake[li].m_CTLE.QuadPart) / m_dwBreakInterval); 
 	 if(ulOffset + 1 <= m_Wake[li].m_ulHistoryRecCount) 
   	 (*p_pnSectionCount) ++;
 	 li --;
 	 if(li < 0) break;
   }
 return(true);
}
bool CWDRR::POSF(CWSNEC *p_pSection, float p_fFraction)
{
 double t, m_dga, k2, k1, delta_h, dz1, dz2, dh1, dh2, delta_h1, dz1CoU, dz2CoU;
 double GroundStart_x1, GroundStart_h1, GroundStart_x2, GroundStart_h2, GroundStart_z1, GroundStart_z2;
 double GroundLifeTime;
 bool DoGround = false;
 double a_of_t, d_of_t, delta_z_of_t, delta_y_of_t;
 double OffPlaneX, OffPlaneZ, OffPlaneH;
 double dDistanceBetweenWakes;
 GeographicLib::Math::real geoNewGeoH; 
 CWS2 WS;
 LARGE_INTEGER liTime;
 double dHeightAboveGround;
 bool bConeOfUncertainty;
 double dB;
 double c_eta_1, c_eta_1Upper, c_eta_1Lower;
 double c_eta_2, c_eta_2Upper, c_eta_2Lower;
 double alpha_str;
 double beta_str;
 double dStratIterationStep; 
 double EDR_current_calculated;
 double N_current_calculated, N_current_calculated_star, N_squared_current_calculated;
 double T_demise, T_demiseUpper, T_demiseLower, T_demise_star, T_demise_starUpper, T_demise_starLower;
 double f_dem, f_demUpper, f_demLower;
 double dD, dDUpper, dDLower;
 double K_w1;
 double k11, k21, k31, k41;
 double k12, k22, k32, k42;
 double k13, k23, k33, k43;
 CStratificationHelper StratHPrev, StratHThis, StratHNext;
 CStratificationHelper StratHPrevUpper, StratHThisUpper, StratHNextUpper;
 CStratificationHelper StratHPrevLower, StratHThisLower, StratHNextLower;
 short i, StratIterations;
 double dL; 
 dStratIterationStep = 0.2; 
 c_eta_1 = c_eta_1Upper = c_eta_1Lower = m_FSG.m_AFs.c1;
 c_eta_2 = c_eta_2Upper = c_eta_2Lower = m_FSG.m_AFs.c2;
 alpha_str = 0.45;
 beta_str = 0.8;
 dStratIterationStep = 0.2;
 dL = 110;
 StratIterations = (short)(m_dwBreakInterval / 10000000 / dStratIterationStep);
 dB = (1 - beta_str) * pow(alpha_str, 2) * 2 * M_PI * m_b; 
 p_pSection->m_ulHistoryStartOrd = m_WHM.m_lRecords;
 p_pSection->m_ulHistoryRecCount = 0;
 liTime = p_pSection->m_CTLE;
 p_pSection->h1 = p_pSection->h10;
 p_pSection->h2 = p_pSection->h20; 
 StratHPrev.Set(p_pSection->m_dg0a, p_pSection->m_dIA, -p_pSection->v0);
 StratHThis = StratHThisUpper = StratHThisLower = StratHPrevUpper = StratHPrevLower = StratHPrev;
 f_dem = f_demUpper = f_demLower = 0; 
 for(;;) { 
   bConeOfUncertainty = false; 
   p_pSection->m_dCurLifeTime = (double)(liTime.QuadPart - p_pSection->m_CTLE.QuadPart) / 10000000; 
   t = p_pSection->m_dCurLifeTime;
   if(m_EDRManager.GetEDR(p_pSection->h1, &EDR_current_calculated))
     StratHThis.m_dEDR_star = pow((EDR_current_calculated * m_b), (1.0/3)) / p_pSection->v0; 
   else
     StratHThis.m_dEDR_star = m_FSG.m_AFs.eta;
   T_demise_star = CDPSTPeter(StratHThis.m_dEDR_star); 
   T_demise = T_demise_star * m_b / p_pSection->v0; 
   if(!DoGround) {
     k1 = m_FSG.m_AFs.c1 / T_demise;
     if(t <= T_demise) {
       m_dga =  p_pSection->m_dg0a * exp(- k1 * t);
       delta_h = p_pSection->m_dg0a * (1 - exp( - k1 * t)) / k1 / (2 * M_PI * m_b);
       dz1 = p_pSection->z10 + delta_h * sin(p_pSection->m_dABr);
       dz2 = p_pSection->z20 + delta_h * sin(p_pSection->m_dABr);
       dh1 = p_pSection->h10 - delta_h * cos(p_pSection->m_dABr);
       dh2 = p_pSection->h20 - delta_h * cos(p_pSection->m_dABr);
       }
     else {
       k2 = m_FSG.m_AFs.c2/ T_demise;
       m_dga =  p_pSection->m_dg0a * exp(-m_FSG.m_AFs.c1 - k2 * (t - T_demise));
       delta_h = p_pSection->m_dg0a * exp(-m_FSG.m_AFs.c1) * (1 - exp( - k2 * (t - T_demise))) / k2 / (2 * M_PI * m_b);
       delta_h1 = p_pSection->m_dg0a * (1 - exp(-m_FSG.m_AFs.c1)) / k1 / (2 * M_PI * m_b);
       dz1 = p_pSection->z10 + (delta_h + delta_h1) * sin(p_pSection->m_dABr);
       dz2 = p_pSection->z20 + (delta_h + delta_h1) * sin(p_pSection->m_dABr);
       dh1 = p_pSection->h10 - (delta_h + delta_h1) * cos(p_pSection->m_dABr);
       dh2 = p_pSection->h20 - (delta_h + delta_h1) * cos(p_pSection->m_dABr);
       }
     p_pSection->m_dga = m_dga;
     p_pSection->h1 = dh1; 
     p_pSection->h2 = dh2; 
     if(m_FSG.m_bUSE && t) { 
       bConeOfUncertainty = true;
       for(i = 0; i < StratIterations; i ++) { 
         if(StratHThis.m_dZ < 0)
           continue;
         if(i) { 
           EDR_current_calculated = 0; 
           if(m_EDRManager.GetEDR(StratHThis.m_dZ, &EDR_current_calculated))
             StratHThis.m_dEDR_star = pow((EDR_current_calculated * m_b), (1.0/3)) / p_pSection->v0; 
           else
             StratHThis.m_dEDR_star = m_FSG.m_AFs.eta;
           T_demise_star = CDPSTPeter(StratHThis.m_dEDR_star); 
           T_demise = T_demise_star * m_b / p_pSection->v0; } 
         StratHThis.m_dAddToIntegralHelperVal = 2 * StratHThis.m_dEDR_star / p_pSection->v0 * pow(m_b / dL, -1.0/3);
         T_demise_starUpper = 1.2 * T_demise_star;
         T_demise_starLower = 0.8 * T_demise_star;
         T_demiseUpper = T_demise_starUpper * m_b / p_pSection->v0;
         T_demiseLower = T_demise_starLower * m_b / p_pSection->v0;
         N_squared_current_calculated = m_FSG.m_AFs.m_dStratification; 
         StratHThis.m_dN_squared_current_calculated = N_squared_current_calculated;
         StratHThisUpper.m_dN_squared_current_calculated = N_squared_current_calculated;
         StratHThisLower.m_dN_squared_current_calculated = N_squared_current_calculated;
	        
         StratHThis.m_dIntegral = StratHPrev.m_dIntegral + TrapzTwoVal(StratHThis.m_dN_squared_current_calculated, StratHPrev.m_dN_squared_current_calculated, StratHThis.m_dZ - StratHPrev.m_dZ);
         StratHThisUpper.m_dIntegral = StratHPrevUpper.m_dIntegral + TrapzTwoVal(StratHThisUpper.m_dN_squared_current_calculated, StratHPrevUpper.m_dN_squared_current_calculated, StratHThisUpper.m_dZ - StratHPrevUpper.m_dZ);
         StratHThisLower.m_dIntegral = StratHPrevLower.m_dIntegral + TrapzTwoVal(StratHThisLower.m_dN_squared_current_calculated, StratHPrevLower.m_dN_squared_current_calculated, StratHThisLower.m_dZ - StratHPrevLower.m_dZ);
         N_current_calculated = sqrt(N_squared_current_calculated);
         N_current_calculated_star = N_current_calculated * 2 * M_PI * pow(m_b, 2) / p_pSection->m_dg0a;
         f_dem = f_dem + dStratIterationStep / (T_demise_star * exp(-0.185 * N_current_calculated_star * T_demise_star)) * p_pSection->v0 / m_b;
         f_demUpper = f_demUpper + dStratIterationStep / (T_demise_starUpper * exp(-0.185 * N_current_calculated_star * T_demise_starUpper)) * p_pSection->v0 / m_b;
         f_demLower = f_demLower + dStratIterationStep / (T_demise_starLower * exp(-0.185 * N_current_calculated_star * T_demise_starLower)) * p_pSection->v0 / m_b;
         if(f_dem > 1)
           c_eta_1 = c_eta_2;
         if(f_demUpper > 1) 
           c_eta_1Upper = c_eta_2Upper;
         if(f_demLower > 1) 
           c_eta_1Lower = c_eta_2Lower;
         dD = c_eta_1 / (2 * M_PI * m_b * T_demise);
         dDUpper = c_eta_1Upper / (2 * M_PI * m_b * T_demiseUpper);
         dDLower = c_eta_1Lower / (2 * M_PI * m_b * T_demiseLower);
         K_w1 = c_eta_1 / T_demise;
         StratHThis.m_dAddToIntegral = StratHPrev.m_dAddToIntegral + TrapzTwoVal(StratHThis.m_dAddToIntegralHelperVal, StratHPrev.m_dAddToIntegralHelperVal, dStratIterationStep);
         k11 = StratHThis.m_dP;
         k21 = StratHThis.m_dP;
         k31 = StratHThis.m_dP;
         k41 = StratHThis.m_dP;
         StratHNext.m_dZ = (k11 + 2 * k21 + 2 * k31 + k41) * dStratIterationStep / 6 + StratHThis.m_dZ;
         k12 = dD * StratHThis.m_dGamma - pow(alpha_str, 2) * StratHThis.m_dIntegral;
         k22 = dD * StratHThis.m_dGamma - pow(alpha_str, 2) * StratHThis.m_dIntegral;
         k32 = dD * StratHThis.m_dGamma - pow(alpha_str, 2) * StratHThis.m_dIntegral;
         k42 = dD * StratHThis.m_dGamma - pow(alpha_str, 2) * StratHThis.m_dIntegral;
         StratHNext.m_dP = (k12 + 2 * k22 + 2 * k32 + k42) * dStratIterationStep / 6 + StratHThis.m_dP;
         k13 = dB * StratHThis.m_dIntegral -  2 * M_PI * m_b * dD * StratHThis.m_dGamma;
         k23 = dB * StratHThis.m_dIntegral - 2 * M_PI * m_b * dD * (StratHThis.m_dGamma + k13 * dStratIterationStep / 2);
         k33 = dB * StratHThis.m_dIntegral -  2 * M_PI * m_b * dD * (StratHThis.m_dGamma + k23 * dStratIterationStep / 2);
         k43 = dB * StratHThis.m_dIntegral - 2 * M_PI * m_b * dD * (StratHThis.m_dGamma + k33 * dStratIterationStep);
         StratHNext.m_dGamma = (k13 + 2 * k23 + 2 * k33 + k43) * dStratIterationStep / 6 + StratHThis.m_dGamma;
         k11 = StratHThisUpper.m_dP;
         k21 = StratHThisUpper.m_dP;
         k31 = StratHThisUpper.m_dP;
         k41 = StratHThisUpper.m_dP;
         StratHNextUpper.m_dZ = (k11 + 2 * k21 + 2 * k31 + k41) * dStratIterationStep / 6 + StratHThisUpper.m_dZ;
         k12 = dDUpper * StratHThisUpper.m_dGamma - pow(alpha_str, 2) * StratHThisUpper.m_dIntegral;
         k22 = dDUpper * StratHThisUpper.m_dGamma - pow(alpha_str, 2) * StratHThisUpper.m_dIntegral;
         k32 = dDUpper * StratHThisUpper.m_dGamma - pow(alpha_str, 2) * StratHThisUpper.m_dIntegral;
         k42 = dDUpper * StratHThisUpper.m_dGamma - pow(alpha_str, 2) * StratHThisUpper.m_dIntegral;
         StratHNextUpper.m_dP = (k12 + 2 * k22 + 2 * k32 + k42) * dStratIterationStep / 6 + StratHThisUpper.m_dP;
         k13 = dB * StratHThisUpper.m_dIntegral -  2 * M_PI * m_b * dDUpper * StratHThisUpper.m_dGamma;
         k23 = dB * StratHThisUpper.m_dIntegral - 2 * M_PI * m_b * dDUpper * (StratHThisUpper.m_dGamma + k13 * dStratIterationStep / 2);
         k33 = dB * StratHThisUpper.m_dIntegral -  2 * M_PI * m_b * dDUpper * (StratHThisUpper.m_dGamma + k23 * dStratIterationStep / 2);
         k43 = dB * StratHThisUpper.m_dIntegral - 2 * M_PI * m_b * dDUpper * (StratHThisUpper.m_dGamma + k33 * dStratIterationStep);
         StratHNextUpper.m_dGamma = (k13 + 2 * k23 + 2 * k33 + k43) * dStratIterationStep / 6 + StratHThisUpper.m_dGamma;
         k11 = StratHThisLower.m_dP;
         k21 = StratHThisLower.m_dP;
         k31 = StratHThisLower.m_dP;
         k41 = StratHThisLower.m_dP;
         StratHNextLower.m_dZ = (k11 + 2 * k21 + 2 * k31 + k41) * dStratIterationStep / 6 + StratHThisLower.m_dZ;
         k12 = dDLower * StratHThisLower.m_dGamma - pow(alpha_str, 2) * StratHThisLower.m_dIntegral;
         k22 = dDLower * StratHThisLower.m_dGamma - pow(alpha_str, 2) * StratHThisLower.m_dIntegral;
         k32 = dDLower * StratHThisLower.m_dGamma - pow(alpha_str, 2) * StratHThisLower.m_dIntegral;
         k42 = dDLower * StratHThisLower.m_dGamma - pow(alpha_str, 2) * StratHThisLower.m_dIntegral;
         StratHNextLower.m_dP = (k12 + 2 * k22 + 2 * k32 + k42) * dStratIterationStep / 6 + StratHThisLower.m_dP;
         k13 = dB * StratHThisLower.m_dIntegral -  2 * M_PI * m_b * dDLower * StratHThisLower.m_dGamma;
         k23 = dB * StratHThisLower.m_dIntegral - 2 * M_PI * m_b * dDLower * (StratHThisLower.m_dGamma + k13 * dStratIterationStep / 2);
         k33 = dB * StratHThisLower.m_dIntegral -  2 * M_PI * m_b * dDLower * (StratHThisLower.m_dGamma + k23 * dStratIterationStep / 2);
         k43 = dB * StratHThisLower.m_dIntegral - 2 * M_PI * m_b * dDLower * (StratHThisLower.m_dGamma + k33 * dStratIterationStep);
         StratHNextLower.m_dGamma = (k13 + 2 * k23 + 2 * k33 + k43) * dStratIterationStep / 6 + StratHThisLower.m_dGamma;
         StratHPrev = StratHThis;
         StratHThis = StratHNext;
         StratHPrevUpper = StratHThisUpper;
         StratHThisUpper = StratHNextUpper;
         StratHPrevLower = StratHThisLower;
         StratHThisLower = StratHNextLower;
         } 
       p_pSection->m_dga = StratHThis.m_dGamma;
	      p_pSection->h1 = StratHThis.m_dZ;
	      p_pSection->h2 = StratHThis.m_dZ;
       } 
     else { 
       StratHPrev.m_dEDR_star = StratHThis.m_dEDR_star; 
       StratHPrev.m_dAddToIntegralHelperVal = StratHThis.m_dAddToIntegralHelperVal;
       }
     p_pSection->x1 = p_pSection->x0 + dz1 * sin(p_pSection->psi);
     p_pSection->x2 = p_pSection->x0 + dz2 * sin(p_pSection->psi);
     p_pSection->z1 = p_pSection->z0 + dz1 * cos(p_pSection->psi);
     p_pSection->z2 = p_pSection->z0 + dz2 * cos(p_pSection->psi);
     p_pSection->x1 += p_pSection->WindX * t;
     p_pSection->x2 += p_pSection->WindX * t;
     p_pSection->z1 += p_pSection->WindZ * t;
     p_pSection->z2 += p_pSection->WindZ * t;
     p_pSection->h1 += p_pSection->WindH * t;
     p_pSection->h2 += p_pSection->WindH * t;
     } 
   else { 
     GroundLifeTime = (double)(liTime.QuadPart - p_pSection->m_Ground_CreationTimeLI.QuadPart) / 10000000;
     a_of_t = p_pSection->m_dGround_c1 / (4 * M_PI) * p_pSection->m_dGroundStart_m_dga * GroundLifeTime + p_pSection->m_dGround_c2;
     d_of_t = (a_of_t + sqrt(4 + a_of_t * a_of_t)) / 2;
     delta_z_of_t = sqrt(1 + d_of_t * d_of_t) / sqrt(p_pSection->m_dGround_c1);
     delta_y_of_t = (1 / d_of_t) * sqrt ((1 + d_of_t * d_of_t) / p_pSection->m_dGround_c1);
     p_pSection->x1 = p_pSection->m_dGroundStart_CenterX + p_pSection->WindX * GroundLifeTime + delta_z_of_t * sin(p_pSection->psi); 
     p_pSection->x2 = p_pSection->m_dGroundStart_CenterX + p_pSection->WindX * GroundLifeTime - delta_z_of_t * sin(p_pSection->psi); 
     p_pSection->z1 = p_pSection->m_dGroundStart_CenterZ + delta_z_of_t * cos(p_pSection->psi); 
     p_pSection->z2 = p_pSection->m_dGroundStart_CenterZ - delta_z_of_t * cos(p_pSection->psi); 
     p_pSection->h1 = delta_y_of_t + p_pSection->m_dGE;
     p_pSection->h2 = delta_y_of_t + p_pSection->m_dGE;
     if(t <= T_demise) {
		     
		     k1 = m_FSG.m_AFs.c1 / T_demise;
		     m_dga =  p_pSection->m_dg0a * exp(- k1 * t); }
     else {
		     
		     k2 = m_FSG.m_AFs.c2 / T_demise;
		     m_dga =  p_pSection->m_dg0a * exp(-m_FSG.m_AFs.c1 - k2 * (t - T_demise)); }
     p_pSection->m_dga = m_dga;
     } 
   dDistanceBetweenWakes = GDBEW(p_pSection); 
   dHeightAboveGround = max(p_pSection->h1 - p_pSection->m_dGE, 0); 
   if(m_FSG.m_bGE && !DoGround && dHeightAboveGround <= 2 * dDistanceBetweenWakes) {
     p_pSection->m_Ground_CreationTimeLI = liTime;
     p_pSection->m_dGroundStart_m_dga = p_pSection->m_dga;
     OffPlaneX = abs((p_pSection->x1 - p_pSection->x2) / 2); 
     OffPlaneZ = abs((p_pSection->z1 - p_pSection->z2) / 2); 
     OffPlaneH = abs((p_pSection->h1 + p_pSection->h2) / 2 - p_pSection->m_dGE);
     p_pSection->m_dGroundStart_CenterX = (p_pSection->x1 + p_pSection->x2) / 2;
     p_pSection->m_dGroundStart_CenterZ = (p_pSection->z1 + p_pSection->z2) / 2;
     GroundStart_x1 = GroundStart_x2 = OffPlaneX;
     GroundStart_z1 = GroundStart_z2 = OffPlaneZ;
     GroundStart_h1 = GroundStart_h2 = p_pSection->h1 - p_pSection->m_dGE;
  	  p_pSection->m_dGround_c1 = 1 / (GroundStart_x1 * GroundStart_x1) + 1 / (GroundStart_h1 * GroundStart_h1);
	    p_pSection->m_dGround_c2 = ((GroundStart_x1 * GroundStart_x1) - (GroundStart_h1 * GroundStart_h1)) / (GroundStart_x1 * GroundStart_h1);
     DoGround = true;
     }
   OGCED(p_pSection);
   if(p_pSection->m_dga < m_FSG.m_fMG) { 
     p_pSection->m_DissipationTimeLI = liTime;
     LIToSystemTime(&p_pSection->m_DissipationTimeLI, &p_pSection->m_DissipationTimeST);
     break;
     }
   p_pSection->rz = p_pSection->rz0 + 1 * sqrt(p_pSection->m_dCurLifeTime);
   p_pSection->rh = p_pSection->rz;
   m_pLC->Reset(p_pSection->m_dIL, p_pSection->m_dInitialLong, p_pSection->m_dIA);
   m_pLC->Reverse(p_pSection->x1, -p_pSection->z1, p_pSection->h1 - p_pSection->m_dIA, p_pSection->m_dCurLat1, p_pSection->m_dCurLong1, geoNewGeoH);
   m_pLC->Reverse(p_pSection->x2, -p_pSection->z2, p_pSection->h2 - p_pSection->m_dIA, p_pSection->m_dCurLat2, p_pSection->m_dCurLong2, geoNewGeoH);
   WS.Reset();
   WS.m_liTime = liTime;
   WS.m_dCiCu = p_pSection->m_dga;
   WS.m_dX1 = p_pSection->x1;
   WS.m_dZ1 = p_pSection->z1;
   WS.m_dX2 = p_pSection->x2;
   WS.m_dZ2 = p_pSection->z2;
   WS.m_dLT1d = p_pSection->m_dCurLat1;
   WS.m_sLL1DT = p_pSection->m_dCurLong1;
   WS.m_dL2TD = p_pSection->m_dCurLat2;
   WS.m_dLLD2 = p_pSection->m_dCurLong2;
   WS.m_dAltitude1 = p_pSection->h1;
   WS.m_dAltitude2 = p_pSection->h2;
   WS.m_dHSe = p_pSection->rz;
   WS.m_deVSe = p_pSection->rh;
   WS.m_dHd = p_pSection->m_dHd;
   WS.m_dBdK = Degrees(p_pSection->m_dABr);
   WS.m_cDangerLevel = p_pSection->m_cDangerLevel;
   WS.m_dEDR_star = StratHPrev.m_dEDR_star;
   WS.m_dEDR = pow(WS.m_dEDR_star * p_pSection->v0, 3) / m_b; 
   WS.m_bUNC = bConeOfUncertainty;
   WS.m_dCiCuUpper = StratHThisUpper.m_dGamma + 0.2 * p_pSection->m_dg0a;
   WS.m_dAltitude1Upper = StratHThisUpper.m_dZ + m_b + StratHPrev.m_dAddToIntegral + p_pSection->WindH * t;
   WS.m_dAltitude2Upper = StratHThisUpper.m_dZ + m_b + StratHPrev.m_dAddToIntegral + p_pSection->WindH * t;
   WS.m_dCiCuLower = StratHThisLower.m_dGamma - 0.2 * p_pSection->m_dg0a;
   WS.m_dAltitude1Lower = StratHThisLower.m_dZ - m_b - StratHPrev.m_dAddToIntegral + p_pSection->WindH * t;
   WS.m_dAltitude2Lower = StratHThisLower.m_dZ - m_b - StratHPrev.m_dAddToIntegral + p_pSection->WindH * t;
   WS.m_dHSeCoU = m_b + StratHPrev.m_dAddToIntegral;
   dz1CoU = dz1 + WS.m_dHSeCoU;
   WS.m_dX1Lefter = p_pSection->x1;
   WS.m_dZ1Lefter = p_pSection->z1;
   WS.m_dX1Righter = p_pSection->x0 + dz1CoU * sin(p_pSection->psi) + p_pSection->WindX * t;
   WS.m_dZ1Righter = p_pSection->z0 + dz1CoU * cos(p_pSection->psi) + p_pSection->WindZ * t;
   dz2CoU = dz2 - WS.m_dHSeCoU;
   WS.m_dX2Lefter = p_pSection->x0 + dz2CoU * sin(p_pSection->psi) + p_pSection->WindX * t;
   WS.m_dZ2Lefter = p_pSection->z0 + dz2CoU * cos(p_pSection->psi) + p_pSection->WindZ * t;
   WS.m_dX2Righter = p_pSection->x2;
   WS.m_dZ2Righter = p_pSection->z2;
   m_pLC->Reverse(p_pSection->x1, -p_pSection->z1, WS.m_dAltitude1Upper - p_pSection->m_dIA, WS.m_dLT1dUpper, WS.m_sLL1DTUpper, geoNewGeoH);
   m_pLC->Reverse(p_pSection->x2, -p_pSection->z2, WS.m_dAltitude2Upper - p_pSection->m_dIA, WS.m_dL2TDUpper, WS.m_dLLD2Upper, geoNewGeoH);
   m_pLC->Reverse(p_pSection->x1, -p_pSection->z1, WS.m_dAltitude1Lower - p_pSection->m_dIA, WS.m_dLT1dLower, WS.m_sLL1DTLower, geoNewGeoH);
   m_pLC->Reverse(p_pSection->x2, -p_pSection->z2, WS.m_dAltitude2Lower - p_pSection->m_dIA, WS.m_dL2TDLower, WS.m_dLLD2Lower, geoNewGeoH);
   m_pLC->Reverse(WS.m_dX1Lefter, -WS.m_dZ1Lefter, WS.m_dAltitude1 - p_pSection->m_dIA, WS.m_dLT1dLefter, WS.m_sLL1DTLefter, geoNewGeoH);
   m_pLC->Reverse(WS.m_dX1Righter, -WS.m_dZ1Righter, WS.m_dAltitude1 - p_pSection->m_dIA, WS.m_dLT1dRighter, WS.m_sLL1DTRighter, geoNewGeoH);
   m_pLC->Reverse(WS.m_dX2Lefter, -WS.m_dZ2Lefter, WS.m_dAltitude2 - p_pSection->m_dIA, WS.m_dL2TDLefter, WS.m_dLLD2Lefter, geoNewGeoH);
   m_pLC->Reverse(WS.m_dX2Righter, -WS.m_dZ2Righter, WS.m_dAltitude2 - p_pSection->m_dIA, WS.m_dL2TDRighter, WS.m_dLLD2Righter, geoNewGeoH);
   if(!m_WHM.AddRecord(&WS, p_fFraction)) {
     strcpy_s(m_ErrorStr, sizeof(m_ErrorStr), m_WHM.m_ErrorStr);
     return(false); }
   p_pSection->m_ulHistoryRecCount ++;
   liTime.QuadPart += m_dwBreakInterval;
   }
 return(true);
}
void CWDRR::OGCED(CWSNEC *p_pSection)
{
 p_pSection->m_cDangerLevel = 3; 
 if(p_pSection->m_dga >= m_FSG.m_fMG) p_pSection->m_cDangerLevel = 2;
 if(p_pSection->m_dga >= 220)	p_pSection->m_cDangerLevel = 1;
 if(p_pSection->m_dga >= 300)	p_pSection->m_cDangerLevel = 0;
}
double CWDRR::GetAirDensity(double p_dAltitudeM)
{
 short i, Count;
 struct rDensity { unsigned short nAltitudeM; double dDensity; };
 rDensity Density[] = { {0, 1.225}, {50, 1.219}, {100, 1.213}, {200, 1.202}, {300, 1.190}, {500, 1.167}, {1000, 1.112}, {2000, 1.007}, {3000, 0.909}, {5000, 0.736}, {8000, 0.526}, {10000, 0.414}, {12000, 0.312}, {15000, 0.195}, {20000, 0.089}, {50000, 0.001027} };
 Count = sizeof(Density) / sizeof(rDensity);
 for(i = 0; i < Count; i ++) {
   if(p_dAltitudeM < Density[i].nAltitudeM) {
     if(!i) return(Density[0].dDensity);
     else return(Density[i - 1].dDensity + (p_dAltitudeM - Density[i - 1].nAltitudeM) / (Density[i].nAltitudeM - Density[i - 1].nAltitudeM) * (Density[i].dDensity - Density[i - 1].dDensity));
     }
   }
 return(Density[Count - 1].dDensity);
}
double CWDRR::GDBEW(CWSNEC *p_pSection)
{
 double xsq = (p_pSection->x1 - p_pSection->x2) * (p_pSection->x1 - p_pSection->x2);
 double zsq = (p_pSection->z1 - p_pSection->z2) * (p_pSection->z1 - p_pSection->z2);
 double hsq = (p_pSection->h1 - p_pSection->h2) * (p_pSection->h1 - p_pSection->h2);
 return(sqrt(xsq + zsq + hsq));
}
double CWDRR::GetMemoryUsedMB()
{
 unsigned long lVal = 0;
 double dValKB;
 dValKB = (double)sizeof(CWDRR) / 1024;
 dValKB += (double)(m_Wake.size() * sizeof(CWS2)) / 1024;
 dValKB += m_WHM.GetMemoryUsedKB();
 return(dValKB / 1024);
}
