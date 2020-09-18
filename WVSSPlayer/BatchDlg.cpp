#include "stdafx.h"
#include "SITARWVSSPlayer.h"
#include "BatchDlg.h"
IMPLEMENT_DYNAMIC(CBatchDlg, CDialog)
BEGIN_MESSAGE_MAP(CBatchDlg, CDialog)
END_MESSAGE_MAP()
#pragma warning(disable : 4996)
CBatchDlg::CBatchDlg(CWnd* pParent )	: CDialog(CBatchDlg::IDD, pParent)
{
 m_Mode = OVERRIDE;
 m_dStart_dZ = m_dEnd_dZ = m_dStart_dH = m_dEnd_dH = m_dStart_dAngle = m_dEnd_dAngle = 0;
 m_lStartFrame = m_lEndFrame = 0;
}
CBatchDlg::~CBatchDlg()
{
}
BOOL CBatchDlg::OnInitDialog()
{
 char tstr[128];
 CheckDlgButton(IDR_BY_OVERRIDE, m_Mode == OVERRIDE);
 CheckDlgButton(IDR_BY_FRAME, m_Mode == FRAME);
 sprintf(tstr, "%.0lf", m_dStart_dZ);
 SetDlgItemText(IDE_START_DZ, tstr);
 sprintf(tstr, "%.0lf", m_dEnd_dZ);
 SetDlgItemText(IDE_END_DZ, tstr); 
 sprintf(tstr, "%.0lf", m_dStart_dH);
 SetDlgItemText(IDE_START_DH, tstr);
 sprintf(tstr, "%.0lf", m_dEnd_dH);
 SetDlgItemText(IDE_END_DH, tstr);
 sprintf(tstr, "%.0lf", m_dStart_dAngle);
 sprintf(tstr, "%.0lf", m_dEnd_dAngle);
 sprintf(tstr, "%ld", m_lStartFrame);
 SetDlgItemText(IDE_START_FRAME, tstr);
 sprintf(tstr, "%ld", m_lEndFrame);
 SetDlgItemText(IDE_END_FRAME, tstr);
 return(TRUE);
}
void CBatchDlg::OnOK()
{
 char tstr[128];
 m_Mode = IsDlgButtonChecked(IDR_BY_OVERRIDE) ? OVERRIDE : FRAME;
 GetDlgItemText(IDE_START_DZ, tstr, sizeof(tstr));
 m_dStart_dZ = atof(tstr);
 GetDlgItemText(IDE_END_DZ, tstr, sizeof(tstr)); 
 m_dEnd_dZ = atof(tstr);
 GetDlgItemText(IDE_START_DH, tstr, sizeof(tstr));
 m_dStart_dH = atof(tstr);
 GetDlgItemText(IDE_END_DH, tstr, sizeof(tstr));
 m_dEnd_dH = atof(tstr);
 GetDlgItemText(IDE_START_FRAME, tstr, sizeof(tstr));
 m_lStartFrame = atoi(tstr);
 GetDlgItemText(IDE_END_FRAME, tstr, sizeof(tstr));
 m_lEndFrame = atoi(tstr);
 EndDialog(IDOK);
}
