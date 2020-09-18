#include "stdafx.h"
#include "EarthElevation.h"
#include "math.h"
#pragma warning(disable : 4996)
using namespace WVSS;
CEarthElevation::CEarthElevation()
{
 Reset();
}
CEarthElevation::~CEarthElevation()
{
 CloseFile();
}
void CEarthElevation::Reset()
{
 m_nCurStartLat = -32768;
 m_nCurStartLong = -32768;
 m_dwCurFilePos = (DWORD)-1L;
 m_hFile = INVALID_HANDLE_VALUE;
 m_nCellSize = -32768;
 m_nCellMax = -32768;
}
void CEarthElevation::CloseFile()
{
 if(m_hFile != INVALID_HANDLE_VALUE)
   CloseHandle(m_hFile);
 m_hFile = INVALID_HANDLE_VALUE;
}
void CEarthElevation::ClearMissingElevationAreaLists()
{
 m_MissingAreas.clear();
}
bool CEarthElevation::GetElevation(double p_dLat, double p_dLong, double *p_pdValue, short p_nPass)
{
 char FileName[MAX_PATH];
 short Row, Col;
 unsigned long lOffset, lFileSize;
 unsigned char cVal[3];
 short nStartLat, nStartLong;
 bool bSouth, bWest, bOpenFile, bProcOK;
 DWORD dwRead;
 char AreaName[16];
 if(!p_pdValue) return(false);
 *p_pdValue = 0; 
 cVal[0] = cVal[1] = cVal[2] = 255; 
 bSouth = (p_dLat < 0);
 bWest = (p_dLong < 0);
 nStartLat = (short)p_dLat + (bSouth ? -1 : 0);
 nStartLong = (short)p_dLong + (bWest ? -1 : 0);
 bOpenFile = false;
 if(nStartLat != m_nCurStartLat || 
    nStartLong != m_nCurStartLong || 
    m_dwCurFilePos == (DWORD)-1L || 
    m_hFile == INVALID_HANDLE_VALUE) { 
   bOpenFile = true;
   CloseFile();
   Reset(); }
 for(;;) {
   bProcOK = false;
   if(bOpenFile) {
     sprintf(FileName, "%s\\", m_FSG.m_EDD);
     sprintf(AreaName, "%c%.2d%c%.3d", bSouth ? 'S' : 'N', abs(nStartLat), bWest ? 'W' : 'E', abs(nStartLong));
     strcpy(FileName + strlen(FileName), AreaName);
     if(p_nPass != -1)
       sprintf(FileName + strlen(FileName), "T%d", p_nPass);
     sprintf(FileName + strlen(FileName), ".hgt", p_nPass);
     m_hFile = CreateFile(FileName, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL | FILE_FLAG_RANDOM_ACCESS, NULL);
     if(m_hFile == INVALID_HANDLE_VALUE) break;
     lFileSize = GetFileSize(m_hFile, NULL);
     if(lFileSize == INVALID_FILE_SIZE) break;
     if(lFileSize == 1201 * 1201 * 2) m_nCellSize = 3;
     else if(lFileSize == 3601 * 3601 * 2) m_nCellSize = 1;
     else break;
     m_nCellMax = 3600 / m_nCellSize;
     m_nCurStartLat = nStartLat;
     m_nCurStartLong = nStartLong;
     m_dwCurFilePos = 0;
     }
   Row = m_nCellMax - (short)((p_dLat - nStartLat) * 3600 / m_nCellSize);
   Col = (short)((p_dLong - nStartLong) * 3600 / m_nCellSize);
   lOffset = (m_nCellMax + 1) * Row + Col;
   lOffset *= 2;
   if(SetFilePointer(m_hFile, (long)lOffset - (long)m_dwCurFilePos, NULL, FILE_CURRENT) == INVALID_SET_FILE_POINTER) {
     m_dwCurFilePos = (DWORD)-1L;
     break; }
   m_dwCurFilePos = lOffset;
   if(!ReadFile(m_hFile, cVal, 2, &dwRead, NULL)) {
     m_dwCurFilePos = (DWORD)-1L; 
     break; }
   cVal[2] = cVal[0];
   (*p_pdValue) = *(short *)(cVal + 1);
   bProcOK = true;
   break; }
 if(!bProcOK) {
   if(m_MissingAreas.find(AreaName) == m_MissingAreas.end())
     m_MissingAreas[AreaName] = 0;
   }
 return(bProcOK);
}
