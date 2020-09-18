#include "TexturedPLane.h"
#include <GL/glu.h>
#include "assert.h"
#define GEOGRAPHICLIB_SHARED_LIB 1
#include "GeographicLib/LocalCartesian.hpp"
#include "GeographicLib\geodesic.hpp"

#pragma warning (disable: 4996)

GeographicLib::Geodesic *CTexturedPlane::m_pGeo;
GeographicLib::LocalCartesian *CTexturedPlane::m_pLC;
QColor CTexturedPlane::m_EarthColor = QColor(255, 255, 255);
bool CTexturedPlane::m_bDrawAxes = false;
double CTexturedPlane::m_dGeographicTileStep = 1;

CTexturedPlane::CTexturedPlane()
{
 m_bAreaCreated = false;
 m_bLoaded = false;
 m_bSelected = false;
 m_bVisible = true;
 //m_bAlternateSurface = false;
 m_pContext = NULL;
 m_nTextureID = 0;
 m_fDX = m_fDZ = m_fElevation = 0;
 //m_nListID = 0;
 m_nListID2 = 0;
 m_fSurfaceRotateX = m_fSurfaceRotateZ = 0;
 //m_fAreaRotateX = m_fAreaRotateZ = 0;
 m_nOutlineListID = 0;
 *m_FileName = '\0';
 m_fSizeLat = m_fSizeLong = 0;
 m_nPlanarXOffset = m_nPlanarZOffset = 0;
 m_nTextureWidth = m_nTextureHeight = 0;
 m_AreaCenterOffsetX = m_AreaCenterOffsetY = m_AreaCenterOffsetZ = 0;
}

CTexturedPlane::~CTexturedPlane()
{
 Unload();
}

void CTexturedPlane::Set(rData *p_pData)
{
 QDir Dir;
 QString strTextureFileName;

 m_bAreaCreated = false;
 m_bLoaded = false;
 m_nTextureID = 0;
 //m_nListID = 0;
 m_nListID2 = 0;
 m_fSurfaceRotateX = m_fSurfaceRotateZ = 0;
 //m_fAreaRotateX = m_fAreaRotateZ = 0;
 m_nOutlineListID = 0;
 m_nPlanarXOffset = m_nPlanarZOffset = 0;
 m_dRefPointLat = m_dRefPointLong = 0;
 m_fSizeLat = p_pData->m_fSizeLat;
 m_fSizeLong = p_pData->m_fSizeLong;
 m_fSizeXM = p_pData->m_fSizeXM;
 m_fSizeZM = p_pData->m_fSizeZM;
 m_bGeographic = false;
 if(*p_pData->m_FileName) {
   strTextureFileName = Dir.absolutePath() + "/" + p_pData->m_FileName;
   strcpy(m_FileName, strTextureFileName.toLocal8Bit()); }
 else {
   *m_FileName = '\0';
   m_bGeographic = true;
   p_pData->m_fDX = p_pData->m_fDZ = p_pData->m_fElevation = 0;
   }
 m_dRefPointLat = p_pData->m_dRefPointLat;
 m_dRefPointLong = p_pData->m_dRefPointLong;
 SetOffsetNumbersFromOldVis(p_pData->m_fDX, p_pData->m_fDZ, p_pData->m_fElevation);
}

