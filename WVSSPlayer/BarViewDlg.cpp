#include "stdafx.h"
#include "SITARWVSSPlayer.h"
#include "BarViewDlg.h"
using namespace WVSS;
IMPLEMENT_DYNAMIC(CBarViewDlg, CDialog)
BEGIN_MESSAGE_MAP(CBarViewDlg, CDialog)
 ON_WM_GETMINMAXINFO()
 ON_WM_LBUTTONDOWN()
 ON_WM_LBUTTONUP()
 ON_WM_MOUSEMOVE()
 ON_WM_LBUTTONDBLCLK()
 ON_WM_ERASEBKGND()
 ON_WM_SIZE()
 ON_BN_CLICKED(IDB_CURPOS, &CBarViewDlg::OnSetButtonClicked)
 ON_BN_CLICKED(IDB_PLAY, &CBarViewDlg::OnPlayButtonClicked)
 ON_EN_CHANGE(IDE_CURPOS, &CBarViewDlg::OnCurFrameNumberChanged)
 ON_WM_HSCROLL()
 ON_WM_CTLCOLOR()
 ON_CBN_SELCHANGE(IDO_PLAYSPEED, &CBarViewDlg::OnSelChangePlaySpeed)
 ON_BN_CLICKED(IDB_MAIN_VIEW, &CBarViewDlg::OnMainViewButtonClicked)
