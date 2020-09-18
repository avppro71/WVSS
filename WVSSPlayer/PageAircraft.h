#pragma once
#include "resource.h"
#include "AdvancedListCtrl.h"
#include <vector>
namespace WVSS {
class CADAA;
class CPageAircraft : public CPropertyPage
{
private:
 CAdvancedListCtrl m_List;
public:
	enum { IDD = IDD_PAGE_AIRCRAFT };
private:
protected:
 virtual BOOL OnInitDialog();
public:
	CPageAircraft(CWnd* pParent);
	~CPageAircraft();
 void UpdateResults(std::vector <CADAA *>*p_pAircraft);
protected:
DECLARE_MESSAGE_MAP()
};
} 