bool CTexturedPlane::Load(QGLContext *p_pContext, double p_dAreaRefPointLat, double p_dAreaRefPointLong)
{
 QDir Dir;
 QPixmap Pixmap;
 GeographicLib::Math::real ZXY[3];
 GLenum GLError;

 if(m_bLoaded) return(true);

 //qDebug() << "CTexturedPlane::Load";

 m_pContext = p_pContext;
 
 if(!m_bGeographic) {
   //qDebug() << "CTexturedPlane::Load not geo";
   Pixmap = QPixmap(QString(m_FileName));
   m_nTextureWidth = Pixmap.width();
   m_nTextureHeight = Pixmap.height();
   m_nTextureID = m_pContext->bindTexture(Pixmap, GL_TEXTURE_2D); // will return 0 on error, but we let the area get created anyway.
   //qDebug() << "CTexturedPlane::Load: w/h/ID=" << QString::number(m_nTextureWidth) << "/" << QString::number(m_nTextureHeight) << "/" << QString::number(m_nTextureID);
   //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	  //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
   }
 else {
   //qDebug() << "CTexturedPlane::Load geo";
   m_nTextureWidth = 0;
   m_nTextureHeight = 0;
   m_nTextureID = 0; }

 Recalc(p_dAreaRefPointLat, p_dAreaRefPointLong); // we need this call to compute the zero point (upper left corner) of the surface in geo coords. The computed value should be used when creating opengl objects.

 ////nDisplayListNum = 1;
 ////while(glIsList(nDisplayListNum++));
 //m_nListID = glGenLists(1); //nDisplayListNum);
 ////qDebug() << "m_nListID = " << QString::number(m_nListID);
 //GLError = glGetError(); 
 ////qDebug() << "glGetError()=" << QString::number(GLError);
 //assert(m_nListID);
 //glNewList(m_nListID, GL_COMPILE);
 //GLError = glGetError();
 ////qDebug() << "glGetError()=" << QString::number(GLError);
 //if(GLError != GL_NO_ERROR) {
 //  //qDebug() << "setting m_nListID = 0";
 //  m_nListID = 0; }
 //else {
 //  if(m_nTextureID) {
 //    glEnable(GL_TEXTURE_2D);
 //    glBindTexture(GL_TEXTURE_2D, m_nTextureID); }
 //  glBegin(GL_TRIANGLES);
 //  glColor3ub(255, 255, 255);
 //  MakeAreaTriangles();
 //  glEnd();
 //  glBindTexture(GL_TEXTURE_2D, 0);
 //  glDisable(GL_TEXTURE_2D);
 //  glEndList(); }
 
 // new way of creating lists
 m_nListID2 = glGenLists(1); //nDisplayListNum);
 //qDebug() << "m_nListID2 = " << QString::number(m_nListID2);
 GLError = glGetError(); 
 //qDebug() << "glGetError()=" << QString::number(GLError);
 assert(m_nListID2);
 glNewList(m_nListID2, GL_COMPILE);
 GLError = glGetError(); 
 //qDebug() << "glGetError()=" << QString::number(GLError);
 if(GLError != GL_NO_ERROR) m_nListID2 = 0;
 else {
   if(m_nTextureID) {
     glEnable(GL_TEXTURE_2D);
     glBindTexture(GL_TEXTURE_2D, m_nTextureID);
     //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
     //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
     glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR); //GL_LINEAR);
     glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_NEAREST); //GL_LINEAR);
     //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
     //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
     glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
     glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE); 
     //glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
     }
   glBegin(GL_TRIANGLES);
   glColor3ub(255, 255, 255);
   MakeAreaTriangles3();
   glEnd();
   glBindTexture(GL_TEXTURE_2D, 0);
   glDisable(GL_TEXTURE_2D);
   glEndList(); }

 m_nOutlineListID = glGenLists(1); //nDisplayListNum);
 //qDebug() << "m_nOutlineListID = " << QString::number(m_nOutlineListID);
 GLError = glGetError(); 
 //qDebug() << "glGetError()=" << QString::number(GLError);
 assert(m_nOutlineListID);
 glNewList(m_nOutlineListID, GL_COMPILE);
 GLError = glGetError(); 
 //qDebug() << "glGetError()=" << QString::number(GLError);
 if(GLError != GL_NO_ERROR) m_nOutlineListID = 0;
 
 else {
   if(!m_bGeographic) {
     // selected outline
     //glBegin(GL_LINE_LOOP);
     //glColor3ub(255, 0, 0);
     //glVertex3f(-m_fSizeXM, 0, 0);
     //glVertex3f(-m_fSizeXM, 0, m_fSizeZM);
     //glVertex3f(0, 0, m_fSizeZM);
     //glVertex3f(0, 0, 0);
     //glEnd();
     //glEndList(); 
     
     //180628
     glBegin(GL_LINE_LOOP);
     glColor3ub(255, 0, 255);
     m_pLC->Forward(m_dSurface0Lat, m_dSurface0Long, 0, ZXY[2], ZXY[0], ZXY[1]);
     glVertex3f(ZXY[0], ZXY[1], ZXY[2]);
     m_pLC->Forward(m_dSurface0Lat, m_dSurface0Long + m_fSizeLong, 0, ZXY[2], ZXY[0], ZXY[1]);
     glVertex3f(ZXY[0], ZXY[1], ZXY[2]);
     m_pLC->Forward(m_dSurface0Lat - m_fSizeLat, m_dSurface0Long + m_fSizeLong, 0, ZXY[2], ZXY[0], ZXY[1]);
     glVertex3f(ZXY[0], ZXY[1], ZXY[2]);
     m_pLC->Forward(m_dSurface0Lat - m_fSizeLat, m_dSurface0Long, 0, ZXY[2], ZXY[0], ZXY[1]);
     glVertex3f(ZXY[0], ZXY[1], ZXY[2]);
     glEnd();
     glEndList();
     
     }
   else {
     glBegin(GL_LINE_LOOP);
     glColor3ub(255, 0, 255);
     m_pLC->Forward(m_dRefPointLat, m_dRefPointLong, 0, ZXY[2], ZXY[0], ZXY[1]);
     glVertex3f(ZXY[0], ZXY[1], ZXY[2]);
     m_pLC->Forward(m_dRefPointLat, m_dRefPointLong + m_fSizeLong, 0, ZXY[2], ZXY[0], ZXY[1]);
     glVertex3f(ZXY[0], ZXY[1], ZXY[2]);
     m_pLC->Forward(m_dRefPointLat - m_fSizeLat, m_dRefPointLong + m_fSizeLong, 0, ZXY[2], ZXY[0], ZXY[1]);
     glVertex3f(ZXY[0], ZXY[1], ZXY[2]);
     m_pLC->Forward(m_dRefPointLat - m_fSizeLat, m_dRefPointLong, 0, ZXY[2], ZXY[0], ZXY[1]);
     glVertex3f(ZXY[0], ZXY[1], ZXY[2]);
     glEnd();
     glEndList(); }
   }

 m_bLoaded = (/*m_nListID && */m_nListID2 && m_nOutlineListID && (m_bGeographic || m_nTextureID));
 //qDebug() << "m_nListID/m_nListID2/m_nOutlineListID/m_bGeographic/m_nTextureID" << QString::number(m_nListID) << "/" << QString::number(m_nListID2) << "/" << QString::number(m_nOutlineListID) << "/" << QString::number(m_bGeographic) << "/" << QString::number(m_nTextureID);
 //qDebug() << "m_bLoaded = " << QString::number(m_bLoaded);

 return(m_bLoaded);
}

