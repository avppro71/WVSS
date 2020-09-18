#include "stdafx.h"
#include "SitarWVSSPlayer.h"
#include "Magnetic.h"
#include "EGM9615_f.h"
CMagnetic::CMagnetic()
{
 m_bInitOK = Init();
}
bool CMagnetic::Init()
{
 char filename[256]; 
 char VersionDate_Large[] = "$Date: 2014-11-21 10:40:43 -0700 (Fri, 21 Nov 2014) $";
 char VersionDate[12];
 int NumTerms, Flag = 1, nMax = 0;
 int epochs = 1;
 sprintf_s(filename, sizeof(filename), "%s\\WMM.COF", g_theApp.m_ThisDir);
 strncpy_s(VersionDate, sizeof(VersionDate), VersionDate_Large + 39, 11);
 VersionDate[11] = '\0';
 if(!MAG_robustReadMagModels(filename, (MAGtype_MagneticModel *(*)[])&m_MagneticModels, epochs)) {
     sprintf_s(m_ErrMsg, sizeof(m_ErrMsg), "WMM.COF not found.");
     return false;
 }
 if(nMax < m_MagneticModels[0]->nMax) nMax = m_MagneticModels[0]->nMax;
 NumTerms = ((nMax + 1) * (nMax + 2) / 2);
 m_TimedMagneticModel = MAG_AllocateModelMemory(NumTerms); 
 if(m_MagneticModels[0] == NULL || m_TimedMagneticModel == NULL)
 {
     MAG_Error(2);
 }
 MAG_SetDefaults(&m_Ellip, &m_Geoid); 
 m_Geoid.GeoidHeightBuffer = GeoidHeightBuffer;
 m_Geoid.Geoid_Initialized = 1;
 return(true);
}
CMagnetic::~CMagnetic()
{
}
double CMagnetic::GetMagVar(MAGtype_CoordGeodetic p_CoordGeodetic, MAGtype_Date p_UserDate)
{
 MAGtype_CoordSpherical CoordSpherical;
 MAG_GeodeticToSpherical(m_Ellip, p_CoordGeodetic, &CoordSpherical); 
 MAG_TimelyModifyMagneticModel(p_UserDate, m_MagneticModels[0], m_TimedMagneticModel); 
 MAG_Geomag(m_Ellip, CoordSpherical, p_CoordGeodetic, m_TimedMagneticModel, &m_GeoMagneticElements); 
 MAG_CalculateGridVariation(p_CoordGeodetic, &m_GeoMagneticElements);
 MAG_WMMErrorCalc(m_GeoMagneticElements.H, &m_Errors);
 return(m_GeoMagneticElements.Decl);
}
