#pragma once
#include "resource.h"
#include "AdvancedListCtrl.h"
#include <vector>
#include "WindServer2.h"
namespace WVSS {
class CPageWeather : public CPropertyPage
{
private:
 CAdvancedListCtrl m_List;
public:
	enum { IDD = IDD_PAGE_WEATHER };
private:
 static int CALLBACK ListDataCompareFunc(LPARAM p_lParam1, LPARAM p_lParam2, LPARAM p_lColIndex);
protected:
 virtual BOOL OnInitDialog();
public:
	CPageWeather(CWnd* pParent);
	~CPageWeather();
 void UpdateResults(const std::vector<CWindPoint3D> *p_pWindPoints);
protected:
DECLARE_MESSAGE_MAP()
};
} 
