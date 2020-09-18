#include "stdafx.h"
#include "SITARWVSSPlayer.h"
#include "PageWakeFrame.h"
using namespace WVSS;
#pragma warning(disable : 4996)
void CPageWakeFrame::CWakeFrameData::Reset()
{
 strcpy_s(m_CallSign, sizeof(m_CallSign), "UNKNOWN");
 strcpy_s(m_AircraftType, sizeof(m_AircraftType), "UNKNOWN");
 m_ddZL = m_ddZC = m_ddZR = 0;
 m_ddHL = m_ddHC = m_ddHR = 0;
 m_bPositionOverride = false;
 m_bGammaOverride = false;
 m_ddZOverride = m_ddHOverride = m_dGammaOverride = 0;
}
IMPLEMENT_DYNAMIC(CPageWakeFrame, CPropertyPage)
BEGIN_MESSAGE_MAP(CPageWakeFrame, CPropertyPage)
END_MESSAGE_MAP()
CPageWakeFrame::CPageWakeFrame(CWnd *p_pParent)	: CPropertyPage(CPageWakeFrame::IDD)
{
}
CPageWakeFrame::~CPageWakeFrame()
{
}
BOOL CPageWakeFrame::OnInitDialog()
{
 CPropertyPage::OnInitDialog();
 m_ListWakeInteraction.SubclassDlgItem(IDC_WAKEFRAME_LIST_INTERACTION, this);
 m_ListWakeInteraction.SetExtendedStyle(m_ListWakeInteraction.GetExtendedStyle() | LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);
 m_ListWakeInteraction.InsertColumn(0, "Callsign", LVCFMT_LEFT, 100);
 m_ListWakeInteraction.InsertColumn(1, "Left core\ndZ/dH/Gamma", LVCFMT_RIGHT, 100);
 m_ListWakeInteraction.InsertColumn(2, "Wake center\ndZ/dH", LVCFMT_RIGHT, 100);
 m_ListWakeInteraction.InsertColumn(3, "Right core\ndZ/dH/Gamma", LVCFMT_RIGHT, 100);
 m_ListWakeInteraction.InsertColumn(4, "Position\noverride", LVCFMT_LEFT, 50);
 m_ListWakeInteraction.InsertColumn(5, "Gamma\noverride", LVCFMT_LEFT, 50);
 m_ListWakeInteraction.InitAdvanced();
 return(TRUE);
}
void CPageWakeFrame::UpdateWakeFrameData()
{
 char tstr[128];
 unsigned short i, RowIndex;
 size_t Count;
 CWakeFrameData *pWFD;
 double dCurdZL, dCurdHL, dCurdZC, dCurdHC, dCurdZR, dCurdHR, dGammaL, dGammaR;
 m_ListWakeInteraction.SetRedraw(FALSE);
 m_ListWakeInteraction.DeleteAllItems();
 Count = m_WakeFrameData.size();
 RowIndex = 0;
 for(i = 0; i < Count; i ++) {
   pWFD = &m_WakeFrameData[i];
  m_ListWakeInteraction.InsertItem(LVIF_TEXT | LVIF_STATE | LVIF_PARAM, RowIndex, pWFD->m_CallSign, 0, 0, 0, 0);
   dCurdZL = pWFD->m_ddZL;
   dCurdHL = pWFD->m_ddHL;
   dCurdZC = pWFD->m_ddZC;
   dCurdHC = pWFD->m_ddHC;
   dCurdZR = pWFD->m_ddZR;
   dCurdHR = pWFD->m_ddHR;
   dGammaL = pWFD->m_dGammaL;
   dGammaR = pWFD->m_dGammaR;
   if(pWFD->m_bPositionOverride) {
     dCurdZL = (pWFD->m_ddZL - pWFD->m_ddZC) + pWFD->m_ddZOverride;
     dCurdHL = (pWFD->m_ddHL - pWFD->m_ddHC) + pWFD->m_ddHOverride;
     dCurdZC = pWFD->m_ddZOverride;
     dCurdHC = pWFD->m_ddHOverride;
     dCurdZR = (pWFD->m_ddZR - pWFD->m_ddZC) + pWFD->m_ddZOverride;
     dCurdHR = (pWFD->m_ddHR - pWFD->m_ddHC) + pWFD->m_ddHOverride;
     }
   if(pWFD->m_bGammaOverride)
     dGammaL = dGammaR = pWFD->m_dGammaOverride;
   sprintf(tstr, "%.1f / %.1f / %.0f", dCurdZL, dCurdHL, dGammaL);
   m_ListWakeInteraction.SetItemText(RowIndex, 1, tstr);
   sprintf(tstr, "%.1f / %.1f", dCurdZC, dCurdHC);
   m_ListWakeInteraction.SetItemText(RowIndex, 2, tstr);
   sprintf(tstr, "%.1f / %.1f / %.0f", dCurdZR, dCurdHR, dGammaR);
   m_ListWakeInteraction.SetItemText(RowIndex, 3, tstr);
   m_ListWakeInteraction.SetItemText(RowIndex, 4, pWFD->m_bPositionOverride ? "Yes" : "No");
   m_ListWakeInteraction.SetItemText(RowIndex, 5, pWFD->m_bGammaOverride ? "Yes" : "No");
   RowIndex ++; }
 m_ListWakeInteraction.SetRedraw();
 m_ListWakeInteraction.UpdateWindow();
}
