#include "stdafx.h"
#include "SITARWVSSPLayer.h"
#include "PageLoad.h"
#include "Global.h"
#include "MainDialog.h"
#pragma warning(disable : 4996)
using namespace WVSS;
BEGIN_MESSAGE_MAP(CPageLoad, CDialog)
 ON_BN_CLICKED(IDB_LOAD, &CPageLoad::OnLoadBtnClicked)
 ON_BN_CLICKED(IDB_RELOAD, &CPageLoad::OnReloadButtonClicked)
END_MESSAGE_MAP()
CPageLoad::CPageLoad(CWnd * ) : CPropertyPage(CPageLoad::IDD)
{
 *m_CurFileName = '\0';
 m_pMainDialog = NULL;
}
CPageLoad::~CPageLoad()
{
}
BOOL CPageLoad::OnInitDialog()
{
 int Index;
 char tstr[32];
 float fVal;
 CPropertyPage::OnInitDialog();
 m_TSCombo.SubclassDlgItem(IDO_TS, this);
 Index = m_TSCombo.InsertString(-1, "0.1 sec");
 m_TSCombo.SetItemData(Index, 1);
 Index = m_TSCombo.InsertString(-1, "0.2 sec");
 m_TSCombo.SetItemData(Index, 2);
 Index = m_TSCombo.InsertString(-1, "0.3 sec");
 m_TSCombo.SetItemData(Index, 3);
 Index = m_TSCombo.InsertString(-1, "0.5 sec");
 m_TSCombo.SetItemData(Index, 5);
 Index = m_TSCombo.InsertString(-1, "1 sec");
 m_TSCombo.SetItemData(Index, 10);
 Index = m_TSCombo.InsertString(-1, "2 sec");
 m_TSCombo.SetItemData(Index, 20);
 Index = m_TSCombo.InsertString(-1, "3 sec");
 m_TSCombo.SetItemData(Index, 30);
 Index = m_TSCombo.InsertString(-1, "5 sec");
 m_TSCombo.SetItemData(Index, 50);
 Index = m_TSCombo.InsertString(-1, "10 sec");
 m_TSCombo.SetItemData(Index, 100);
 Index = m_TSCombo.InsertString(-1, "20 sec");
 m_TSCombo.SetItemData(Index, 200);
 Index = m_TSCombo.InsertString(-1, "30 sec");
 m_TSCombo.SetItemData(Index, 300);
 m_TSCombo.SetCurSel(4);
 m_FileTypeCombo.SubclassDlgItem(IDO_FILE_TYPE, this);
 Index = m_FileTypeCombo.InsertString(-1, "Auto");
 m_FileTypeCombo.SetItemData(Index, IWVSSSeverity::FILETYPE_NONE);
 Index = m_FileTypeCombo.InsertString(-1, "SITAR CSV");
 m_FileTypeCombo.SetItemData(Index, IWVSSSeverity::FILETYPE_ISA);
 Index = m_FileTypeCombo.InsertString(-1, "FDR");
 m_FileTypeCombo.SetItemData(Index, IWVSSSeverity::FILETYPE_FDR);
 Index = m_FileTypeCombo.InsertString(-1, "WVSS JSON");
 m_FileTypeCombo.SetItemData(Index, IWVSSSeverity::FILETYPE_WVSS_JSON);
 Index = m_FileTypeCombo.InsertString(-1, "MITRE CSV");
 m_FileTypeCombo.SetItemData(Index, IWVSSSeverity::FILETYPE_MITRE);
 Index = m_FileTypeCombo.InsertString(-1, "ASAIC JSON");
 m_FileTypeCombo.SetItemData(Index, IWVSSSeverity::FILETYPE_ASAIC_JSON);
 Index = m_FileTypeCombo.InsertString(-1, "Multiple");
 m_FileTypeCombo.SetItemData(Index, IWVSSSeverity::FILETYPE_MULTI);
 m_FileTypeCombo.SetCurSel(0);
 m_WindDirectionEdit.SubclassDlgItem(IDE_WINDDIRECTION, this);
 m_WindSpeedEdit.SubclassDlgItem(IDE_WINDSPEED, this);
 CheckDlgButton(IDC_MAKE_AUX_FILES, m_FSG.m_bCOF);
 CheckDlgButton(IDC_REAL_WEATHER, m_FSG.m_bRW);
 CheckDlgButton(IDC_NO_WIND_EXTRAPOLATION, m_FSG.m_bNWE);
 fVal = (float)Degrees(m_FSG.m_dWDRT - M_PI);
 if(fVal < 0) fVal += 360;
 sprintf(tstr, "%.0f", fVal);
 m_WindDirectionEdit.SetWindowText(tstr);
 fVal = (float)atof(tstr);
 sprintf(tstr, "%.0f", m_FSG.m_dWS * 3.6 / 1.852); 
 m_WindSpeedEdit.SetWindowText(tstr);
 sprintf(tstr, "%.0f", m_FSG.m_fMG);
 SetDlgItemText(IDE_MIN_GAMMA, tstr);
 return(TRUE);
}
void CPageLoad::FillFlagsFromControls(CWEAIF *p_pFlags)
{
 char tstr[32];
 m_FSG = *p_pFlags;
 m_FSG.m_bCOF = !!IsDlgButtonChecked(IDC_MAKE_AUX_FILES);
 m_FSG.m_bRW = !!IsDlgButtonChecked(IDC_REAL_WEATHER);
 m_FSG.m_bNWE = !!IsDlgButtonChecked(IDC_NO_WIND_EXTRAPOLATION);
 m_WindDirectionEdit.GetWindowText(tstr, sizeof(tstr));
 m_FSG.m_dWDRT = Radians(atof(tstr)) - M_PI;
 if(m_FSG.m_dWDRT < 0) m_FSG.m_dWDRT += 2 * M_PI; 
 m_WindSpeedEdit.GetWindowText(tstr, sizeof(tstr));
 m_FSG.m_dWS = atof(tstr) * 1.852 / 3.6; 
 GetDlgItemText(IDE_MIN_GAMMA, tstr, sizeof(tstr));
 m_FSG.m_fMG = (float)atof(tstr);
 *p_pFlags = m_FSG;
}
void CPageLoad::OnReloadButtonClicked()
{
 int CurFrame;
 CurFrame = m_pMainDialog->GetFrameNumber();
 m_FSG.m_bCOF = !!IsDlgButtonChecked(IDC_MAKE_AUX_FILES);
 m_pMainDialog->LoadFile(m_CurFileName);
 m_pMainDialog->SetFrameNumber(CurFrame);
}
void CPageLoad::OnLoadBtnClicked()
{
 char DefFileName[32], DefExt[32];
 char FilePath[256], FileTitle[128], FileExt[32];
 int Index;
 CFileDialog *pDlg = NULL;
 strcpy(DefExt, "*.dat");
 *FilePath = '\0';
 Index = m_FileTypeCombo.GetCurSel();
 if(Index != CB_ERR) {
   Index = m_FileTypeCombo.GetItemData(Index);
   switch(Index) {
     case IWVSSSeverity::FILETYPE_ISA: strcpy(DefExt, "*.dat"); break;
     case IWVSSSeverity::FILETYPE_FDR: strcpy(DefExt, "*.fdr"); break;
     case IWVSSSeverity::FILETYPE_WVSS_JSON: strcpy(DefExt, "*.json"); break;
     case IWVSSSeverity::FILETYPE_ASAIC_JSON: strcpy(DefExt, "*.json"); break;
     case IWVSSSeverity::FILETYPE_MITRE: strcpy(DefExt, "*.mitre"); break;
     case IWVSSSeverity::FILETYPE_MULTI: strcpy(DefExt, "*.mdf"); break;
     } }
 if(GetFilePartsFromName(m_CurFileName, FilePath, FileTitle, FileExt)) {
   if(*FileExt)
     sprintf(DefExt, "*.%s", FileExt);
   }
 if(!*FilePath)
   sprintf(FilePath, "%s\\WVSSData", g_theApp.m_ThisDir);
 strcpy(DefFileName, DefExt);
 pDlg = new CFileDialog(TRUE, DefExt, DefFileName, 0, "SITAR CSV (*.dat)|*.dat|WVSS JSON (*.json)|*.json|FDR Files (*.fdr)|*.fdr|MITRE Files (*.mitre)|*.mitre|ASAIC JSON (*.json)|*.json|Multi-source Files (*.mdf)|*.mdf|Text Files (*.txt)|*.txt|Comma-separated (*.csv)|*.csv|All Files (*.*)|*.*|");
 pDlg->m_pOFN->lpstrInitialDir = FilePath;
 if(pDlg->DoModal() != IDOK) return;
 m_pMainDialog->LoadFile(pDlg->GetPathName()); 
 if(pDlg) delete pDlg;
}
DWORD CPageLoad::GetBreakIntervalFromControl()
{
 int iVal;
 iVal = m_TSCombo.GetCurSel();
 iVal = m_TSCombo.GetItemData(iVal);
 return((LONGLONG)1000000 * iVal);
}
void CPageLoad::SetInterpolationControlFromVal(short p_nVal)
{
 short i;
 for(i = 0; i < m_TSCombo.GetCount(); i ++) {
   if(m_TSCombo.GetItemData(i) == p_nVal) {
     m_TSCombo.SetCurSel(i);
     break; } }
}
void CPageLoad::SetFileTypeControlFromVal(short p_nVal)
{
 short i;
 for(i = 0; i < m_FileTypeCombo.GetCount(); i ++) {
   if(m_FileTypeCombo.GetItemData(i) == p_nVal) {
     m_FileTypeCombo.SetCurSel(i);
     break; } }
}
