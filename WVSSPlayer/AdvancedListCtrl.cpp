#include "stdafx.h"
#include "AdvancedListCtrl.h"
#pragma warning(disable : 4996)
IMPLEMENT_DYNCREATE(CAdvancedListCtrl, CListCtrl)
BEGIN_MESSAGE_MAP(CAdvancedListCtrl, CListCtrl)
END_MESSAGE_MAP()
CAdvancedListCtrl::CAdvancedListCtrl()
{
 m_OtherBrush.CreateSolidBrush(RGB(255, 0, 0));
 m_RegBrush.CreateSolidBrush(RGB(255, 255, 255));
 m_SelBrush.CreateSolidBrush(RGB(0, 0, 64));
}
CAdvancedListCtrl::~CAdvancedListCtrl()
{
 m_OtherBrush.DeleteObject();
 m_RegBrush.DeleteObject();
 m_SelBrush.DeleteObject();
 m_HeaderExFont.DeleteObject();
}
void CAdvancedListCtrl::InitAdvanced()
{
 char tstr[128], *pWork;
 HDITEM hdItem;
 CHeaderCtrl *pHeader;
 short i;
 CDC *pDC;
 short nCurRows, nMaxRows = 1;
 unsigned short usVal;
 BOOL bVal;
 LOGFONT LogFont;
 TEXTMETRIC TM;
 int ColumnWidth[MAXCOLUMNS];
 CFont *pOldFont;
 if(!(pHeader = GetHeaderCtrl())) return;
 VERIFY(m_HeaderEx.SubclassWindow(pHeader->m_hWnd));	
 pDC = m_HeaderEx.GetDC();
 pOldFont = (CFont *)pDC->SelectStockObject(DEFAULT_GUI_FONT);
 m_HeaderEx.GetFont()->GetLogFont(&LogFont);
 pDC->GetTextMetrics(&TM);
 hdItem.mask = HDI_TEXT;
 hdItem.pszText = tstr;
 hdItem.cchTextMax = sizeof(tstr);
 for(i = 0; i < m_HeaderEx.GetItemCount(); i++) {
	  bVal = m_HeaderEx.GetItem(i, &hdItem);
	  nCurRows = 0;
	  pWork = tstr;
	  ColumnWidth[i] = 0;
	  pWork = strtok(tstr, "\n");
	  while(pWork) {
	    usVal = (unsigned short)pDC->GetTextExtent(pWork, strlen(pWork)).cx;
	    if(usVal > ColumnWidth[i])
	      ColumnWidth[i] = usVal;
	    nCurRows ++;
	    pWork = strtok(NULL, "\n");
	    }
	  if(nCurRows > nMaxRows) nMaxRows = nCurRows;
	  }
 pDC->SelectObject(pOldFont);
 m_HeaderEx.ReleaseDC(pDC);
 LogFont.lfHeight *= nMaxRows;
 bVal = m_HeaderExFont.CreateFontIndirect(&LogFont);
	m_HeaderEx.SetFont(&m_HeaderExFont);
 hdItem.mask = HDI_FORMAT | HDI_WIDTH;
 for(i = 0; i < m_HeaderEx.GetItemCount(); i++) {
	  bVal = m_HeaderEx.GetItem(i, &hdItem);
	  hdItem.fmt |= HDF_OWNERDRAW;
	  
	  hdItem.cxy += 1; 
	  m_HeaderEx.SetItem(i, &hdItem);
	  }
 m_HeaderEx.SetOffsets(GetSystemMetrics(SM_CXBORDER) + 1, 1);
}
void CAdvancedListCtrl::DrawItem(LPDRAWITEMSTRUCT p_lpDrawItemStruct)
{
 char tstr[128];
 CRect Rect;
 short i, Left, CellOffset = 3;
 int ScrollPos, nColCount;
 CBrush *pBrush;
 bool bSelected = false;
 COLORREF OldTextColor;
 DWORD ColAlignments[MAXCOLUMNS];
 CHeaderCtrl *pHeaderCtrl;
 LVCOLUMN ColumnData;
 ScrollPos = GetScrollPos(SB_HORZ);
 pBrush = &m_RegBrush;
 if(GetItemData(p_lpDrawItemStruct->itemID))
   pBrush = &m_OtherBrush;
 if(p_lpDrawItemStruct->itemState & ODS_SELECTED) {
   pBrush = &m_SelBrush;
   bSelected = true; }
 pHeaderCtrl = GetHeaderCtrl();
 nColCount = pHeaderCtrl->GetItemCount();
 ::FillRect(p_lpDrawItemStruct->hDC, &p_lpDrawItemStruct->rcItem, (HBRUSH)pBrush->m_hObject);
 memset(&ColumnData, 0, sizeof(ColumnData));
 for(i = 0; i < nColCount; i ++) {
   ColumnData.mask = LVCF_FMT;
   GetColumn(i, &ColumnData);
   if(ColumnData.fmt & LVCFMT_CENTER) ColAlignments[i] = DT_CENTER;
   else if(ColumnData.fmt & LVCFMT_RIGHT) ColAlignments[i] = DT_RIGHT;
   else ColAlignments[i] = DT_LEFT;
   }
 Left = -ScrollPos;
 for(i = 0; i < nColCount; i ++) {
   GetItemText(p_lpDrawItemStruct->itemID, i, tstr, sizeof(tstr));
   Rect = p_lpDrawItemStruct->rcItem;
   Rect.left = Left + 1 - i + CellOffset;
   Rect.right = Rect.left + GetColumnWidth(i) - 1 - 2 * CellOffset;
   if(bSelected) {
     OldTextColor = ::GetTextColor(p_lpDrawItemStruct->hDC);
     ::SetTextColor(p_lpDrawItemStruct->hDC, RGB(255, 255, 255));
     }
   ::DrawText(p_lpDrawItemStruct->hDC, tstr, strlen(tstr), &Rect, ColAlignments[i]);
   if(bSelected)
     ::SetTextColor(p_lpDrawItemStruct->hDC, OldTextColor);
   Left += GetColumnWidth(i) + 1; }
}
