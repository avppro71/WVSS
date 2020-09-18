#pragma once
#include "AnCtl.h"
namespace WVSS {
class CBarViewDlg : public CDialog
{
	DECLARE_DYNAMIC(CBarViewDlg)
private:
 short m_nHeight;
 CBrush m_BkBrush;
 CPen m_BkPen;
 CAnchorCtl m_Anchor;
protected:
public:
 enum { IDD = IDD_BARVIEW };
 UINT m_nMainForm_SetButtonID;
 UINT m_nMainForm_PlayButtonID;
 UINT m_nMainForm_PlayComboID;
 CEdit *m_pMainForm_CurFrameNumberEdit, m_CurFrameNumberEdit;
 CComboBox m_PlaySpeedCombo;
 HWND m_hMainForm_PlaySpeedComboWnd;
 bool *m_pbReflectToBarWnd; 
 bool m_bReflectToMainWnd; 
 CScrollBar m_Scroll;
 HWND m_hMainForm_ScrollBarWnd;
 bool m_bMoving;
 CPoint m_MovePoint;
private:
protected:
 virtual BOOL OnInitDialog();
 virtual void OnCancel();
 virtual void OnOK();
public:
	CBarViewDlg(CWnd *p_pParent = NULL);   
	virtual ~CBarViewDlg();
protected:
	DECLARE_MESSAGE_MAP()
public:
 afx_msg void OnGetMinMaxInfo(MINMAXINFO* lpMMI);
 afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
 afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
 afx_msg void OnMouseMove(UINT nFlags, CPoint point);
 afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
 afx_msg BOOL OnEraseBkgnd(CDC* pDC);
 afx_msg void OnSize(UINT nType, int cx, int cy);
 afx_msg void OnSetButtonClicked();
 afx_msg void OnPlayButtonClicked();
 afx_msg void OnCurFrameNumberChanged();
 afx_msg void OnHScroll(UINT p_nSBCode, UINT p_nPos, CScrollBar *p_pScrollBar);
 afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
 afx_msg void OnSelChangePlaySpeed();
 afx_msg void OnMainViewButtonClicked();
};
} 
