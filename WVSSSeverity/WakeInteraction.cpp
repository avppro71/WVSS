#include "stdafx.h"
#include "WakeInteraction.h"
#include "Severity.h"
#define _USE_MATH_DEFINES
#include "math.h"
#include "global.h"
#include "GeometricVector.h"
#include "GeographicLib/geodesic.hpp"
#include "GeographicLib/LocalCartesian.hpp"
#pragma warning(disable : 4996)
using namespace WVSS;
CWESC::CWESC()
{
 m_pAircraft = NULL;
 m_pfMessage = NULL;
 m_pfUAP = NULL;
 memset(m_FilePrefixWithExt, 0, sizeof(m_FilePrefixWithExt));
 m_pSensitivityAnalysisSetup = NULL;
 m_pLC = new GeographicLib::LocalCartesian(0, 0, 0);
 m_lFrame = 0;
 RSOtp();
}
CWESC::~CWESC()
{
 if(m_pLC) delete m_pLC;
}
void CWESC::RSOtp()
{
 m_CSSO.clear();
}
void CWESC::CSAELV(CTrackPoint *p_pTP, short p_nGeneratorAircraftOrdinal, short p_nEtAfOrdinal, const CAPM::RAPL3 *p_pEnteringPlaneParams)
{
 char tstr[512];
 double dLocalPlaneX, dLocalPlaneZ, dLocalPlaneH, dStartPlaneZ, dStartPlaneH, dStartClosingAngleRad;
 long lWaitTime;
 rAircraftPosition LAData;
 CSOC SeverityOutput;
 FILETIME FT;
 LARGE_INTEGER LI;
 FILE *fFile = NULL;
 CMatrix ViewMatrix;
 bool bVal;
 double X[14]; 
 double dH_max; 
 double G_max; 
 double G_p_max; 
 double mx_wake_max = AVP_BAD_WX_WAKE_VALUE; 
 short i;
 double t;
 double A_flap = 0; 
 double mx_wake, cy_wake;
 double dt = 0.01;
 double F[9];
 double ro = CWDRR::GetAirDensity(m_WCSS.m_dEAMH); 
 short PilotResponse = m_FSG.m_nPR;
 double T_pilot = 0.2; 
 double L = p_pEnteringPlaneParams->m_dWSM; 
 double J_xx = p_pEnteringPlaneParams->m_sXXJI; 
 double S = p_pEnteringPlaneParams->m_dWAM; 
 double mx_flap = -0.14; 
 double G_lim = 10 * M_PI / 180; 
 double G_p_lim = 15 * M_PI / 180; 
 double I_lim;
 double mx_wx = p_pEnteringPlaneParams->m_dMWXX; 
 double T_lim;
 double A_flap_max = 30 * M_PI / 180;
 double T_flap = 0.5;
 double dStartLocalCSRoe; 
 double vw_y, vw_z, vw_y_max, vw_z_max, vw_combined_max, vw_y_first, vw_z_first;
 double dVal;
 double dWakeGamma = 0;
 if(!m_pAircraft) return;
 dStartClosingAngleRad = m_WCSS.m_dCARd;
 dStartPlaneZ = m_WCSS.m_dPTCWCD;
 dStartPlaneH = m_WCSS.m_dEtAfLCH - m_WCSS.m_dCH;
 dLocalPlaneX = 0;
 dLocalPlaneZ = 0;
 dLocalPlaneH = 0;
 if(m_FSG.m_bUCOU) {
   if(m_WCSS.m_dVertOffsetUp)
     m_WCSS.m_dVertOffsetUp = m_WCSS.m_dVertOffsetUp;
   if(dStartPlaneH > 0) {
     dStartPlaneH -= m_WCSS.m_dVertOffsetUp;
     if(dStartPlaneH < 0)
       dStartPlaneH = 0;
     }
   else if(dStartPlaneH < 0) {
     dStartPlaneH += m_WCSS.m_dVertOffsetDown;
     if(dStartPlaneH > 0)
       dStartPlaneH = 0;
     }
   }
 sprintf(tstr, "Closing angle %.1f deg, dZ=%.1f, dH=%.1f, m_dga(l/r)=%.0lf/%.0lf",
         Degrees(m_WCSS.m_dCARd), 
         m_WCSS.m_dEtAfLCZ - m_WCSS.m_dCZ,
         m_WCSS.m_dEtAfLCH - m_WCSS.m_dCH, 
         m_WCSS.m_dLtG, m_WCSS.m_dRGt);
 Message(tstr);
 m_WCSS.m_dPVCSRoeRad = p_pTP->m_dAzimuth;
 m_WCSS.m_dPVPHCtiRad = p_pTP->m_dAttitudeAngleDeg * M_PI / 180;
 m_WCSS.m_dPVBankRad = 0;
 m_WCSS.m_dPVXOffset = 0;
 m_WCSS.m_dPVZOffset = 0;
 m_WCSS.m_dPVHOffset = 0;
 dWakeGamma = (m_WCSS.m_dRGt + m_WCSS.m_dLtG) / 2; 
 if(m_FSG.m_bOverrideProximityValues) {
   if(m_FSG.m_bStartPlaneHOverride)
     dStartPlaneH = m_FSG.m_dStartPlaneHOverride;
   if(m_FSG.m_bStartPlaneZOverride)
     dStartPlaneZ = m_FSG.m_dStartPlaneZOverride;
   if(m_FSG.m_bClosingAngleRadOverride)
     dStartClosingAngleRad = m_FSG.m_dCARdOverride;
   }
 else 
   if(!m_WCSS.m_bLWakeOK && !m_WCSS.m_bRWakeOK) return;
 if(m_FSG.m_bOverrideGamma)
   dWakeGamma = m_FSG.m_dWakeGammaOverrideValue;
 if(m_pSensitivityAnalysisSetup)
   m_pSensitivityAnalysisSetup->AdjustValue(&dWakeGamma, CSensitivityAnalysisSetup::AVP_SAVT_CURRENT_GAMMA);
 dStartLocalCSRoe = dStartClosingAngleRad;
 m_WCSS.m_dPVCSRoeRad += dStartLocalCSRoe;
 CTmatf(&ViewMatrix, &m_WCSS);
 dH_max = 0;
 G_max = 0;
 G_p_max = 0;
 fFile = OpenOutputFile2(m_FilePrefixWithExt, " XArray.csv", "wb", m_FSG.m_bCOF);
 if(fFile) {
   sprintf(tstr, "X1;X2;X3;X4;X5;X6;X7;X8;X9;X10;X11;X12;X13;mx_wake;vw_z;vw_y\r\n");
   fprintf(fFile, tstr); }
 SeverityOutput.m_dMaxAltitudeLossM = SeverityOutput.m_dMaxBankRad = SeverityOutput.m_dMaxAngularSpeedOfBankRadPerS = 0;
 SeverityOutput.m_lPlayerPos = m_lFrame; 
 *SeverityOutput.m_GeneratorCallSign = '\0';
 *SeverityOutput.m_FollowerCallSign = '\0';
 vw_y_max = AVP_BAD_VW_Y_VALUE;
 vw_z_max = AVP_BAD_VW_Z_VALUE;
 vw_combined_max = AVP_BAD_VW_VALUE;
 vw_y_first = AVP_BAD_VW_Y_VALUE;
 vw_z_first = AVP_BAD_VW_Z_VALUE;
 X[1] = 0; 
 X[2] = 0; 
 X[3] = 0; 
 X[4] = p_pTP->m_dSpeedMS; 
 X[5] = dStartClosingAngleRad; 
 X[6] = 0; 
 X[7] = 0; 
 X[8] = 0; 
 X[9] = 0; 
 X[10] = 0; 
 X[11] = 0; 
 X[12] = 0; 
 X[13] = 0; 
 for(t = 0;; t += dt) {
   if(!m_FSG.m_bFAS && t > m_FSG.m_dFT)
     break; 
   lWaitTime = GetTickCount();
   switch(PilotResponse) {
     case 0: A_flap = 0; break;
     case 1:
       if(abs(X[7]) < G_lim && abs(X[8]) < G_p_lim) {
         I_lim = 0;
         A_flap = 0;
         }
       else {
         if(!I_lim) {
           I_lim = 1;
           T_lim = t;
           }
         else {
           if(I_lim == 1 && t - T_lim >= T_pilot || I_lim == 2) {
             I_lim = 2;
             A_flap = A_flap_max;
             if(X[7] < 0)
               A_flap = -A_flap;
             }
           }
         }
       break;
     case 2:
       if(t > T_flap && abs(X[7]) > M_PI / 18) {
         A_flap = A_flap_max;
         if(X[7] < 0)
           A_flap = -A_flap;
         }
       else
         A_flap = 0;
       break;
     }
   camcxyfw(&mx_wake, &cy_wake, X[8], dStartPlaneZ + X[3], dStartPlaneH + X[2], X[5], dWakeGamma, p_pEnteringPlaneParams, X[4], &vw_y, &vw_z);
   if(t == 0) { vw_y_first = vw_y; vw_z_first = vw_z; }
   X[11] = mx_wake;
   X[12] = 0.5 * ro * X[4] * X[4] * S * cy_wake / (9.81 * p_pEnteringPlaneParams->m_dMLMK); 
   F[1] = X[4] * cos(X[5]) * cos(X[6]); 
   F[2] = X[4] * sin(X[6]); 
   F[3] = -X[4] * sin(X[5]) * cos(X[6]); 
   F[4] = -9.81 * sin(X[6]); 
   F[5] = -9.81 / (X[4] * cos(X[6])) * sin(X[7]) * (1 + X[12]); 
   F[6] = 9.81 / X[4] * (cos(X[7]) * (1 + X[12]) - cos(X[6])); 
   F[7] = X[8];
   F[8] = 0.5 * ro * X[4] * X[4] * S * L * (mx_wx * X[8] * L / X[4] - mx_wake + mx_flap * A_flap) / J_xx; 
   for(i = 1; i <= 8; i ++) X[i] = X[i] + F[i] * dt;
   X[9] = F[5];
   X[10] = F[6];
   X[13] = A_flap;
   if(abs(G_max) < abs(X[7]))
     G_max = X[7];
   if(abs(G_p_max) < abs(X[8]))
     G_p_max = X[8];
   if(abs(dH_max) < abs(X[2]))
     dH_max = X[2];
   if(mx_wake_max == AVP_BAD_WX_WAKE_VALUE || abs(mx_wake) > abs(mx_wake_max))
     mx_wake_max = mx_wake;
  dVal = sqrt(vw_y * vw_y + vw_z * vw_z);
   if(vw_combined_max == AVP_BAD_VW_VALUE || abs(dVal) > abs(vw_combined_max)) {
     vw_combined_max = dVal;
     vw_y_max = vw_y;
     vw_z_max = vw_z; }
   if(fFile) {
     sprintf(tstr, "%lf;%lf;%lf;%lf;%lf;%lf;%lf;%lf;%lf;%lf;%lf;%lf;%lf;%lf;%lf;%lf\r\n", X[1], X[2], X[3], X[4], X[5], X[6], X[7], X[8], X[9], X[10], X[11], X[12], X[13], mx_wake, vw_y_max, vw_z_max);
     fprintf(fFile, tstr);
     }
   dLocalPlaneX = X[1];
   dLocalPlaneZ = X[3];
   dLocalPlaneH = X[2];
   TTRSNF(&dLocalPlaneX, &dLocalPlaneZ, &dLocalPlaneH, &ViewMatrix);
   if(m_FSG.m_bEF) {
     LAData.x_m = m_WCSS.m_dEtAfLCX + dLocalPlaneZ;
     LAData.z_m = m_WCSS.m_dEtAfLCZ - dLocalPlaneX;
     LAData.h_m = m_WCSS.m_dEtAfLCH + dLocalPlaneH;
     LAData.m_dBdK = X[7] * 180 / M_PI;
     LAData.m_dPDict = X[6] * 180 / M_PI;
     LAData.m_dHd = (p_pTP->m_dAzimuth - (X[5] - dStartLocalCSRoe)) * 180 / M_PI - 90;
     if(LAData.m_dHd < 0) LAData.m_dHd += 360;
     LAData.m_sWSMig = p_pEnteringPlaneParams->m_dWSM; 
     UAPuos(&LAData, p_nEtAfOrdinal, p_pTP);
     lWaitTime = (long)((dt * 1000 - (GetTickCount() - lWaitTime)) / m_FSG.m_nFS);
     Sleep(max(lWaitTime, 0));
     }
   if(m_FSG.m_bFAS && abs(mx_wake) < 0.01 && abs(cy_wake) < 0.01)
     break;
   }
 if(fFile) fclose(fFile);
 strcpy(SeverityOutput.m_GeneratorCallSign, m_pAircraft->at(p_nGeneratorAircraftOrdinal)->m_CallSign);
 strcpy(SeverityOutput.m_FollowerCallSign, m_pAircraft->at(p_nEtAfOrdinal)->m_CallSign);
 SeverityOutput.m_dMaxBankRad = G_max;
 SeverityOutput.m_dMaxAngularSpeedOfBankRadPerS = G_p_max;
 SeverityOutput.m_dMaxAltitudeLossM = dH_max;
 SeverityOutput.m_dTimeFromGeneratorS = m_WCSS.m_dTFLS;
 SeverityOutput.m_dmx_wake = mx_wake_max;
 SeverityOutput.m_dvw_y_max = vw_y_max;
 SeverityOutput.m_dvw_z_max = vw_z_max;
 SeverityOutput.m_dGeneratorLatDeg = m_WCSS.m_dAGLD;
 SeverityOutput.m_dGeneratorLongDeg = m_WCSS.m_dAGLlD;
 SeverityOutput.m_dGeneratorAltitudeM = m_WCSS.m_dGAHM;
 SeverityOutput.m_dFollowerLatDeg = m_WCSS.m_dEALD;
 SeverityOutput.m_dFollowerLongDeg = m_WCSS.m_dEALLD;
 SeverityOutput.m_dFollowerAltitudeM = m_WCSS.m_dEAMH;
 SeverityOutput.m_dGFDM = m_WCSS.m_dGFDM;
 SeverityOutput.m_dWindSpeedMS = p_pTP->m_dWindSpeedMS;
 SeverityOutput.m_dWindDirectionDegTo = p_pTP->m_dWDRT * 180 / M_PI;
 SeverityOutput.m_dPlaneToWakeHorz = m_WCSS.m_dPTCWCD;
 SeverityOutput.m_dPlaneToWakeVert = m_WCSS.m_dEtAfLCH - m_WCSS.m_dCH;
 SeverityOutput.m_dTimeInWake = t;
 Message("Entry: vw_y=%lf, vw_z=%lf, vw=%lf", vw_y_first, vw_z_first, sqrt(vw_y_first * vw_y_first + vw_z_first * vw_z_first));
 Message("Maximums: bank=%.1lf deg, angular valocity of bank = %.2lf, altitude change = %.1lf m(%.1lf ft)", G_max * 180 / M_PI, G_p_max * 180 / M_PI, dH_max, dH_max / 0.3048);
 Message("Maximums: mx_wake = %.6lf, vw_y=%lf, vw_z=%lf", mx_wake_max, vw_y_max, vw_z_max);
 if(fFile = OpenOutputFile2(m_FilePrefixWithExt, " Maximums.csv", "ab", m_FSG.m_bCOF, &bVal)) {
   if(!bVal)
     fprintf(fFile, "Callsign;Time;Mass_kg;Flight time s;dt;Pilot response;Entry dZ;dZ used;Entry dH;dH used;Entry dAngle_deg;dAngle_deg used;Entry m_dga L;Entry m_dga R;m_dga used;Max bank deg;Max speed of bank;alt change m;alt change ft;max mx_wake;max vw_y;max vw_x;Entry vw_y;Entry vw_z;Entry vw;\r\n");
   fprintf(fFile, "%s;%.4d-%.2d-%.2d %.2d:%.2d:%.2d.%.3d;%lf;%lf;%lf;%d;%lf;%lf;%lf;%lf;%lf;%lf;%lf;%lf;%lf;%lf;%lf;%lf;%lf;%lf;%lf;%lf;%lf;%lf;%lf;\r\n",
     SeverityOutput.m_FollowerCallSign,
     p_pTP->m_CreationTime.wYear, p_pTP->m_CreationTime.wMonth, p_pTP->m_CreationTime.wDay, p_pTP->m_CreationTime.wHour, p_pTP->m_CreationTime.wMinute, p_pTP->m_CreationTime.wSecond, p_pTP->m_CreationTime.wMilliseconds,
     p_pEnteringPlaneParams->m_dMLMK,
     m_FSG.m_dFT, dt,
     PilotResponse,
     m_WCSS.m_dPTCWCD, dStartPlaneZ, m_WCSS.m_dEtAfLCH - m_WCSS.m_dCH, dStartPlaneH,
     Degrees(m_WCSS.m_dCARd), Degrees(dStartClosingAngleRad),
     m_WCSS.m_dLtG, m_WCSS.m_dRGt, dWakeGamma,
     Degrees(G_max), Degrees(G_p_max), dH_max, dH_max / 0.3048,
     mx_wake_max, vw_y_max, vw_z_max,
     vw_y_first, vw_z_first, sqrt(vw_y_first * vw_y_first + vw_z_first * vw_z_first));
   fclose(fFile); }
 LI.QuadPart = m_WCSS.m_llTTt;
 FT.dwHighDateTime = LI.HighPart;
 FT.dwLowDateTime = LI.LowPart;
 FileTimeToSystemTime(&FT, &SeverityOutput.m_EntryTime);
 m_CSSO.push_back(SeverityOutput);
}
void CWESC::camcxyfw(double *p_mx_wake, double *p_cy_wake, double p_Bank, double p_DeltaZ, double p_DeltaH, double p_dCurAngle, double p_dGamma, const CAPM::RAPL3 *p_pEnteringPlaneParams, double p_dSpeedMS, double *p_pd_vw_y, double *p_pd_vw_z)
{
 *p_mx_wake = 0.1;
 *p_cy_wake = 0.5;
 double b0_wake; 
 double m_dga = p_Bank;
 b0_wake = m_WCSS.m_dDBWC; 
 double delta_y = p_DeltaH; 
 #define NP 11
 short i, Np = NP - 1;
 double S = p_pEnteringPlaneParams->m_dWAM; 
 double L = p_pEnteringPlaneParams->m_dWSM; 
 double Vms = p_dSpeedMS;
 double etta = p_pEnteringPlaneParams->m_dWRT; 
 double lam = p_pEnteringPlaneParams->m_dWAS; 
 double cy_profile = M_PI * lam/ ( 1 + sqrt( 1 + 0.5 * lam * lam ) );
 double dz = L/Np;
 double shift_z = p_DeltaZ; 
 double G_wake = p_dGamma; 
 double b1 = 2 * S/( L * ( 1 + etta) );
 double b2 = etta * b1;
 double z0 = - 0.5 * ( L + dz ); 
 double d = 2.* ( b2 - b1 )/L;
 double rc = 0.035 * b0_wake;
 double cg = cos(m_dga);
 double sg = sin(m_dga);
 double mx_wake = 0;
 double cy_wake = 0;
 double zw[NP], vw_y[NP], vw_z[NP], vw_n[NP];
 *p_pd_vw_y = *p_pd_vw_z = 0;
 for(i = 1; i <= Np; i ++) {
   zw[i] = z0 + i * dz;
   zw[i] = (z0 + i * dz) * cos(p_dCurAngle); 
   double d_z = shift_z + zw[i] * cg - 0.5 * b0_wake;
   double d_y = delta_y + zw[i] * sg;
   double dy2 = d_y * d_y;
   double bz = b1 + abs(zw[i]) * d; 
   vw_y[i] = d_z / (d_z * d_z + dy2 + rc * rc);
   vw_z[i] = -d_y/( d_z * d_z + dy2 + rc * rc );
   double d_z1 = shift_z + zw[i] * cg + 0.5 * b0_wake;
   vw_y[i] = vw_y[i] - d_z1 /(d_z1 * d_z1 + dy2 + rc * rc);
   vw_z[i] = vw_z[i] + d_y /(d_z1 * d_z1 + dy2 + rc * rc);
   vw_y[i] = vw_y[i] * G_wake/( 2 * M_PI);
   vw_z[i] = vw_z[i] * G_wake/( 2 * M_PI);
   vw_n[i] = vw_y[i] * cg - vw_z[i] * sg;
   double dcy = cy_profile * bz * vw_n[i];
   double dmx = cy_profile * bz * zw[i] * vw_n[i];
   cy_wake = cy_wake + dcy;
   mx_wake = mx_wake + dmx;
   (*p_pd_vw_y) += vw_y[i];
   (*p_pd_vw_z) += vw_z[i];
 }
 *p_mx_wake = mx_wake * dz/( S * L * Vms );
 *p_cy_wake = cy_wake * dz/( S * Vms ); 
}
void CWESC::CTmatf(CMatrix *p_pViewMatrix, CWC2 *p_pWakeCross)
{
 double dSinX, dCosX, dSinZ, dCosZ, dSinH, dCosH;
 CMatrix translate, rotate_x, rotate_z, rotate_h, result_1, result_2;
 p_pViewMatrix->Identity();
 dSinX = sin(p_pWakeCross->m_dPVPHCtiRad);
 dCosX = cos(p_pWakeCross->m_dPVPHCtiRad);
 dSinZ = sin(p_pWakeCross->m_dPVBankRad);
 dCosZ = cos(p_pWakeCross->m_dPVBankRad);
 dSinH = sin(p_pWakeCross->m_dPVCSRoeRad);
 dCosH = cos(p_pWakeCross->m_dPVCSRoeRad);
 translate.Identity();
 rotate_x.Identity();
 rotate_z.Identity();
 rotate_h.Identity();
 translate[3][0] = p_pWakeCross->m_dPVXOffset;
 translate[3][1] = p_pWakeCross->m_dPVZOffset;
 translate[3][2] = p_pWakeCross->m_dPVHOffset;
 rotate_x[1][1] = dCosX;
 rotate_x[1][2] = dSinX;
 rotate_x[2][1] = -dSinX;
 rotate_x[2][2] = dCosX;
 rotate_z[0][0] =  dCosZ;
 rotate_z[0][2] = -dSinZ;
 rotate_z[2][0] = dSinZ;
 rotate_z[2][2] = dCosZ;
 rotate_h[0][0] = dCosH;
 rotate_h[0][1] = dSinH;
 rotate_h[1][0] = -dSinH;
 rotate_h[1][1] = dCosH;
 MultMatrix(&rotate_h, &rotate_x, &result_1);
 MultMatrix(&result_1, &rotate_z, &result_2);
 MultMatrix(&result_2, &translate, p_pViewMatrix);
}
void CWESC::TTRSNF(double *p_pdX, double *p_pdZ, double *p_pdH, CMatrix *p_pMatrix)
{
 short i, j;
 double pp[] = { *p_pdX, *p_pdZ, *p_pdH, 1 };
	double tp[] = {0, 0, 0, 0};
	for(i = 0; i < 4; i ++)
	  for(j = 0; j < 4; j ++)
		   tp[i] += pp[j] * (*p_pMatrix)[j][i];
	*p_pdX = tp[0];
	*p_pdZ = tp[1];
	*p_pdH = tp[2];
}
void CWESC::MultMatrix(CMatrix *p_pM1, CMatrix *p_pM2, CMatrix *p_pMDest)
{
	short i, j, k;
	p_pMDest->Reset();
	for(i = 0; i < 4; i ++) {
	  for(j = 0; j < 4; j ++)
		   for(k = 0; k < 4; k ++)
		     p_pMDest->M[i][k] += p_pM1->M[i][j] * p_pM2->M[j][k];
	}
}
void CWESC::Message(const char *p_pFormat, ...)
{
 char tstr[256];
 va_list ArgList;
 if(!p_pFormat) return;
 va_start(ArgList, p_pFormat);
 vsprintf_s(tstr, sizeof(tstr), p_pFormat, ArgList);
 va_end(ArgList);
 if(m_pfMessage) 
   m_pfMessage(tstr);
}
void CWESC::UAPuos(rAircraftPosition *p_pAircraftPosition, short p_nAircraftOrdinal, CTrackPoint *p_pTP)
{
 if(!m_pfUAP) return;
 m_pfUAP(p_pAircraftPosition, p_nAircraftOrdinal, p_pTP);
}
bool CWESC::PWC3(CTrackPoint *p_pTP, short p_nGeneratorAircraftOrdinal, short p_nEtAfOrdinal)
{
 FCWP3(p_pTP);
 if(!m_FSG.m_bUCOU) {
   if(!FPTWA2(m_WCSS.m_pEE->m_pAircraft)) return(false); }
 else {
   if(!FPTWA2(m_WCSS.m_pEE->m_pAircraft)) return(false);
   }
 if(m_WCSS.m_bLWakeOK || m_WCSS.m_bRWakeOK || m_FSG.m_bOverrideProximityValues)
   CSAELV(p_pTP, p_nGeneratorAircraftOrdinal, p_nEtAfOrdinal, m_WCSS.m_pEE->m_pAircraft);
 return(true);
}
bool CWESC::FPTWA2(const CAPM::RAPL3 *p_pPlaneParams)
{
 CGeometricVector3D WakeVector, PlaneVector[3];
 double dRWakeDistance[3], dLWakeDistance[3], dSinRHeading, dCosRHeading, dSinLHeading, dCosLHeading;
 PlaneVector[0] = CGeometricVector3D(m_WCSS.m_dEtAfLCX, m_WCSS.m_dEtAfLCZ, m_WCSS.m_dEtAfLCH);
 PlaneVector[1] = CGeometricVector3D(m_WCSS.m_dEtAfLCX, m_WCSS.m_dEtAfLCZ + p_pPlaneParams->m_dWSM / 2, m_WCSS.m_dEtAfLCH); 
 PlaneVector[2] = CGeometricVector3D(m_WCSS.m_dEtAfLCX, m_WCSS.m_dEtAfLCZ - p_pPlaneParams->m_dWSM / 2, m_WCSS.m_dEtAfLCH); 
 PlaneVector[0] = CGeometricVector3D(m_WCSS.m_dEtAfLCX, m_WCSS.m_dEtAfLCZ, m_WCSS.m_dEtAfLCH);
 PlaneVector[1] = CGeometricVector3D(m_WCSS.m_dEtAfRWtpiLCX, m_WCSS.m_dEtAfRWtpiLCZ, m_WCSS.m_dEtAfRWtpiLCH); 
 PlaneVector[2] = CGeometricVector3D(m_WCSS.m_dEtAfLWtpiLCX, m_WCSS.m_dEtAfLWtpiLCZ, m_WCSS.m_dEtAfLWtpiLCH); 
 dSinRHeading = sin(Radians(m_WCSS.m_dRHeadingDeg));
 dCosRHeading = cos(Radians(m_WCSS.m_dRHeadingDeg));
 dSinLHeading = sin(Radians(m_WCSS.m_dLHeadingDeg));
 dCosLHeading = cos(Radians(m_WCSS.m_dLHeadingDeg));
 if(m_WCSS.m_bRWakeOK) {
#pragma message("circle-assumption dWakeUSRad should be replaced by an ellipse. m_dRHorzExtent and m_WCSS.m_dRVertExtent may be different!")
   m_WCSS.m_dRWakeUSRad = (m_WCSS.m_dRHorzExtent + m_WCSS.m_dRVertExtent) / 2;
   WakeVector = CGeometricVector3D(m_WCSS.m_dRX, m_WCSS.m_dRZ, m_WCSS.m_dRH);
   dRWakeDistance[0] = PlaneVector[0].Distance(WakeVector); 
   dRWakeDistance[1] = PlaneVector[1].Distance(WakeVector); 
   dRWakeDistance[2] = PlaneVector[2].Distance(WakeVector); 
   Message("Right wake, radius %.1fm, plane to wake distances (L wintip/Plane center/R wingtip) %.1fm/%.1fm/%.1fm, tolerance %.1fm", 
     m_WCSS.m_dRWakeUSRad, dRWakeDistance[2], dRWakeDistance[0], dRWakeDistance[1], m_WCSS.m_dRWakeUSRad + m_FSG.m_dWWT);
   m_WCSS.m_dRDistance = fabs(dRWakeDistance[0]);
   if(fabs(dRWakeDistance[1]) < m_WCSS.m_dRDistance)
     m_WCSS.m_dRDistance = fabs(dRWakeDistance[1]);
   if(fabs(dRWakeDistance[2]) < m_WCSS.m_dRDistance)
     m_WCSS.m_dRDistance = fabs(dRWakeDistance[2]);
   m_WCSS.m_bRWakeOK = false;
   if(m_WCSS.m_dRDistance <= m_WCSS.m_dRWakeUSRad + m_FSG.m_dWWT)
     m_WCSS.m_bRWakeOK = true;
   if(fabs(dRWakeDistance[0]) <= p_pPlaneParams->m_dWSM / 2 && m_WCSS.m_dEAMH - m_WCSS.m_dRGeoH <= m_WCSS.m_dRVertExtent + m_FSG.m_dWWT)
     m_WCSS.m_bRWakeOK = true;
   if(m_WCSS.m_bRWakeOK) Message("Right wake entry!"); }
 if(m_WCSS.m_bLWakeOK) {
   m_WCSS.m_dLWakeUSRad = (m_WCSS.m_dLHorzExtent + m_WCSS.m_dLVertExtent) / 2;
   WakeVector = CGeometricVector3D(m_WCSS.m_dLX * dSinLHeading + m_WCSS.m_dLZ * dCosLHeading, m_WCSS.m_dLX * dCosLHeading - m_WCSS.m_dLZ * dSinLHeading, m_WCSS.m_dLH);
   dLWakeDistance[0] = PlaneVector[0].Distance(WakeVector); 
   dLWakeDistance[1] = PlaneVector[1].Distance(WakeVector); 
   dLWakeDistance[2] = PlaneVector[2].Distance(WakeVector); 
   Message("Left wake, radius %.1fm plane to wake distances (L wintip/Plane center/R wingtip) %.1fm/%.1fm/%.1fm, tolerance %.1fm", 
     m_WCSS.m_dLWakeUSRad, dLWakeDistance[2], dLWakeDistance[0], dLWakeDistance[1], m_WCSS.m_dLWakeUSRad + m_FSG.m_dWWT);
   m_WCSS.m_dLDistance = fabs(dLWakeDistance[0]);
   if(fabs(dLWakeDistance[1]) < m_WCSS.m_dLDistance)
     m_WCSS.m_dLDistance = fabs(dLWakeDistance[1]);
   if(fabs(dLWakeDistance[2]) < m_WCSS.m_dLDistance)
     m_WCSS.m_dLDistance = fabs(dLWakeDistance[2]);
   m_WCSS.m_bLWakeOK = false;
   if(m_WCSS.m_dLDistance <= m_WCSS.m_dLWakeUSRad + m_FSG.m_dWWT)
     m_WCSS.m_bLWakeOK = true;
   if(fabs(dLWakeDistance[0]) <= p_pPlaneParams->m_dWSM / 2 && m_WCSS.m_dEAMH - m_WCSS.m_dLGeoH <= m_WCSS.m_dLVertExtent + m_FSG.m_dWWT)
     m_WCSS.m_bLWakeOK = true;
   if(m_WCSS.m_bLWakeOK) Message("Left wake entry!"); }
 return(true);
}
bool CWESC::FillPlaneToWakeAttitude3(const CAPM::RAPL3 *p_pPlaneParams)
{
 CGeometricVector3D WakeVector, PlaneVector;
 double dAircraftToWakeSectionPlaneDistance;
 dAircraftToWakeSectionPlaneDistance = DistanceFromPointToPlane(
   m_WCSS.m_dEtAfLCX, m_WCSS.m_dEtAfLCZ, m_WCSS.m_dEtAfLCH, 
   m_WCSS.m_dRX, m_WCSS.m_dRZ, m_WCSS.m_dRH,
   m_WCSS.m_dLX, m_WCSS.m_dLZ, m_WCSS.m_dLH,
   (m_WCSS.m_dRX + m_WCSS.m_dLX) / 2, (m_WCSS.m_dRZ + m_WCSS.m_dLZ) / 2, (m_WCSS.m_dRH + m_WCSS.m_dLH) / 2 + 100);
 if(dAircraftToWakeSectionPlaneDistance > p_pPlaneParams->m_dWSM / 2)
   m_WCSS.m_bRWakeOK = false;
   m_WCSS.m_bLWakeOK = false;
   return(true); 
 if(m_WCSS.m_bRWakeOK) {
   if((m_WCSS.m_dRX + (m_WCSS.m_dRHorzExtent + p_pPlaneParams->m_dWSM / 2)) * (m_WCSS.m_dRX - (m_WCSS.m_dRHorzExtent + p_pPlaneParams->m_dWSM / 2)) > 0 ||
      (m_WCSS.m_dRH + m_WCSS.m_dRVertExtent) * (m_WCSS.m_dRH - m_WCSS.m_dRVertExtent) > 0)
     m_WCSS.m_bRWakeOK = false;
   else Message("Right wake entry!"); }
 if(m_WCSS.m_bLWakeOK) {
   if((m_WCSS.m_dLX + (m_WCSS.m_dLHorzExtent + p_pPlaneParams->m_dWSM / 2)) * (m_WCSS.m_dLX - (m_WCSS.m_dLHorzExtent + p_pPlaneParams->m_dWSM / 2)) > 0 ||
      (m_WCSS.m_dLH + m_WCSS.m_dLVertExtent) * (m_WCSS.m_dLH - m_WCSS.m_dLVertExtent) > 0)
     m_WCSS.m_bLWakeOK = false;
   else Message("Left wake entry!"); }
 return(true); 
}
CWESC CWESC::MakeSafeCopy()
{
 CWESC WI = *this;
 WI.m_pLC = NULL;
 return(WI);
}
void CWESC::FCWP3(CTrackPoint *p_pTP)
{
 short i, j;
 GeographicLib::Math::real GeoPoints[2][3], PlanarPoints[2][3]; 
 CGeometricVector3D GVector1, GVector2;
 double dPlaneToWakeAngles[2], dRatio, *pdAuxVals[6];
 double dModules[2], dVal[2];
 bool *pbAuxVals; 
 double dSinHeading, dCosHeading;
 dSinHeading = sin(Radians(m_WCSS.m_dEAHD));
 dCosHeading = cos(Radians(m_WCSS.m_dEAHD));
 m_pLC->Reset(p_pTP->m_dLatDeg, p_pTP->m_dLongDeg, p_pTP->m_nAltitudeM);
 m_WCSS.m_dEtAfLWtpiLCX = m_WCSS.m_dEtAfLCX;
 m_WCSS.m_dEtAfLWtpiLCZ = m_WCSS.m_dEtAfLCZ - m_WCSS.m_pEE->m_pAircraft->m_dWSM / 2;
 m_WCSS.m_dEtAfLWtpiLCH = m_WCSS.m_dEtAfLCH;
 m_WCSS.m_dEtAfRWtpiLCX = m_WCSS.m_dEtAfLCX;
 m_WCSS.m_dEtAfRWtpiLCZ = m_WCSS.m_dEtAfLCZ + m_WCSS.m_pEE->m_pAircraft->m_dWSM / 2;
 m_WCSS.m_dEtAfRWtpiLCH = m_WCSS.m_dEtAfLCH;
 dVal[0] = m_WCSS.m_dEtAfLWtpiLCX * dSinHeading + m_WCSS.m_dEtAfLWtpiLCZ * dCosHeading;
 dVal[1] = m_WCSS.m_dEtAfLWtpiLCX * dCosHeading - m_WCSS.m_dEtAfLWtpiLCZ * dSinHeading;
 m_WCSS.m_dEtAfLWtpiLCX = dVal[0];
 m_WCSS.m_dEtAfLWtpiLCZ = dVal[1];
 dVal[0] = m_WCSS.m_dEtAfRWtpiLCX * dSinHeading + m_WCSS.m_dEtAfRWtpiLCZ * dCosHeading;
 dVal[1] = m_WCSS.m_dEtAfRWtpiLCX * dCosHeading - m_WCSS.m_dEtAfRWtpiLCZ * dSinHeading;
 m_WCSS.m_dEtAfRWtpiLCX = dVal[0];
 m_WCSS.m_dEtAfRWtpiLCZ = dVal[1];
 m_pLC->Reverse(m_WCSS.m_dEtAfLWtpiLCX, m_WCSS.m_dEtAfLWtpiLCZ, m_WCSS.m_dEtAfLWtpiLCH, m_WCSS.m_dEtAfLWtpiLatDeg, m_WCSS.m_dEtAfLWtpiLongDeg, m_WCSS.m_dEtAfLWtpiGeoH_m);
 m_pLC->Reverse(m_WCSS.m_dEtAfRWtpiLCX, m_WCSS.m_dEtAfRWtpiLCZ, m_WCSS.m_dEtAfRWtpiLCH, m_WCSS.m_dEtAfRWtpiLatDeg, m_WCSS.m_dEtAfRWtpiLongDeg, m_WCSS.m_dEtAfRWtpiGeoH_m);
 m_WCSS.m_bLWakeOK = m_WCSS.m_bRWakeOK = false;
 m_WCSS.m_dRGt = m_WCSS.m_dLtG = 0;
 dPlaneToWakeAngles[0] = dPlaneToWakeAngles[1] = 100; 
 m_WCSS.m_dVertOffsetUp = m_WCSS.m_dVertOffsetDown = 0; 
 for(i = 0; i < 2; i ++) {
   if(!m_WCSS.m_bWSOKP[i] || !m_WCSS.m_bWSOKP[i + 1])
     continue; 
   for(j = 0; j < 2; j ++) { 
     if(!j && m_WCSS.m_bRWakeOK || j && m_WCSS.m_bLWakeOK)
       continue; 
     GeoPoints[0][0] = !j ? m_WCSS.m_WSNWE[i].m_dLT1d : m_WCSS.m_WSNWE[i].m_dL2TD;
     GeoPoints[0][1] = !j ? m_WCSS.m_WSNWE[i].m_sLL1DT : m_WCSS.m_WSNWE[i].m_dLLD2;
     GeoPoints[0][2] = !j ? m_WCSS.m_WSNWE[i].m_dAltitude1 : m_WCSS.m_WSNWE[i].m_dAltitude2;
     GeoPoints[1][0] = !j ? m_WCSS.m_WSNWE[i + 1].m_dLT1d : m_WCSS.m_WSNWE[i + 1].m_dL2TD;
     GeoPoints[1][1] = !j ? m_WCSS.m_WSNWE[i + 1].m_sLL1DT : m_WCSS.m_WSNWE[i + 1].m_dLLD2;
     GeoPoints[1][2] = !j ? m_WCSS.m_WSNWE[i + 1].m_dAltitude1 : m_WCSS.m_WSNWE[i + 1].m_dAltitude2;
     pdAuxVals[0] = !j ? &m_WCSS.m_dRX : &m_WCSS.m_dLX;
     pdAuxVals[1] = !j ? &m_WCSS.m_dRZ : &m_WCSS.m_dLZ;
     pdAuxVals[2] = !j ? &m_WCSS.m_dRH : &m_WCSS.m_dLH;
     pdAuxVals[3] = !j ? &m_WCSS.m_dRHorzExtent : &m_WCSS.m_dLHorzExtent;
     pdAuxVals[4] = !j ? &m_WCSS.m_dRVertExtent : &m_WCSS.m_dLVertExtent;
     pdAuxVals[5] = !j ? &m_WCSS.m_dRGt : &m_WCSS.m_dLtG;
     pbAuxVals = !j ? &m_WCSS.m_bRWakeOK : &m_WCSS.m_bLWakeOK;
     m_pLC->Forward(GeoPoints[0][0], GeoPoints[0][1], GeoPoints[0][2], PlanarPoints[0][0], PlanarPoints[0][1], PlanarPoints[0][2]);
     m_pLC->Forward(GeoPoints[1][0], GeoPoints[1][1], GeoPoints[1][2], PlanarPoints[1][0], PlanarPoints[1][1], PlanarPoints[1][2]);
     GVector1 = CGeometricVector3D(0, 0, 0) - CGeometricVector3D(PlanarPoints[0][0], PlanarPoints[0][1], PlanarPoints[0][2]); 
     dModules[0] = GVector1.Module(); 
     GVector2 = CGeometricVector3D(PlanarPoints[1][0], PlanarPoints[1][1], PlanarPoints[1][2]) - CGeometricVector3D(PlanarPoints[0][0], PlanarPoints[0][1], PlanarPoints[0][2]); 
     dModules[0] = GVector2.Module(); 
     dPlaneToWakeAngles[0] = GVector1.AngleDeg(GVector2);
     GVector1 = CGeometricVector3D(0, 0, 0) - CGeometricVector3D(PlanarPoints[1][0], PlanarPoints[1][1], PlanarPoints[1][2]); 
     dModules[1] = GVector1.Module(); 
     GVector2 = CGeometricVector3D(PlanarPoints[0][0], PlanarPoints[0][1], PlanarPoints[0][2]) - CGeometricVector3D(PlanarPoints[1][0], PlanarPoints[1][1], PlanarPoints[1][2]); 
     dModules[1] = GVector2.Module(); 
     dPlaneToWakeAngles[1] = GVector1.AngleDeg(GVector2);
     if(abs(dPlaneToWakeAngles[0]) > 90 || abs(dPlaneToWakeAngles[1]) > 90)
       continue; 
     dRatio = 1.0 - GVector1.Module() * cos(Radians(dPlaneToWakeAngles[1])) / GVector2.Module(); 
     *(pdAuxVals[0]) = PlanarPoints[0][0] + (PlanarPoints[1][0] - PlanarPoints[0][0]) * dRatio;
     *(pdAuxVals[1]) = PlanarPoints[0][1] + (PlanarPoints[1][1] - PlanarPoints[0][1]) * dRatio;
     *(pdAuxVals[2]) = PlanarPoints[0][2] + (PlanarPoints[1][2] - PlanarPoints[0][2]) * dRatio;
     *(pdAuxVals[3]) = m_WCSS.m_WSNWE[i].m_dHSe + (m_WCSS.m_WSNWE[i + 1].m_dHSe - m_WCSS.m_WSNWE[i].m_dHSe) * dRatio;
     *(pdAuxVals[4]) = m_WCSS.m_WSNWE[i].m_deVSe + (m_WCSS.m_WSNWE[i + 1].m_deVSe - m_WCSS.m_WSNWE[i].m_deVSe) * dRatio;
     *(pdAuxVals[5]) = m_WCSS.m_WSNWE[i].m_dCiCu + (m_WCSS.m_WSNWE[i + 1].m_dCiCu - m_WCSS.m_WSNWE[i].m_dCiCu) * dRatio;
     if(m_FSG.m_bUCOU) {
       dVal[0] = m_WCSS.m_WSNWE[i].m_deVSe;
       dVal[1] = m_WCSS.m_WSNWE[i + 1].m_deVSe;
       if(m_WCSS.m_WSNWE[i].m_bUNC)
         dVal[0] += (m_WCSS.m_WSNWE[i].m_dAltitude1Upper - m_WCSS.m_WSNWE[i].m_dAltitude1Lower) / 2;
       if(m_WCSS.m_WSNWE[i + 1].m_bUNC)
         dVal[1] += (m_WCSS.m_WSNWE[i + 1].m_dAltitude1Upper - m_WCSS.m_WSNWE[i + 1].m_dAltitude1Lower) / 2;
       *(pdAuxVals[4]) = dVal[0] + (dVal[1] - dVal[0]) * dRatio;
       dVal[0] = m_WCSS.m_WSNWE[i].m_dHSe;
       dVal[1] = m_WCSS.m_WSNWE[i + 1].m_dHSe;
       if(m_WCSS.m_WSNWE[i].m_bUNC)
         dVal[0] += m_WCSS.m_WSNWE[i].m_dHSeCoU;
       if(m_WCSS.m_WSNWE[i + 1].m_bUNC)
         dVal[1] += m_WCSS.m_WSNWE[i + 1].m_dHSeCoU;
       *(pdAuxVals[3]) = dVal[0] + (dVal[1] - dVal[0]) * dRatio;
       m_WCSS.m_dVertOffsetUp = 0;
       m_WCSS.m_dVertOffsetDown = 0;
       if(m_WCSS.m_WSNWE[i].m_bUNC) {
         m_WCSS.m_dVertOffsetUp = (m_WCSS.m_WSNWE[i].m_dAltitude1Upper - m_WCSS.m_WSNWE[i].m_dAltitude1);
         m_WCSS.m_dVertOffsetDown = (m_WCSS.m_WSNWE[i].m_dAltitude1 - m_WCSS.m_WSNWE[i].m_dAltitude1Lower);
         }
       if(m_WCSS.m_WSNWE[i + 1].m_bUNC) {
         m_WCSS.m_dVertOffsetUp = (m_WCSS.m_WSNWE[i + 1].m_dAltitude1Upper - m_WCSS.m_WSNWE[i + 1].m_dAltitude1);
         m_WCSS.m_dVertOffsetDown = (m_WCSS.m_WSNWE[i + 1].m_dAltitude1 - m_WCSS.m_WSNWE[i + 1].m_dAltitude1Lower);
         }
       dVal[0] = (!m_WCSS.m_WSNWE[i].m_bUNC) ? m_WCSS.m_WSNWE[i].m_dCiCu : m_WCSS.m_WSNWE[i].m_dCiCuUpper;
       dVal[1] = (!m_WCSS.m_WSNWE[i + 1].m_bUNC) ? m_WCSS.m_WSNWE[i + 1].m_dCiCu : m_WCSS.m_WSNWE[i + 1].m_dCiCuUpper;
       *(pdAuxVals[5]) = dVal[0] + (dVal[1] - dVal[0]) * dRatio;
       }
     *pbAuxVals = true;
     }
   }
 if(!m_WCSS.m_bRWakeOK) { 
   m_pLC->Forward(m_WCSS.m_WSNWE[1].m_dLT1d, m_WCSS.m_WSNWE[1].m_sLL1DT, m_WCSS.m_WSNWE[1].m_dAltitude1, PlanarPoints[0][0], PlanarPoints[0][1], PlanarPoints[0][2]);
   m_WCSS.m_dRX = PlanarPoints[0][0];
   m_WCSS.m_dRZ = PlanarPoints[0][1];
   m_WCSS.m_dRH = PlanarPoints[0][2];
   m_WCSS.m_dRHorzExtent = m_WCSS.m_WSNWE[1].m_dHSe;
   m_WCSS.m_dRVertExtent = m_WCSS.m_WSNWE[1].m_deVSe;
   m_WCSS.m_dVertOffsetUp = 0;
   m_WCSS.m_dVertOffsetDown = 0;
   if(m_FSG.m_bUCOU && m_WCSS.m_WSNWE[1].m_bUNC) {
     m_WCSS.m_dRVertExtent += (m_WCSS.m_WSNWE[1].m_dAltitude1Upper - m_WCSS.m_WSNWE[1].m_dAltitude1Lower) / 2;
     m_WCSS.m_dRHorzExtent += m_WCSS.m_WSNWE[1].m_dHSeCoU;
     m_WCSS.m_dVertOffsetUp = (m_WCSS.m_WSNWE[1].m_dAltitude1Upper - m_WCSS.m_WSNWE[1].m_dAltitude1);
     m_WCSS.m_dVertOffsetDown = (m_WCSS.m_WSNWE[1].m_dAltitude1 - m_WCSS.m_WSNWE[1].m_dAltitude1Lower);
     }
   m_WCSS.m_dRGt = m_WCSS.m_WSNWE[1].m_dCiCu;
   if(m_FSG.m_bUCOU && m_WCSS.m_WSNWE[1].m_bUNC)
     m_WCSS.m_dRGt = m_WCSS.m_WSNWE[1].m_dCiCuUpper;
   m_WCSS.m_bRWakeOK = true; }
 if(!m_WCSS.m_bLWakeOK) { 
   m_pLC->Forward(m_WCSS.m_WSNWE[1].m_dL2TD, m_WCSS.m_WSNWE[1].m_dLLD2, m_WCSS.m_WSNWE[1].m_dAltitude2, PlanarPoints[0][0], PlanarPoints[0][1], PlanarPoints[0][2]);
   m_WCSS.m_dLX = PlanarPoints[0][0];
   m_WCSS.m_dLZ = PlanarPoints[0][1];
   m_WCSS.m_dLH = PlanarPoints[0][2];
   m_WCSS.m_dLHorzExtent = m_WCSS.m_WSNWE[1].m_dHSe;
   m_WCSS.m_dLVertExtent = m_WCSS.m_WSNWE[1].m_deVSe;
   m_WCSS.m_dVertOffsetUp = 0;
   m_WCSS.m_dVertOffsetDown = 0;
   if(m_FSG.m_bUCOU && m_WCSS.m_WSNWE[1].m_bUNC) {
     m_WCSS.m_dLVertExtent += (m_WCSS.m_WSNWE[1].m_dAltitude2Upper - m_WCSS.m_WSNWE[1].m_dAltitude2Lower) / 2;
     m_WCSS.m_dLHorzExtent += m_WCSS.m_WSNWE[1].m_dHSeCoU;
     m_WCSS.m_dVertOffsetUp = (m_WCSS.m_WSNWE[1].m_dAltitude2Upper - m_WCSS.m_WSNWE[1].m_dAltitude2);
     m_WCSS.m_dVertOffsetDown = (m_WCSS.m_WSNWE[1].m_dAltitude2 - m_WCSS.m_WSNWE[1].m_dAltitude2Lower);
     }
   m_WCSS.m_dLtG = m_WCSS.m_WSNWE[1].m_dCiCu;
   if(m_FSG.m_bUCOU && m_WCSS.m_WSNWE[1].m_bUNC)
     m_WCSS.m_dLtG = m_WCSS.m_WSNWE[1].m_dCiCuUpper;
   m_WCSS.m_bLWakeOK = true; }
 ASSERT(m_WCSS.m_bRWakeOK && m_WCSS.m_bLWakeOK);
 m_WCSS.m_dRHeadingDeg = m_WCSS.m_WSNWE[1].m_dHd;
 m_WCSS.m_dLHeadingDeg = m_WCSS.m_WSNWE[1].m_dHd;
 m_WCSS.m_dDBWC =  sqrt((m_WCSS.m_dRX - m_WCSS.m_dLX) * (m_WCSS.m_dRX - m_WCSS.m_dLX) +
                                                   (m_WCSS.m_dRZ - m_WCSS.m_dLZ) * (m_WCSS.m_dRZ - m_WCSS.m_dLZ) +
                                                   (m_WCSS.m_dRH - m_WCSS.m_dLH) * (m_WCSS.m_dRH - m_WCSS.m_dLH));
 m_WCSS.m_dCX = (m_WCSS.m_dLX + m_WCSS.m_dRX) / 2;
 m_WCSS.m_dCZ = (m_WCSS.m_dLZ + m_WCSS.m_dRZ) / 2;
 m_WCSS.m_dCH = (m_WCSS.m_dLH + m_WCSS.m_dRH) / 2;
 m_WCSS.m_dPTCWCD = sqrt(m_WCSS.m_dCX * m_WCSS.m_dCX + m_WCSS.m_dCZ * m_WCSS.m_dCZ);
 m_WCSS.m_dCARd = p_pTP->m_dAzimuth - Radians(m_WCSS.m_dRHeadingDeg);
 if(m_WCSS.m_dCARd < 0) m_WCSS.m_dCARd += M_PI * 2;
 m_pLC->Reverse(m_WCSS.m_dLX, m_WCSS.m_dLZ, m_WCSS.m_dLH, m_WCSS.m_dLLat, m_WCSS.m_dLLong, m_WCSS.m_dLGeoH);
 m_pLC->Reverse(m_WCSS.m_dRX, m_WCSS.m_dRZ, m_WCSS.m_dRH, m_WCSS.m_dRLat, m_WCSS.m_dRLong, m_WCSS.m_dRGeoH);
 m_pLC->Reverse(m_WCSS.m_dCX, m_WCSS.m_dCZ, m_WCSS.m_dCH, m_WCSS.m_dCLat, m_WCSS.m_dCLong, m_WCSS.m_dCGeoH);
}
