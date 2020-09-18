#pragma once
#include "resource.h"
#include "IWVSSSeverity.h"
#pragma warning(disable : 4996)
namespace WVSS {
class CMainDialog;
class CPageLoad : public CPropertyPage
{
private:
 CWEAIF m_FSG;
 CEdit m_WindDirectionEdit, m_WindSpeedEdit;
 CComboBox m_TSCombo, m_FileTypeCombo;
 char m_CurFileName[MAX_PATH];
public:
	enum { IDD = IDD_PAGE_LOAD };
 CMainDialog *m_pMainDialog;
private:
protected:
 virtual BOOL OnInitDialog();
public:
	CPageLoad(CWnd* pParent);
	~CPageLoad();
 void SetFlags(CWEAIF *p_pFlags) { m_FSG = *p_pFlags; }
 void FillFlagsFromControls(CWEAIF *p_pFlags);
 void SetLoadedFileText(const char *p_pStr) { SetDlgItemText(IDS_FILE, p_pStr); }
 void SetFileName(const char *p_pFileName) { strcpy(m_CurFileName, p_pFileName); }
 DWORD GetBreakIntervalFromControl();
 IWVSSSeverity::eFileType GetFileType() { return((IWVSSSeverity::eFileType)m_FileTypeCombo.GetItemData(m_FileTypeCombo.GetCurSel())); }
 void SetInterpolationControlFromVal(short p_nVal);
 void SetFileTypeControlFromVal(short p_nVal);
protected:
DECLARE_MESSAGE_MAP()
 afx_msg void OnReloadButtonClicked();
 afx_msg void OnLoadBtnClicked();
};
} 
