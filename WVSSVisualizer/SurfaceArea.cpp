#include "SurfaceArea.h"
#include "TexturedPlane.h"
#define GEOGRAPHICLIB_SHARED_LIB 1
#include "GeographicLib/LocalCartesian.hpp"
#include "assert.h"

GeographicLib::LocalCartesian *CSurfaceArea::m_pLC;
short CSurfaceArea::m_nBlankEarthTiles = 0;
double CSurfaceArea::m_dGeographicTileSize = 1;

CSurfaceArea::CSurfaceArea()
{
 Reset();
}

CSurfaceArea::~CSurfaceArea()
{
}

void CSurfaceArea::Reset()
{
 size_t i;

 m_bOn = false;

 Unload();
 for(i = 0; i < m_TexturedPlanes.size(); i ++)
   delete m_TexturedPlanes[i];
 m_TexturedPlanes.clear();
 memset(&m_Data, 0, sizeof(m_Data));
}

void CSurfaceArea::AddTexturedPlane(CTexturedPlane *p_pTexturedPlane)
{
 m_TexturedPlanes.push_back(p_pTexturedPlane);
}

bool CSurfaceArea::Load(QGLContext *p_pContext)
{
 size_t i;
 bool bRet;

 bRet = true;
 m_pLC->Reset(m_Data.m_dCenterLat, m_Data.m_dCenterLong, 0);
 AddEarthTiles();
 //qDebug() << QString::number(m_TexturedPlanes.size()) << " textured areas";
 for(i = 0; i < m_TexturedPlanes.size(); i ++) {
   //qDebug() << "m_TexturedPlanes[i]->Load";
   if(!m_TexturedPlanes[i]->Load(p_pContext, m_Data.m_dCenterLat, m_Data.m_dCenterLong)) {
     //qDebug() << "m_TexturedPlanes[i]->Load failed";
     bRet = false; }
   }

 return(bRet);
}

bool CSurfaceArea::Unload()
{
 size_t i;
 bool bRet;

 bRet = true;
 for(i = 0; i < m_TexturedPlanes.size(); i ++) {
   if(!m_TexturedPlanes[i]->Unload())
   bRet = false; }

 return(bRet);
}

void CSurfaceArea::Draw()
{
 size_t i;

 if(!m_bOn) return;
 for(i = 0; i < m_TexturedPlanes.size(); i ++) {
   if(!m_TexturedPlanes[i]->IsGeographic()) continue;
   m_TexturedPlanes[i]->Draw(); }
 for(i = 0; i < m_TexturedPlanes.size(); i ++) {
   if(m_TexturedPlanes[i]->IsGeographic()) continue;
   m_TexturedPlanes[i]->Draw(); }
}

void CSurfaceArea::Recalc()
{
 size_t i;

 m_pLC->Reset(m_Data.m_dCenterLat, m_Data.m_dCenterLong, 0);
 for(i = 0; i < m_TexturedPlanes.size(); i ++)
   m_TexturedPlanes[i]->Recalc(m_Data.m_dCenterLat, m_Data.m_dCenterLong);
}

void CSurfaceArea::AddEarthTiles()
{
 CTexturedPlane *pTexturedPlane;
 CTexturedPlane::rData TexturedPlaneData;
 double dLat, dLong, dStartLat, dEndLat, dStartLong, dEndLong, dLatStep, dLongStep;
 short LatAdd, LongAdd;

 LatAdd = 1 + ((abs(m_Data.m_dCenterLat - (int)m_Data.m_dCenterLat) >= 0.5) ? 1 : 0);
 if(m_Data.m_dCenterLat < 0) LatAdd *= -1;
   
 LongAdd = 0 + ((abs(m_Data.m_dCenterLong - (int)m_Data.m_dCenterLong) >= 0.5) ? 1 : 0);
 if(m_Data.m_dCenterLong < 0) LongAdd *= -1;

 dLatStep = dLongStep = m_dGeographicTileSize;

 dStartLat = (int)m_Data.m_dCenterLat + LatAdd - m_nBlankEarthTiles * dLatStep;
 if(dStartLat < -90) dStartLat = -90;
 dEndLat = (int)m_Data.m_dCenterLat + LatAdd + m_nBlankEarthTiles * dLatStep;
 if(dEndLat > 90) dEndLat = 90;

 dStartLong = (int)m_Data.m_dCenterLong + LongAdd - m_nBlankEarthTiles * dLongStep;
 if(dStartLong < (int)m_Data.m_dCenterLong - 90) dStartLong = (int)m_Data.m_dCenterLong - 90;
 dEndLong = (int)m_Data.m_dCenterLong + LongAdd + m_nBlankEarthTiles * dLongStep;
 if(dEndLong > (int)m_Data.m_dCenterLong + 90) dEndLong = (int)m_Data.m_dCenterLong + 90;
 
 for(dLong = dStartLong; dLong < dEndLong; dLong += dLongStep) {
   for(dLat = dStartLat; dLat < dEndLat; dLat += dLatStep) {
     memset(&TexturedPlaneData, 0, sizeof(TexturedPlaneData));
     TexturedPlaneData.m_dRefPointLat = dLat;
     TexturedPlaneData.m_dRefPointLong = dLong;
     TexturedPlaneData.m_fSizeLat = dLatStep;
     TexturedPlaneData.m_fSizeLong = dLongStep;
     pTexturedPlane = new CTexturedPlane();
     if(!pTexturedPlane) { assert(false); return; }
     pTexturedPlane->Set(&TexturedPlaneData);
     AddTexturedPlane(pTexturedPlane);
     }
   }
}

void CSurfaceArea::RemoveEarthTiles()
{
 std::vector<CTexturedPlane *>::iterator It;

 for(It = m_TexturedPlanes.begin(); It != m_TexturedPlanes.end();) {
   if(!(*It)->IsGeographic()) {
     (*It)->Unload();
     delete (*It);
     m_TexturedPlanes.erase(It);
     }
   else ++It;
   }
}

GLfloat CSurfaceArea::GetMaxSurfaceElevation()
{
 size_t i;
 float fCurElevation, fMaxElevation = -1000;

 for(i = 0; i < m_TexturedPlanes.size(); i ++) {
   fCurElevation = m_TexturedPlanes[i]->GetElevation();
   if(fCurElevation > fMaxElevation)
       fMaxElevation = fCurElevation; }
 return(fMaxElevation == -1000 ? 0 : fMaxElevation);
}
