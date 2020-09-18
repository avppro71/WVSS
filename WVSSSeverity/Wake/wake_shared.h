#pragma once
#pragma pack(push, 1)
class CWVSSScenario
{
public:
 char m_AirportName[128];
 char m_ScenarioName[32];
 double m_dCenterLatDeg, m_dCenterLongDeg;
public:
 CWVSSScenario() { Reset(); }
 ~CWVSSScenario() { }
 void Reset() { memset(this, 0, sizeof(CWVSSScenario)); }
};
#pragma pack(pop)
