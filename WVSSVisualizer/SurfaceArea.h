#pragma once

#include <QTOpenGL>

namespace GeographicLib {
class LocalCartesian;
}

class CTexturedPlane;
class CSurfaceArea
{
public:
 struct rData {
   char Name[32];
   double m_dCenterLat, m_dCenterLong;
   };

private:
protected:
 std::vector<CTexturedPlane *>m_TexturedPlanes;
 rData m_Data;

public:
 bool m_bOn;
 static GeographicLib::LocalCartesian *m_pLC;
 static short m_nBlankEarthTiles; // to fill more space on the screen, to make it look like there is a full Earth displayed
 static double m_dGeographicTileSize;

private:
protected:
 void AddEarthTiles();
 void RemoveEarthTiles();

public:
 void Reset();
 CSurfaceArea();
 ~CSurfaceArea();
 void Set(rData *p_pData) { m_Data = *p_pData; }
 const char *GetAreaName() { return(m_Data.Name); }
 void AddTexturedPlane(CTexturedPlane *p_pTexturedPlane);
 void Recalc();
 bool Load(QGLContext *p_pContext);
 bool Unload();
 void Draw();
 size_t GetTexturedPlanesCount() { return(m_TexturedPlanes.size()); }
 CTexturedPlane *GetTexturedPlaneByIndex(size_t p_Index) { if(p_Index > m_TexturedPlanes.size() - 1) return(NULL); return(m_TexturedPlanes[p_Index]); }
 double GetAreaCenterLat() { return(m_Data.m_dCenterLat); }
 double GetAreaCenterLong() { return(m_Data.m_dCenterLong); }
 float GetMaxSurfaceElevation();
};