END_MESSAGE_MAP()
CBarViewDlg::CBarViewDlg(CWnd* pParent ) : CDialog(CBarViewDlg::IDD, pParent)
{
 m_Anchor.AddCtl(IDS_TIME, MCTL_RIGHT | MCTL_KEEPSIZE);
 m_Anchor.AddCtl(IDB_CURPOS, MCTL_RIGHT | MCTL_KEEPSIZE);
 m_Anchor.AddCtl(IDE_CURPOS, MCTL_RIGHT | MCTL_KEEPSIZE);
 m_Anchor.AddCtl(IDB_MAIN_VIEW, MCTL_RIGHT | MCTL_KEEPSIZE);
 m_Anchor.AddCtl(IDC_SCROLL1, MCTL_RIGHT | MCTL_KEEPHEIGHT);
 m_nHeight = -1;
 m_BkBrush.CreateSolidBrush(RGB(128, 128, 128));
 m_BkPen.CreatePen(PS_SOLID, 1, RGB(192, 192, 192));
 m_nMainForm_SetButtonID = 0;
 m_pMainForm_CurFrameNumberEdit = NULL;
 m_hMainForm_ScrollBarWnd = 0;
 m_hMainForm_PlaySpeedComboWnd = 0;
 m_nMainForm_PlayComboID = 0;
 m_bReflectToMainWnd = true;
 m_bMoving = false;
}
CBarViewDlg::~CBarViewDlg()
{
 m_BkBrush.DeleteObject();
}
BOOL CBarViewDlg::OnInitDialog()
{
 CRect Rect;
 CDialog::OnInitDialog();
 m_Anchor.SetupAnchoredControls(this);
 GetWindowRect(Rect);
 m_nHeight = Rect.Height();
 SetWindowPos(&wndTopMost, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE);
 m_CurFrameNumberEdit.SubclassDlgItem(IDE_CURPOS, this);
 m_Scroll.SubclassDlgItem(IDC_SCROLL1, this);
 m_PlaySpeedCombo.SubclassDlgItem(IDO_PLAYSPEED, this);
 return(TRUE);
}
void CBarViewDlg::OnGetMinMaxInfo(MINMAXINFO *p_lpMMI)
{
 CRect Rect;
 BOOL bRet;
 if(m_nHeight != -1) {
   bRet = SystemParametersInfo(SPI_GETWORKAREA, 0, Rect, 0);
   p_lpMMI->ptMinTrackSize.y = p_lpMMI->ptMaxTrackSize.y = m_nHeight;
   p_lpMMI->ptMaxSize.x = Rect.Width();
   p_lpMMI->ptMaxSize.y = m_nHeight;
   p_lpMMI->ptMaxPosition.x = 0;
   p_lpMMI->ptMaxPosition.y = Rect.bottom - m_nHeight;
   }
 CDialog::OnGetMinMaxInfo(p_lpMMI);
}
void CBarViewDlg::OnCancel()
{
 GetParent()->ShowWindow(SW_SHOW);
 CDialog::OnCancel();
}
void CBarViewDlg::OnOK()
{
}
void CBarViewDlg::OnLButtonDown(UINT p_nFlags, CPoint p_Point)
{
 m_bMoving = true;
 m_MovePoint.x = p_Point.x;
 m_MovePoint.y = p_Point.y;
 SetCapture();
 CDialog::OnLButtonDown(p_nFlags, p_Point);
}
void CBarViewDlg::OnLButtonUp(UINT p_nFlags, CPoint p_Point)
{
 if(m_bMoving) {
   ReleaseCapture();
   m_bMoving = false;
   }
 CDialog::OnLButtonUp(p_nFlags, p_Point);
}
void CBarViewDlg::OnMouseMove(UINT p_nFlags, CPoint p_Point)
{
 CRect Rect;
 if(m_bMoving) {
   GetWindowRect(Rect);
   MoveWindow(Rect.left + p_Point.x - m_MovePoint.x, Rect.top + p_Point.y - m_MovePoint.y, Rect.Width(), Rect.Height());
   }
 CDialog::OnMouseMove(p_nFlags, p_Point);
}
void CBarViewDlg::OnLButtonDblClk(UINT nFlags, CPoint point)
{
 WINDOWPLACEMENT WP;
 GetWindowPlacement(&WP);
 ShowWindow(WP.showCmd == SW_MAXIMIZE ? SW_RESTORE : SW_MAXIMIZE);
 CDialog::OnLButtonDblClk(nFlags, point);
}
BOOL CBarViewDlg::OnEraseBkgnd(CDC *p_pDC)
{
 CRect Rect;
 CPoint Point;
 GetClientRect(Rect);
 p_pDC->SelectStockObject(NULL_PEN);
 p_pDC->SelectObject(&m_BkBrush);
 Rect.right ++;
 Rect.bottom ++;
 Point.x = Point.y = 15;
 p_pDC->Rectangle(Rect);
 Rect.right --;
 Rect.bottom --;
 p_pDC->SelectObject(&m_BkPen);
 p_pDC->SelectStockObject(NULL_BRUSH);
 Rect.DeflateRect(5, 5);
 Point.x = Point.y = 15;
 p_pDC->RoundRect(Rect, Point);
 return(TRUE);
}
void CBarViewDlg::OnSize(UINT nType, int cx, int cy)
{
 BOOL WindowVisible;
 CDialog::OnSize(nType, cx, cy);
 WindowVisible = IsWindowVisible();
 if(WindowVisible)
   SetRedraw(FALSE);
 m_Anchor.CorrectAnchoredControls();
 if(WindowVisible) {
   SetRedraw(TRUE);
   Invalidate();
   UpdateWindow(); }
}
void CBarViewDlg::OnSetButtonClicked()
{
 CWnd *pWnd = NULL;
 if(!m_nMainForm_SetButtonID) return;
 pWnd = GetParent()->GetDlgItem(m_nMainForm_SetButtonID);
 if(!pWnd) return;
 GetParent()->PostMessage(WM_COMMAND, MAKEWPARAM(m_nMainForm_SetButtonID, BN_CLICKED), (LPARAM)pWnd->m_hWnd);
}
void CBarViewDlg::OnPlayButtonClicked()
{
 CWnd *pWnd = NULL;
 if(!m_nMainForm_PlayButtonID) return;
 pWnd = GetParent()->GetDlgItem(m_nMainForm_PlayButtonID);
 if(!pWnd) return;
 GetParent()->PostMessage(WM_COMMAND, MAKEWPARAM(m_nMainForm_PlayButtonID, BN_CLICKED), (LPARAM)pWnd->m_hWnd);
}
void CBarViewDlg::OnCurFrameNumberChanged()
{
 char tstr[32];
 if(!m_bReflectToMainWnd || !m_pMainForm_CurFrameNumberEdit) return;
 GetDlgItemText(IDE_CURPOS, tstr, sizeof(tstr) - 1);
 if(m_pbReflectToBarWnd) *m_pbReflectToBarWnd = false;
 m_pMainForm_CurFrameNumberEdit->SetWindowText(tstr);
 if(m_pbReflectToBarWnd) *m_pbReflectToBarWnd = true;
}
afx_msg void CBarViewDlg::OnHScroll(UINT p_nSBCode, UINT p_nPos, CScrollBar *p_pScrollBar)
{
 GetParent()->PostMessage(WM_HSCROLL, MAKELONG(p_nSBCode, p_nPos), (LPARAM)m_hMainForm_ScrollBarWnd);
 CDialog::OnHScroll(p_nSBCode, p_nPos, p_pScrollBar);
}
HBRUSH CBarViewDlg::OnCtlColor(CDC *p_pDC, CWnd *p_pWnd, UINT p_nCtlColor)
{
 HBRUSH hBrush = CDialog::OnCtlColor(p_pDC, p_pWnd, p_nCtlColor);
 if(p_nCtlColor == CTLCOLOR_STATIC) {
   p_pDC->SetBkMode(TRANSPARENT);
   hBrush = (HBRUSH)m_BkBrush.m_hObject;
   }
 return(hBrush);
}
void CBarViewDlg::OnSelChangePlaySpeed()
{
 CWnd *pWnd = NULL;
 if(!m_hMainForm_PlaySpeedComboWnd || !m_nMainForm_PlayComboID) return;
 ::SendMessage(m_hMainForm_PlaySpeedComboWnd, CB_SETCURSEL, m_PlaySpeedCombo.GetCurSel(), 0);
 pWnd = GetParent()->GetDlgItem(m_nMainForm_PlayComboID);
 if(!pWnd) return;
 if(m_pbReflectToBarWnd) *m_pbReflectToBarWnd = false;
 GetParent()->SendMessage(WM_COMMAND, MAKEWPARAM(m_nMainForm_PlayComboID, CBN_SELCHANGE), (LPARAM)pWnd->m_hWnd);
 if(m_pbReflectToBarWnd) *m_pbReflectToBarWnd = true;
}
void CBarViewDlg::OnMainViewButtonClicked()
{
 OnCancel();
}
