#pragma once
#include "HeaderCtrlExt.h"
class CAdvancedListCtrl : public CListCtrl
{
private:
 enum eFlags { MAXCOLUMNS = 128 };
 CBrush m_OtherBrush, m_RegBrush, m_SelBrush;
 CHeaderCtrlEx m_HeaderEx;
 CFont m_HeaderExFont;
protected:
public:
private:
protected:
 DECLARE_DYNCREATE(CAdvancedListCtrl)
public:
 CAdvancedListCtrl();
 ~CAdvancedListCtrl(); 
 void InitAdvanced();
 virtual void DrawItem(LPDRAWITEMSTRUCT p_lpDrawItemStruct);
protected:
	DECLARE_MESSAGE_MAP()
};
