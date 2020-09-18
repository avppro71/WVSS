#include "stdafx.h"
#include "PageSensitivity.h"
#include "Global.h"
#include "SensitivityAnalysis.h"
#pragma warning(disable : 4996)
using namespace WVSS;
BEGIN_MESSAGE_MAP(CPageSensitivity, CDialog)
END_MESSAGE_MAP()
CPageSensitivity::CPageSensitivity(CWnd * ) : CPropertyPage(CPageSensitivity::IDD)
{
 m_pSA = NULL;
}
CPageSensitivity::~CPageSensitivity()
{
}
BOOL CPageSensitivity::OnInitDialog()
{
 CPropertyPage::OnInitDialog();
 m_List.SubclassDlgItem(IDC_SENSITIVITY_LIST, this);
 m_List.SetExtendedStyle(m_List.GetExtendedStyle() | LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);
 m_List.InsertColumn(0, "Name", LVCFMT_LEFT, 140);
 m_List.InsertColumn(1, "Value", LVCFMT_LEFT, 100);
 m_List.InsertColumn(2, "Action Type", LVCFMT_LEFT, 100);
 m_List.InitAdvanced();
 UpdateControls();
 return(TRUE);
}
void CPageSensitivity::UpdateControls()
{
 int RowIndex;
 char NameStr[32], ValueStr[128];
 double dVal;
 m_List.SetRedraw(FALSE);
 m_List.DeleteAllItems();
 RowIndex = 0;
 sprintf(NameStr, "On/Off");
 sprintf(ValueStr, "%s", m_pSA->m_bSensitivityAnalysis ? "On" : "Off");
 m_List.InsertItem(LVIF_TEXT | LVIF_STATE | LVIF_PARAM, RowIndex, NameStr, 0, 0, 0, 0);
 m_List.SetItemText(RowIndex, 1, ValueStr);
 m_List.SetItemData(RowIndex++, m_pSA->m_bSensitivityAnalysis ? 1 : 0);
 sprintf(NameStr, "Section number");
 sprintf(ValueStr, "%ld", m_pSA->m_lSectionNum);
 m_List.InsertItem(LVIF_TEXT | LVIF_STATE | LVIF_PARAM, RowIndex, NameStr, 0, 0, 0, 0);
 m_List.SetItemText(RowIndex, 1, ValueStr);
 m_List.SetItemText(RowIndex++, 2, CSensitivityAnalysisSetup::m_ValueAdjTypeNames[m_pSA->m_SectionNumAdjType]);
 sprintf(NameStr, "Plane type");
 sprintf(ValueStr, "%s", m_pSA->m_PlaneType);
 m_List.InsertItem(LVIF_TEXT | LVIF_STATE | LVIF_PARAM, RowIndex, NameStr, 0, 0, 0, 0);
 m_List.SetItemText(RowIndex, 1, ValueStr);
 m_List.SetItemText(RowIndex++, 2, CSensitivityAnalysisSetup::m_ValueAdjTypeNames[m_pSA->m_PlaneTypeAdjType]);
 sprintf(NameStr, "Model c1");
 sprintf(ValueStr, "%.2lf", m_pSA->m_dC1);
 m_List.InsertItem(LVIF_TEXT | LVIF_STATE | LVIF_PARAM, RowIndex, NameStr, 0, 0, 0, 0);
 m_List.SetItemText(RowIndex, 1, ValueStr);
 m_List.SetItemText(RowIndex++, 2, CSensitivityAnalysisSetup::m_ValueAdjTypeNames[m_pSA->m_C1AdjType]);
 sprintf(NameStr, "Model c2");
 sprintf(ValueStr, "%.2lf", m_pSA->m_dC2);
 m_List.InsertItem(LVIF_TEXT | LVIF_STATE | LVIF_PARAM, RowIndex, NameStr, 0, 0, 0, 0);
 m_List.SetItemText(RowIndex, 1, ValueStr);
 m_List.SetItemText(RowIndex++, 2, CSensitivityAnalysisSetup::m_ValueAdjTypeNames[m_pSA->m_C2AdjType]);
 sprintf(NameStr, "Model EDR");
 sprintf(ValueStr, "%.2lf", m_pSA->m_dEDR);
 m_List.InsertItem(LVIF_TEXT | LVIF_STATE | LVIF_PARAM, RowIndex, NameStr, 0, 0, 0, 0);
 m_List.SetItemText(RowIndex, 1, ValueStr);
 m_List.SetItemText(RowIndex++, 2, CSensitivityAnalysisSetup::m_ValueAdjTypeNames[m_pSA->m_EDRAdjType]);
 sprintf(NameStr, "Circulation");
 sprintf(ValueStr, "%.0lf", m_pSA->m_dCurrentWakeGamma);
 m_List.InsertItem(LVIF_TEXT | LVIF_STATE | LVIF_PARAM, RowIndex, NameStr, 0, 0, 0, 0);
 m_List.SetItemText(RowIndex, 1, ValueStr);
 m_List.SetItemText(RowIndex++, 2, CSensitivityAnalysisSetup::m_ValueAdjTypeNames[m_pSA->m_CurrentWakeGammaAdjType]);
 sprintf(NameStr, "Wingspan (m)");
 sprintf(ValueStr, "%.2lf", m_pSA->m_dWingSpan_m);
 m_List.InsertItem(LVIF_TEXT | LVIF_STATE | LVIF_PARAM, RowIndex, NameStr, 0, 0, 0, 0);
 m_List.SetItemText(RowIndex, 1, ValueStr);
 m_List.SetItemText(RowIndex++, 2, CSensitivityAnalysisSetup::m_ValueAdjTypeNames[m_pSA->m_WingSpanAdjType]);
 sprintf(NameStr, "Altitude (m)");
 sprintf(ValueStr, "%.2lf", m_pSA->m_dPlaneAltitude_m);
 m_List.InsertItem(LVIF_TEXT | LVIF_STATE | LVIF_PARAM, RowIndex, NameStr, 0, 0, 0, 0);
 m_List.SetItemText(RowIndex, 1, ValueStr);
 m_List.SetItemText(RowIndex++, 2, CSensitivityAnalysisSetup::m_ValueAdjTypeNames[m_pSA->m_PlaneAltitudeAdjType]);
 sprintf(NameStr, "Plane mass (kg)");
 sprintf(ValueStr, "%.0lf", m_pSA->m_dPlaneMass_kg);
 m_List.InsertItem(LVIF_TEXT | LVIF_STATE | LVIF_PARAM, RowIndex, NameStr, 0, 0, 0, 0);
 m_List.SetItemText(RowIndex, 1, ValueStr);
 m_List.SetItemText(RowIndex++, 2, CSensitivityAnalysisSetup::m_ValueAdjTypeNames[m_pSA->m_PlaneMassAdjType]);
 sprintf(NameStr, "Wind direction (deg from)");
 dVal = m_pSA->m_dWindDirection_rad_to - M_PI;
 if(dVal < 0) dVal += 2 * M_PI;
 sprintf(ValueStr, "%.0lf", Degrees(dVal));
 m_List.InsertItem(LVIF_TEXT | LVIF_STATE | LVIF_PARAM, RowIndex, NameStr, 0, 0, 0, 0);
 m_List.SetItemText(RowIndex, 1, ValueStr);
 m_List.SetItemText(RowIndex++, 2, CSensitivityAnalysisSetup::m_ValueAdjTypeNames[m_pSA->m_WindDirectionAdjType]);
 sprintf(NameStr, "Wind speed (kts)");
 sprintf(ValueStr, "%.0lf", m_pSA->m_dWS * 3.6 / 1.852);
 m_List.InsertItem(LVIF_TEXT | LVIF_STATE | LVIF_PARAM, RowIndex, NameStr, 0, 0, 0, 0);
 m_List.SetItemText(RowIndex, 1, ValueStr);
 m_List.SetItemText(RowIndex++, 2, CSensitivityAnalysisSetup::m_ValueAdjTypeNames[m_pSA->m_WindSpeedAdjType]);
 sprintf(NameStr, "Aircraft speed (kts)");
 sprintf(ValueStr, "%.0lf", m_pSA->m_dASMS * 3.6 / 1.852);
 m_List.InsertItem(LVIF_TEXT | LVIF_STATE | LVIF_PARAM, RowIndex, NameStr, 0, 0, 0, 0);
 m_List.SetItemText(RowIndex, 1, ValueStr);
 m_List.SetItemText(RowIndex++, 2, CSensitivityAnalysisSetup::m_ValueAdjTypeNames[m_pSA->m_AircraftSpeedAdjType]);
 sprintf(NameStr, "Initial circulation");
 sprintf(ValueStr, "%.0lf", m_pSA->m_dInitialWakeGamma);
 m_List.InsertItem(LVIF_TEXT | LVIF_STATE | LVIF_PARAM, RowIndex, NameStr, 0, 0, 0, 0);
 m_List.SetItemText(RowIndex, 1, ValueStr);
 m_List.SetItemText(RowIndex++, 2, CSensitivityAnalysisSetup::m_ValueAdjTypeNames[m_pSA->m_InitialWakeGammaAdjType]);
 m_List.SetRedraw();
 m_List.UpdateWindow();
}
