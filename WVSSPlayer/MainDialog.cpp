#include "stdafx.h"
#include "SITARWVSSPlayer.h"
#include "MainDialog.h"
#include "ProgressDlg.h"
#include "BarViewDlg.h"
#include "global.h"
#include "PageSeverityResults.h"
#include "PageAircraft.h"
#include "PageOverride.h"
#include "PageDisplay.h"
#include "PageWakeOptions.h"
#include "PageLoad.h"
#include "PageFrame.h"
#include "PageComms.h"
#include "PageWakeFrame.h"
#pragma warning(disable : 4996)
using namespace WVSS;
IMPLEMENT_DYNAMIC(CMainDialog, CDialog)
BEGIN_MESSAGE_MAP(CMainDialog, CDialog)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
 ON_WM_HSCROLL()
 ON_CBN_SELCHANGE(IDO_PLAYSPEED, &CMainDialog::OnSelChangePlaySpeed)
 ON_BN_CLICKED(IDB_PLAY, &CMainDialog::OnPlayBtnClicked)
 ON_BN_CLICKED(IDB_CUR_FRAME, &CMainDialog::OnCurFrameClicked)
 ON_BN_CLICKED(IDB_SETFRAME, &CMainDialog::OnSetFrameButtonClicked)
 ON_COMMAND(ID_MOVE_UL, &CMainDialog::OnButtonUL)
 ON_COMMAND(ID_MOVE_UR, &CMainDialog::OnButtonUR)
 ON_COMMAND(ID_MOVE_LL, &CMainDialog::OnButtonLL)
 ON_COMMAND(ID_MOVE_LR, &CMainDialog::OnButtonLR)
 ON_COMMAND(ID_TIME_PREV, &CMainDialog::OnTimePrev)
 ON_COMMAND(ID_TIME_NEXT, &CMainDialog::OnTimeNext)  
 ON_BN_CLICKED(IDB_COMMS_CONNECT, &CMainDialog::OnBnClickedCommsConnect)
 ON_BN_CLICKED(IDB_VIS_CENTER_ON, &CMainDialog::OnBnClickedCenterViewOn)
 ON_EN_CHANGE(IDE_CURPOS, &CMainDialog::OnCurFrameNumberChanged)
 ON_BN_CLICKED(IDB_BARVIEW, &CMainDialog::OnBarViewClicked)
 ON_COMMAND(ID_AFTERSTART, &CMainDialog::OnAfterStart)
