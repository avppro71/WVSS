#pragma once
#include "IWVSSSeverity.h"
#include "resource.h"
namespace WVSS {
class CPageInfo;
class CMainDialog;
class CPageOverride : public CPropertyPage
{
	DECLARE_DYNAMIC(CPageOverride)
private:
 CProgressCtrl m_Progress;
 CSpinButtonCtrl m_dZSpin, m_dHSpin;
 CEdit m_dZEdit, m_dHEdit, m_dAngleEdit, m_dGammaEdit;
 double m_dBatchStep; 
protected:
public:
 enum { IDD = IDD_PAGE_OVERRIDE };
 CPageInfo *m_pInfoPage;
 CMainDialog *m_pMainDialog;
public:
 class COverrideData {
   public:
    double m_ddZ, m_ddH, m_ddAngle, m_dGamma;
    COverrideData () { Reset(); }
    void Reset();
   };
private:
protected:
public:
	CPageOverride(CWnd* pParent = NULL);   
	virtual ~CPageOverride();
 void FillFlagsFromControls(CWEAIF *p_pFlags);
 void SetBatchStep(double p_bBatchStep) { m_dBatchStep = p_bBatchStep; }
 void UpdateOverrideData(COverrideData *p_pData);
protected:
	virtual BOOL OnInitDialog();
	DECLARE_MESSAGE_MAP()
 afx_msg void OnOverrideGammaClicked();
 afx_msg void OnOverridePositionClicked();
 afx_msg void OnVScroll(UINT p_nSBCode, UINT p_nPos, CScrollBar *p_pScrollBar);
 afx_msg void OnBatchButtonClicked();
};
} 
