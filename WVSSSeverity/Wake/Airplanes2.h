#pragma once
#include <vector>
#include <map>
#define AVP_AIRPLANE_FILE_BIN_PREFIX "SITARWVSSAIRCRAFT"
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
class WVSSAPIEX CAPM
{
private:
 CStringList m_ErrorList;
public:
 class CAP3T {
   public:
    char m_TN[16];
    CAP3T() { }
    ~CAP3T() { }
    bool operator < (const CAP3T &p_AirplaneType) const;
    CAP3T &operator = (CAP3T &p_AirplaneType);
   };
 enum eICAOCat { ICAO_LIGHT, ICAO_MEDIUM, ICAO_HEAVY };
 enum eFAACat { FAA_SMALL, FAA_LARGE, FAA_HEAVY, FAA_SUPER };
 struct RAPL3 {
  char m_TN[16];
  double m_dMTMK; 
  double m_dMLMK; 
  double m_dWSM; 
  double m_dLSTK; 
  double m_dWAM; 
  double m_dWAS; 
  double m_dWRT; 
  double m_dMWXX; 
  double m_sXXJI; 
  double m_dWPXOM; 
  eICAOCat m_ICAOCat;
  eFAACat m_FAACat;
 };
protected: 
 std::map<CAP3T, RAPL3> m_APMA;
private:
 bool PTALe(char *p_pStr, RAPL3 *p_pAirplane3);
public:
 CAPM();
 ~CAPM();
 bool LoadFromFile(const char *p_pFileName, bool p_bBinary);
 bool SaveToFile(const char *p_pFileName, bool p_bBinary);
 const RAPL3 *GALBNa(LPCSTR p_pTypeName); 
 void AddAirplane(RAPL3 *p_pAirplane);
 static const char *GetICAOCatName(eICAOCat p_Cat);
 static const char *GetFAACatName(eFAACat p_Cat);
};
