#pragma once
#include "WVSSPlayerSheet2.h"
#include "Severity.h"
#include "AppSock.h"
#include "resource.h"
namespace WVSS {
class CWVSSPlayerSheet;
class CBarViewDlg;
class CPageSeverityResults;
class CPageAircraft;
class CPageOverride;
class CPageDisplay;
class CPageWakeOptions;
class CPageLoad;
class CPageInfo;
class CFramePage;
class CPageComms;
class CPageSensitivity;
class CPageWeather;
class CPageWakeFrame;
class CMainDialog : public CDialog
{
 friend class CPageDisplay;
 friend class CPageInfo;
	DECLARE_DYNAMIC(CMainDialog)
public:
 enum eLocation { UL, UR, LL, LR };
 enum eTimeAdvance { PREV, NEXT };
private:
 CSeverity m_Severity;
 CWVSSPlayerSheet2 *m_pSheet;
 CScrollBar m_Scroll;
 CEdit m_PosEdit;
 CComboBox m_PlaySpeedCombo;
 static HHOOK m_hHook;
 HACCEL m_hAccel;
 static CMainDialog *m_pThisDialog;
 SCROLLINFO m_SI; 
 short m_nScrollPageSize;
 CAppSocket m_Socket;
 CBarViewDlg *m_pBarViewDlg;
 bool m_bReflectToBarWnd;
 CEdit *m_pBarWnd_CurFrameNumberEdit;
 DWORD m_lLastFrameTime;
 DWORD m_dwPlaySleep;
 bool m_bPlaying, m_bStopping;
 bool m_bDrawTracks;
 CWEAIF m_FSG;
 CPageDisplay::CDisplayFlags m_DisplayFlags;
protected:
 HICON m_hIcon;
public:
 enum { IDD = IDD_WVSS_MAIN };
 CPageSeverityResults *m_pSeverityResPage;
 CPageAircraft *m_pAircraftPage;
 CPageOverride *m_pOverridePage;
 CPageDisplay *m_pDisplayPage;
 CPageWakeOptions *m_pWakeOptionsPage;
 CPageLoad *m_pLoadPage;
 CPageInfo *m_pInfoPage;
 CPageFrame *m_pFramePage;
 CPageComms *m_pCommsPage;
 CPageSensitivity *m_pSensitivityPage;
 CPageWeather *m_pWeatherPage;
 CPageWakeFrame *m_pWakeFramePage;
private:
 void InitTabCtrl();
 void OnFileLoaded(LPCSTR p_pFileName);
 bool DoScrollSet(UINT nSBCode, UINT p_nPos);
 bool ConnectSocket();
 void OnLocationBtn(eLocation p_Location);
 void DrawDistanceCircles(CTrackPoint *p_pTrackPoint, std::vector<rWVSSToVisCircle>*p_pVisCircles);
 void PrepareTrackDataForVisualization(rWVSSToVisUnlimitedLine **p_ppVisTracks, unsigned long *p_pulTrackDataCount, unsigned long *p_pulTrackDataSize);
 void MakeAirBlockText2(short p_nAircraftOrdinal, CTrackPoint *p_pTP);
 void OnTimePrevNext(eTimeAdvance p_TA);
 void UAPuos(rAircraftPosition *p_pAircraftPosition, short p_nAircraftOrdinal, CTrackPoint *p_pTP);
 void VisualizeCirclesAndLines(int p_nCurPos, std::vector<rWVSSToVisCircle> *p_pVisCircles, std::vector<rWVSSToVisLine> *p_pVisLines);
 void VisualizeTracks(rWVSSToVisUnlimitedLine *p_pVisUnlimitedLines, unsigned long p_ulTrackCount, unsigned long p_ulDataSize);
 void VisualizeAircraft(std::vector<rWVSSToVisPlane> *p_pVisAircraft);
 void LoadAircraftIntoVisualizer();
 void CreateBarView();
 void Recalc2(int p_nCurPos);
 void GetSettings(const char *p_pINIFileName);
protected:
 virtual BOOL OnInitDialog();
public:
	CMainDialog(CWnd* pParent = NULL);   
	virtual ~CMainDialog();
	bool LoadFile(LPCSTR p_pFileName);
	bool DoLoadFile(LPCSTR p_pFileName);
 static bool StaticOnLoadFile(const char *p_pFileName);
	static LRESULT CALLBACK HookProc(int nCode, WPARAM wParam, LPARAM lParam);
	LRESULT CALLBACK ThisHookProc(int nCode, WPARAM wParam, LPARAM lParam);
 static void StaticMessage(const char *p_pStr);
 static void StaticFromSocketMessage(const char *p_pStr);
 static void StaticOnFileLoaded(const char *p_pFileName);
	static UINT StaticPlayThreadFunc(LPVOID p_pParam);
	UINT PlayThreadFunc();
	static void StaticUAPuos(rAircraftPosition *p_pAircraftPosition, short p_nAircraftOrdinal, CTrackPoint *p_pTP);
 static void StaticSocketConnected(bool p_bConnected);
 static float StaticGetProgress();
 float GetProgress();
 void SetFrameNumber(int p_nFrameNum);
 int GetFrameNumber();
 const std::vector<CWESC> *GetCurFrameWakeInteractions();
 void RerunCurrentFrame();
 static void StaticSocketMessageSent(short p_nMsgType);
protected:
	DECLARE_MESSAGE_MAP()
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnHScroll(UINT p_nSBCode, UINT p_nPos, CScrollBar *p_pScrollBar);
 afx_msg void OnSelChangePlaySpeed();
 afx_msg void OnPlayBtnClicked();
 afx_msg void OnCurFrameClicked();
 afx_msg void OnSetFrameButtonClicked();
 afx_msg void OnBnClickedCommsConnect();
 afx_msg void OnBnClickedCenterViewOn();
 afx_msg void OnButtonUL();
 afx_msg void OnButtonUR();
 afx_msg void OnButtonLL();
 afx_msg void OnButtonLR();
 afx_msg void OnCurFrameNumberChanged();
 afx_msg void OnBarViewClicked();
 afx_msg void OnTimePrev();
 afx_msg void OnTimeNext();
 afx_msg void OnAfterStart();
};
} 
