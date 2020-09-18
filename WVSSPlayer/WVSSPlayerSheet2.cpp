#include "stdafx.h"
#include "WVSSPlayerSheet2.h"
using namespace WVSS;
CWVSSTabCtrl::CWVSSTabCtrl()
{
}
CWVSSTabCtrl::~CWVSSTabCtrl()
{
}
IMPLEMENT_DYNAMIC(CWVSSPlayerSheet2, CPropertySheet)
BEGIN_MESSAGE_MAP(CWVSSPlayerSheet2, CPropertySheet)
END_MESSAGE_MAP()
CWVSSPlayerSheet2::CWVSSPlayerSheet2(UINT nIDCaption, CWnd* pParentWnd, UINT iSelectPage) :CPropertySheet(nIDCaption, pParentWnd, iSelectPage)
{
 ASSERT(0);
}
CWVSSPlayerSheet2::CWVSSPlayerSheet2(LPCTSTR pszCaption, CWnd* pParentWnd, UINT iSelectPage) : CPropertySheet(pszCaption, pParentWnd, iSelectPage)
{
 int i;
 for(;;) {
   m_pLoadPage = new CPageLoad(this);
   if(!m_pLoadPage) break;
   AddPage(m_pLoadPage);
   m_pInfoPage = new CPageInfo(this);
   if(!m_pInfoPage) break;
   AddPage(m_pInfoPage);
   m_pAircraftPage = new CPageAircraft(this);
   if(!m_pAircraftPage) break;
   AddPage(m_pAircraftPage);
   m_pWeatherPage = new CPageWeather(this);
   if(!m_pWeatherPage) break;
   AddPage(m_pWeatherPage);
   m_pSeverityResPage = new CPageSeverityResults(this);
   if(!m_pSeverityResPage) break;
   AddPage(m_pSeverityResPage);
   m_pFramePage = new CPageFrame(this);
   if(!m_pFramePage) break;
   AddPage(m_pFramePage);
   m_pWakeFramePage = new CPageWakeFrame(this);
   if(!m_pWakeFramePage) break;
   AddPage(m_pWakeFramePage);
   m_pDisplayPage = new CPageDisplay(this);
   if(!m_pDisplayPage) break;
   AddPage(m_pDisplayPage);
   m_pWakeOptionsPage = new CPageWakeOptions(this);
   if(!m_pWakeOptionsPage) break;
   AddPage(m_pWakeOptionsPage);
   m_pOverridePage = new CPageOverride(this);
   if(!m_pOverridePage) break;
   AddPage(m_pOverridePage);
   m_pCommsPage = new CPageComms(this);
   if(!m_pCommsPage) break;
   AddPage(m_pCommsPage);
   m_pSensitivityPage = new CPageSensitivity(this);
   if(!m_pSensitivityPage) break;
   AddPage(m_pSensitivityPage);
   m_psh.dwFlags |= PSH_NOAPPLYNOW | PSH_NOCONTEXTHELP;
   m_psh.dwFlags &= ~PSH_HASHELP;
   break; }
 for(i = 0; i < GetPageCount(); i ++)
   GetPage(i)->m_psp.dwFlags &= ~PSP_HASHELP; 
}
CWVSSPlayerSheet2::~CWVSSPlayerSheet2()
{
 if(m_pAircraftPage) delete m_pAircraftPage;
 if(m_pSeverityResPage) delete m_pSeverityResPage;
 if(m_pDisplayPage) delete m_pDisplayPage;
 if(m_pWakeOptionsPage) delete m_pWakeOptionsPage;
 if(m_pLoadPage) delete m_pLoadPage;
 if(m_pInfoPage) delete m_pInfoPage;
 if(m_pFramePage) delete m_pFramePage;
 if(m_pWakeFramePage) delete m_pWakeFramePage;
 if(m_pOverridePage) delete m_pOverridePage;
 if(m_pCommsPage) delete m_pCommsPage;
 if(m_pSensitivityPage) delete m_pSensitivityPage;
 if(m_pWeatherPage) delete m_pWeatherPage;
}
BOOL CWVSSPlayerSheet2::OnInitDialog()
{
 int i;
 CPropertySheet::OnInitDialog();
 for(i = GetPageCount() - 1; i >= 0; i--)
   SetActivePage(i);
 return(TRUE);
}