bool CTexturedPlane::Unload()
{
 bool bRet;

 bRet = true;
 //if(m_nListID) {
 //  glDeleteLists(m_nListID, 1);
 //  if(bRet) bRet = (glGetError() == GL_NO_ERROR);
 //  }
 //qDebug() << "ListID " + QString::number(m_nListID) + " deleted with error " + QString::number(glGetError());

 if(m_nListID2) {
   glDeleteLists(m_nListID2, 1);
   if(bRet) bRet = (glGetError() == GL_NO_ERROR);
   }
 //qDebug() << "ListID2 " + QString::number(m_nListID2) + " deleted with error " + QString::number(glGetError());

 if(m_nOutlineListID) {
   glDeleteLists(m_nOutlineListID, 1);
   if(bRet) bRet = (glGetError() == GL_NO_ERROR);
   }
 //qDebug() << "Outline ListID " + QString::number(m_nOutlineListID) + " deleted with error " + QString::number(glGetError());

 if(m_nTextureID) {
   m_pContext->deleteTexture(m_nTextureID);
   if(bRet) bRet = (glGetError() == GL_NO_ERROR);
   //qDebug() << "Texture " + QString::number(m_nTextureID) + " deleted with error " + QString::number(glGetError());
   }
 m_nTextureID = 0;
 m_nTextureWidth = m_nTextureHeight = 0;
 m_bLoaded = false;
 return(bRet);
}

void CTexturedPlane::SetOffsetNumbersFromOldVis(float p_fDX, float p_fDZ, float p_fElevation)
{
 m_fDX = p_fDX;
 m_fDZ = p_fDZ;
 m_fElevation = p_fElevation;
}

void CTexturedPlane::GetOffsetNumbersFromOldVis(float *p_pfDX, float *p_pfDZ, float *p_pfElevation)
{
 *p_pfDX = m_fDX;
 *p_pfDZ = m_fDZ;
 *p_pfElevation = m_fElevation;
}

