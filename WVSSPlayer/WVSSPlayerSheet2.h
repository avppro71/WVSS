#pragma once
#include "resource.h"
#include "PageAircraft.h"
#include "PageSeverityResults.h"
#include "PageOverride.h"
#include "PageDisplay.h"
#include "PageWakeOptions.h"
#include "PageLoad.h"
#include "PageInfo.h"
#include "PageFrame.h"
#include "PageComms.h"
#include "PageSensitivity.h"
#include "PageWeather.h"
#include "PageWakeFrame.h"
namespace WVSS {
class CWVSSTabCtrl : public CTabCtrl
{
private:
protected:
public:
private:
protected:
public:
 CWVSSTabCtrl();
 ~CWVSSTabCtrl();
};
class CWVSSPlayerSheet2 : public CPropertySheet
{
private:
 DECLARE_DYNAMIC(CWVSSPlayerSheet2)
 CWVSSTabCtrl m_TabCtrl;
protected:
public:
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
protected:
public:
 CWVSSPlayerSheet2(UINT nIDCaption, CWnd* pParentWnd = NULL, UINT iSelectPage = 0);
 CWVSSPlayerSheet2(LPCTSTR pszCaption, CWnd* pParentWnd = NULL, UINT iSelectPage = 0);
 virtual ~CWVSSPlayerSheet2();
 virtual BOOL OnInitDialog();
protected:
 DECLARE_MESSAGE_MAP()
public:
};
} 
