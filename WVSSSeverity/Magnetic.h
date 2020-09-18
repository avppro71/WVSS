#pragma once
#include "GeomagnetismHeader.h"
class CMagnetic
{
public:
 bool m_bInitOK;
 char m_ErrMsg[256];
 MAGtype_Ellipsoid m_Ellip;
 MAGtype_Geoid m_Geoid;
 MAGtype_MagneticModel *m_MagneticModels[1], *m_TimedMagneticModel;
 MAGtype_GeoMagneticElements m_GeoMagneticElements, m_Errors;
public:
 CMagnetic();
 ~CMagnetic();
 bool Init();
 double GetMagVar(MAGtype_CoordGeodetic p_CoordGeodetic, MAGtype_Date p_UserDate);
};