void CTexturedPlane::Draw()
{
 //GeographicLib::Math::real rVal[3];

 if(!m_bLoaded) return;
 
 //glEnable(GL_BLEND);
 //glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

 glPushMatrix();
 
 //if(!m_bAlternateSurface) {
   //glTranslatef(m_nPlanarXOffset, m_fElevation, m_nPlanarZOffset);    // трансляция
   //if(m_bVisible)
   //  glCallList(m_nListID);

   // test
   //glRotatef(m_fAreaRotateX, 1.0f, 0.0f, 0.0f);
   //glRotatef(m_fAreaRotateZ, 0.0f, 0.0f, 1.0f);

   if(m_bGeographic)
     m_bGeographic = m_bGeographic;
   else if(!m_bGeographic)
     m_bGeographic = m_bGeographic;

   // good
   //glTranslatef(0, 2 * m_fElevation, 0);
   //glRotatef(m_fSurfaceRotateX, 1.0f, 0.0f, 0.0f);
   //glRotatef(m_fSurfaceRotateZ, 0.0f, 0.0f, 1.0f);

   //glTranslatef(m_Surface0OffsetX, -m_Surface0OffsetY, m_Surface0OffsetZ);
   if(m_bVisible)
     glCallList(m_nListID2);
   // end good
   //}
 //else {
 //  // test
 //  //glTranslatef(m_fDZ * (m_fSize / m_nTextureWidth), m_fElevation, -m_fDX * (m_fSize / m_nTextureHeight));    // трансляция
 //  //if(m_bVisible)
 //  //  glCallList(m_nListID);
 //  // end test

 //  // good
 //  //glTranslatef(0, 2 * m_fElevation, 0);
 //  //glRotatef(m_fSurfaceRotateX, 1.0f, 0.0f, 0.0f);
 //  //glRotatef(m_fSurfaceRotateZ, 0.0f, 0.0f, 1.0f);
 //  //glTranslatef(m_Surface0OffsetX, -m_Surface0OffsetY, m_Surface0OffsetZ);
 //  //if(m_bVisible)
 //  //  glCallList(m_nListID2);
 //  // end good

 //  // test
 //  glRotatef(m_fAreaRotateX, 1.0f, 0.0f, 0.0f);
 //  glRotatef(m_fAreaRotateZ, 0.0f, 0.0f, 1.0f);

 //  glTranslatef(0, 2 * m_fElevation, 0);
 //  glRotatef(m_fSurfaceRotateX, 1.0f, 0.0f, 0.0f);
 //  glRotatef(m_fSurfaceRotateZ, 0.0f, 0.0f, 1.0f);
 //  glTranslatef(m_Surface0OffsetX, -m_Surface0OffsetY, m_Surface0OffsetZ);
 //  if(m_bVisible)
 //    glCallList(m_nListID2);
 //  // end test
 //  }

 if(m_bSelected)
   glCallList(m_nOutlineListID);

 glPopMatrix();
 
 if(!m_bDrawAxes || !m_bSelected) return;

 if(m_AreaCenterOffsetX || m_AreaCenterOffsetY || m_AreaCenterOffsetZ) {
   // area center point coordinate axes for textured surfaces
   glPushMatrix();
   //glTranslatef(0, 2 * m_fElevation, 0);
   //glRotatef(m_fSurfaceRotateX, 1.0f, 0.0f, 0.0f);
   //glRotatef(m_fSurfaceRotateZ, 0.0f, 0.0f, 1.0f);
   glTranslatef(m_AreaCenterOffsetX, m_AreaCenterOffsetY, m_AreaCenterOffsetZ);
   //glTranslatef(rVal[0], rVal[1], rVal[2]);
   glLineWidth(3.0f); 
   glBegin(GL_LINES); // построение линии
      glColor4f(0.7f, 0.00f, 0.00f, 1.0f);
      glVertex3f(2000.0f,  0.0f,  0.0f); // первая точка
      glVertex3f(0.0f,  0.0f,  0.0f); // вторая точка
      glColor4f(0.00f, 0.7f, 0.00f, 1.0f);
      glVertex3f(0.0f,  2000.0f,  0.0f);
      glVertex3f(0.0f, 0.0f,  0.0f);
      glColor4f(0.00f, 0.00f, 0.7f, 1.0f);
      glVertex3f(0.0f,  0.0f,  2000.0f);
      glVertex3f(0.0f,  0.0f, 0.0f);
   glEnd();  
   glPopMatrix(); }
 
 
 glPushMatrix();
 // surface center (upper left point) coordinate axes
 //glTranslatef(0, 2 * m_fElevation, 0);
 //glRotatef(m_fSurfaceRotateX, 1.0f, 0.0f, 0.0f);
 //glRotatef(m_fSurfaceRotateZ, 0.0f, 0.0f, 1.0f);
 glTranslatef(m_Surface0OffsetX, m_Surface0OffsetY, m_Surface0OffsetZ);
 glLineWidth(3.0f); 
 glBegin(GL_LINES); // построение линии
    glColor4f(0.5f, 0.00f, 0.00f, 1.0f);
    glVertex3f(1000.0f,  0.0f,  0.0f); // первая точка
    glVertex3f(0.0f,  0.0f,  0.0f); // вторая точка
    glColor4f(0.00f, 0.5f, 0.00f, 1.0f);
    glVertex3f(0.0f,  1000.0f,  0.0f);
    glVertex3f(0.0f, 0.0f,  0.0f);
    glColor4f(0.00f, 0.00f, 0.5f, 1.0f);
    glVertex3f(0.0f,  0.0f,  1000.0f);
    glVertex3f(0.0f,  0.0f, 0.0f);
 glEnd();  
 
 glPopMatrix();
}

