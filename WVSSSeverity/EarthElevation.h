#pragma once
#include "IWVSSSeverity.h"
#include <map>
namespace WVSS {
class CEarthElevation
{
private:
private:
 short m_nCurStartLat, m_nCurStartLong; 
 DWORD m_dwCurFilePos; 
 HANDLE m_hFile;
 CWEAIF m_FSG; 
 short m_nCellSize, m_nCellMax;
 std::map<CString, bool>m_MissingAreas;
protected:
public:
private:
 void Reset();
protected:
public:
 CEarthElevation();
 ~CEarthElevation();
 void SetFlags(CWEAIF *p_pFlags) { m_FSG = *p_pFlags; }
 bool GetElevation(double p_dLat, double p_dLong, double *p_pdValue, short p_nPass = -1);
 void CloseFile();
 const std::map<CString, bool> *GetMissingElevationAreas() { return(&m_MissingAreas); }
 void ClearMissingElevationAreaLists();
};
} 
