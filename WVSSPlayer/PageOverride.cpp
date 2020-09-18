#include "stdafx.h"
#include "PageOverride.h"
#include "Global.h"
#include "BatchDlg.h"
#include "PageInfo.h"
#include "MainDialog.h"
#pragma warning(disable : 4996)
using namespace WVSS;
void CPageOverride::COverrideData::Reset()
{
 m_ddZ = m_ddH = m_ddAngle = m_dGamma = 0;
}
IMPLEMENT_DYNAMIC(CPageOverride, CPropertyPage)
BEGIN_MESSAGE_MAP(CPageOverride, CPropertyPage)
 ON_BN_CLICKED(IDC_OVERRIDE_GAMMA, &CPageOverride::OnOverrideGammaClicked)
 ON_BN_CLICKED(IDC_DZDHOVERRIDE, &CPageOverride::OnOverridePositionClicked)
 ON_BN_CLICKED(IDB_BATCH, &CPageOverride::OnBatchButtonClicked)
 ON_WM_VSCROLL()
END_MESSAGE_MAP()
CPageOverride::CPageOverride(CWnd* pParent )	: CPropertyPage(CPageOverride::IDD)
{
 m_dBatchStep = 1;
 m_pInfoPage = NULL;
 m_pMainDialog = NULL;
}
CPageOverride::~CPageOverride()
{
}
BOOL CPageOverride::OnInitDialog()
{
	CDialog::OnInitDialog();
	CheckDlgButton(IDC_DZDHOVERRIDE, false);
	CheckDlgButton(IDC_OVERRIDE_GAMMA, false);
	m_dZEdit.SubclassDlgItem(IDE_dZ, this);
	m_dZSpin.SubclassDlgItem(IDSC_dZ, this);
	m_dZSpin.SetBuddy(&m_dZEdit);
	m_dZSpin.SetRange(-10000, 10000);
	m_dHEdit.SubclassDlgItem(IDE_dH, this);
	m_dHSpin.SubclassDlgItem(IDSC_dH, this);
	m_dHSpin.SetBuddy(&m_dHEdit);
	m_dHSpin.SetRange(-10000, 10000);
 m_dAngleEdit.SubclassDlgItem(IDE_dAngle, this);
 m_dGammaEdit.SubclassDlgItem(IDE_OVERRIDE_GAMMA, this);
 OnOverridePositionClicked();
 OnOverrideGammaClicked();
 m_Progress.SubclassDlgItem(IDP_PROGRESS, this);
 return(TRUE);
}
void CPageOverride::OnOverridePositionClicked()
{
 bool bVal;
 bVal = !!IsDlgButtonChecked(IDC_DZDHOVERRIDE);
 m_dZEdit.EnableWindow(bVal);
 m_dHEdit.EnableWindow(bVal);
 m_dAngleEdit.EnableWindow(bVal);
 GetDlgItem(IDS_dZ)->EnableWindow(bVal);
 GetDlgItem(IDS_dH)->EnableWindow(bVal);
 GetDlgItem(IDS_dAngle)->EnableWindow(bVal);
 m_dZSpin.EnableWindow(bVal);
 m_dHSpin.EnableWindow(bVal);
}
void CPageOverride::OnOverrideGammaClicked()
{
 m_dGammaEdit.EnableWindow(IsDlgButtonChecked(IDC_OVERRIDE_GAMMA));
}
afx_msg void CPageOverride::OnVScroll(UINT p_nSBCode, UINT p_nPos, CScrollBar *p_pScrollBar)
{
 CDialog::OnVScroll(p_nSBCode, p_nPos, p_pScrollBar);
 if(p_nSBCode == SB_ENDSCROLL && ((CWnd *)p_pScrollBar == (CWnd *)&m_dZSpin || (CWnd *)p_pScrollBar == (CWnd *)&m_dHSpin)) {
   m_pMainDialog->RerunCurrentFrame();
   }
}
void CPageOverride::FillFlagsFromControls(CWEAIF *p_pFlags)
{
 char tstr[256];
 p_pFlags->m_bOverrideProximityValues = false;
 p_pFlags->m_bOverrideGamma = false;
 if(IsDlgButtonChecked(IDC_DZDHOVERRIDE)) {
   p_pFlags->m_bOverrideProximityValues = true;
   p_pFlags->m_bStartPlaneHOverride = p_pFlags->m_bStartPlaneZOverride = p_pFlags->m_bClosingAngleRadOverride = false;
   m_dHEdit.GetWindowText(tstr, sizeof(tstr));
   if(*tstr) {
     p_pFlags->m_bStartPlaneHOverride = true;
     p_pFlags->m_dStartPlaneHOverride = atof(tstr); }
   m_dZEdit.GetWindowText(tstr, sizeof(tstr));
   if(*tstr) {
     p_pFlags->m_bStartPlaneZOverride = true;
     p_pFlags->m_dStartPlaneZOverride = atof(tstr); }
   m_dAngleEdit.GetWindowText(tstr, sizeof(tstr));
   if(*tstr) {
     p_pFlags->m_bClosingAngleRadOverride = true;
     p_pFlags->m_dCARdOverride = atof(tstr) * M_PI / 180; }
   }
 if(IsDlgButtonChecked(IDC_OVERRIDE_GAMMA)) {
   p_pFlags->m_bOverrideGamma = true;
   m_dGammaEdit.GetWindowText(tstr, sizeof(tstr));
   p_pFlags->m_dWakeGammaOverrideValue = atof(tstr); }
}
void CPageOverride::OnBatchButtonClicked()
{
 CBatchDlg Dlg;
 double dVal1, dVal2; 
 char tstr[512];
 bool bOverrideCB;
 DWORD lTotal;
 const CWESC *pCurWakeInteraction;
 const std::vector<CWESC> *pWakeInteractions;
 double dMaxBank[2], dMaxRateOfBank[2], dAltitudeLoss[2], dMaxTimeInWake;
 CEdit *pEdit;
 bool bMaxNumbersObtained;
 Dlg.m_Mode = CBatchDlg::OVERRIDE;
 if(Dlg.DoModal() != IDOK) return;
 bOverrideCB = !!IsDlgButtonChecked(IDC_DZDHOVERRIDE);
 CheckDlgButton(IDC_DZDHOVERRIDE, TRUE);
 if(Dlg.m_Mode != CBatchDlg::OVERRIDE) {
   MessageBox("Only override is presently implemented");
   return; }
 dMaxBank[0] = 1000;
 dMaxBank[1] = -1000;
 dMaxRateOfBank[0] = 1000;
 dMaxRateOfBank[1] = -1000;
 dAltitudeLoss[0] = 1000;
 dAltitudeLoss[1] = -1000;
 dMaxTimeInWake = 0;
 bMaxNumbersObtained = false;
 lTotal = 0;
 m_Progress.SetRange(0, 100);
 m_pInfoPage->SetPrintMessages(false);
 dVal1 = Dlg.m_dStart_dZ;
 dVal2 = Dlg.m_dStart_dH;
 while(true) {
   dVal1 = Dlg.m_dStart_dZ;
   while(true) {
     sprintf(tstr, "%.0lf", dVal1);
     SetDlgItemText(IDE_dZ, tstr);
     sprintf(tstr, "%.0lf", dVal2);
     SetDlgItemText(IDE_dH, tstr);
     m_pMainDialog->RerunCurrentFrame();
     pWakeInteractions = m_pMainDialog->GetCurFrameWakeInteractions();
     if(pWakeInteractions) {
       if(pWakeInteractions->size() > 0) {
         pCurWakeInteraction = &pWakeInteractions->at(0);
         if(pCurWakeInteraction->m_CSSO.size() > 0) {
           if(pCurWakeInteraction->m_CSSO[0].m_dMaxBankRad < dMaxBank[0])
             dMaxBank[0] = pCurWakeInteraction->m_CSSO[0].m_dMaxBankRad;
           if(pCurWakeInteraction->m_CSSO[0].m_dMaxBankRad > dMaxBank[1])
             dMaxBank[1] = pCurWakeInteraction->m_CSSO[0].m_dMaxBankRad;
           if(pCurWakeInteraction->m_CSSO[0].m_dMaxAngularSpeedOfBankRadPerS < dMaxRateOfBank[0])
             dMaxRateOfBank[0] = pCurWakeInteraction->m_CSSO[0].m_dMaxAngularSpeedOfBankRadPerS;
           if(pCurWakeInteraction->m_CSSO[0].m_dMaxAngularSpeedOfBankRadPerS > dMaxRateOfBank[1])
             dMaxRateOfBank[1] = pCurWakeInteraction->m_CSSO[0].m_dMaxAngularSpeedOfBankRadPerS;
           if(pCurWakeInteraction->m_CSSO[0].m_dMaxAltitudeLossM < dAltitudeLoss[0])
             dAltitudeLoss[0] = pCurWakeInteraction->m_CSSO[0].m_dMaxAltitudeLossM;
           if(pCurWakeInteraction->m_CSSO[0].m_dMaxAltitudeLossM > dAltitudeLoss[1])
             dAltitudeLoss[1] = pCurWakeInteraction->m_CSSO[0].m_dMaxAltitudeLossM;
           if(pCurWakeInteraction->m_CSSO[0].m_dTimeInWake > dMaxTimeInWake)
             dMaxTimeInWake = pCurWakeInteraction->m_CSSO[0].m_dTimeInWake;
           bMaxNumbersObtained = true;
           }
         }
       }
     lTotal ++;
     if(!(lTotal % 20)) {
       m_Progress.SetPos((int)((double)lTotal / ((Dlg.m_dEnd_dH - Dlg.m_dStart_dH + 1) * (Dlg.m_dEnd_dZ - Dlg.m_dStart_dZ + 1)) * 100));
       m_Progress.UpdateWindow();
       m_dZEdit.UpdateWindow();
       m_dHEdit.UpdateWindow();
       }
     dVal1 += m_dBatchStep;
     if(dVal1 > Dlg.m_dEnd_dZ) break;
     }
   dVal2 += m_dBatchStep;
   if(dVal2 > Dlg.m_dEnd_dH) break;
   }
 pEdit = (CEdit *)GetDlgItem(IDE_OVERRIDE_BATCH_RESULTS);
 if(pEdit) {
   pEdit->SetWindowText("");
   if(bMaxNumbersObtained) {
     sprintf(tstr, "Longest time in wake: %.1f sec\r\n"
                   "Min/Max bank deg %.1f / %.1f\r\n"
                   "Min/Max rate of bank deg/s %.1f / %.1f\r\n"
                   "Min/Max altitude loss ft %.1f / %.1f", 
             dMaxTimeInWake,
             Degrees(dMaxBank[0]), Degrees(dMaxBank[1]),
             Degrees(dMaxRateOfBank[0]), Degrees(dMaxRateOfBank[1]),
             dAltitudeLoss[0], dAltitudeLoss[1]);
     pEdit->SetWindowText(tstr); }
   }
 m_Progress.SetPos(100);
 m_Progress.UpdateWindow();
 CheckDlgButton(IDC_DZDHOVERRIDE, bOverrideCB);
 m_pInfoPage->SetPrintMessages(true);
 MessageBox("Batch completed");
}
void CPageOverride::UpdateOverrideData(COverrideData *p_pData)
{
 char tstr[32];
 double dVal;
 if(!IsDlgButtonChecked(IDC_DZDHOVERRIDE)) { 
   sprintf(tstr, "%.0f", p_pData->m_ddZ);
   m_dZEdit.SetWindowText(tstr);
   sprintf(tstr, "%.0f", p_pData->m_ddH);
   m_dHEdit.SetWindowText(tstr);
   dVal = p_pData->m_ddAngle;
   if(dVal < 0) dVal += 360;
   sprintf(tstr, "%.0f", dVal);
   m_dAngleEdit.SetWindowText(tstr); }
 if(!IsDlgButtonChecked(IDC_OVERRIDE_GAMMA)) {
   sprintf(tstr, "%.0f", p_pData->m_dGamma);
   m_dGammaEdit.SetWindowText(tstr); }
}
