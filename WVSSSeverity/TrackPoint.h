#pragma once
namespace WVSS {
class CTrackPoint
{
public:
 char m_CallSign[32];
 SYSTEMTIME m_CreationTime;
 SYSTEMTIME m_CreationTimeOrig;
 LARGE_INTEGER m_CTLE; 
 double m_dLifetime;
 char m_ACModel[16];
 double m_dLatDeg, m_dLongDeg;
 double m_dAltitude100Ft; 
 short m_nAltitudeM;
 double m_dAzimuth, m_dAzimuthDeg, m_dAzimuthBack, m_dAzimuthBackDeg;
 double m_dDistSphere, m_dDistKm;
 double m_dTimeSpanS;
 double m_dAveSpeedMS, m_dVSpeedMS;
 DWORD m_lStartIndex, m_lPoints;
 bool m_bFirstPoint, m_bLastPoint;
 short m_nAircraftOrdinal;
 double m_dAccel; 
 double m_dSpeedMS;
 double m_dInitialLegSpeedMS; 
 double m_dAttitudeAngleDeg; 
 char m_DepartureAirport[16];
 char m_ArrivalAirport[16];
 double m_dBankDeg;
 double m_dWeightKg;
 double m_dWDRT;
 double m_dWindSpeedMS;
 bool m_bInterpolate;
 double m_dElevation; 
 long m_lTimeShift_s; 
public:
 CTrackPoint() { Init(); }
 void Init() { memset(this, 0, sizeof(CTrackPoint)); m_dWDRT = AVP_BAD_WIND_VALUE; m_dWindSpeedMS = AVP_BAD_WIND_VALUE; }
 bool operator < (CTrackPoint &p_Pt);
};
} 
