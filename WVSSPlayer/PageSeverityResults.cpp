#include "stdafx.h"
#include "PageSeverityResults.h"
#include "Global.h"
#pragma warning(disable : 4996)
using namespace WVSS;
BEGIN_MESSAGE_MAP(CPageSeverityResults, CPropertyPage)
END_MESSAGE_MAP()
CPageSeverityResults::CPageSeverityResults(CWnd * ) : CPropertyPage(CPageSeverityResults::IDD)
{
}
CPageSeverityResults::~CPageSeverityResults()
{
}
BOOL CPageSeverityResults::OnInitDialog()
{
 CPropertyPage::OnInitDialog();
 m_List.SubclassDlgItem(IDC_SEVERITY_RES_LIST, this);
 m_List.SetExtendedStyle(m_List.GetExtendedStyle() | LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);
 m_List.InsertColumn(0, "Leader", LVCFMT_LEFT, 100);
 m_List.InsertColumn(1, "Follower", LVCFMT_LEFT, 100);
 m_List.InsertColumn(2, "Time", LVCFMT_LEFT, 120);
 m_List.InsertColumn(3, "Time From\nLeader (s)", LVCFMT_RIGHT, 60);
 m_List.InsertColumn(4, "Max Bank\n(deg)", LVCFMT_RIGHT, 60);
 m_List.InsertColumn(5, "Max Angular Speed\nof Bank (deg/s)", LVCFMT_RIGHT, 105);
 m_List.InsertColumn(6, "Max Altitude\nLoss (ft)", LVCFMT_RIGHT, 70);
 m_List.InsertColumn(7, "Aircraft\nDistance (nm)", LVCFMT_RIGHT, 73);
 m_List.InsertColumn(8, "Wind Direction\n(deg)", LVCFMT_RIGHT, 80);
 m_List.InsertColumn(9, "Wind Speed\n(kts)", LVCFMT_RIGHT, 70);
 m_List.InsertColumn(10, "Plane to Wake\nDistance Horz (m)", LVCFMT_RIGHT, 98);
 m_List.InsertColumn(11, "Plane to Wake\nDistance Vert (m)", LVCFMT_RIGHT, 95);
 m_List.InsertColumn(12, "Frame", LVCFMT_RIGHT, 52);
 m_List.InitAdvanced();
 return(TRUE);
}
void CPageSeverityResults::UpdateResults(IWVSSSeverity *p_pSeverity)
{
 char tstr[128];
 unsigned short i, RowIndex;
 double dWindDirFrom;
 m_List.SetRedraw(FALSE);
 m_List.DeleteAllItems();
 RowIndex = 0;
 for(i = 0; i < p_pSeverity->m_FullSeverityOutput.size(); i ++) {
   sprintf_s(tstr, sizeof(tstr), "%s", p_pSeverity->m_FullSeverityOutput[i].m_GeneratorCallSign);
   m_List.InsertItem(LVIF_TEXT | LVIF_STATE | LVIF_PARAM, RowIndex, tstr, 0, 0, 0, 0);
   m_List.SetItemText(RowIndex, 1, p_pSeverity->m_FullSeverityOutput[i].m_FollowerCallSign);
   sprintf_s(tstr, sizeof(tstr), "%.4d-%.2d-%.2d %.2d:%.2d:%.2d", p_pSeverity->m_FullSeverityOutput[i].m_EntryTime.wYear, p_pSeverity->m_FullSeverityOutput[i].m_EntryTime.wMonth, p_pSeverity->m_FullSeverityOutput[i].m_EntryTime.wDay, p_pSeverity->m_FullSeverityOutput[i].m_EntryTime.wHour, p_pSeverity->m_FullSeverityOutput[i].m_EntryTime.wMinute, p_pSeverity->m_FullSeverityOutput[i].m_EntryTime.wSecond);
   m_List.SetItemText(RowIndex, 2, tstr);
   sprintf_s(tstr, sizeof(tstr), "%.1lf", p_pSeverity->m_FullSeverityOutput[i].m_dTimeFromGeneratorS);
   m_List.SetItemText(RowIndex, 3, tstr);
   sprintf_s(tstr, sizeof(tstr), "%.1lf", Degrees(p_pSeverity->m_FullSeverityOutput[i].m_dMaxBankRad));
   m_List.SetItemText(RowIndex, 4, tstr);
   sprintf_s(tstr, sizeof(tstr), "%.1lf", Degrees(p_pSeverity->m_FullSeverityOutput[i].m_dMaxAngularSpeedOfBankRadPerS));
   m_List.SetItemText(RowIndex, 5, tstr);
   sprintf_s(tstr, sizeof(tstr), "%.1lf", p_pSeverity->m_FullSeverityOutput[i].m_dMaxAltitudeLossM / 0.3048);
   m_List.SetItemText(RowIndex, 6, tstr);
   sprintf_s(tstr, sizeof(tstr), "%.1lf", p_pSeverity->m_FullSeverityOutput[i].m_dGFDM  / 1852);
   m_List.SetItemText(RowIndex, 7, tstr);
   dWindDirFrom = p_pSeverity->m_FullSeverityOutput[i].m_dWindDirectionDegTo - 180;
   if(dWindDirFrom < 0) dWindDirFrom += 360;
   sprintf_s(tstr, sizeof(tstr), "%.1lf", dWindDirFrom);
   m_List.SetItemText(RowIndex, 8, tstr);
   sprintf_s(tstr, sizeof(tstr), "%.1lf", p_pSeverity->m_FullSeverityOutput[i].m_dWindSpeedMS * 3.6 / 1.852); 
   m_List.SetItemText(RowIndex, 9, tstr);
   sprintf_s(tstr, sizeof(tstr), "%.1lf", p_pSeverity->m_FullSeverityOutput[i].m_dPlaneToWakeHorz);
   m_List.SetItemText(RowIndex, 10, tstr);
   sprintf_s(tstr, sizeof(tstr), "%.1lf", p_pSeverity->m_FullSeverityOutput[i].m_dPlaneToWakeVert);
   m_List.SetItemText(RowIndex, 11, tstr);
   sprintf_s(tstr, sizeof(tstr), "%ld", p_pSeverity->m_FullSeverityOutput[i].m_lPlayerPos);
   m_List.SetItemText(RowIndex, 12, tstr);
   RowIndex ++; }
 m_List.SetRedraw();
 m_List.UpdateWindow();
}
