#include "StdAfx.h"
#include "EDRManager.h"
using namespace WVSS;
bool CEDRManager::rEDRRecord::operator < (const rEDRRecord &p_Other) const
{
 return(m_dAltitude_m < p_Other.m_dAltitude_m);
}
CEDRManager::CEDRManager()
{
 m_pRecords = NULL;
 m_lRecords = m_lAvlRecords = 0;
}
CEDRManager::~CEDRManager()
{
 if(m_pRecords) delete [] m_pRecords;
}
bool CEDRManager::AddRecord(rEDRRecord *p_pEDRRecord)
{
 int Add = 100;
 rEDRRecord *pRecords;
 if(m_lRecords == m_lAvlRecords) {
   pRecords = new rEDRRecord[m_lAvlRecords + Add];
   if(!pRecords) return(false);
   memset(pRecords, 0,sizeof(rEDRRecord) * (m_lAvlRecords + Add));
   if(m_pRecords) {
     memcpy(pRecords, m_pRecords, sizeof(rEDRRecord) * m_lRecords);
     delete [] m_pRecords; }
   m_pRecords = pRecords;
   m_lAvlRecords += Add; }
 m_pRecords[m_lRecords] = *p_pEDRRecord;
 m_lRecords ++;
 return(true);
}
void CEDRManager::Sort()
{
 qsort(m_pRecords, m_lRecords, sizeof(rEDRRecord), StaticCompare);
}
bool CEDRManager::GetEDR(double p_dAltitude, double *p_pdEDRDimensioned)
{
 unsigned long Left, Right, Mid = 0;
 if(!p_pdEDRDimensioned || !m_lRecords) return(false);
 if(p_dAltitude <= m_pRecords[0].m_dAltitude_m) {
   *p_pdEDRDimensioned = m_pRecords[0].m_dEDR;
   return(true); }
 if(p_dAltitude >= m_pRecords[m_lRecords - 1].m_dAltitude_m) {
   *p_pdEDRDimensioned = m_pRecords[m_lRecords - 1].m_dEDR;
   return(true); }
 Left = 0;
 Right = m_lRecords - 1;
 for(;;) {
		  Mid = (Left + Right) / 2;
		  if(p_dAltitude < m_pRecords[Mid].m_dAltitude_m)       
			   Right = Mid - 1;
		  else if(p_dAltitude > m_pRecords[Mid].m_dAltitude_m)  
			   Left = Mid + 1;
		  else {
			   *p_pdEDRDimensioned = m_pRecords[Mid].m_dEDR;
			   return(true); }
		  if(Left > Right)
			   break; }
 ASSERT(Left > 0);
 Left --;
 Right = Left + 1;
 ASSERT(Right < m_lRecords);
 *p_pdEDRDimensioned = m_pRecords[Left].m_dEDR + (m_pRecords[Right].m_dEDR - m_pRecords[Left].m_dEDR) * (p_dAltitude - m_pRecords[Left].m_dAltitude_m) / (m_pRecords[Right].m_dAltitude_m - m_pRecords[Left].m_dAltitude_m);
 return(true);
}
int CEDRManager::StaticCompare(const void *p_p1, const void *p_p2)
{
 rEDRRecord *p1, *p2;
 p1 = (rEDRRecord *)p_p1;
 p2 = (rEDRRecord *)p_p2;
 if(p1->m_dAltitude_m < p2->m_dAltitude_m) return(-1);
 if(p1->m_dAltitude_m > p2->m_dAltitude_m) return(1);
 return(0);
}