void CTexturedPlane::Recalc(double p_dAreaRefPointLat, double p_dAreaRefPointLong)
// p_pLC is set to the area center
{
 float LCSceneZOffset_m, LCSceneXOffset_m;
 GeographicLib::Math::real s12[2], dVal[4];

 if((m_fDX || m_fDZ) && (!m_nTextureWidth || !m_nTextureHeight)) {
   assert(false);
   return; }

 m_dAreaRefPointLat = p_dAreaRefPointLat;
 m_dAreaRefPointLong = p_dAreaRefPointLong;

 if(!m_nTextureHeight || !m_nTextureWidth) {
   // texture not specified, this is a geometric area
   //m_pLC->Forward(m_dRefPointLat, m_dRefPointLong, 0/*fRefPointElevation*/, m_AreaCenterOffsetZ, m_AreaCenterOffsetX, m_AreaCenterOffsetY);
   m_pLC->Forward(m_dRefPointLat, m_dRefPointLong, 0/*fRefPointElevation*/, m_Surface0OffsetZ, m_Surface0OffsetX, m_Surface0OffsetY);
   m_dSurface0Lat = m_dRefPointLat;
   m_dSurface0Long = m_dRefPointLong;
   //m_Surface0OffsetX = m_Surface0OffsetY = m_Surface0OffsetZ = 0;
   m_AreaCenterOffsetZ = m_AreaCenterOffsetX = m_AreaCenterOffsetY = 0;
   m_nPlanarXOffset = 0;
   m_nPlanarZOffset = 0;
   m_fSurfaceRotateZ = 0;
   m_fSurfaceRotateX = 0;
   //m_fAreaRotateZ = -m_pGeo->Inverse(p_dAreaRefPointLat, p_dAreaRefPointLong, m_dRefPointLat, p_dAreaRefPointLong, s12[0]);
   //m_fAreaRotateX = -m_pGeo->Inverse(m_dRefPointLat, p_dAreaRefPointLong, m_dRefPointLat, m_dRefPointLong, s12[1]);
   m_fSurfaceRotateZ = -m_pGeo->Inverse(p_dAreaRefPointLat, p_dAreaRefPointLong, m_dRefPointLat, p_dAreaRefPointLong, s12[0]);
   m_fSurfaceRotateX = -m_pGeo->Inverse(m_dRefPointLat, p_dAreaRefPointLong, m_dRefPointLat, m_dRefPointLong, s12[1]);
   return; }

 // get meter offsets from the area center to the surface reference point
 m_pLC->Forward(m_dRefPointLat, m_dRefPointLong, 0/*fRefPointElevation*/, m_AreaCenterOffsetZ, m_AreaCenterOffsetX, m_AreaCenterOffsetY);

 // get meter offsets from the surface reference point to the surface upper left corner using texture width and height. Texture has to have been loaded.
 LCSceneXOffset_m = m_fDZ * (m_fSizeXM / m_nTextureHeight);
 LCSceneZOffset_m = m_fDX * (m_fSizeZM / m_nTextureWidth);

 // compute opengl xyz offsets of the texture ref point from the scene center
 m_nPlanarXOffset = m_AreaCenterOffsetX + LCSceneXOffset_m;
 m_nPlanarZOffset = m_AreaCenterOffsetZ + -LCSceneZOffset_m;

 m_fSurfaceRotateZ = m_fSurfaceRotateX = 0;
 // get latitude, longitude and angles of rotation of the surface upper left corner from the reference point lat long and texture offset
 m_fSurfaceRotateZ = -m_pGeo->Direct(m_dRefPointLat, m_dRefPointLong, 0, LCSceneXOffset_m, m_dSurface0Lat, m_dSurface0Long); // put back
 m_fSurfaceRotateX = -m_pGeo->Direct(m_dSurface0Lat, m_dSurface0Long, -90, LCSceneZOffset_m, m_dSurface0Lat, m_dSurface0Long); // put back
 //m_pLC->Forward(m_dSurface0Lat, m_dSurface0Long, m_fElevation, m_Surface0OffsetZ, m_Surface0OffsetX, m_Surface0OffsetY);
 m_pLC->Forward(m_dSurface0Lat, m_dSurface0Long, m_fElevation, m_Surface0OffsetZ, m_Surface0OffsetX, m_Surface0OffsetY);

 m_fSurfaceRotateZ = m_pGeo->Inverse(p_dAreaRefPointLat, p_dAreaRefPointLong, m_dSurface0Lat, p_dAreaRefPointLong, s12[0]); // test, above values seem wrong
 m_fSurfaceRotateX = m_pGeo->Inverse(p_dAreaRefPointLat, p_dAreaRefPointLong, p_dAreaRefPointLat, m_dSurface0Long, s12[1]); // test, above values seem wrong

 //m_fAreaRotateZ = -m_pGeo->Inverse(p_dAreaRefPointLat, p_dAreaRefPointLong, m_dRefPointLat, p_dAreaRefPointLong, s12[0]);
 //m_fAreaRotateX = -m_pGeo->Inverse(m_dRefPointLat, p_dAreaRefPointLong, m_dRefPointLat, m_dRefPointLong, s12[1]);


 // 180628
 // get meter offsets from the area center to the surface reference point
 m_pLC->Forward(m_dRefPointLat, m_dRefPointLong, 0, m_AreaCenterOffsetZ, m_AreaCenterOffsetX, m_AreaCenterOffsetY);
 // get meter offsets from the surface reference point to the surface upper left corner (offsets are computed from it in imaging software) using texture width and height. Texture has to have been loaded.
 LCSceneXOffset_m = m_fDZ * (m_fSizeXM / m_nTextureHeight); // North
 LCSceneZOffset_m = m_fDX * (m_fSizeZM / m_nTextureWidth); // East
 // compute opengl XZ offsets of the surface upper left corner from the area center
 m_nPlanarXOffset = m_AreaCenterOffsetX + LCSceneXOffset_m;
 m_nPlanarZOffset = m_AreaCenterOffsetZ + -LCSceneZOffset_m;
 m_fSurfaceRotateZ = m_fSurfaceRotateX = 0;
 // get latitude and longitude of the surface upper? left? corner from the reference point lat long and texture offset
 m_pGeo->Direct(m_dRefPointLat, m_dRefPointLong, 0, LCSceneXOffset_m, m_dSurface0Lat, dVal[0]);
 m_pGeo->Direct(m_dRefPointLat, m_dRefPointLong, -90, LCSceneZOffset_m, dVal[0], m_dSurface0Long);
 // compute coordinates of the bottom lat and right long
 m_pGeo->Direct(m_dSurface0Lat, m_dSurface0Long, 180, m_fSizeXM, dVal[0], dVal[1]);
 m_pGeo->Direct(m_dSurface0Lat, m_dSurface0Long, 90, m_fSizeZM, dVal[2], dVal[3]);
 // compute geographic sizes of the surface area
 m_fSizeLat = m_dSurface0Lat - dVal[0];
 m_fSizeLong = dVal[3] - m_dSurface0Long;
 //m_dSurface0Lat = dVal[0]; // bottom left corner is the new surface reference point
 //m_Surface0OffsetX = m_Surface0OffsetY = m_Surface0OffsetZ = 0; // no translation, coordinates are already set
 m_pLC->Forward(m_dSurface0Lat, m_dSurface0Long, 0, m_Surface0OffsetZ, m_Surface0OffsetX, m_Surface0OffsetY);
}