END_MESSAGE_MAP()
CMainDialog *CMainDialog::m_pThisDialog = NULL;
HHOOK CMainDialog::m_hHook = 0;
CMainDialog::CMainDialog(CWnd* pParent ) : CDialog(CMainDialog::IDD, pParent)
{
 m_pThisDialog = this;
 m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
 m_pSheet = new CWVSSPlayerSheet2("AVP");
 m_pLoadPage = m_pSheet->m_pLoadPage;
 m_pLoadPage->m_pMainDialog = this;
 m_pInfoPage = m_pSheet->m_pInfoPage;
 m_pInfoPage->m_pMainDialog = this;
 m_pAircraftPage = m_pSheet->m_pAircraftPage;
 m_pSeverityResPage = m_pSheet->m_pSeverityResPage;
 m_pDisplayPage = m_pSheet->m_pDisplayPage;
 m_pDisplayPage->m_pMainDialog = this;
 m_pWakeOptionsPage = m_pSheet->m_pWakeOptionsPage;
 m_pOverridePage = m_pSheet->m_pOverridePage;
 m_pOverridePage->m_pMainDialog = this;
 m_pOverridePage->m_pInfoPage = m_pInfoPage;
 m_pFramePage = m_pSheet->m_pFramePage;
 m_pCommsPage = m_pSheet->m_pCommsPage;
 m_pSensitivityPage = m_pSheet->m_pSensitivityPage;
 m_pSensitivityPage->m_pSA = &m_Severity.m_SA;
 m_pWeatherPage = m_pSheet->m_pWeatherPage;
 m_pWakeFramePage = m_pSheet->m_pWakeFramePage;
 GetSettings(g_theApp.m_IniFileName);
 m_pDisplayPage->SetFlags(&m_DisplayFlags);
 m_pWakeOptionsPage->SetFlags(&m_FSG);
 m_pLoadPage->SetFlags(&m_FSG);
 m_pBarViewDlg = new CBarViewDlg(this);
 m_bReflectToBarWnd = true;
 m_pBarViewDlg->m_pbReflectToBarWnd = &m_bReflectToBarWnd;
 m_pBarWnd_CurFrameNumberEdit = NULL;
 m_dwPlaySleep = 1;
 m_bPlaying = false;
 m_bStopping = false;
 m_bDrawTracks = false;
 m_Severity.m_pfMessage = StaticMessage;
 m_Severity.m_pfOnFileLoaded = StaticOnFileLoaded;
 m_Severity.m_pfUAP = StaticUAPuos;
}
CMainDialog::~CMainDialog()
{
 if(m_pBarViewDlg) {
   m_pBarViewDlg->DestroyWindow();
   delete m_pBarViewDlg; }
 if(m_pSheet) delete m_pSheet;
}
BOOL CMainDialog::OnInitDialog()
{
 int Index;
 bool bVal;
 CComboBox *pPlaneCombo;
 CDialog::OnInitDialog();
	SetIcon(m_hIcon, TRUE);			
	SetIcon(m_hIcon, FALSE);		
 m_pSheet->Create(this, WS_VISIBLE | WS_CHILD | DS_CONTROL | WS_TABSTOP, 0);
 m_hHook = SetWindowsHookEx(WH_GETMESSAGE, HookProc, AfxGetInstanceHandle(), GetCurrentThreadId());
 m_hAccel = LoadAccelerators(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDR_MAIN));
 m_Scroll.SubclassDlgItem(IDC_SCROLL1, this);
 m_PosEdit.SubclassDlgItem(IDE_CURPOS, this);
 m_PlaySpeedCombo.SubclassDlgItem(IDO_PLAYSPEED, this);
 Index = m_PlaySpeedCombo.InsertString(-1, "1x");
 m_PlaySpeedCombo.SetItemData(Index, 1);
 Index = m_PlaySpeedCombo.InsertString(-1, "2x");
 m_PlaySpeedCombo.SetItemData(Index, 2);
 Index = m_PlaySpeedCombo.InsertString(-1, "4x");
 m_PlaySpeedCombo.SetItemData(Index, 4);
 Index = m_PlaySpeedCombo.InsertString(-1, "8x");
 m_PlaySpeedCombo.SetItemData(Index, 8);
 Index = m_PlaySpeedCombo.InsertString(-1, "10x");
 m_PlaySpeedCombo.SetItemData(Index, 10);
 Index = m_PlaySpeedCombo.InsertString(-1, "16x");
 m_PlaySpeedCombo.SetItemData(Index, 16);
 Index = m_PlaySpeedCombo.InsertString(-1, "32x");
 m_PlaySpeedCombo.SetItemData(Index, 32);
 m_PlaySpeedCombo.SetCurSel(0);
 pPlaneCombo = (CComboBox *)GetDlgItem(IDO_VIS_CENTER_ON);
 if(pPlaneCombo) {
   Index = pPlaneCombo->InsertString(-1, "Area center");
   pPlaneCombo->SetItemData(Index, (DWORD_PTR)0xFFFFFFFF);
   pPlaneCombo->SetCurSel(Index); }
 m_SI.cbSize = sizeof(SCROLLINFO);     
 m_SI.fMask = SIF_ALL;     
 m_SI.nMin = 0;
 m_SI.nMax = 0;
 m_SI.nPage = 1;
 m_SI.nPos = 0;
 m_SI.nTrackPos = 0;
 m_Scroll.SetScrollInfo(&m_SI);
 CreateBarView();
 if(!AfxSocketInit())
   m_pInfoPage->AddToMessageList("Failed to Initialize Sockets");
 m_Socket.m_pfMessage = StaticFromSocketMessage;
 m_Socket.m_pfSocketConnected = StaticSocketConnected;
 m_Socket.m_pfMessageSent = StaticSocketMessageSent;
 ConnectSocket();
 bVal = m_Severity.Init(); 
 ASSERT(bVal);
 PostMessage(WM_COMMAND, ID_AFTERSTART, 0);	
 return(TRUE);
}
void CMainDialog::GetSettings(const char *p_pINIFileName)
{
	DWORD dwRetVal;
 short i, Len;
 char tstr[256], *pWork;
 Len = sizeof(tstr) - 1;
 dwRetVal = GetPrivateProfileString("SETTINGS", "PlaneSizeCorrection", "1", tstr, Len, p_pINIFileName);
 m_DisplayFlags.m_nPlaneSizeCorrection = atoi(tstr);
 dwRetVal = GetPrivateProfileString("SETTINGS", "WakeSectionSizeCorrection", "1", tstr, Len, p_pINIFileName);
 m_DisplayFlags.m_nWakeSectionSizeCorrection = atoi(tstr);
 dwRetVal = GetPrivateProfileString("SETTINGS", "InitialPlaneToWakeCrossDistance", "300", tstr, Len, p_pINIFileName);
 m_FSG.m_dIPWCD = atof(tstr);
 m_DisplayFlags.m_bShowAirBlocks = !!GetPrivateProfileInt("SETTINGS", "ShowAirBlocks", 1, p_pINIFileName);
 m_DisplayFlags.m_bShowConeOfUncertainty = (GetPrivateProfileInt("SETTINGS", "ShowConeOfUncertainty", 1, p_pINIFileName) == 1);
 m_DisplayFlags.m_bShowLinesFromAircraftCenter = (GetPrivateProfileInt("SETTINGS", "ShowLinesFromAircraftCenter", 1, p_pINIFileName) == 1);
 m_DisplayFlags.m_bShowLinesFromWtpis = (GetPrivateProfileInt("SETTINGS", "ShowLinesFromWtpis", 1, p_pINIFileName) == 1);
 memset(m_DisplayFlags.m_DistanceRingsNM, 0, sizeof(m_DisplayFlags.m_DistanceRingsNM));
 dwRetVal = GetPrivateProfileString("SETTINGS", "DistanceCirclesNM", "", tstr, Len, p_pINIFileName);
 pWork = strtok(tstr, ", ");
 i = 0;
 while(pWork) {
   m_DisplayFlags.m_DistanceRingsNM[i] = (float)atof(pWork);
   i ++;
   pWork = strtok(NULL, ", "); }
 dwRetVal = GetPrivateProfileString("SETTINGS", "BatchStep", "1", tstr, Len, p_pINIFileName);
 m_pOverridePage->SetBatchStep((float)atof(tstr));
 m_Severity.m_FSG = m_FSG;
 m_Severity.GetSettings(p_pINIFileName);
 m_FSG = m_Severity.m_FSG;
 m_Severity.LoadWindServer();
}
bool CMainDialog::LoadFile(LPCSTR p_pFileName)
{
 CProgressDlg ProgressDlg(this);
 ProgressDlg.m_Mode = CProgressDlg::LOAD;
 ProgressDlg.m_RunFunctions[ProgressDlg.m_Mode] = StaticOnLoadFile;
 ProgressDlg.m_pFileName = p_pFileName;
 ProgressDlg.m_pfProgress = StaticGetProgress;
 m_pSheet->SetActivePage(1);
 ProgressDlg.DoModal();
 return(ProgressDlg.GetProcRes());
}
bool CMainDialog::DoLoadFile(LPCSTR p_pFileName)
{
 char *pWork, tstr[256], FileName[256];
 CSeverity::eFileType FileType;
 FILE *fFile = NULL;
 short i, Count;
 CComboBox *pPlaneCombo;
 int Index;
 m_pLoadPage->SetLoadedFileText("Currently loaded file");
 pPlaneCombo = (CComboBox *)GetDlgItem(IDO_VIS_CENTER_ON);
 if(pPlaneCombo)
   pPlaneCombo->ResetContent();
 m_Scroll.SetScrollPos(0);
 m_Scroll.SetScrollRange(0, 0);
 m_pBarViewDlg->m_Scroll.SetScrollPos(0);
 m_pBarViewDlg->m_Scroll.SetScrollRange(0, 0);
 if(m_pDisplayPage)
   m_pDisplayPage->FillRingAircraftList();
 m_Severity.m_dwBreakInterval = m_pLoadPage->GetBreakIntervalFromControl();
 FileType = m_pLoadPage->GetFileType();
 m_pOverridePage->FillFlagsFromControls(&m_FSG);
 m_pWakeOptionsPage->FillFlagsFromControls(&m_FSG);
 m_pLoadPage->FillFlagsFromControls(&m_FSG);
 m_Severity.m_FSG = m_FSG;
 if(FileType == CSeverity::FILETYPE_NONE)
   m_Severity.GetFileTypeDateAndSizeFromName(p_pFileName, &FileType, NULL, NULL);
 strcpy(FileName, p_pFileName);
 if(FileType != CSeverity::FILETYPE_MULTI) {
   if(GetFullPathName(FileName, sizeof(tstr), tstr, NULL))
     strcpy(FileName, tstr);
   if(!m_Severity.LoadFile(FileName, FileType, true, true)) {
     m_pInfoPage->AddToMessageList("%s: track file load failure. ", FileName);
     return(false); } }
 else {
   m_pInfoPage->AddToMessageList("Aggregate track file %s", FileName);
   fFile = fopen(FileName, "rt");
   if(!fFile) return(false);
   Count = 0;
   while(fgets(tstr, sizeof(tstr) - 1, fFile)) {
     if(pWork = strrchr(tstr, '\n')) *pWork = '\0';
     if(!*tstr || *tstr == '#') continue;
     Count ++; }
   fseek(fFile, 0, SEEK_SET);
   for(i = 0; i < Count; i ++) {
     if(!fgets(tstr, sizeof(tstr) - 1, fFile)) break;
     if(pWork = strrchr(tstr, '\n')) *pWork = '\0';
     if(!*tstr || *tstr == '#') continue;
     GetFullPathName(tstr, sizeof(FileName), FileName, NULL);
     if(!m_Severity.LoadFile(FileName, IWVSSSeverity::FILETYPE_NONE, !i ? true : false, (i == Count - 1) ? true : false)) {
       m_pInfoPage->AddToMessageList("%s: track file load failure.", FileName);
       return(false); } }
   fclose(fFile);
   }
 if(m_pAircraftPage)
   m_pAircraftPage->UpdateResults(&m_Severity.m_Aircraft);
 if(m_pSeverityResPage)
   m_pSeverityResPage->UpdateResults(&m_Severity);
 if(m_pWeatherPage)
   m_pWeatherPage->UpdateResults(m_Severity.GetWindPointVector());
 if(m_pDisplayPage)
   m_pDisplayPage->FillRingAircraftList(&m_Severity.m_Aircraft);
 if(pPlaneCombo) {
   Index = pPlaneCombo->InsertString(-1, "Area center");
   pPlaneCombo->SetItemData(Index, (DWORD_PTR)0xFFFFFFFF);
   for(i = 0; i < (short)m_Severity.m_Aircraft.size(); i++) {
     Index = pPlaneCombo->InsertString(-1, m_Severity.m_Aircraft[i]->m_CallSign);
     pPlaneCombo->SetItemData(Index, i); }
   pPlaneCombo->SetCurSel(0); }
 LoadAircraftIntoVisualizer();
 m_pLoadPage->SetFileName(p_pFileName);
 return(true);
}
bool CMainDialog::StaticOnLoadFile(const char *p_pFileName)
{
 if(!m_pThisDialog) return(false);
 return(m_pThisDialog->DoLoadFile(p_pFileName));
}
void CMainDialog::OnFileLoaded(LPCSTR p_pFileName)
{
 m_Scroll.GetScrollInfo(&m_SI);
 m_SI.nMax = m_Severity.m_dwTSCount - 1;
 m_SI.nPos = 1; 
 m_Scroll.SetScrollInfo(&m_SI);
 m_pBarViewDlg->m_Scroll.SetScrollInfo(&m_SI);
 m_nScrollPageSize = m_SI.nMax / 60;
 m_pLoadPage->SetLoadedFileText(p_pFileName);
 m_bDrawTracks = true;
 PostMessage(WM_HSCROLL, MAKELONG(SB_LEFT, 0), (LPARAM)m_Scroll.m_hWnd);
}
void CMainDialog::OnSelChangePlaySpeed()
{
 int nVal;
 m_dwPlaySleep = 1;
 nVal = m_PlaySpeedCombo.GetCurSel();
 if(m_bReflectToBarWnd)
   m_pBarViewDlg->m_PlaySpeedCombo.SetCurSel(nVal);
 if(nVal == CB_ERR) return;
 nVal = m_PlaySpeedCombo.GetItemData(nVal);
 m_dwPlaySleep = m_Severity.m_dwBreakInterval / 10000 / nVal;
}
LRESULT CALLBACK CMainDialog::HookProc(int nCode, WPARAM wParam, LPARAM lParam)
{
 if(!m_pThisDialog)
   return CallNextHookEx(m_hHook, nCode, wParam, lParam);
 return m_pThisDialog->ThisHookProc(nCode, wParam, lParam);
}
LRESULT CALLBACK CMainDialog::ThisHookProc(int nCode, WPARAM wParam, LPARAM lParam)
{
 LRESULT nResult = 1;
 if(nCode == HC_ACTION && wParam == PM_REMOVE) {
   MSG *pMsg = (MSG*)lParam;
   if(pMsg->message >= WM_KEYFIRST && pMsg->message <= WM_KEYLAST)
     if(GetForegroundWindow() == this) {
       HWND gf = GetFocus()->m_hWnd; 
       if(DLGC_HASSETSEL & ~::SendMessage(gf, WM_GETDLGCODE, 0, 0) || ES_READONLY & GetWindowLong(gf, GWL_STYLE)) {
         if(TranslateAcceleratorW(m_hWnd, m_hAccel, pMsg)) {
           pMsg->message = WM_NULL;
           nResult = 0; } } } }
 if(nCode < 0 || nResult)
   return CallNextHookEx(m_hHook, nCode, wParam, lParam);
 return nResult;
}
void CMainDialog::StaticMessage(const char *p_pStr)
{
 if(!m_pThisDialog) return;
 m_pThisDialog->m_pInfoPage->AddToMessageList(p_pStr);
}
void CMainDialog::StaticFromSocketMessage(const char *p_pStr)
{
 if(!m_pThisDialog) return;
 m_pThisDialog->m_pCommsPage->AddToMessageList(p_pStr);
}
void CMainDialog::StaticOnFileLoaded(const char *p_pFileName)
{
 if(!m_pThisDialog) return;
 m_pThisDialog->OnFileLoaded(p_pFileName);
}
void CMainDialog::OnPlayBtnClicked()
{
 if(m_bPlaying) { m_bStopping = true; return; }
 AfxBeginThread(StaticPlayThreadFunc, this);
}
UINT CMainDialog::StaticPlayThreadFunc(LPVOID p_pParam)
{
 return(((CMainDialog *)p_pParam)->PlayThreadFunc());
}
UINT CMainDialog::PlayThreadFunc()
{
 OnSelChangePlaySpeed();
 m_bStopping = false;
 m_bPlaying = true;
 SetDlgItemText(IDB_PLAY, "Stop");
 m_pBarViewDlg->SetDlgItemText(IDB_PLAY, "Stop");
 while(m_bPlaying && !m_bStopping) {
   m_lLastFrameTime = GetTickCount();
   SendMessage(WM_HSCROLL, MAKELONG(SB_LINERIGHT, 0), (LPARAM)m_Scroll.m_hWnd);
   m_lLastFrameTime = GetTickCount() - m_lLastFrameTime;
   if(m_lLastFrameTime >= 100000)
     ASSERT(0);
   if(m_dwPlaySleep > m_lLastFrameTime)
     Sleep(m_dwPlaySleep - m_lLastFrameTime);
   }
 SetDlgItemText(IDB_PLAY, "Play");
 m_pBarViewDlg->SetDlgItemText(IDB_PLAY, "Play");
 m_bPlaying = false;
 m_bStopping = false;
 return 0;
}
void CMainDialog::OnCurFrameClicked() { RerunCurrentFrame(); }
void CMainDialog::RerunCurrentFrame() { Recalc2(GetFrameNumber()); }
void CMainDialog::OnSetFrameButtonClicked()
{
 int CurPos;
 char tstr[32];
 m_PosEdit.GetWindowText(tstr, sizeof(tstr));
 CurPos = atoi(tstr);
 if(CurPos < m_SI.nMin) CurPos = m_SI.nMin;
 else if(CurPos > m_SI.nMax) CurPos = m_SI.nMax;
 SetFrameNumber(CurPos);
}
int CMainDialog::GetFrameNumber()
{
 return(m_Scroll.GetScrollPos());
}
void CMainDialog::SetFrameNumber(int p_nFrameNum)
{
 char tstr[32];
 sprintf(tstr, "%d", p_nFrameNum);
 m_PosEdit.SetWindowText(tstr);
 PostMessage(WM_HSCROLL, MAKELONG(SB_THUMBPOSITION, p_nFrameNum), (LPARAM)m_Scroll.m_hWnd);
}
afx_msg void CMainDialog::OnHScroll(UINT nSBCode, UINT p_nPos, CScrollBar* pScrollBar)
{
 if(nSBCode == SB_ENDSCROLL) return;
 if(!DoScrollSet(nSBCode, p_nPos)) return;
 RerunCurrentFrame(); 
 CDialog::OnHScroll(nSBCode, p_nPos, pScrollBar);
}
void CMainDialog::OnPaint()
{
	if (IsIconic()) {
		CPaintDC dc(this); 
		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;
		dc.DrawIcon(x, y, m_hIcon); }
	else
		CDialog::OnPaint();
}
HCURSOR CMainDialog::OnQueryDragIcon() {	return static_cast<HCURSOR>(m_hIcon); }
bool CMainDialog::DoScrollSet(UINT nSBCode, UINT p_nPos)
{
 int PosToSet, CurPos = m_Scroll.GetScrollPos();
 short SingleAdd = 1;
 if(GetKeyState(VK_LSHIFT) & 0x8000) SingleAdd = 10;
 PosToSet = CurPos;
 switch (nSBCode) {
   case SB_LEFT: PosToSet = m_SI.nMin; break;     
   case SB_RIGHT: PosToSet = m_SI.nMax; break;     
   case SB_ENDSCROLL: break;  
   case SB_LINELEFT:
     PosToSet = CurPos - SingleAdd;
     if(CurPos < m_SI.nMin) CurPos = m_SI.nMin; 
     break; 
   case SB_LINERIGHT:
     PosToSet = CurPos + SingleAdd;
     if(CurPos >= m_SI.nMax) CurPos = m_SI.nMax;
     if(m_bPlaying && CurPos == m_SI.nMax)
       m_bStopping = true;
     break;  
   case SB_PAGELEFT:    
     if(CurPos > m_SI.nMin) PosToSet = max(m_SI.nMin, CurPos - m_nScrollPageSize);
     break;
   case SB_PAGERIGHT:      
     if(CurPos < m_SI.nMax) PosToSet = min(m_SI.nMax, CurPos + m_nScrollPageSize);
     break;
   case SB_THUMBPOSITION: 
      PosToSet = p_nPos;      
      break;
   case SB_THUMBTRACK:   
      PosToSet = p_nPos;     
      break;
   }
 if(PosToSet == CurPos) return(false);
 CurPos = PosToSet;
 m_Scroll.SetScrollPos(CurPos);
 m_pBarViewDlg->m_Scroll.SetScrollPos(CurPos);
 return(true);
}
bool CMainDialog::ConnectSocket()
{
 return(m_Socket.DoConnect("127.0.0.1", 1701));
}
void CMainDialog::OnBnClickedCommsConnect()
{
 if(m_Socket.IsConnected()) return;
 ConnectSocket();
}
void CMainDialog::OnButtonUL() { OnLocationBtn(UL); }
void CMainDialog::OnButtonUR() { OnLocationBtn(UR); }
void CMainDialog::OnButtonLL() { OnLocationBtn(LL); }
void CMainDialog::OnButtonLR() { OnLocationBtn(LR); }
void CMainDialog::OnLocationBtn(eLocation p_Location)
{
 APPBARDATA AppBarData;
 CRect Rect;
 int WndWidth, WndHeight, HorzRes, VertRes;
 CDC *pDC = GetDC();
 AppBarData.cbSize = sizeof(AppBarData);
 AppBarData.hWnd = m_hWnd;
 SHAppBarMessage(ABM_GETTASKBARPOS, &AppBarData);
 HorzRes = pDC->GetDeviceCaps(HORZRES);
 VertRes = pDC->GetDeviceCaps(VERTRES) - (AppBarData.rc.bottom - AppBarData.rc.top);
 ReleaseDC(pDC);
 GetWindowRect(Rect);
 WndWidth = Rect.Width();
 WndHeight = Rect.Height();
 switch(p_Location) {
   case UL: Rect.SetRect(0, 0, Rect.Width(), Rect.Height()); break;
   case UR: Rect.SetRect(HorzRes - Rect.Width(), 0, 0, 0); break;
   case LL: Rect.SetRect(0, VertRes - Rect.Height(), 0, 0); break;
   case LR: Rect.SetRect(HorzRes - Rect.Width(), VertRes - Rect.Height(), 0, 0); break; }
 Rect.right = Rect.left + WndWidth;
 Rect.bottom = Rect.top + WndHeight;
 MoveWindow(Rect);
}
void CMainDialog::DrawDistanceCircles(CTrackPoint *p_pTrackPoint, std::vector<rWVSSToVisCircle>*p_pVisCircles)
{
 short i, Count;
 rWVSSToVisCircle VisCircle;
 Count = sizeof(m_DisplayFlags.m_DistanceRingsNM) / sizeof(float);
 for(i = 0; i < Count; i ++) {
   if(m_DisplayFlags.m_DistanceRingsNM[i] <= 0) continue;
   VisCircle.m_Center.m_dLat = p_pTrackPoint ? p_pTrackPoint->m_dLatDeg : 101;
   VisCircle.m_Center.m_dLong = p_pTrackPoint ? p_pTrackPoint->m_dLongDeg : 0;
   VisCircle.m_Center.m_dH = p_pTrackPoint ? p_pTrackPoint->m_nAltitudeM : 0;
   VisCircle.m_Color[0] = 0;
   VisCircle.m_Color[1] = 192;
   VisCircle.m_Color[2] = 0;
   VisCircle.m_fUSRad = m_DisplayFlags.m_DistanceRingsNM[i] * 1852;
   VisCircle.m_fLineWidth = 2;
   VisCircle.m_fHeading = 0;
   VisCircle.m_fPHCti = 90;
   VisCircle.m_fBank = 0;
   p_pVisCircles->push_back(VisCircle); }
}
void CMainDialog::MakeAirBlockText2(short p_nAircraftOrdinal, CTrackPoint *p_pTP)
{
#pragma message("CMainDialog::MakeAirBlockText")
}
void CMainDialog::OnTimePrev() { OnTimePrevNext(PREV); }
void CMainDialog::OnTimeNext() { OnTimePrevNext(NEXT); }
void CMainDialog::OnTimePrevNext(eTimeAdvance p_TA)
{
 PostMessage(WM_HSCROLL, MAKELONG(p_TA == NEXT ? SB_LINERIGHT : SB_LINELEFT, 0), (LPARAM)m_Scroll.m_hWnd); 
}
void CMainDialog::StaticUAPuos(rAircraftPosition *p_pAircraftPosition, short p_nAircraftOrdinal, CTrackPoint *p_pTP)
{
 if(!m_pThisDialog) return;
 m_pThisDialog->UAPuos(p_pAircraftPosition, p_nAircraftOrdinal, p_pTP);
}
void CMainDialog::UAPuos(rAircraftPosition *p_pAircraftPosition, short p_nAircraftOrdinal, CTrackPoint *p_pTP)
{
#pragma message("CMainDialog::UAPuos")
}
void CMainDialog::StaticSocketConnected(bool p_bConnected)
{
 if(!m_pThisDialog) return;
 if(p_bConnected) {
   m_pThisDialog->m_bDrawTracks = true;
   m_pThisDialog->m_pDisplayPage->OnScenarioChanged();
   m_pThisDialog->LoadAircraftIntoVisualizer(); }
}
void CMainDialog::StaticSocketMessageSent(short p_nMsgType)
{
 if(!m_pThisDialog) return;
 m_pThisDialog->m_pCommsPage->AddToMessageList("Comms message <%s> sent", g_theApp.m_MsgNameMap[(eWVSSMsgType)p_nMsgType]);
}
float CMainDialog::StaticGetProgress()
{
 if(!m_pThisDialog) return(false);
 return(m_pThisDialog->GetProgress());
}
float CMainDialog::GetProgress()
{
 return(m_Severity.GetProcessedPercentage());
}
void CMainDialog::LoadAircraftIntoVisualizer()
{
 bool bVal;
 unsigned long lMsgSize;
 size_t i;
 char *pCharMsg;
 CWVSSMsgHeader *pMsg;
 CWVSSPlaneTypeList *pAircraftTypeListMsg;
 rWVSSPlaneType *pPlaneType;
 lMsgSize = sizeof(CWVSSPlaneTypeList);
 lMsgSize += m_Severity.m_Aircraft.size() * sizeof(rWVSSPlaneType);
 pCharMsg = new char[lMsgSize];
 if(!pCharMsg) return;
 pMsg = (CWVSSMsgHeader *)pCharMsg;
 pMsg->msg_size = lMsgSize;
 pMsg->msg_type = WVSS_msg_loadplane;
 pAircraftTypeListMsg = (CWVSSPlaneTypeList *)pCharMsg;
 pAircraftTypeListMsg->m_nPlaneTypeCount = m_Severity.m_Aircraft.size(); 
 pPlaneType = (rWVSSPlaneType *)(pCharMsg + sizeof(CWVSSPlaneTypeList));
 for(i = 0; i < m_Severity.m_Aircraft.size(); i ++) {
   memset(pPlaneType->m_PlaneType, 0, sizeof(pPlaneType->m_PlaneType));
   strncpy(pPlaneType->m_PlaneType, m_Severity.m_Aircraft[i]->m_ACModel, sizeof(pPlaneType->m_PlaneType) - 1);
   ++ pPlaneType;
   }
 bVal = m_Socket.SendMsg(pMsg);
 bVal = bVal;
}
void CMainDialog::OnCurFrameNumberChanged()
{
 char tstr[32];
 if(!m_bReflectToBarWnd || !m_pBarWnd_CurFrameNumberEdit) return;
 GetDlgItemText(IDE_CURPOS, tstr, sizeof(tstr) - 1);
 m_pBarViewDlg->m_bReflectToMainWnd = false;
 m_pBarWnd_CurFrameNumberEdit->SetWindowText(tstr);
 m_pBarViewDlg->m_bReflectToMainWnd = true;
}
void CMainDialog::OnBarViewClicked()
{
 if(!m_pBarViewDlg) return;
 ShowWindow(SW_HIDE);
 m_pBarViewDlg->ShowWindow(SW_SHOWMAXIMIZED);
}
void CMainDialog::CreateBarView()
{
 short nVal[2];
 char tstr[32];
 int Index;
 GetDlgItem(IDB_BARVIEW)->EnableWindow(FALSE);
 if(!m_pBarViewDlg) return;
 if(!m_pBarViewDlg->Create(CBarViewDlg::IDD) == TRUE) return;
 m_pBarViewDlg->m_nMainForm_SetButtonID = IDB_SETFRAME;
 m_pBarViewDlg->m_nMainForm_PlayButtonID = IDB_PLAY;
 m_pBarViewDlg->m_pMainForm_CurFrameNumberEdit = &m_PosEdit;
 m_pBarWnd_CurFrameNumberEdit = &m_pBarViewDlg->m_CurFrameNumberEdit;
 m_pBarViewDlg->m_hMainForm_ScrollBarWnd = m_Scroll.m_hWnd;
 m_pBarViewDlg->m_hMainForm_PlaySpeedComboWnd = m_PlaySpeedCombo.m_hWnd;
 m_pBarViewDlg->m_nMainForm_PlayComboID = IDO_PLAYSPEED;
 m_Scroll.GetScrollInfo(&m_SI, SIF_ALL);
 m_pBarViewDlg->m_Scroll.SetScrollInfo(&m_SI);
 for(nVal[0] = 0; nVal[0] < m_PlaySpeedCombo.GetCount(); nVal[0] ++) {
   m_PlaySpeedCombo.GetLBText(nVal[0], tstr);
   nVal[1] = (short)m_PlaySpeedCombo.GetItemData(nVal[0]);
   Index = m_pBarViewDlg->m_PlaySpeedCombo.InsertString(-1, tstr);
   m_pBarViewDlg->m_PlaySpeedCombo.SetItemData(Index, nVal[1]);
   }
 m_pBarViewDlg->m_PlaySpeedCombo.SetCurSel(m_PlaySpeedCombo.GetCurSel());
 GetDlgItem(IDB_BARVIEW)->EnableWindow(TRUE);
}
void CMainDialog::OnAfterStart()
{
 short i, Count;
 bool bCloseWhenDone = false;
	Count = (short)g_theApp.m_CmdLineInfo.m_StartupOperations.size();
	for(i = 0; i < Count; i ++) {
	  switch(g_theApp.m_CmdLineInfo.m_StartupOperations[i].m_OperType) {
	    case CSWPCommandLineInfo::MAKEOUTPUTFILES:
	      m_FSG.m_bCOF = true;
	      break;
	    case CSWPCommandLineInfo::INTERPOLATION:
	      m_pLoadPage->SetInterpolationControlFromVal(atoi(g_theApp.m_CmdLineInfo.m_StartupOperations[i].m_Data));
	      break;
	    case CSWPCommandLineInfo::FILETYPE:
	      m_pLoadPage->SetFileTypeControlFromVal(atoi(g_theApp.m_CmdLineInfo.m_StartupOperations[i].m_Data));
	      break;
	    default: break; } }
	m_pLoadPage->SetFlags(&m_FSG);
	for(i = 0; i < Count; i ++) {
	  switch(g_theApp.m_CmdLineInfo.m_StartupOperations[i].m_OperType) {
	    case CSWPCommandLineInfo::OPENFILE:
	      LoadFile(g_theApp.m_CmdLineInfo.m_StartupOperations[i].m_Data);
	      break;
	    default: break; } }
	for(i = 0; i < Count; i ++) {
	  switch(g_theApp.m_CmdLineInfo.m_StartupOperations[i].m_OperType) {
	    case CSWPCommandLineInfo::POSITION:
       SetFrameNumber(atoi(g_theApp.m_CmdLineInfo.m_StartupOperations[i].m_Data));
	      break;
	    default: break; } }
}
void CMainDialog::Recalc2(int p_nCurPos)
{
 char tstr[128];
 unsigned short i, TSCount, AircraftOrdinal;
 DWORD lVal;
 long lStartIndex, lEndIndex, lj;
 LONGLONG llCurTime;
 LARGE_INTEGER CurTimeLI;
 FILETIME FT;
 SYSTEMTIME ST;
 double dVal;
 CWS2 CurWakeSection2;
 std::vector<rWVSSToVisCircle>VisCircles;
 rWVSSToVisCircle VisCircle;
 std::vector<rWVSSToVisPlane>VisPlanes;
 rWVSSToVisPlane VisPlane;
 std::vector<rWVSSToVisLine>VisLines;
 rWVSSToVisLine VisLine;
 CPageFrame::CFrameData FrameData;
 CPageOverride::COverrideData OverrideData;
 CPageWakeFrame::CWakeFrameData WakeFrameData;
 CWESC *pCurWakeInteraction;
 rWVSSToVisUnlimitedLine *pVisTracks = NULL;
 unsigned long ulTrackDataCount, ulTrackDataSize;
 char CallSignToFollow[32];
 CComboBox *pPlaneCombo;
 int Index;
 DWORD dwVal;
 if(!m_Severity.m_dwTSCount) return;
 if((DWORD)p_nCurPos > m_Severity.m_dwTSCount - 1) return;
 pPlaneCombo = (CComboBox *)GetDlgItem(IDO_VIS_CENTER_ON);
 if(pPlaneCombo) { 
   Index = pPlaneCombo->GetCurSel();
   if(Index != CB_ERR) {
     dwVal = pPlaneCombo->GetItemData(Index);
     *CallSignToFollow = '\0';
     if(dwVal != 0xFFFFFFFF && IsDlgButtonChecked(IDC_VIS_CENTER_ON))
       strcpy(CallSignToFollow, m_Severity.m_Aircraft[dwVal]->m_CallSign); } }
 sprintf(tstr, "%ld", p_nCurPos);
 m_PosEdit.SetWindowText(tstr);
 llCurTime = m_Severity.m_llMinTime + (LONGLONG)p_nCurPos * m_Severity.m_dwBreakInterval;
 CurTimeLI.QuadPart = llCurTime;
 FT.dwHighDateTime = CurTimeLI.HighPart;
 FT.dwLowDateTime = CurTimeLI.LowPart;
 FileTimeToSystemTime(&FT, &ST);
 sprintf(tstr, "Time: %.4d-%.2d-%.2d %.2d:%.2d:%.2d", ST.wYear, ST.wMonth, ST.wDay, ST.wHour, ST.wMinute, ST.wSecond);
 SetDlgItemText(IDS_TIME, tstr);
 m_pBarViewDlg->SetDlgItemText(IDS_TIME, tstr);
 m_pDisplayPage->FillFlagsFromControls(&m_DisplayFlags);
 m_pOverridePage->FillFlagsFromControls(&m_FSG);
 m_pWakeOptionsPage->FillFlagsFromControls(&m_FSG);
 m_pLoadPage->FillFlagsFromControls(&m_FSG);
 m_Severity.m_FSG = m_FSG;
 m_pFramePage->ResetFrameData();
 m_pWakeFramePage->ResetWakeFrameData();
 m_Severity.SetAircraftFlags(p_nCurPos); 
 TSCount = m_Severity.m_pIntTS[p_nCurPos].m_TP.size();
 if(m_bDrawTracks)
   PrepareTrackDataForVisualization(&pVisTracks, &ulTrackDataCount, &ulTrackDataSize);
 for(i = 0; i < TSCount; i ++) {
   lVal = m_Severity.m_pIntTS[p_nCurPos].m_TP[i];
   AircraftOrdinal = m_Severity.m_TrackPointsInt[lVal].m_nAircraftOrdinal;
   dVal = m_Severity.m_TrackPointsInt[lVal].m_dWDRT;
   dVal -= M_PI; 
   if(dVal < 0) dVal += 2 * M_PI;
   FrameData.Reset();
   strcpy(FrameData.m_CallSign, m_Severity.m_Aircraft[AircraftOrdinal]->m_CallSign);
   FrameData.m_dWindDirectionFrom_deg = Degrees(dVal);
   FrameData.m_dWindSpeed_kts = m_Severity.m_TrackPointsInt[lVal].m_dWindSpeedMS * 3.6 / 1.852;
   FrameData.m_bWindSet = true;
   FrameData.m_dSpeedMS = m_Severity.m_TrackPointsInt[lVal].m_dSpeedMS;
   FrameData.m_nAltitudeM = m_Severity.m_TrackPointsInt[lVal].m_nAltitudeM;
   FrameData.m_dLatDeg = m_Severity.m_TrackPointsInt[lVal].m_dLatDeg;
   FrameData.m_dLongDeg = m_Severity.m_TrackPointsInt[lVal].m_dLongDeg;
   strcpy(FrameData.m_AircraftType, m_Severity.m_Aircraft[AircraftOrdinal]->m_ACModel);
   m_pFramePage->AddFrameData(&FrameData);
     memset(&VisPlane, 0, sizeof(VisPlane));
     if(m_Severity.m_Aircraft[AircraftOrdinal]->m_pAircraft)
       strncpy(VisPlane.m_PlaneType.m_PlaneType, m_Severity.m_Aircraft[AircraftOrdinal]->m_pAircraft->m_TN, sizeof(VisPlane.m_PlaneType.m_PlaneType) - 1);
     else
       strncpy(VisPlane.m_PlaneType.m_PlaneType, m_Severity.m_Aircraft[AircraftOrdinal]->m_ACModel, sizeof(VisPlane.m_PlaneType.m_PlaneType) - 1);
     strncpy(VisPlane.m_CallSign.m_CallSign, m_Severity.m_Aircraft[AircraftOrdinal]->m_CallSign, sizeof(VisPlane.m_CallSign.m_CallSign) - 1);
     VisPlane.m_Position.m_dLat = m_Severity.m_TrackPointsInt[lVal].m_dLatDeg;
     VisPlane.m_Position.m_dLong = m_Severity.m_TrackPointsInt[lVal].m_dLongDeg;
     VisPlane.m_Position.m_dH = m_Severity.m_TrackPointsInt[lVal].m_nAltitudeM;
     VisPlane.m_dHeading = m_Severity.m_TrackPointsInt[lVal].m_dAzimuthDeg;
     VisPlane.m_dPHCti = m_Severity.m_TrackPointsInt[lVal].m_dAttitudeAngleDeg;
     VisPlane.m_dBank = m_Severity.m_TrackPointsInt[lVal].m_dBankDeg;
     VisPlane.m_nScale = m_DisplayFlags.m_nPlaneSizeCorrection;
     VisPlane.m_bShowAirBlock = m_DisplayFlags.m_bShowAirBlocks;
     if(m_Severity.m_Aircraft[AircraftOrdinal]->m_pAircraft)
       VisPlane.m_dWingspan = m_Severity.m_Aircraft[AircraftOrdinal]->m_pAircraft->m_dWSM;
     else
       VisPlane.m_dWingspan = 30; 
     VisPlane.m_bCenterTo = !stricmp(m_Severity.m_Aircraft[AircraftOrdinal]->m_CallSign, CallSignToFollow);
     VisPlanes.push_back(VisPlane);
     if(AircraftOrdinal == m_DisplayFlags.m_nRingAirplaneNum - 1)
       DrawDistanceCircles(&m_Severity.m_TrackPointsInt[lVal], &VisCircles);
   }
 if(!m_DisplayFlags.m_nRingAirplaneNum) 
   DrawDistanceCircles(NULL, &VisCircles);
 for(i = 0; i < m_Severity.m_Aircraft.size(); i ++) { 
   AircraftOrdinal = i;
   lStartIndex = p_nCurPos - m_Severity.m_Aircraft[AircraftOrdinal]->m_lStartPos;
   m_Severity.m_Aircraft[AircraftOrdinal]->m_Wake.SetFlags(&m_FSG); 
   m_Severity.m_Aircraft[AircraftOrdinal]->m_Wake.CSRae(CurTimeLI, &lStartIndex, &m_Severity.m_Aircraft[AircraftOrdinal]->m_lCurSectionCount);
   lEndIndex = lStartIndex - m_Severity.m_Aircraft[AircraftOrdinal]->m_lCurSectionCount;
   if(lStartIndex - lEndIndex > 0) {
     FrameData.Reset();
     strcpy(FrameData.m_CallSign, m_Severity.m_Aircraft[AircraftOrdinal]->m_CallSign);
     FrameData.m_lStartWakeSection = lStartIndex;
     FrameData.m_lEndWakeSection = lEndIndex;
     FrameData.m_bWakeSet = true;
     m_pFramePage->AddFrameData(&FrameData);
     }
   for(lj = lStartIndex; lj > lEndIndex; lj--) {
     if(!m_Severity.m_Aircraft[AircraftOrdinal]->m_Wake.GS2EC(lj, CurTimeLI, &CurWakeSection2))
       continue;
     if(!m_DisplayFlags.m_ShowSections[0] && !m_DisplayFlags.m_ShowSections[1] || lj >= m_DisplayFlags.m_ShowSections[0] && lj <= m_DisplayFlags.m_ShowSections[1]) { 
       VisCircle.m_Center.m_dLat = CurWakeSection2.m_dLT1d;
       VisCircle.m_Center.m_dLong = CurWakeSection2.m_sLL1DT;
       VisCircle.m_Center.m_dH = CurWakeSection2.m_dAltitude1;
       VisCircle.m_Color[0] = VisCircle.m_Color[1] = VisCircle.m_Color[2] = 0;
       switch(CurWakeSection2.m_cDangerLevel) { 
         case 0: VisCircle.m_Color[0] = 255; break;
         case 1: VisCircle.m_Color[0] = VisCircle.m_Color[1] = 255; break;
         case 2: VisCircle.m_Color[1] = 255; break;
         default: VisCircle.m_Color[0] = VisCircle.m_Color[1] = VisCircle.m_Color[2] = 255; break;
         }
       VisCircle.m_fUSRad = (float)((CurWakeSection2.m_dHSe + CurWakeSection2.m_deVSe) / 2 * m_DisplayFlags.m_nWakeSectionSizeCorrection);
       VisCircle.m_fLineWidth = (float)1 + m_DisplayFlags.m_nWakeSectionSizeCorrection;
       VisCircle.m_fHeading = (float)CurWakeSection2.m_dHd;
       VisCircle.m_fPHCti = 0;
       VisCircle.m_fBank = (float)CurWakeSection2.m_dBdK;
       VisCircles.push_back(VisCircle);
       VisCircle.m_Center.m_dLat = CurWakeSection2.m_dL2TD;
       VisCircle.m_Center.m_dLong = CurWakeSection2.m_dLLD2;
       VisCircle.m_Center.m_dH = CurWakeSection2.m_dAltitude2;
       VisCircles.push_back(VisCircle);
       if(CurWakeSection2.m_bUNC && m_DisplayFlags.m_bShowConeOfUncertainty) {
         VisLine.m_Color[0] = VisCircle.m_Color[0];
         VisLine.m_Color[1] = VisCircle.m_Color[1];
         VisLine.m_Color[2] = VisCircle.m_Color[2];
         VisLine.m_fLineWidth = (float)1; 
         VisLine.m_Points[0].m_dLat = CurWakeSection2.m_dLT1dLower;
         VisLine.m_Points[0].m_dLong = CurWakeSection2.m_sLL1DTLower;
         VisLine.m_Points[0].m_dH = CurWakeSection2.m_dAltitude1Lower;
         VisLine.m_Points[1].m_dLat = CurWakeSection2.m_dLT1dUpper;
         VisLine.m_Points[1].m_dLong = CurWakeSection2.m_sLL1DTUpper;
         VisLine.m_Points[1].m_dH = CurWakeSection2.m_dAltitude1Upper;
         VisLines.push_back(VisLine);
         VisLine.m_Points[0].m_dLat = CurWakeSection2.m_dL2TDLower;
         VisLine.m_Points[0].m_dLong = CurWakeSection2.m_dLLD2Lower;
         VisLine.m_Points[0].m_dH = CurWakeSection2.m_dAltitude2Lower;
         VisLine.m_Points[1].m_dLat = CurWakeSection2.m_dL2TDUpper;
         VisLine.m_Points[1].m_dLong = CurWakeSection2.m_dLLD2Upper;
         VisLine.m_Points[1].m_dH = CurWakeSection2.m_dAltitude2Upper;
         VisLines.push_back(VisLine);
         VisLine.m_Color[0] = VisCircle.m_Color[0];
         VisLine.m_Color[1] = VisCircle.m_Color[1];
         VisLine.m_Color[2] = VisCircle.m_Color[2];
         VisLine.m_fLineWidth = (float)1; 
         VisLine.m_Points[0].m_dLat = CurWakeSection2.m_dLT1dLefter;
         VisLine.m_Points[0].m_dLong = CurWakeSection2.m_sLL1DTLefter;
         VisLine.m_Points[0].m_dH = CurWakeSection2.m_dAltitude1;
         VisLine.m_Points[1].m_dLat = CurWakeSection2.m_dLT1dRighter;
         VisLine.m_Points[1].m_dLong = CurWakeSection2.m_sLL1DTRighter;
         VisLine.m_Points[1].m_dH = CurWakeSection2.m_dAltitude1;
         VisLines.push_back(VisLine);
         VisLine.m_Color[0] = VisCircle.m_Color[0];
         VisLine.m_Color[1] = VisCircle.m_Color[1];
         VisLine.m_Color[2] = VisCircle.m_Color[2];
         VisLine.m_fLineWidth = (float)1; 
         VisLine.m_Points[0].m_dLat = CurWakeSection2.m_dL2TDLefter;
         VisLine.m_Points[0].m_dLong = CurWakeSection2.m_dLLD2Lefter;
         VisLine.m_Points[0].m_dH = CurWakeSection2.m_dAltitude2;
         VisLine.m_Points[1].m_dLat = CurWakeSection2.m_dL2TDRighter;
         VisLine.m_Points[1].m_dLong = CurWakeSection2.m_dLLD2Righter;
         VisLine.m_Points[1].m_dH = CurWakeSection2.m_dAltitude2;
         VisLines.push_back(VisLine);
         }
       }
     }
   } 
 m_Severity.Recalc2(p_nCurPos, false);
 m_pFramePage->UpdateFrameData();
 OverrideData.Reset();
 SetWindowText("WVSS Player");
 for(i = 0; i < m_Severity.m_pIntTS[p_nCurPos].m_WakeInteractions.size(); i++) {
   pCurWakeInteraction = &m_Severity.m_pIntTS[p_nCurPos].m_WakeInteractions[i];
   if(!i) {
     OverrideData.m_ddZ = pCurWakeInteraction->m_WCSS.m_dCZ;
     OverrideData.m_ddH = pCurWakeInteraction->m_WCSS.m_dCH;
     OverrideData.m_ddAngle = pCurWakeInteraction->m_WCSS.m_dCARd * 180 / M_PI;
     OverrideData.m_dGamma = (pCurWakeInteraction->m_WCSS.m_dLtG + pCurWakeInteraction->m_WCSS.m_dLtG) / 2; }
   WakeFrameData.Reset();
   WakeFrameData.SetCallSign(pCurWakeInteraction->m_WCSS.m_pEE ? pCurWakeInteraction->m_WCSS.m_pEE->m_CallSign : NULL);
   WakeFrameData.SATe(pCurWakeInteraction->m_WCSS.m_pEE ? pCurWakeInteraction->m_WCSS.m_pEE->m_ACModel : NULL);
   WakeFrameData.m_ddZC = pCurWakeInteraction->m_WCSS.m_dCZ;
   WakeFrameData.m_ddHC = pCurWakeInteraction->m_WCSS.m_dCH;
   WakeFrameData.m_ddZL = pCurWakeInteraction->m_WCSS.m_dLZ;
   WakeFrameData.m_ddHL = pCurWakeInteraction->m_WCSS.m_dLH;
   WakeFrameData.m_dGammaL = pCurWakeInteraction->m_WCSS.m_dLtG;
   WakeFrameData.m_ddZR = pCurWakeInteraction->m_WCSS.m_dRZ;
   WakeFrameData.m_ddHR = pCurWakeInteraction->m_WCSS.m_dRH;
   WakeFrameData.m_dGammaR = pCurWakeInteraction->m_WCSS.m_dRGt;
   WakeFrameData.m_bPositionOverride = m_FSG.m_bOverrideProximityValues;
   if(WakeFrameData.m_bPositionOverride) {
     WakeFrameData.m_ddZOverride = m_FSG.m_dStartPlaneZOverride;
     WakeFrameData.m_ddHOverride = m_FSG.m_dStartPlaneHOverride; }
   WakeFrameData.m_bGammaOverride = m_FSG.m_bOverrideGamma;
   if(WakeFrameData.m_bGammaOverride)
     WakeFrameData.m_dGammaOverride = m_FSG.m_dWakeGammaOverrideValue;
   m_pWakeFramePage->AddWakeFrameData(&WakeFrameData);
   if(!i) {
     sprintf(tstr, "WVSS Player - [time in wake %.1f sec. Severity: bank %.1f deg, rate %.1f deg/s, alt loss %.1f ft]",
       pCurWakeInteraction->m_CSSO[0].m_dTimeInWake,
       Degrees(pCurWakeInteraction->m_CSSO[0].m_dMaxBankRad),
       Degrees(pCurWakeInteraction->m_CSSO[0].m_dMaxAngularSpeedOfBankRadPerS),
       pCurWakeInteraction->m_CSSO[0].m_dMaxAltitudeLossM / 0.3048);
     SetWindowText(tstr); }
   }
 m_pOverridePage->UpdateOverrideData(&OverrideData);
 m_pWakeFramePage->UpdateWakeFrameData();
 if(m_bDrawTracks) { 
   VisualizeTracks(pVisTracks, ulTrackDataCount, ulTrackDataSize);
   m_bDrawTracks = false; }
 VisualizeAircraft(&VisPlanes);
 VisualizeCirclesAndLines(p_nCurPos, &VisCircles, &VisLines); 
}
void CMainDialog::PrepareTrackDataForVisualization(rWVSSToVisUnlimitedLine **p_ppVisTracks, unsigned long *p_pulTrackDataCount, unsigned long *p_pulTrackDataSize)
{
 unsigned long ulDataSize, li, lj, ulStartPoint, ulEndPoint;
 rWVSSToVisUnlimitedLine *pVisUnlimitedLine;
 rWVSSGeoPoint *pWVSSGeoPoint;
 char *pData;
 const short nLineColorCount = 10;
 short nCurColor;
 unsigned char LineColors[nLineColorCount][3] = { {65, 111, 166}, {168, 66, 63}, {134, 164, 74}, {110, 84, 141}, {61, 150, 174}, {218, 129, 55}, {142, 165, 203}, {106, 142, 141}, {181, 202, 146}, {165, 151, 185} };
 ASSERT(p_ppVisTracks && p_pulTrackDataCount && p_pulTrackDataSize);
 ulDataSize = m_Severity.m_Aircraft.size() * sizeof(rWVSSToVisUnlimitedLine);
 for(li = 0; li < m_Severity.m_Aircraft.size(); li++)
   ulDataSize += m_Severity.m_TrackPointsInt[m_Severity.m_Aircraft[li]->m_lStartTrackPointIndex].m_lPoints * sizeof(rWVSSGeoPoint);
 pData = new char[ulDataSize];
 if(!pData) return;
 memset(pData,0, ulDataSize);
 nCurColor = 0;
 pWVSSGeoPoint = (rWVSSGeoPoint *)pData;
 for(li = 0; li < m_Severity.m_Aircraft.size(); li++) {
   pVisUnlimitedLine = (rWVSSToVisUnlimitedLine *)pWVSSGeoPoint;
   ulStartPoint = m_Severity.m_Aircraft[li]->m_lStartTrackPointIndex;
   pVisUnlimitedLine->p_lPointCount = m_Severity.m_TrackPointsInt[ulStartPoint].m_lPoints;
   ulEndPoint = ulStartPoint + pVisUnlimitedLine->p_lPointCount;
   pVisUnlimitedLine->m_fLineWidth = 2;
   pVisUnlimitedLine->m_Color[0] = LineColors[nCurColor][0];
   pVisUnlimitedLine->m_Color[1] = LineColors[nCurColor][1];
   pVisUnlimitedLine->m_Color[2] = LineColors[nCurColor][2];
   pWVSSGeoPoint = (rWVSSGeoPoint *)(pVisUnlimitedLine + 1);
   for(lj = ulStartPoint; lj < ulEndPoint; lj ++) {
     pWVSSGeoPoint->m_dLat = m_Severity.m_TrackPointsInt[lj].m_dLatDeg;
     pWVSSGeoPoint->m_dLong = m_Severity.m_TrackPointsInt[lj].m_dLongDeg;
     pWVSSGeoPoint->m_dH = m_Severity.m_TrackPointsInt[lj].m_nAltitudeM;
     ++ pWVSSGeoPoint; }
   nCurColor ++;
   if(nCurColor >= nLineColorCount) nCurColor = 0;
   }
 *p_ppVisTracks = (rWVSSToVisUnlimitedLine *)pData;
 *p_pulTrackDataCount = m_Severity.m_Aircraft.size();
 *p_pulTrackDataSize = ulDataSize;
}
void CMainDialog::VisualizeAircraft(std::vector<rWVSSToVisPlane> *p_pVisAircraft)
{
 bool bVal;
 unsigned long lMsgSize;
 size_t i;
 char *pCharMsg;
 CWVSSMsgHeader *pMsg;
 CWVSSPlanePosition *pPlanePositionMsg;
 rWVSSToVisPlane *pPlanePosition;
 lMsgSize = sizeof(CWVSSPlanePosition);
 lMsgSize += p_pVisAircraft->size() * sizeof(rWVSSToVisPlane);
 pCharMsg = new char[lMsgSize];
 if(!pCharMsg) return;
 pMsg = (CWVSSMsgHeader *)pCharMsg;
 pMsg->msg_size = lMsgSize;
 pMsg->msg_type = WVSS_msg_planepos;
 pPlanePositionMsg = (CWVSSPlanePosition *)pCharMsg;
 pPlanePositionMsg->p_nWVSSToVisPlaneCount = p_pVisAircraft->size();
 pPlanePosition = (rWVSSToVisPlane *)(pCharMsg + sizeof(CWVSSPlanePosition));
 for(i = 0; i < p_pVisAircraft->size(); i ++) {
   *pPlanePosition = p_pVisAircraft->at(i);
   ++ pPlanePosition;
   }
 bVal = m_Socket.SendMsg(pMsg);
 bVal = bVal;
}
void CMainDialog::VisualizeCirclesAndLines(int p_nCurPos, std::vector<rWVSSToVisCircle> *p_pVisCircles, std::vector<rWVSSToVisLine> *p_pVisLines)
{
 bool bVal, bCircles, bRectangles, bLinesFromCenter, bLinesFromWintips;
 unsigned long lMsgSize;
 char *pCharMsg, *pCurChar;
 CWVSSMsgHeader *pMsg;
 CWVSSData *pDataMsg;
 rWVSSToVisCircle Circle, *pCircle;
 rWVSSToVisLine Line, *pLine;
 rWVSSToVisRectangle Rectangle, *pRectangle;
 CWESC *pCurWakeInteraction;
 size_t i, WakeInteractions, LineCount, CircleCount, RectangleCount;
 DWORD WakeInteractionHits;
 bCircles = true;
 bRectangles = false;
 bLinesFromCenter = m_DisplayFlags.m_bShowLinesFromAircraftCenter;
 bLinesFromWintips = m_DisplayFlags.m_bShowLinesFromWtpis;
 WakeInteractions = m_Severity.m_pIntTS[p_nCurPos].m_WakeInteractions.size();
 WakeInteractionHits = 0;
 for(i = 0; i < WakeInteractions; i ++) {
   pCurWakeInteraction = &m_Severity.m_pIntTS[p_nCurPos].m_WakeInteractions[i];
   if(pCurWakeInteraction->m_WCSS.m_bRWakeOK)
     WakeInteractionHits ++;
   if(pCurWakeInteraction->m_WCSS.m_bLWakeOK)
     WakeInteractionHits ++; }
 LineCount = p_pVisLines->size();
 CircleCount = p_pVisCircles->size();
 RectangleCount = 0;
 if(bLinesFromCenter)
   LineCount += WakeInteractionHits;
 if(bLinesFromWintips) 
   LineCount += WakeInteractionHits * 2; 
 if(bCircles)
   CircleCount += WakeInteractionHits;
 if(bRectangles)
   RectangleCount += WakeInteractionHits;
 lMsgSize = sizeof(CWVSSData);
 lMsgSize += LineCount * sizeof(rWVSSToVisLine);
 lMsgSize += CircleCount * sizeof(rWVSSToVisCircle);
 lMsgSize += RectangleCount * sizeof(rWVSSToVisRectangle);
 pCharMsg = new char[lMsgSize];
 if(!pCharMsg) return;
 pMsg = (CWVSSMsgHeader *)pCharMsg;
 pMsg->msg_size = lMsgSize;
 pMsg->msg_type = WVSS_msg_data;
 pDataMsg = (CWVSSData *)pCharMsg;
 pDataMsg->m_ulWVSSToVisLineCount = LineCount;
 pDataMsg->m_ulWVSSToVisCircleCount = CircleCount;
 pDataMsg->m_ulWVSSToVisRectangleCount = RectangleCount;
 pCurChar = pCharMsg + sizeof(CWVSSData);
 pLine = (rWVSSToVisLine *)pCurChar;
 for(i = 0; i < WakeInteractions; i ++) {
   pCurWakeInteraction = &m_Severity.m_pIntTS[p_nCurPos].m_WakeInteractions[i];
   if(pCurWakeInteraction->m_WCSS.m_bRWakeOK && bLinesFromCenter) {
     Line.m_Points[0].m_dLat = pCurWakeInteraction->m_WCSS.m_dEALD;
     Line.m_Points[0].m_dLong = pCurWakeInteraction->m_WCSS.m_dEALLD;
     Line.m_Points[0].m_dH = pCurWakeInteraction->m_WCSS.m_dEAMH;
     Line.m_Points[1].m_dLat = pCurWakeInteraction->m_WCSS.m_dRLat;
     Line.m_Points[1].m_dLong = pCurWakeInteraction->m_WCSS.m_dRLong;
     Line.m_Points[1].m_dH = pCurWakeInteraction->m_WCSS.m_dRGeoH;
     Line.m_fLineWidth = 1;
     Line.m_Color[0] = pLine->m_Color[1] = pLine->m_Color[2] = 255;
     *pLine = Line;
     ++ pLine; }
   if(pCurWakeInteraction->m_WCSS.m_bLWakeOK && bLinesFromCenter) {
     Line.m_Points[0].m_dLat = pCurWakeInteraction->m_WCSS.m_dEALD;
     Line.m_Points[0].m_dLong = pCurWakeInteraction->m_WCSS.m_dEALLD;
     Line.m_Points[0].m_dH = pCurWakeInteraction->m_WCSS.m_dEAMH;
     Line.m_Points[1].m_dLat = pCurWakeInteraction->m_WCSS.m_dLLat;
     Line.m_Points[1].m_dLong = pCurWakeInteraction->m_WCSS.m_dLLong;
     Line.m_Points[1].m_dH = pCurWakeInteraction->m_WCSS.m_dLGeoH;
     Line.m_fLineWidth = 1;
     Line.m_Color[0] = pLine->m_Color[1] = pLine->m_Color[2] = 255;
     *pLine = Line;
     ++ pLine; }
   if(pCurWakeInteraction->m_WCSS.m_bLWakeOK && bLinesFromWintips) {
     Line.m_Points[0].m_dLat = pCurWakeInteraction->m_WCSS.m_dEtAfLWtpiLatDeg;
     Line.m_Points[0].m_dLong = pCurWakeInteraction->m_WCSS.m_dEtAfLWtpiLongDeg;
     Line.m_Points[0].m_dH = pCurWakeInteraction->m_WCSS.m_dEtAfLWtpiGeoH_m;
     Line.m_Points[1].m_dLat = pCurWakeInteraction->m_WCSS.m_dLLat;
     Line.m_Points[1].m_dLong = pCurWakeInteraction->m_WCSS.m_dLLong;
     Line.m_Points[1].m_dH = pCurWakeInteraction->m_WCSS.m_dLGeoH;
     Line.m_fLineWidth = 1;
     Line.m_Color[0] = pLine->m_Color[1] = 0;
     pLine->m_Color[2] = 255;
     *pLine = Line;
     ++ pLine; }
   if(pCurWakeInteraction->m_WCSS.m_bRWakeOK && bLinesFromWintips) {
     Line.m_Points[0].m_dLat = pCurWakeInteraction->m_WCSS.m_dEtAfLWtpiLatDeg;
     Line.m_Points[0].m_dLong = pCurWakeInteraction->m_WCSS.m_dEtAfLWtpiLongDeg;
     Line.m_Points[0].m_dH = pCurWakeInteraction->m_WCSS.m_dEtAfLWtpiGeoH_m;
     Line.m_Points[1].m_dLat = pCurWakeInteraction->m_WCSS.m_dRLat;
     Line.m_Points[1].m_dLong = pCurWakeInteraction->m_WCSS.m_dRLong;
     Line.m_Points[1].m_dH = pCurWakeInteraction->m_WCSS.m_dRGeoH;
     Line.m_fLineWidth = 1;
     Line.m_Color[0] = pLine->m_Color[1] = 0;
     pLine->m_Color[2] = 255;
     *pLine = Line;
     ++ pLine; }
   if(pCurWakeInteraction->m_WCSS.m_bLWakeOK && bLinesFromWintips) {
     Line.m_Points[0].m_dLat = pCurWakeInteraction->m_WCSS.m_dEtAfRWtpiLatDeg;
     Line.m_Points[0].m_dLong = pCurWakeInteraction->m_WCSS.m_dEtAfRWtpiLongDeg;
     Line.m_Points[0].m_dH = pCurWakeInteraction->m_WCSS.m_dEtAfRWtpiGeoH_m;
     Line.m_Points[1].m_dLat = pCurWakeInteraction->m_WCSS.m_dLLat;
     Line.m_Points[1].m_dLong = pCurWakeInteraction->m_WCSS.m_dLLong;
     Line.m_Points[1].m_dH = pCurWakeInteraction->m_WCSS.m_dLGeoH;
     Line.m_fLineWidth = 1;
     Line.m_Color[0] = pLine->m_Color[1] = 0;
     pLine->m_Color[2] = 255;
     *pLine = Line;
     ++ pLine; }
   if(pCurWakeInteraction->m_WCSS.m_bRWakeOK && bLinesFromWintips) {
     Line.m_Points[0].m_dLat = pCurWakeInteraction->m_WCSS.m_dEtAfRWtpiLatDeg;
     Line.m_Points[0].m_dLong = pCurWakeInteraction->m_WCSS.m_dEtAfRWtpiLongDeg;
     Line.m_Points[0].m_dH = pCurWakeInteraction->m_WCSS.m_dEtAfRWtpiGeoH_m;
     Line.m_Points[1].m_dLat = pCurWakeInteraction->m_WCSS.m_dRLat;
     Line.m_Points[1].m_dLong = pCurWakeInteraction->m_WCSS.m_dRLong;
     Line.m_Points[1].m_dH = pCurWakeInteraction->m_WCSS.m_dRGeoH;
     Line.m_fLineWidth = 1;
     Line.m_Color[0] = pLine->m_Color[1] = 0;
     pLine->m_Color[2] = 255;
     *pLine = Line;
     ++ pLine;
     }
   }
 for(i = 0; i < p_pVisLines->size(); i ++) {
   *pLine = p_pVisLines->at(i);
   ++ pLine; }
 pCurChar += LineCount * sizeof(rWVSSToVisLine); 
 pCircle = (rWVSSToVisCircle *)pCurChar;
 for(i = 0; i < WakeInteractions; i ++) {
   pCurWakeInteraction = &m_Severity.m_pIntTS[p_nCurPos].m_WakeInteractions[i];
   if(pCurWakeInteraction->m_WCSS.m_bRWakeOK) {
     Circle.m_Center.m_dLat = pCurWakeInteraction->m_WCSS.m_dRLat;
     Circle.m_Center.m_dLong = pCurWakeInteraction->m_WCSS.m_dRLong;
     Circle.m_Center.m_dH = pCurWakeInteraction->m_WCSS.m_dRGeoH;
     Circle.m_fUSRad = (float)(pCurWakeInteraction->m_WCSS.m_dRWakeUSRad * m_DisplayFlags.m_nWakeSectionSizeCorrection);
     Circle.m_fLineWidth = (float)1 + m_DisplayFlags.m_nWakeSectionSizeCorrection;
     Circle.m_fHeading = (float)pCurWakeInteraction->m_WCSS.m_dRHeadingDeg;
     Circle.m_fPHCti = 0;
     Circle.m_fBank = (float)Degrees(pCurWakeInteraction->m_WCSS.m_dPVBankRad);
     Circle.m_Color[0] = Circle.m_Color[1] = Circle.m_Color[2] = 255;
     *pCircle = Circle;
     ++ pCircle; }
   if(pCurWakeInteraction->m_WCSS.m_bLWakeOK) {
     Circle.m_Center.m_dLat = pCurWakeInteraction->m_WCSS.m_dLLat;
     Circle.m_Center.m_dLong = pCurWakeInteraction->m_WCSS.m_dLLong;
     Circle.m_Center.m_dH = pCurWakeInteraction->m_WCSS.m_dLGeoH;
     Circle.m_fUSRad = (float)(pCurWakeInteraction->m_WCSS.m_dLWakeUSRad * m_DisplayFlags.m_nWakeSectionSizeCorrection);
     Circle.m_fLineWidth = (float)1 + m_DisplayFlags.m_nWakeSectionSizeCorrection;
     Circle.m_fHeading = (float)pCurWakeInteraction->m_WCSS.m_dLHeadingDeg;
     Circle.m_fPHCti = 0;
     Circle.m_fBank = (float)Degrees(pCurWakeInteraction->m_WCSS.m_dPVBankRad);
     Circle.m_Color[0] = Circle.m_Color[1] = Circle.m_Color[2] = 255;
     *pCircle = Circle;
     ++ pCircle; }
   }
 for(i = 0; i < p_pVisCircles->size(); i ++) {
   *pCircle = p_pVisCircles->at(i);
   ++ pCircle; }
 pCurChar += CircleCount * sizeof(rWVSSToVisCircle);
 if(bRectangles) {
   pRectangle = (rWVSSToVisRectangle *)pCurChar;
   for(i = 0; i < WakeInteractions; i ++) {
     pCurWakeInteraction = &m_Severity.m_pIntTS[p_nCurPos].m_WakeInteractions[i];
     if(pCurWakeInteraction->m_WCSS.m_bRWakeOK) {
       Rectangle.m_Center.m_dLat = pCurWakeInteraction->m_WCSS.m_dRLat;
       Rectangle.m_Center.m_dLong = pCurWakeInteraction->m_WCSS.m_dRLong;
       Rectangle.m_Center.m_dH = pCurWakeInteraction->m_WCSS.m_dRGeoH;
       Rectangle.m_fVertExtent = (float)(pCurWakeInteraction->m_WCSS.m_dRVertExtent * m_DisplayFlags.m_nWakeSectionSizeCorrection);
       Rectangle.m_fHorzExtent = (float)(pCurWakeInteraction->m_WCSS.m_dRHorzExtent * m_DisplayFlags.m_nWakeSectionSizeCorrection);
       Rectangle.m_fLineWidth = (float)1 + m_DisplayFlags.m_nWakeSectionSizeCorrection;
       Rectangle.m_fHeading = (float)pCurWakeInteraction->m_WCSS.m_dRHeadingDeg;
       Rectangle.m_fPHCti = 0;
       Rectangle.m_fBank = (float)Degrees(pCurWakeInteraction->m_WCSS.m_dPVBankRad);
       Rectangle.m_Color[0] = Rectangle.m_Color[1] = Rectangle.m_Color[2] = 255;
       *pRectangle = Rectangle;
       ++ pRectangle; }
     if(pCurWakeInteraction->m_WCSS.m_bLWakeOK) {
       Rectangle.m_Center.m_dLat = pCurWakeInteraction->m_WCSS.m_dLLat;
       Rectangle.m_Center.m_dLong = pCurWakeInteraction->m_WCSS.m_dLLong;
       Rectangle.m_Center.m_dH = pCurWakeInteraction->m_WCSS.m_dLGeoH;
       Rectangle.m_fVertExtent = (float)(pCurWakeInteraction->m_WCSS.m_dLVertExtent * m_DisplayFlags.m_nWakeSectionSizeCorrection);
       Rectangle.m_fHorzExtent = (float)(pCurWakeInteraction->m_WCSS.m_dLHorzExtent * m_DisplayFlags.m_nWakeSectionSizeCorrection);
       Rectangle.m_fLineWidth = (float)1 + m_DisplayFlags.m_nWakeSectionSizeCorrection;
       Rectangle.m_fHeading = (float)pCurWakeInteraction->m_WCSS.m_dLHeadingDeg;
       Rectangle.m_fPHCti = 0;
       Rectangle.m_fBank = (float)Degrees(pCurWakeInteraction->m_WCSS.m_dPVBankRad);
       Rectangle.m_Color[0] = Rectangle.m_Color[1] = Rectangle.m_Color[2] = 255;
       *pRectangle = Rectangle;
       ++ pRectangle; }
     } }
 pCurChar += RectangleCount * sizeof(rWVSSToVisRectangle);
 bVal = m_Socket.SendMsg(pMsg);
 bVal = bVal;
}
void CMainDialog::VisualizeTracks(rWVSSToVisUnlimitedLine *p_pVisUnlimitedLines, unsigned long p_ulTrackCount, unsigned long p_ulDataSize)
{
 bool bVal;
 unsigned long lMsgSize;
 char *pCharMsg;
 CWVSSMsgHeader *pMsg;
 CWVSSTrack *pTrackMsg;
 lMsgSize = sizeof(CWVSSTrack) + p_ulDataSize;
 pCharMsg = new char[lMsgSize];
 if(!pCharMsg) {
   delete [] p_pVisUnlimitedLines;
   return; }
 pMsg = (CWVSSMsgHeader *)pCharMsg;
 pMsg->msg_size = lMsgSize;
 pMsg->msg_type = WVSS_msg_track;
 pTrackMsg = (CWVSSTrack *)pCharMsg;
 pTrackMsg->m_ulWVSSToVisUnlimitedLineCount = p_ulTrackCount;
 memcpy(pCharMsg + sizeof(CWVSSTrack), (char *)p_pVisUnlimitedLines, p_ulDataSize);
 delete [] p_pVisUnlimitedLines;
 bVal = m_Socket.SendMsg(pMsg);
 bVal = bVal;
}
void CMainDialog::OnBnClickedCenterViewOn()
{
 bool bVal;
 unsigned long lMsgSize;
 char *pCharMsg;
 CWVSSMsgHeader *pMsg;
 CWVSSCenterToPlane *pCenterToMsg;
 CComboBox *pPlaneCombo;
 char CallSign[32];
 int Index;
 DWORD dwVal;
 pPlaneCombo = (CComboBox *)GetDlgItem(IDO_VIS_CENTER_ON);
 if(!pPlaneCombo) return;
 if((Index = pPlaneCombo->GetCurSel()) == CB_ERR) return;
 dwVal = pPlaneCombo->GetItemData(Index);
 if(dwVal == 0xFFFFFFFF)
   *CallSign = '\0';
 else
   strcpy(CallSign, m_Severity.m_Aircraft[dwVal]->m_CallSign);
 lMsgSize = sizeof(CWVSSCenterToPlane);
 pCharMsg = new char[lMsgSize];
 if(!pCharMsg) return;
 pMsg = (CWVSSMsgHeader *)pCharMsg;
 pMsg->msg_size = lMsgSize;
 pMsg->msg_type = WVSS_msg_centertoplane;
 pCenterToMsg = (CWVSSCenterToPlane *)pCharMsg;
 memset(pCenterToMsg->m_CallSign.m_CallSign, 0, sizeof(pCenterToMsg->m_CallSign.m_CallSign));
 strncpy(pCenterToMsg->m_CallSign.m_CallSign, CallSign, sizeof(pCenterToMsg->m_CallSign.m_CallSign) - 1);
 bVal = m_Socket.SendMsg(pMsg);
 bVal = bVal;
}
const std::vector<CWESC> *CMainDialog::GetCurFrameWakeInteractions()
{
 return(&m_Severity.m_pIntTS[GetFrameNumber()].m_WakeInteractions);
}
