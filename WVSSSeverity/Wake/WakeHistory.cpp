#include "stdafx.h"
#include "WakeHistory.h"
#include<new>
using namespace WVSS;
void CWS2::Reset()
{
 m_liTime.QuadPart = 0;
 m_dCiCu = 0;
 m_dLT1d = m_dL2TD = m_sLL1DT = m_dLLD2 = 0;
 m_dAltitude1 = m_dAltitude2 = 0;
 m_dHSe = m_deVSe = 0;
 m_dHd = 0;
 m_dBdK = 0;
 m_cDangerLevel = 0;
 m_dEDR_star = 0;
 m_dEDR = 0;
 m_bUNC = false;
 m_dCiCuUpper = 0;
 m_dAltitude1Upper = m_dAltitude2Upper = 0;
 m_dCiCuLower = 0;
 m_dAltitude1Lower = m_dAltitude2Lower = 0;
}
CWSHM::CWSHM()
{
 Reset();
}
CWSHM::~CWSHM()
{
 if(m_pRecords) delete [] m_pRecords;
 Reset();
}
void CWSHM::Reset()
{
 m_pRecords = NULL;
 m_lRecords = m_lAvlRecords = 0;
 m_lMemReallocCount = 0;
 memset(m_ErrorStr, 0, sizeof(m_ErrorStr));
}
bool CWSHM::AddRecord(CWS2 *p_pRec, float p_fFraction)
{
 CWS2 *pRecords;
 unsigned long lAddRec = 1000;
 try {
   if(m_lRecords == m_lAvlRecords) {
     if(p_fFraction)
       lAddRec = max((unsigned long)((float)m_lAvlRecords * (1.0 / p_fFraction - 1)), lAddRec);
     pRecords = new CWS2[m_lAvlRecords + lAddRec];
     if(!pRecords) return(false);
     m_lMemReallocCount ++;
     memset(pRecords, 0, sizeof(CWS2) * (m_lAvlRecords + lAddRec));
     if(m_pRecords) {
       memcpy(pRecords, m_pRecords, sizeof(CWS2) * m_lRecords);
       delete [] m_pRecords;
       }
     m_pRecords = pRecords;
     m_lAvlRecords += lAddRec;
     } }
 catch(CMemoryException *pME) {
   sprintf_s(m_ErrorStr, sizeof(m_ErrorStr), "Wake history memory allocation of %ld bytes failed: ", (unsigned long)sizeof(CWS2) * (m_lAvlRecords + lAddRec));
   pME->GetErrorMessage(m_ErrorStr + strlen(m_ErrorStr), sizeof(m_ErrorStr) - strlen(m_ErrorStr));
   return(false); }
 m_pRecords[m_lRecords] = *p_pRec;
 m_lRecords ++;
 return(true);
}
double CWSHM::GetMemoryUsedKB()
{
 double dVal;
 dVal = (double)(m_lAvlRecords * sizeof(CWS2)) / 1024;
 return(dVal);
}