/*
void CTexturedPlane::MakeAreaTriangles()
{
 GeographicLib::Math::real ZXY[6][3]; 

 if(!m_bGeographic) {
   glTexCoord2f(0.0f, 0.0f); glVertex3f(-m_fSizeXM, 0, 0);
   glTexCoord2f(1.0f, 0.0f); glVertex3f(-m_fSizeXM, 0, m_fSizeZM);
   glTexCoord2f(0.0f, 1.0f); glVertex3f(0, 0, 0);
   glTexCoord2f(1.0f, 0.0f); glVertex3f(-m_fSizeXM, 0, m_fSizeZM);
   glTexCoord2f(1.0f, 1.0f); glVertex3f(0, 0, m_fSizeZM);
   glTexCoord2f(0.0f, 1.0f); glVertex3f(0, 0, 0);
   }
 else {
   m_pLC->Forward(m_dRefPointLat - m_fSize, m_dRefPointLong, 0, ZXY[0][0], ZXY[0][1], ZXY[0][2]);
   glVertex3f(ZXY[0][0], ZXY[0][1], ZXY[0][2]);
   m_pLC->Forward(m_dRefPointLat - m_fSize, m_dRefPointLong + m_fSize, 0, ZXY[1][0], ZXY[1][1], ZXY[1][2]);
   glVertex3f(ZXY[1][0], ZXY[1][1], ZXY[1][2]);
   m_pLC->Forward(m_dRefPointLat, m_dRefPointLong, 0, ZXY[2][0], ZXY[2][1], ZXY[2][2]);
   glVertex3f(ZXY[2][0], ZXY[2][1], ZXY[2][2]);
   ZXY[3][0] = ZXY[1][0];
   ZXY[3][1] = ZXY[1][1];
   ZXY[3][2] = ZXY[1][2];
   m_pLC->Forward(m_dRefPointLat, m_dRefPointLong + m_fSize, 0, ZXY[4][0], ZXY[4][1], ZXY[4][2]);
   glVertex3f(ZXY[4][0], ZXY[4][1], ZXY[4][2]);
   ZXY[5][0] = ZXY[2][0];
   ZXY[5][1] = ZXY[2][1];
   ZXY[5][2] = ZXY[2][2]; }
}
*/

