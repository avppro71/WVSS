#include "stdafx.h"
#include "SITARWVSSPLayer.h"
#include "PageComms.h"
#pragma warning(disable : 4996)
using namespace WVSS;
BEGIN_MESSAGE_MAP(CPageComms, CDialog)
 ON_BN_CLICKED(IDB_CLEAR_HISTORY, &CPageComms::OnClearHistoryButtonClicked)
END_MESSAGE_MAP()
CPageComms::CPageComms(CWnd * ) : CPropertyPage(CPageComms::IDD)
{
}
CPageComms::~CPageComms()
{
}
BOOL CPageComms::OnInitDialog()
{
 CPropertyPage::OnInitDialog();
 m_InfoList.SubclassDlgItem(IDL_INFO, this);
 return(TRUE);
}
void CPageComms::OnClearHistoryButtonClicked()
{
 m_InfoList.ResetContent();
}
void CPageComms::AddToMessageList(const char *p_pFormat, ...)
{
 char tstr[256];
 va_list ArgList;
 if(!p_pFormat) return;
 va_start(ArgList, p_pFormat);
 vsprintf_s(tstr, sizeof(tstr), p_pFormat, ArgList);
 va_end(ArgList);
 m_InfoList.InsertString(-1, tstr);
 m_InfoList.SetTopIndex(m_InfoList.GetCount() - 1);
}
