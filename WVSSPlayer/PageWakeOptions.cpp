#include "stdafx.h"
#include "PageWakeOptions.h"
#pragma warning(disable : 4996)
using namespace WVSS;
BEGIN_MESSAGE_MAP(CPageWakeOptions, CDialog)
 ON_BN_CLICKED(IDC_CALC_SEVERITY, &CPageWakeOptions::OnCalcSeverityButtonClicked)
END_MESSAGE_MAP()
CPageWakeOptions::CPageWakeOptions(CWnd * ) : CPropertyPage(CPageWakeOptions::IDD)
{
}
CPageWakeOptions::~CPageWakeOptions()
{
}
BOOL CPageWakeOptions::OnInitDialog()
{
 char tstr[32];
 int Index;
 CPropertyPage::OnInitDialog();
 m_FlightSpeedCombo.SubclassDlgItem(IDO_FLIGHT_SPEED, this);
 Index = m_FlightSpeedCombo.InsertString(-1, "1x");
 m_FlightSpeedCombo.SetItemData(Index, 1);
 Index = m_FlightSpeedCombo.InsertString(-1, "2x");
 m_FlightSpeedCombo.SetItemData(Index, 2);
 Index = m_FlightSpeedCombo.InsertString(-1, "3x");
 m_FlightSpeedCombo.SetItemData(Index, 3);
 Index = m_FlightSpeedCombo.InsertString(-1, "4x");
 m_FlightSpeedCombo.SetItemData(Index, 4);
 Index = m_FlightSpeedCombo.InsertString(-1, "8x");
 m_FlightSpeedCombo.SetItemData(Index, 8);
 Index = m_FlightSpeedCombo.InsertString(-1, "16x");
 m_FlightSpeedCombo.SetItemData(Index, 16);
 Index = m_FlightSpeedCombo.InsertString(-1, "32x");
 m_FlightSpeedCombo.SetItemData(Index, 32);
 m_FlightSpeedCombo.SetCurSel(2);
 m_FSG.m_nFS = 3;
 CheckDlgButton(IDC_USE_GROUND_EFFECT_ALGORITHM, m_FSG.m_bGE);
 CheckDlgButton(IDC_USE_STRATIFICATION_EFFECT_ALGORITHM, m_FSG.m_bUSE);
 CheckDlgButton(IDC_USE_COU_FOR_SEVERITY, m_FSG.m_bUCOU);
 CheckDlgButton(IDC_CALC_SEVERITY, m_FSG.m_bCS); 
 CheckDlgButton(IDC_FLIGHT_AUTO_STOP, m_FSG.m_bFAS);
 m_FlightTimeEdit.SubclassDlgItem(IDE_FLIGHT_TIME, this);
 sprintf(tstr, "%d", (short)m_FSG.m_dFT);
 m_FlightTimeEdit.SetWindowText(tstr);
 sprintf(tstr, "%.1f", m_FSG.m_dIPWCD);
 SetDlgItemText(IDE_INITIALPLANETOWAKECROSSDISTANCE, tstr);
 sprintf(tstr, "%.1f", m_FSG.m_dWWT);
 SetDlgItemText(IDE_WAKESECTIONTOWINGTIPTOLERANCE, tstr);
 m_PilotResponseCombo.SubclassDlgItem(IDO_PILOT_RESPONSE, this);
 Index = m_PilotResponseCombo.InsertString(-1, "Do nothing");
 m_PilotResponseCombo.SetItemData(Index, 0);
 Index = m_PilotResponseCombo.InsertString(-1, "Apply full opposite elerons 0.5 sec after bank > 10 deg and speed of bank > 15 deg/s");
 m_PilotResponseCombo.SetItemData(Index, 1);
 Index = m_PilotResponseCombo.InsertString(-1, "Apply full opposite elerons if bank > 15 deg");
 m_PilotResponseCombo.SetItemData(Index, 2);
 m_PilotResponseCombo.SetCurSel(0);
 m_FSG.m_nPR = 0;
 OnCalcSeverityButtonClicked(); 
 return(TRUE);
}
void CPageWakeOptions::FillFlagsFromControls(CWEAIF *p_pFlags)
{
 char tstr[32];
 m_FSG = *p_pFlags;
 m_FSG.m_bGE = !!IsDlgButtonChecked(IDC_USE_GROUND_EFFECT_ALGORITHM);
 m_FSG.m_bUSE = !!IsDlgButtonChecked(IDC_USE_STRATIFICATION_EFFECT_ALGORITHM);
 m_FSG.m_bUCOU = !!IsDlgButtonChecked(IDC_USE_COU_FOR_SEVERITY);
 m_FSG.m_bCS = !!IsDlgButtonChecked(IDC_CALC_SEVERITY);
 m_FSG.m_bEF = !!IsDlgButtonChecked(IDC_EMULATE_FLIGHT);
 m_FSG.m_bFAS = !!IsDlgButtonChecked(IDC_FLIGHT_AUTO_STOP);
 m_FlightTimeEdit.GetWindowText(tstr, sizeof(tstr));
 m_FSG.m_dFT = atof(tstr);
 GetDlgItemText(IDE_INITIALPLANETOWAKECROSSDISTANCE, tstr, sizeof(tstr));
 m_FSG.m_dIPWCD = atof(tstr);
 GetDlgItemText(IDE_WAKESECTIONTOWINGTIPTOLERANCE, tstr, sizeof(tstr));
 m_FSG.m_dWWT = atof(tstr);
 m_FSG.m_nPR = (short)m_PilotResponseCombo.GetItemData(m_PilotResponseCombo.GetCurSel()); 
 *p_pFlags = m_FSG;
}
void CPageWakeOptions::OnCalcSeverityButtonClicked()
{
 BOOL bVal = IsDlgButtonChecked(IDC_CALC_SEVERITY);
 GetDlgItem(IDC_EMULATE_FLIGHT)->EnableWindow(bVal);
 GetDlgItem(IDS_FLIGHT_TIME)->EnableWindow(bVal);
 m_FlightTimeEdit.EnableWindow(bVal);
 m_FlightSpeedCombo.EnableWindow(bVal);
 GetDlgItem(IDC_FLIGHT_AUTO_STOP)->EnableWindow(bVal);
}