void CTexturedPlane::MakeAreaTriangles3()
{
 int x, z, StepZ = 1000, StepX = 1000;
 int SizeX, SizeZ;
 int CurSizeZ, CurSizeX;
 double BL[6][2], Coord[6][3], TexCoord[6][2], SavedCenter[3];
 double dCurRefPointLat, dCurRefPointLong;
 double dTexVCenterRatio;
 double SceneXOffset_m;
 double LeftTexRatio, RightTexRatio;

 if(m_bGeographic) {
   MakeAreaTriangles3Geographic();
   return; }

 //MakeAreaTriangles3TexturesTest();
 //return;

 dTexVCenterRatio = 1.0 - m_fDZ / m_nTextureHeight;
 SceneXOffset_m = m_fDZ * (m_fSizeXM / m_nTextureHeight);

 SizeZ = (int)m_fSizeZM;
 SizeX = (int)m_fSizeXM;
 
 SavedCenter[0] = m_pLC->LatitudeOrigin();
 SavedCenter[1] = m_pLC->LongitudeOrigin();
 SavedCenter[2] = m_pLC->HeightOrigin();

 m_pLC->Reset(m_dRefPointLat, m_dRefPointLong, 0);
 dCurRefPointLat = m_dRefPointLat;
 dCurRefPointLong = m_dRefPointLong;
 
 dCurRefPointLat = m_dSurface0Lat; // 20180628
 dCurRefPointLong = m_dSurface0Long; // 20180628
 //m_pLC->Reset(dCurRefPointLat, dCurRefPointLong, 0); // 20180628
 m_pLC->Reset(m_dAreaRefPointLat, m_dAreaRefPointLong, 0); // 20180628

 for(z = 0; z < SizeZ; z += StepZ) {
   for(x = 0; x < SizeX; x += StepX) {
     CurSizeZ = StepZ;
     if(z + CurSizeZ > SizeZ) CurSizeZ = SizeZ - z;
     CurSizeX = StepX;
     if(x + CurSizeX > SizeX) CurSizeX = SizeX - x;
     
     m_pGeo->Direct(dCurRefPointLat, dCurRefPointLong, 180, x + CurSizeX, BL[0][0], BL[0][1]);
     m_pGeo->Direct(BL[0][0], BL[0][1], 90, z, BL[0][0], BL[0][1]);
     m_pGeo->Direct(dCurRefPointLat, dCurRefPointLong, 180, x + CurSizeX, BL[1][0], BL[1][1]);
     m_pGeo->Direct(BL[1][0], BL[1][1], 90, z + CurSizeZ, BL[1][0], BL[1][1]);
     m_pGeo->Direct(dCurRefPointLat, dCurRefPointLong, 180, x, BL[2][0], BL[2][1]);
     m_pGeo->Direct(BL[2][0], BL[2][1], 90, z, BL[2][0], BL[2][1]);
     BL[3][0] = BL[1][0];
     BL[3][1] = BL[1][1];
     m_pGeo->Direct(dCurRefPointLat, dCurRefPointLong, 180, x, BL[4][0], BL[4][1]);
     m_pGeo->Direct(BL[4][0], BL[4][1], 90, z + CurSizeZ, BL[4][0], BL[4][1]);
     BL[5][0] = BL[2][0];
     BL[5][1] = BL[2][1];

     m_pLC->Forward(BL[0][0], BL[0][1], 0, Coord[0][2], Coord[0][0], Coord[0][1]); // Coord: Z, X, Y
     m_pLC->Forward(BL[1][0], BL[1][1], 0, Coord[1][2], Coord[1][0], Coord[1][1]); // Coord: Z, X, Y
     m_pLC->Forward(BL[2][0], BL[2][1], 0, Coord[2][2], Coord[2][0], Coord[2][1]); // Coord: Z, X, Y
     m_pLC->Forward(BL[3][0], BL[3][1], 0, Coord[3][2], Coord[3][0], Coord[3][1]); // Coord: Z, X, Y
     m_pLC->Forward(BL[4][0], BL[4][1], 0, Coord[4][2], Coord[4][0], Coord[4][1]); // Coord: Z, X, Y
     m_pLC->Forward(BL[5][0], BL[5][1], 0, Coord[5][2], Coord[5][0], Coord[5][1]); // Coord: Z, X, Y

     if(!x) {
       LeftTexRatio = (Coord[2][0] - Coord[0][0]) / CurSizeX;
       RightTexRatio = (Coord[4][0] - Coord[3][0]) / CurSizeX;
       }

     TexCoord[0][0] = (float)z / SizeZ;
     TexCoord[0][1] = dTexVCenterRatio + ((SceneXOffset_m - (x + CurSizeX)) / m_fSizeXM) * LeftTexRatio;
     TexCoord[1][0] = TexCoord[0][0] + (float)CurSizeZ / m_fSizeZM;
     TexCoord[1][1] = dTexVCenterRatio + ((SceneXOffset_m - (x + CurSizeX)) / m_fSizeXM) * RightTexRatio;
     TexCoord[2][0] = TexCoord[0][0];
     TexCoord[2][1] = dTexVCenterRatio + ((SceneXOffset_m - x) / m_fSizeXM) * LeftTexRatio;
     TexCoord[3][0] = TexCoord[1][0];
     TexCoord[3][1] = TexCoord[1][1];
     TexCoord[4][0] = TexCoord[1][0];
     TexCoord[4][1] = dTexVCenterRatio + ((SceneXOffset_m - x) / m_fSizeXM) * RightTexRatio;
     TexCoord[5][0] = TexCoord[2][0];
     TexCoord[5][1] = TexCoord[2][1];

     glTexCoord2f(TexCoord[0][0], TexCoord[0][1]); glVertex3f(Coord[0][0], Coord[0][1], Coord[0][2]);
     glTexCoord2f(TexCoord[1][0], TexCoord[1][1]); glVertex3f(Coord[1][0], Coord[1][1], Coord[1][2]);
     glTexCoord2f(TexCoord[2][0], TexCoord[2][1]); glVertex3f(Coord[2][0], Coord[2][1], Coord[2][2]);
     glTexCoord2f(TexCoord[3][0], TexCoord[3][1]); glVertex3f(Coord[3][0], Coord[3][1], Coord[3][2]);
     glTexCoord2f(TexCoord[4][0], TexCoord[4][1]); glVertex3f(Coord[4][0], Coord[4][1], Coord[4][2]);
     glTexCoord2f(TexCoord[5][0], TexCoord[5][1]); glVertex3f(Coord[5][0], Coord[5][1], Coord[5][2]);
     }
   }

 m_pLC->Reset(SavedCenter[0], SavedCenter[1], SavedCenter[2]);
}

