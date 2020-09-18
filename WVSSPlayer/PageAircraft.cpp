#include "stdafx.h"
#include "PageAircraft.h"
#include "Global.h"
#include "Severity.h"
#include "Airplanes2.h"
#pragma warning(disable : 4996)
using namespace WVSS;
BEGIN_MESSAGE_MAP(CPageAircraft, CDialog)
END_MESSAGE_MAP()
CPageAircraft::CPageAircraft(CWnd * ) : CPropertyPage(CPageAircraft::IDD)
{
}
CPageAircraft::~CPageAircraft()
{
}
BOOL CPageAircraft::OnInitDialog()
{
 CPropertyPage::OnInitDialog();
 m_List.SubclassDlgItem(IDC_AIRCRAFT_LIST, this);
 m_List.SetExtendedStyle(m_List.GetExtendedStyle() | LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);
 m_List.InsertColumn(0, "Callsign", LVCFMT_LEFT, 100);
 m_List.InsertColumn(1, "Model", LVCFMT_LEFT, 50);
 m_List.InsertColumn(2, "ICAO Cat", LVCFMT_LEFT, 60);
 m_List.InsertColumn(3, "FAA Cat", LVCFMT_LEFT, 60);
 m_List.InsertColumn(4, "Wingspan", LVCFMT_LEFT, 60);
 m_List.InsertColumn(5, "Start frame", LVCFMT_RIGHT, 60);
 m_List.InsertColumn(6, "Frames", LVCFMT_RIGHT, 50);
 m_List.InsertColumn(7, "End frame", LVCFMT_RIGHT, 70);
 m_List.InsertColumn(8, "Aircraft OK", LVCFMT_LEFT, 70);
 m_List.InsertColumn(9, "Departure", LVCFMT_LEFT, 60);
 m_List.InsertColumn(10, "Arrival", LVCFMT_LEFT, 60);
 m_List.InitAdvanced();
 return(TRUE);
}
void CPageAircraft::UpdateResults(std::vector <CADAA *>*p_pAircraft)
{
 char tstr[128];
 unsigned short i, RowIndex;
 size_t Count;
 CADAA *pACData;
 m_List.SetRedraw(FALSE);
 m_List.DeleteAllItems();
 Count = p_pAircraft->size();
 RowIndex = 0;
 for(i = 0; i < Count; i ++) {
   pACData = p_pAircraft->at(i);
   m_List.InsertItem(LVIF_TEXT | LVIF_STATE | LVIF_PARAM, RowIndex, pACData->m_CallSign, 0, 0, 0, 0);
   m_List.SetItemText(RowIndex, 1, pACData->m_ACModel);
   sprintf_s(tstr, sizeof(tstr), "%s", pACData->m_pAircraft ? CAPM::GetICAOCatName(pACData->m_pAircraft->m_ICAOCat) : "UNKNOWN");
   m_List.SetItemText(RowIndex, 2, tstr);
   sprintf_s(tstr, sizeof(tstr), "%s", pACData->m_pAircraft ? CAPM::GetFAACatName(pACData->m_pAircraft->m_FAACat) : "UNKNOWN");
   m_List.SetItemText(RowIndex, 3, tstr);
   strcpy_s(tstr, sizeof(tstr), "UNKNOWN");
   if(pACData->m_pAircraft)
     sprintf_s(tstr, sizeof(tstr), "%.1f m", pACData->m_pAircraft->m_dWSM);
   m_List.SetItemText(RowIndex, 4, tstr);
   sprintf_s(tstr, sizeof(tstr), "%ld", pACData->m_lStartPos);
   m_List.SetItemText(RowIndex, 5, tstr);
   sprintf_s(tstr, sizeof(tstr), "%ld", pACData->m_lTrackPointCount);
   m_List.SetItemText(RowIndex, 6, tstr);
   sprintf_s(tstr, sizeof(tstr), "%ld", pACData->m_lStartPos + pACData->m_lTrackPointCount);
   m_List.SetItemText(RowIndex, 7, tstr);
   sprintf_s(tstr, sizeof(tstr), "%s", pACData->m_pAircraft ? "OK" : "UNKNOWN");
   m_List.SetItemText(RowIndex, 8, tstr);
   m_List.SetItemText(RowIndex, 9, pACData->m_DepartureAirport);
   m_List.SetItemText(RowIndex, 10, pACData->m_ArrivalAirport);
   RowIndex ++; }
 m_List.SetRedraw();
 m_List.UpdateWindow();
}
