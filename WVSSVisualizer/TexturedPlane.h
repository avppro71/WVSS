#pragma once

#include <QTOpenGL>
#define GEOGRAPHICLIB_SHARED_LIB 1
#include "GeographicLib/Math.hpp"

namespace GeographicLib {
class LocalCartesian;
class Geodesic;
}

class CTexturedPlane
{
public:
 struct rData { // used for serialization
   char m_FileName[256];
   float m_fSizeLat, m_fSizeLong; // the size of the object this texture is laid upon.
   float m_fSizeXM, m_fSizeZM;
   double m_dRefPointLat, m_dRefPointLong;
   float m_fDX, m_fDZ, m_fElevation;
   };

private:
protected:
 bool m_bAreaCreated;
 bool m_bLoaded;
 bool m_bGeographic;
 QGLContext *m_pContext;
 GLuint m_nTextureID;
 float m_fDX, m_fDZ, m_fElevation;
 //GLint m_nListID;
 GLint m_nListID2;
 GLint m_nOutlineListID;
 char m_FileName[256];
 float m_fSizeLat, m_fSizeLong, m_fSizeXM, m_fSizeZM; // the size of the object this texture is laid upon.
 int m_nPlanarXOffset, m_nPlanarZOffset; // where X is North, Z is East, Y is up. In meters.
 double m_dAreaRefPointLat, m_dAreaRefPointLong;
 double m_dRefPointLat, m_dRefPointLong;
 double m_dSurface0Lat, m_dSurface0Long;
 int m_nTextureWidth, m_nTextureHeight;
 GeographicLib::Math::real m_AreaCenterOffsetX, m_AreaCenterOffsetY, m_AreaCenterOffsetZ;
 GeographicLib::Math::real m_Surface0OffsetX, m_Surface0OffsetY, m_Surface0OffsetZ;

public:
 bool m_bVisible;
 bool m_bSelected;
 //bool m_bAlternateSurface;
 float m_fSurfaceRotateX, m_fSurfaceRotateZ; // angles of rotation needed to position the vertical axis up over the reference point of the surface
 //float m_fAreaRotateX, m_fAreaRotateZ; // additional angles of rotation from the area center to the reference point of the surface
 static GeographicLib::Geodesic *m_pGeo;
 static GeographicLib::LocalCartesian *m_pLC;
 static QColor m_EarthColor;
 static bool m_bDrawAxes;
 static double m_dGeographicTileStep;

private:
protected:
 //void MakeAreaTriangles();
 //void MakeAreaTriangles2();
 void MakeAreaTriangles3();
 void MakeAreaTriangles3Geographic();
 //void MakeAreaTriangles2(int p_nSizeX, int p_nSizeZ, int p_nZ0, int p_nZ0);
 //void MakeAreaTriangles3TexturesTest();

public:
 CTexturedPlane();
 void Set(rData *p_pData);
 ~CTexturedPlane();
 bool Load(QGLContext *p_pContext, double p_dAreaRefPointLat, double p_dAreaRefPointLong);
 bool Unload();
 void SetOffsetNumbersFromOldVis(float p_fDX, float p_fDZ, float p_fElevation);
 void GetOffsetNumbersFromOldVis(float *p_pfDX, float *p_pfDZ, float *p_pfElevation);
 void Draw();
 void Recalc(double p_dAreaRefPointLat, double p_dAreaRefPointLong);
 bool IsGeographic() { return(m_bGeographic); }
 float GetElevation() { return(m_fElevation); }
};
