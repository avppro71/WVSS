#pragma once
#include <list>
namespace WVSS {
class CEDRManager
{
private:
protected:
public:
 struct rEDRRecord {
   double m_dAltitude_m; 
   double m_dEDR; 
   bool rEDRRecord::operator < (const rEDRRecord &p_Other) const;
   };
public:
 rEDRRecord *m_pRecords;
 unsigned long m_lRecords, m_lAvlRecords;
private:
 static int StaticCompare(const void *p_p1, const void *p_p2);
protected:
public:
 CEDRManager();
 ~CEDRManager();
 bool AddRecord(rEDRRecord *p_pEDRRecord); 
 void Sort();
 bool GetEDR(double p_dAltitude, double *p_pdEDRDimensioned);
};
} 