void CTexturedPlane::MakeAreaTriangles3Geographic()
{
 double z, x, dSizeZ, dSizeX, dStepZ, dStepX, Coord[6][3], dCurSizeZ, dCurSizeX;

 dSizeZ = m_fSizeLong;
 dSizeX = m_fSizeLat;

 dStepX = m_dGeographicTileStep;
 dStepZ = m_dGeographicTileStep;

 //glColor4f(0.4f, 0.4f, 0.4f, 0.5f);
 //glColor4ub(58, 76, 52, 1);
 glColor4ub(m_EarthColor.red(), m_EarthColor.green(), m_EarthColor.blue(), 1);

 for(z = 0; z < dSizeZ; z += dStepZ) {
   for(x = 0; x < dSizeX; x += dStepX) {
     dCurSizeZ = dStepZ;
     if(z + dCurSizeZ > dSizeZ) dCurSizeZ = dSizeZ - z;
     dCurSizeX = dStepX;
     if(x + dCurSizeX > dSizeX) dCurSizeX = dSizeX - x;

     m_pLC->Forward(m_dRefPointLat - (x + dCurSizeX), m_dRefPointLong + z, 0, Coord[0][2], Coord[0][0], Coord[0][1]); // Coord: Z, X, Y
     m_pLC->Forward(m_dRefPointLat - (x + dCurSizeX), m_dRefPointLong + (z + dCurSizeZ), 0, Coord[1][2], Coord[1][0], Coord[1][1]); // Coord: Z, X, Y
     m_pLC->Forward(m_dRefPointLat - x, m_dRefPointLong + z, 0, Coord[2][2], Coord[2][0], Coord[2][1]); // Coord: Z, X, Y
     Coord[3][0] = Coord[1][0];
     Coord[3][1] = Coord[1][1];
     Coord[3][2] = Coord[1][2];
     m_pLC->Forward(m_dRefPointLat - x, m_dRefPointLong + (z + dCurSizeZ), 0, Coord[4][2], Coord[4][0], Coord[4][1]); // Coord: Z, X, Y
     Coord[5][0] = Coord[2][0];
     Coord[5][1] = Coord[2][1];
     Coord[5][2] = Coord[2][2];

     glVertex3f(Coord[0][0], Coord[0][1], Coord[0][2]);
     glVertex3f(Coord[1][0], Coord[1][1], Coord[1][2]);
     glVertex3f(Coord[2][0], Coord[2][1], Coord[2][2]);
     glVertex3f(Coord[3][0], Coord[3][1], Coord[3][2]);
     glVertex3f(Coord[4][0], Coord[4][1], Coord[4][2]);
     glVertex3f(Coord[5][0], Coord[5][1], Coord[5][2]);
     }
   }
}

/*
void CTexturedPlane::MakeAreaTriangles3TexturesTest()
{
 double z, x, dSizeZ, dSizeX, dStepZ, dStepX, Coord[6][3], dCurSizeZ, dCurSizeX;

 dSizeZ = m_fSizeLong;
 dSizeX = m_fSizeLat;

 dStepX = m_dGeographicTileStep;
 dStepZ = m_dGeographicTileStep;

 //glColor4f(0.4f, 0.4f, 0.4f, 0.5f);
 //glColor4ub(58, 76, 52, 1);
 glColor4ub(255, 0, 0, 1);

 for(z = 0; z < dSizeZ; z += dStepZ) {
   for(x = 0; x < dSizeX; x += dStepX) {
     dCurSizeZ = dStepZ;
     if(z + dCurSizeZ > dSizeZ) dCurSizeZ = dSizeZ - z;
     dCurSizeX = dStepX;
     if(x + dCurSizeX > dSizeX) dCurSizeX = dSizeX - x;

     m_pLC->Forward(m_dRefPointLat - (x + dCurSizeX), m_dRefPointLong + z, 0, Coord[0][2], Coord[0][0], Coord[0][1]); // Coord: Z, X, Y
     m_pLC->Forward(m_dRefPointLat - (x + dCurSizeX), m_dRefPointLong + (z + dCurSizeZ), 0, Coord[1][2], Coord[1][0], Coord[1][1]); // Coord: Z, X, Y
     m_pLC->Forward(m_dRefPointLat - x, m_dRefPointLong + z, 0, Coord[2][2], Coord[2][0], Coord[2][1]); // Coord: Z, X, Y
     Coord[3][0] = Coord[1][0];
     Coord[3][1] = Coord[1][1];
     Coord[3][2] = Coord[1][2];
     m_pLC->Forward(m_dRefPointLat - x, m_dRefPointLong + (z + dCurSizeZ), 0, Coord[4][2], Coord[4][0], Coord[4][1]); // Coord: Z, X, Y
     Coord[5][0] = Coord[2][0];
     Coord[5][1] = Coord[2][1];
     Coord[5][2] = Coord[2][2];

     glVertex3f(Coord[0][0], Coord[0][1], Coord[0][2]);
     glVertex3f(Coord[1][0], Coord[1][1], Coord[1][2]);
     glVertex3f(Coord[2][0], Coord[2][1], Coord[2][2]);
     glVertex3f(Coord[3][0], Coord[3][1], Coord[3][2]);
     glVertex3f(Coord[4][0], Coord[4][1], Coord[4][2]);
     glVertex3f(Coord[5][0], Coord[5][1], Coord[5][2]);
     }
   }
}
*/
