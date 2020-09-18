#include "stdafx.h"
#include "PageDisplay.h"
#include "SITARWVSSPLayer.h"
#include "WVSSIPProtocol.h"
#include "MainDialog.h"
#pragma warning(disable : 4996)
using namespace WVSS;
BEGIN_MESSAGE_MAP(CPageDisplay, CDialog)
 ON_CBN_SELCHANGE(IDO_SCENARIO, &CPageDisplay::OnSelChangeScenario)
END_MESSAGE_MAP()
CPageDisplay::CDisplayFlags::CDisplayFlags()
{
 m_nPlaneSizeCorrection = 1;
 m_nWakeSectionSizeCorrection = 1;
 m_nRingAirplaneNum = -1;
 m_bShowAirBlocks = false;
 m_ShowSections[0] = m_ShowSections[1] = -1;
 m_bShowConeOfUncertainty = false;
 memset(m_DistanceRingsNM, 0, sizeof(m_DistanceRingsNM));
}
CPageDisplay::CPageDisplay(CWnd * ) : CPropertyPage(CPageDisplay::IDD)
{
 LOGFONT LogFont;
 memset(&LogFont, 0, sizeof(LogFont));
 strcpy(LogFont.lfFaceName, "Courier New");
 LogFont.lfHeight = -12;
 LogFont.lfWeight = 400;
 m_ScenarioFont.CreateFontIndirect(&LogFont);
 m_pMainDialog = NULL;
}
CPageDisplay::~CPageDisplay()
{
 m_ScenarioFont.DeleteObject();
}
BOOL CPageDisplay::OnInitDialog()
{
	DWORD dwRetVal;
	char EntryName[64], tstr[256], CurScenarioName[32], *pFormatStr;
	short i, Len, ScenarioCount;
	int Index;
	CWVSSScenario Scenario;
 CPropertyPage::OnInitDialog();
 itoa(m_DisplayFlags.m_nPlaneSizeCorrection, tstr, 10);
 SetDlgItemText(IDE_PLANE_SIZE, tstr);
 itoa(m_DisplayFlags.m_nWakeSectionSizeCorrection, tstr, 10);
 SetDlgItemText(IDE_WAKE_SIZE, tstr);
 CheckDlgButton(IDC_AIR_BLOCKS, m_DisplayFlags.m_bShowAirBlocks);
 CheckDlgButton(IDC_SHOW_CONE_OF_UNCERTAINTY, m_DisplayFlags.m_bShowConeOfUncertainty);
 CheckDlgButton(IDC_SHOW_LINES_FROM_AC_CENTER, m_DisplayFlags.m_bShowLinesFromAircraftCenter);
 CheckDlgButton(IDC_SHOW_LINES_FROM_WINGTIPS, m_DisplayFlags.m_bShowLinesFromWtpis);
 dwRetVal = GetPrivateProfileString("SCENARIO", "CurScenario", "", CurScenarioName, sizeof(CurScenarioName) - 1, g_theApp.m_IniFileName);
 m_RingAircraftCombo.SubclassDlgItem(IDO_RING_AIRCRAFT, this);
 FillRingAircraftList(NULL);
 *tstr = '\0';
 for(i = 0; i < sizeof(m_DisplayFlags.m_DistanceRingsNM) / sizeof(float); i ++) {
   if(m_DisplayFlags.m_DistanceRingsNM[i] <= 0) continue;
   pFormatStr = m_DisplayFlags.m_DistanceRingsNM[i] - short(m_DisplayFlags.m_DistanceRingsNM[i]) < 0.09 ? "%s%.0f" : "%s%.1f";
   sprintf(tstr + strlen(tstr), pFormatStr, !i ? "" : ",", m_DisplayFlags.m_DistanceRingsNM[i]); }
 SetDlgItemText(IDE_CIRCLE_RADII, tstr);
 m_ScenarioCombo.SubclassDlgItem(IDO_SCENARIO, this);
 Len = sizeof(Scenario.m_ScenarioName) - 1;
 m_ScenarioCombo.SetFont(&m_ScenarioFont);
 dwRetVal = GetPrivateProfileString("SCENARIO", "ScenarioCount", "0", tstr, Len, g_theApp.m_IniFileName);
 ScenarioCount = atoi(tstr);
 for(i = 0; i < ScenarioCount; i ++) {
   sprintf(EntryName, "SCENARIO%d", i + 1);
   dwRetVal = GetPrivateProfileString("SCENARIO", EntryName, "", Scenario.m_ScenarioName, sizeof(Scenario.m_ScenarioName) - 1, g_theApp.m_IniFileName);
   if(dwRetVal == Len - 1 || dwRetVal == Len - 2 || !*Scenario.m_ScenarioName) break;
   sprintf(EntryName, "SCENARIO_%s", Scenario.m_ScenarioName);
   dwRetVal = GetPrivateProfileString(EntryName, "Name", "", Scenario.m_AirportName, sizeof(Scenario.m_AirportName) - 1, g_theApp.m_IniFileName);
   strcpy(tstr, Scenario.m_ScenarioName);
   if(*Scenario.m_AirportName) sprintf(tstr + strlen(tstr), " - %s", Scenario.m_AirportName);
   Index = m_ScenarioCombo.AddString(tstr);
   m_ScenarioCombo.SetItemData(Index, i);
   m_Scenarios[i] = Scenario;
   }
 if((Index = m_ScenarioCombo.FindString(-1, CurScenarioName)) == CB_ERR)
   Index = 0;
 m_ScenarioCombo.SetCurSel(Index);  
 OnSelChangeScenario();
 return(TRUE);
}
void CPageDisplay::FillFlagsFromControls(CDisplayFlags *p_pFlags)
{
 short CurVal;
 char tstr[128], *pWork;
 DWORD_PTR dwptrVal;
 m_DisplayFlags = *p_pFlags;
 GetDlgItemText(IDE_PLANE_SIZE, tstr, sizeof(tstr));
 m_DisplayFlags.m_nPlaneSizeCorrection = atoi(tstr);
 if(m_DisplayFlags.m_nPlaneSizeCorrection < 1) m_DisplayFlags.m_nPlaneSizeCorrection = 1;
 GetDlgItemText(IDE_WAKE_SIZE, tstr, sizeof(tstr));
 m_DisplayFlags.m_nWakeSectionSizeCorrection = atoi(tstr);
 if(m_DisplayFlags.m_nWakeSectionSizeCorrection < 1) m_DisplayFlags.m_nWakeSectionSizeCorrection = 1;
 m_DisplayFlags.m_bShowAirBlocks = !!IsDlgButtonChecked(IDC_AIR_BLOCKS);
 m_DisplayFlags.m_bShowConeOfUncertainty = !!IsDlgButtonChecked(IDC_SHOW_CONE_OF_UNCERTAINTY);
 m_DisplayFlags.m_bShowLinesFromAircraftCenter = !!IsDlgButtonChecked(IDC_SHOW_LINES_FROM_AC_CENTER);
 m_DisplayFlags.m_bShowLinesFromWtpis = !!IsDlgButtonChecked(IDC_SHOW_LINES_FROM_WINGTIPS);
 m_DisplayFlags.m_ShowSections[0] = m_DisplayFlags.m_ShowSections[1] = 0;
 GetDlgItemText(IDE_SHOW_SECTIONS, tstr, sizeof(tstr));
 if(*tstr) {
   m_DisplayFlags.m_ShowSections[0] = atoi(tstr);
   m_DisplayFlags.m_ShowSections[1] = m_DisplayFlags.m_ShowSections[0];
   if(strchr(tstr, '-'))
     m_DisplayFlags.m_ShowSections[1] = atoi(strchr(tstr, '-') + 1);
   }
 m_DisplayFlags.m_nRingAirplaneNum = -1;
 if(m_RingAircraftCombo.GetCurSel() != CB_ERR) {
   dwptrVal = m_RingAircraftCombo.GetItemData(m_RingAircraftCombo.GetCurSel());
   m_DisplayFlags.m_nRingAirplaneNum = (short)((dwptrVal != DWORD_PTR(0xFFFFFFFF)) ? dwptrVal : -1); }
 memset(m_DisplayFlags.m_DistanceRingsNM, 0, sizeof(m_DisplayFlags.m_DistanceRingsNM));
 GetDlgItemText(IDE_CIRCLE_RADII, tstr, sizeof(tstr));
 pWork = strtok(tstr, ", ");
 CurVal = 0;
 while(pWork) {
   m_DisplayFlags.m_DistanceRingsNM[CurVal] = (float)atof(pWork);
   CurVal ++;
   pWork = strtok(NULL, ", "); }
 *p_pFlags = m_DisplayFlags;
}
void CPageDisplay::OnSelChangeScenario()
{
 OnScenarioChanged();
}
void CPageDisplay::OnScenarioChanged()
{
 int Index;
 std::map<short, CWVSSScenario>::iterator It;
 Index = m_ScenarioCombo.GetCurSel();
 if(Index == CB_ERR) return;
 Index = m_ScenarioCombo.GetItemData(Index);
 It = m_Scenarios.find(Index);
 if(It == m_Scenarios.end()) { MessageBox("Scenario not found!"); return; }
 bool bVal;
 CWVSSArea *pAreaMsg;
 char *pCharMsg = new char[sizeof(CWVSSArea)];
 if(!pCharMsg) return;
 pAreaMsg = (CWVSSArea *)pCharMsg;
 pAreaMsg->msg_size = sizeof(CWVSSArea);
 pAreaMsg->msg_type = WVSS_msg_area;
 strcpy_s(pAreaMsg->m_AreaName, sizeof(pAreaMsg->m_AreaName), It->second.m_ScenarioName);
 bVal = m_pMainDialog->m_Socket.SendMsg(pAreaMsg);
 bVal = bVal;
}
void CPageDisplay::FillRingAircraftList(std::vector <CADAA *>*p_pAircraft )
{
 int Index, SaveIndex = 0;
 size_t i;
 m_RingAircraftCombo.ResetContent();
 Index = m_RingAircraftCombo.InsertString(-1, "Not shown");
 m_RingAircraftCombo.SetItemData(Index, DWORD_PTR(0xFFFFFFFF));
 if(m_DisplayFlags.m_nRingAirplaneNum == -1)
   SaveIndex = Index;
 Index = m_RingAircraftCombo.InsertString(-1, "Visualizer area center");
 m_RingAircraftCombo.SetItemData(Index, DWORD_PTR(0));
 if(m_DisplayFlags.m_nRingAirplaneNum == 0)
   SaveIndex = Index;
 if(p_pAircraft) {
   for(i = 0; i < p_pAircraft->size(); i ++) {
     Index = m_RingAircraftCombo.InsertString(-1, p_pAircraft->at(i)->m_CallSign);
     m_RingAircraftCombo.SetItemData(Index, DWORD_PTR(i + 1));
     if(i + 1 == m_DisplayFlags.m_nRingAirplaneNum)
       SaveIndex = Index;
     } }
 m_RingAircraftCombo.SetCurSel(SaveIndex);
}
