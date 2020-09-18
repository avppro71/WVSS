#include "stdafx.h"
#include "WindServer.h"
#include "GeographicLib/geodesic.hpp"
#pragma warning(disable : 4996)
CWindServer::CWindServer()
{
 m_p2dArray = NULL;
 m_l2dArraySize = 0;
 m_pGeo = new GeographicLib::Geodesic(GeographicLib::Constants::WGS84_a(), GeographicLib::Constants::WGS84_f());
 m_bReady = false;
 *m_LastLoadFileName = '\0';
}
CWindServer::~CWindServer()
{
 if(m_p2dArray) delete [] m_p2dArray;
 if(m_pGeo) delete m_pGeo;
}
bool CWindServer::SetDone()
{
 unsigned long li;
 for(li = 0; li < m_l2dArraySize; li ++)
  if(m_p2dArray[li].m_dWDRT == AVP_BAD_WIND_VALUE || m_p2dArray[li].m_dWindSpeedMS == AVP_BAD_WIND_VALUE)
    break;
 m_bReady = (li == m_l2dArraySize);
 return(m_bReady);
}
bool CWindServer::Set2dArraySize(unsigned long p_lSize)
{
 if(m_p2dArray) delete [] m_p2dArray;
 m_l2dArraySize = 0;
 m_p2dArray = new C2D[p_lSize];
 if(!m_p2dArray) return(false);
 memset(m_p2dArray, 0, sizeof(C2D) * p_lSize);
 m_l2dArraySize = p_lSize;
 return(true);
}
bool CWindServer::GetValue(C2D *p_p2D)
{
 unsigned long li, liMin = 0xFFFFFFFF;
 GeographicLib::Math::real Azimut1Deg, Azimut2Deg, DistM, m12, M12, M21, S12, MinDist;
 MinDist = 100000000;
 for(li = 0; li < m_l2dArraySize; li ++) {
   m_pGeo->Inverse(p_p2D->dLat, p_p2D->dLong, m_p2dArray[li].dLat, m_p2dArray[li].dLong, DistM, Azimut1Deg, Azimut2Deg, m12, M12, M21, S12);
   if(DistM < MinDist) {
     MinDist = DistM;
     liMin = li;
     }
   }
 if(liMin != 0xFFFFFFFF && MinDist < 10000) {
   *p_p2D = m_p2dArray[liMin];
   return(true);
   }
 return(false);
}
bool CWindServer::GetValueExact(unsigned long p_lIndex, C2D *p_p2D)
{
 if(p_lIndex < 0 || p_lIndex >= m_l2dArraySize) return(false);
 *p_p2D = m_p2dArray[p_lIndex];
 return(true);
}
bool CWindServer::SaveToFile(const char *p_pFileName )
{
 FILE *fFile;
 char FileName[256];
 size_t stWritten;
 if(p_pFileName) strcpy(FileName, p_pFileName);
 else strcpy(FileName, m_LastLoadFileName);
 fFile = fopen(FileName, "wb");
 if(!fFile) return(false);
 stWritten = fwrite(&m_l2dArraySize, sizeof(m_l2dArraySize), 1, fFile);
 ASSERT(stWritten = 1);
 stWritten = fwrite(m_p2dArray, sizeof(C2D), m_l2dArraySize, fFile);
 ASSERT(stWritten = m_l2dArraySize);
 fclose(fFile);
 return(true);
}
bool CWindServer::LoadFromFile(const char *p_pFileName)
{
 FILE *fFile = fopen(p_pFileName, "rb");
 size_t stRead;
 strcpy(m_LastLoadFileName, p_pFileName);
 m_bReady = false;
 if(!fFile) return(false);
 stRead = fread(&m_l2dArraySize, sizeof(m_l2dArraySize), 1, fFile);
 ASSERT(stRead = 1);
 if(!Set2dArraySize(m_l2dArraySize))
   return(false);
 stRead = fread(m_p2dArray, sizeof(C2D), m_l2dArraySize, fFile);
 ASSERT(stRead = m_l2dArraySize);
 fclose(fFile);
 m_bReady = true;
 return(true);
}
