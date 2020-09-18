#include "stdafx.h"
#include "PageFrame.h"
#include "Global.h"
#include "Severity.h"
#include "Airplanes2.h"
#pragma warning(disable : 4996)
using namespace WVSS;
void CPageFrame::CFrameData::Reset()
{
 *m_CallSign = '\0';
 m_bWindSet = m_bWakeSet = false;
 m_dWindDirectionFrom_deg = 0;
 m_dWindSpeed_kts = 0;
 m_lStartWakeSection = -1;
 m_lEndWakeSection = -1;
 m_dLatDeg = m_dLongDeg = 0;
}
BEGIN_MESSAGE_MAP(CPageFrame, CDialog)
END_MESSAGE_MAP()
CPageFrame::CPageFrame(CWnd * ) : CPropertyPage(CPageFrame::IDD)
{
}
CPageFrame::~CPageFrame()
{
}
BOOL CPageFrame::OnInitDialog()
{
 CPropertyPage::OnInitDialog();
 m_ListPlane.SubclassDlgItem(IDC_FRAME_LIST_PLANE, this);
 m_ListWake.SubclassDlgItem(IDC_FRAME_LIST_WAKE, this);
 m_ListPlane.SetExtendedStyle(m_ListPlane.GetExtendedStyle() | LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);
 m_ListPlane.InsertColumn(0, "Callsign", LVCFMT_LEFT, 100);
 m_ListPlane.InsertColumn(1, "Type", LVCFMT_LEFT, 50);
 m_ListPlane.InsertColumn(2, "Speed (kts)", LVCFMT_RIGHT, 60);
 m_ListPlane.InsertColumn(3, "Altitude (ft/m)", LVCFMT_RIGHT, 80);
 m_ListPlane.InsertColumn(4, "Wind dir (deg)", LVCFMT_RIGHT, 80);
 m_ListPlane.InsertColumn(5, "Wind speed (kts)", LVCFMT_RIGHT, 100);
 m_ListPlane.InsertColumn(6, "Lat", LVCFMT_RIGHT, 70);
 m_ListPlane.InsertColumn(7, "Long", LVCFMT_RIGHT, 70);
 m_ListWake.SetExtendedStyle(m_ListWake.GetExtendedStyle() | LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);
 m_ListWake.InsertColumn(0, "Callsign", LVCFMT_LEFT, 100);
 m_ListWake.InsertColumn(1, "Start section", LVCFMT_RIGHT, 70);
 m_ListWake.InsertColumn(2, "End section", LVCFMT_RIGHT, 70);
 m_ListWake.InsertColumn(3, "Wake sections", LVCFMT_RIGHT, 80);
 m_ListPlane.InitAdvanced();
 m_ListWake.InitAdvanced();
 return(TRUE);
}
void CPageFrame::UpdateFrameData()
{
 char tstr[128];
 unsigned short i, RowIndex;
 size_t Count;
 CFrameData *pFD;
 double dWindFromDeg180;
 m_ListPlane.SetRedraw(FALSE);
 m_ListPlane.DeleteAllItems();
 m_ListWake.SetRedraw(FALSE);
 m_ListWake.DeleteAllItems();
 Count = m_FrameData.size();
 RowIndex = 0;
 for(i = 0; i < Count; i ++) {
   pFD = &m_FrameData[i];
   if(!pFD->m_bWindSet) continue;
   m_ListPlane.InsertItem(LVIF_TEXT | LVIF_STATE | LVIF_PARAM, RowIndex, pFD->m_CallSign, 0, 0, 0, 0);
   m_ListPlane.SetItemText(RowIndex, 1, pFD->m_AircraftType);
   sprintf(tstr, "%.0f", pFD->m_dSpeedMS * 3.6 / 1.852); 
   m_ListPlane.SetItemText(RowIndex, 2, tstr);
   sprintf(tstr, "%.0f/%.0f", pFD->m_nAltitudeM / 0.3048, pFD->m_nAltitudeM);
   m_ListPlane.SetItemText(RowIndex, 3, tstr);
   dWindFromDeg180 = pFD->m_dWindDirectionFrom_deg;
   if(dWindFromDeg180 > 180) dWindFromDeg180 -=360;
   sprintf(tstr, "%.1lf", dWindFromDeg180);
   m_ListPlane.SetItemText(RowIndex, 4, tstr);
   sprintf(tstr, "%.1lf", pFD->m_dWindSpeed_kts);
   m_ListPlane.SetItemText(RowIndex, 5, tstr);
   sprintf(tstr, "%.6lf", pFD->m_dLatDeg);
   m_ListPlane.SetItemText(RowIndex, 6, tstr);
   sprintf(tstr, "%.6lf", pFD->m_dLongDeg);
   m_ListPlane.SetItemText(RowIndex, 7, tstr);
   RowIndex ++; }
 Count = m_FrameData.size();
 RowIndex = 0;
 for(i = 0; i < Count; i ++) {
   pFD = &m_FrameData[i];
   if(!pFD->m_bWakeSet) continue;
   m_ListWake.InsertItem(LVIF_TEXT | LVIF_STATE | LVIF_PARAM, RowIndex, pFD->m_CallSign, 0, 0, 0, 0);
   sprintf(tstr, "%ld", pFD->m_lStartWakeSection);
   m_ListWake.SetItemText(RowIndex, 1, tstr);
   sprintf(tstr, "%ld", pFD->m_lStartWakeSection - pFD->m_lEndWakeSection);
   m_ListWake.SetItemText(RowIndex, 2, tstr);
   sprintf(tstr, "%ld", pFD->m_lEndWakeSection + 1);
   m_ListWake.SetItemText(RowIndex, 3, tstr);
   RowIndex ++; }
 m_ListPlane.SetRedraw();
 m_ListPlane.UpdateWindow();
 m_ListWake.SetRedraw();
 m_ListWake.UpdateWindow();
}
