#include "stdafx.h"
#include "HeaderCtrlExt.h"
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
BEGIN_MESSAGE_MAP(CHeaderCtrlEx, CHeaderCtrl)
END_MESSAGE_MAP()
CHeaderCtrlEx::CHeaderCtrlEx()
{
 m_nHorizontalOffset = 0;
 m_nVerticalOffset = 0;
}
CHeaderCtrlEx::~CHeaderCtrlEx()
{
}
void CHeaderCtrlEx::DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct)
{
 HDITEM hdi;
 TCHAR tstr[256];
 UINT uFormat;
 BOOL bVal;
 ASSERT(lpDrawItemStruct->CtlType == ODT_HEADER);
 hdi.mask = HDI_TEXT | HDI_FORMAT | HDI_WIDTH;
 hdi.pszText = tstr;
 hdi.cchTextMax = sizeof(tstr);
 bVal = GetItem(lpDrawItemStruct->itemID, &hdi);
	CDC::FromHandle(lpDrawItemStruct->hDC)->SelectStockObject(DEFAULT_GUI_FONT);
 lpDrawItemStruct->rcItem.top += m_nVerticalOffset;
	if(hdi.fmt & HDF_CENTER) uFormat = DT_CENTER;
	else if(hdi.fmt & HDF_RIGHT) uFormat = DT_RIGHT;
 else uFormat = DT_LEFT;
 ::DrawFrameControl(lpDrawItemStruct->hDC, &lpDrawItemStruct->rcItem, DFC_BUTTON, DFCS_BUTTONPUSH);
	switch(uFormat) {
	  case DT_LEFT: lpDrawItemStruct->rcItem.left += m_nHorizontalOffset; break;
	  case DT_RIGHT: lpDrawItemStruct->rcItem.right -= 2 * m_nHorizontalOffset; break; }
 ::DrawText(lpDrawItemStruct->hDC, tstr, strlen(tstr), &lpDrawItemStruct->rcItem, uFormat);
 CDC::FromHandle(lpDrawItemStruct->hDC)->SelectStockObject(SYSTEM_FONT);
}
