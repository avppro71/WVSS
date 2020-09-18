#include "stdafx.h"
#include "PageWeather.h"
#pragma warning(disable : 4996)
using namespace WVSS;
BEGIN_MESSAGE_MAP(CPageWeather, CDialog)
END_MESSAGE_MAP()
CPageWeather::CPageWeather(CWnd * ) : CPropertyPage(CPageWeather::IDD)
{
}
CPageWeather::~CPageWeather()
{
}
BOOL CPageWeather::OnInitDialog()
{
 CPropertyPage::OnInitDialog();
 m_List.SubclassDlgItem(IDC_WEATHER_LIST, this);
 m_List.SetExtendedStyle(m_List.GetExtendedStyle() | LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);
 m_List.InsertColumn(0, "Station Code", LVCFMT_LEFT, 70);
 m_List.InsertColumn(1, "DateTime (UTC)", LVCFMT_LEFT, 130);
 m_List.InsertColumn(2, "Surface Elevation\n(m)", LVCFMT_RIGHT, 90);
 m_List.InsertColumn(3, "Measurement Altitude\nabove Surface (m)", LVCFMT_RIGHT, 120);
 m_List.InsertColumn(4, "Wind Direction\n(deg)", LVCFMT_RIGHT, 80);
 m_List.InsertColumn(5, "Wind Speed\n(kts)", LVCFMT_RIGHT, 70);
 m_List.InitAdvanced();
 return(TRUE);
}
void CPageWeather::UpdateResults(const std::vector <CWindPoint3D> *p_pWindPoints)
{
 char tstr[128];
 unsigned short RowIndex;
 std::vector <CWindPoint3D>::const_iterator cIt;
 double dVal;
 unsigned long lVal;
 m_List.SetRedraw(FALSE);
 m_List.DeleteAllItems();
 RowIndex = 0;
 for(cIt = p_pWindPoints->begin(); cIt != p_pWindPoints->end(); ++cIt) {
   RowIndex = m_List.InsertItem(LVIF_TEXT | LVIF_STATE | LVIF_PARAM, RowIndex, (*cIt).m_LocationText, 0, 0, 0, 0);
   sprintf(tstr, "%.4d-%.2d-%.2d %.2d:%.2d:%.2d", (*cIt).m_MeasurementDT.wYear, (*cIt).m_MeasurementDT.wMonth, (*cIt).m_MeasurementDT.wDay, (*cIt).m_MeasurementDT.wHour, (*cIt).m_MeasurementDT.wMinute, (*cIt).m_MeasurementDT.wSecond);
   m_List.SetItemText(RowIndex, 1, tstr);
   lVal = (unsigned long)(100000000 * ((*cIt).m_MeasurementDT.wYear % 100)) + 
          (unsigned long)(1000000 * (*cIt).m_MeasurementDT.wMonth) + 
          (unsigned long)(10000 * (*cIt).m_MeasurementDT.wDay) + 
          (unsigned long)(100 * (*cIt).m_MeasurementDT.wHour) + 
          (unsigned long)(1 * (*cIt).m_MeasurementDT.wMinute);
   m_List.SetItemData(RowIndex, lVal);
   sprintf(tstr, "%.0lf", (*cIt).m_dSurfaceElevation);
   m_List.SetItemText(RowIndex, 2, tstr);
   sprintf(tstr, "%.0lf", (*cIt).m_dAltitude);
   m_List.SetItemText(RowIndex, 3, tstr);
   dVal = (*cIt).m_dWindDirectionDegTo - 180;
   if(dVal < -180) dVal += 360;
   sprintf(tstr, "%.0lf", dVal);
   m_List.SetItemText(RowIndex, 4, tstr);
   sprintf(tstr, "%.1lf", (*cIt).m_dWindSpeedMS * 3.6 / 1.852); 
   m_List.SetItemText(RowIndex, 5, tstr);
   RowIndex ++; }
 m_List.SortItems(ListDataCompareFunc, 0);
 for(RowIndex = 0; RowIndex < m_List.GetItemCount(); RowIndex ++)
   m_List.SetItemData(RowIndex, NULL); 
 m_List.SetRedraw();
 m_List.UpdateWindow();
}
int CALLBACK CPageWeather::ListDataCompareFunc(LPARAM p_lParam1, LPARAM p_lParam2, LPARAM p_lColIndex)
{
 if((unsigned long)p_lParam1 < (unsigned long)p_lParam2) return(-1);
 else if((unsigned long)p_lParam1 > (unsigned long)p_lParam2) return(1);
 return(0);
}
