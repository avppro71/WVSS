#include "DrawPrimitive.h"
#include <QDebug>
#include "qmath.h"
#define GEOGRAPHICLIB_SHARED_LIB 1
#include "GeographicLib/LocalCartesian.hpp"
#include "PlaneLibrary.h"
#include <GL/glu.h>
#include "qgl.h"

#pragma warning(disable : 4996)

QGLWidget *CDrawPrimitive::m_pGLWidget = NULL;
QFont CPrimitive_Plane::m_Font;
QColor CPrimitive_Plane::m_TextColors[CPrimitive_Plane::m_TextColorCount] = { QColor(255, 255, 255), QColor(255, 0, 0), QColor(0, 255, 0), QColor(0, 0, 255), QColor(0, 0, 0) };
short CPrimitive_Plane::m_nTextColorIndex = 0;
bool CPrimitive_Plane::m_bShowAirblock = true;

CDrawPrimitive::CDrawPrimitive()
{
 Reset();
}

CDrawPrimitive::~CDrawPrimitive()
{
 //qDebug() << "~CDrawPrimitive";
}

void CDrawPrimitive::Reset()
{
 m_bRecalcFromGeo = true;
 //m_Type = AVP_DP_NONE;
}

/************************************************/

CPrimitive_Line::CPrimitive_Line()
{
 Reset();
}

CPrimitive_Line::~CPrimitive_Line()
{
 //qDebug() << "~CPrimitive_Line";
}

void CPrimitive_Line::Reset()
{
 //m_Type = AVP_DP_LINE;
 m_PointsGeo[0].m_dLat = m_PointsGeo[0].m_dLong = m_PointsGeo[0].m_dH = 0;
 m_PointsGeo[1].m_dLat = m_PointsGeo[1].m_dLong = m_PointsGeo[1].m_dH = 0;
 m_dX[0] = m_dX[1] = m_dY[0] = m_dY[1] = m_dZ[0] = m_dZ[1] = 0;
 m_fLineWidth = 1;
 m_Color[0] = 255;
 m_Color[1] = 255;
 m_Color[2] = 0;
}

void CPrimitive_Line::Draw()
{
 glLineWidth(m_fLineWidth); 
 glColor3ub(m_Color[0], m_Color[1], m_Color[2]);
 glBegin(GL_LINES);
 glVertex3f(m_dX[0], m_dY[0], m_dZ[0]);
 glVertex3f(m_dX[1], m_dY[1], m_dZ[1]);
 glEnd();     
}

void CPrimitive_Line::Recalc(GeographicLib::LocalCartesian *p_pLC)
// p_pLC: X - East, Y - North
// computes planar xyz of the circle center from the geo center. The geo center is assumed to be set to the proper value;
{
 GeographicLib::Math::real LCX, LCY, LCZ;
 if(!p_pLC || !m_bRecalcFromGeo) return;
 
 p_pLC->Forward(m_PointsGeo[0].m_dLat, m_PointsGeo[0].m_dLong, m_PointsGeo[0].m_dH, LCX, LCY, LCZ);
 m_dZ[0] = LCX; // LCX is East, so it's our Z
 m_dX[0] = LCY; // LCY is North, so it's our X
 m_dY[0] = LCZ; // LCZ is up, so it's our Y
 p_pLC->Forward(m_PointsGeo[1].m_dLat, m_PointsGeo[1].m_dLong, m_PointsGeo[1].m_dH, LCX, LCY, LCZ);
 m_dZ[1] = LCX; // LCX is East, so it's our Z
 m_dX[1] = LCY; // LCY is North, so it's our X
 m_dY[1] = LCZ; // LCZ is up, so it's our Y
}

/************************************************/

CPrimitive_Circle::CPrimitive_Circle()
{
 Reset();
}

CPrimitive_Circle::~CPrimitive_Circle()
{
 //qDebug() << "~CPrimitive_Circle";
}

void CPrimitive_Circle::Reset()
{
 //m_Type = AVP_DP_CIRCLE;
 m_CenterGeo.m_dLat = m_CenterGeo.m_dLong = m_CenterGeo.m_dH = 0;
 m_dCenter_planar[0] = m_dCenter_planar[1] = m_dCenter_planar[2] = 0;
 m_fRadius = 1;
 m_Color[0] = m_Color[1] = m_Color[2] = 255;
 m_fLineWidth = 1;
 m_fHeading = m_fPitch = m_fBank = 0;
}

void CPrimitive_Circle::Draw()
{
 short i, nCirclePoints = 20;
 GLdouble dTheta;

 glLineWidth(m_fLineWidth);

 glEnable(GL_BLEND);
 glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
 
 glColor4ub(m_Color[0], m_Color[1], m_Color[2], 255);

 glPushMatrix();
 glTranslatef(m_dCenter_planar[0], m_dCenter_planar[1], m_dCenter_planar[2]);    // трансляция
 //assert(0);
 //put lines below back and continue looking for a fix
 glRotatef(m_fHeading, 0.0f, -1.0f, 0.0f); // поворот вокруг оси Y
 glRotatef(m_fPitch, 0.0f, 0.0f, 1.0f); // поворот вокруг оси Z
 glRotatef(m_fBank, 1.0f, 0.0f, 0.0f); // поворот вокруг оси X         

 glBegin(GL_LINE_LOOP);
 for(i = 0; i < nCirclePoints; i++) {
   dTheta = (2 * M_PI * i) / nCirclePoints;
   glVertex3f(0, m_fRadius * cos(dTheta), m_fRadius * sin(dTheta));
   }
 glEnd();
   
 glDisable(GL_BLEND);

 glPopMatrix();
}

void CPrimitive_Circle::Recalc(GeographicLib::LocalCartesian *p_pLC)
// p_pLC: X - East, Y - North, Z - up
// computes planar xyz of the circle center from the geo center. The geo center is assumed to be set to the proper value;
{
 GeographicLib::Math::real LCX, LCY, LCZ;
 if(!p_pLC || !m_bRecalcFromGeo) return;

 p_pLC->Forward(m_CenterGeo.m_dLat, m_CenterGeo.m_dLong, m_CenterGeo.m_dH, LCX, LCY, LCZ);
 m_dCenter_planar[2] = LCX; // LCX is East, so it's our Z
 m_dCenter_planar[0] = LCY; // LCY is North, so it's our X
 m_dCenter_planar[1] = LCZ; // LCZ is up, so it's our Y
}

/************************************************/

CPrimitive_Rectangle::CPrimitive_Rectangle()
{
 Reset();
}

CPrimitive_Rectangle::~CPrimitive_Rectangle()
{
 //qDebug() << "~CPrimitive_Rectangle";
}

void CPrimitive_Rectangle::Reset()
{
 m_CenterGeo.m_dLat = m_CenterGeo.m_dLong = m_CenterGeo.m_dH = 0;
 m_dCenter_planar[0] = m_dCenter_planar[1] = m_dCenter_planar[2] = 0;
 m_fHorzExtent = m_fVertExtent = 1;
 m_Color[0] = m_Color[1] = m_Color[2] = 255;
 m_fLineWidth = 1;
 m_fHeading = m_fPitch = m_fBank = 0;
}

void CPrimitive_Rectangle::Draw()
{
 glLineWidth(m_fLineWidth);
 glColor3ub(m_Color[0], m_Color[1], m_Color[2]);

 glPushMatrix();
 glTranslatef(m_dCenter_planar[0], m_dCenter_planar[1], m_dCenter_planar[2]);    // трансляция
 glRotatef(m_fHeading, 0.0f, -1.0f, 0.0f); // поворот вокруг оси Y
 glRotatef(m_fPitch, 0.0f, 0.0f, 1.0f); // поворот вокруг оси Z
 glRotatef(m_fBank, 1.0f, 0.0f, 0.0f); // поворот вокруг оси X         

 glBegin(GL_LINE_LOOP);
 glVertex3f(0, -m_fVertExtent, -m_fHorzExtent);
 glVertex3f(0, m_fVertExtent, -m_fHorzExtent);
 glVertex3f(0, m_fVertExtent, m_fHorzExtent);
 glVertex3f(0, -m_fVertExtent, m_fHorzExtent);
 glEnd();
   
 glPopMatrix();
}

void CPrimitive_Rectangle::Recalc(GeographicLib::LocalCartesian *p_pLC)
// p_pLC: X - East, Y - North, Z - up
// computes planar xyz of the circle center from the geo center. The geo center is assumed to be set to the proper value;
{
 GeographicLib::Math::real LCX, LCY, LCZ;
 if(!p_pLC || !m_bRecalcFromGeo) return;

 p_pLC->Forward(m_CenterGeo.m_dLat, m_CenterGeo.m_dLong, m_CenterGeo.m_dH, LCX, LCY, LCZ);
 m_dCenter_planar[2] = LCX; // LCX is East, so it's our Z
 m_dCenter_planar[0] = LCY; // LCY is North, so it's our X
 m_dCenter_planar[1] = LCZ; // LCZ is up, so it's our Y
}

/************************************************/

CPlaneLibrary *CPrimitive_Plane::m_pPlaneLibrary;

CPrimitive_Plane::CPrimitive_Plane()
{
 Reset();
}

CPrimitive_Plane::~CPrimitive_Plane()
{
 //qDebug() << "~CPrimitive_Plane";
}

void CPrimitive_Plane::Reset()
{
 memset(m_PlaneType, 0, sizeof(m_PlaneType));
 //*m_PlaneType = '\0';
 m_CenterGeo.m_dLat = m_CenterGeo.m_dLong = m_CenterGeo.m_dH = 0;
 m_dCenter_planar[0] = m_dCenter_planar[1] = m_dCenter_planar[2] = 0;
 m_fHeading = m_fPitch = m_fBank = 0;
 m_nScale = 1;
 m_nVTextBlockOffset = 15;
}

void CPrimitive_Plane::Recalc(GeographicLib::LocalCartesian *p_pLC)
// p_pLC: X - East, Y - North, Z - up
// computes planar xyz of the circle center from the geo center. The geo center is assumed to be set to the proper value;
{
 GeographicLib::Math::real LCX, LCY, LCZ;
 if(!p_pLC) return;

 p_pLC->Forward(m_CenterGeo.m_dLat, m_CenterGeo.m_dLong, m_CenterGeo.m_dH, LCX, LCY, LCZ);
 m_dCenter_planar[2] = LCX; // LCX is East, so it's our Z
 m_dCenter_planar[0] = LCY; // LCY is North, so it's our X
 m_dCenter_planar[1] = LCZ; // LCZ is up, so it's our Y
}

void CPrimitive_Plane::Draw()
{
 short nCirclePoints = 20;
 C3DObj *p3DObj;
 float fScale;
 double dWingspan;

 glPushMatrix();
 glTranslatef(m_dCenter_planar[0], m_dCenter_planar[1], m_dCenter_planar[2]);    // трансляция
 glRotatef(m_fHeading, 0.0f, -1.0f, 0.0f); // поворот вокруг оси Y
 glRotatef(m_fPitch, 0.0f, 0.0f, 1.0f); // поворот вокруг оси Z
 glRotatef(m_fBank, 1.0f, 0.0f, 0.0f); // поворот вокруг оси X         

 if(m_pPlaneLibrary) {
   p3DObj = m_pPlaneLibrary->GetPlaneObjectByType(m_PlaneType, &dWingspan);
   if(p3DObj) {
     fScale = 1;
     if(dWingspan) fScale = m_dWingspan / dWingspan;
     fScale *= m_nScale;
     if(abs(fScale - 1) > 0.05) glScalef(fScale, fScale, fScale);
     p3DObj->Draw(); } }
   
 glPopMatrix();
 if(m_bShowAirblock)
   RenderTextBlock();

 // test plane center lines
 //glPushMatrix();
 //glLineWidth(2); 
 //glBegin(GL_LINES);
 //glColor3ub(255, 0, 0);
 //glVertex3f(m_dCenter_planar[0], m_dCenter_planar[1], m_dCenter_planar[2]);
 //glVertex3f(m_dCenter_planar[0] + 100, m_dCenter_planar[1], m_dCenter_planar[2]);
 //glColor3ub(0, 255, 0);
 //glVertex3f(m_dCenter_planar[0], m_dCenter_planar[1], m_dCenter_planar[2]);
 //glVertex3f(m_dCenter_planar[0], m_dCenter_planar[1] + 100, m_dCenter_planar[2]);
 //glColor3ub(0, 0, 255);
 //glVertex3f(m_dCenter_planar[0], m_dCenter_planar[1], m_dCenter_planar[2]);
 //glVertex3f(m_dCenter_planar[0], m_dCenter_planar[1], m_dCenter_planar[2] + 100);
 //glEnd();
 //glPopMatrix();
}

void CPrimitive_Plane::SetFont(QFont p_Font)
{
 m_Font = p_Font;
}

void CPrimitive_Plane::RenderTextBlock()
{
 char tstr[80];
 QPoint TextPoint;
 GLdouble Projection[16];
 GLdouble Modelview[16];
 GLint Viewport[4];
 GLdouble ScreenCoords[3];
 short LineCount = 2;

 m_pGLWidget->setFont(m_Font);

 // convert from world to screen
 glGetDoublev(GL_PROJECTION_MATRIX, Projection);
 glGetDoublev(GL_MODELVIEW_MATRIX, Modelview);
 glGetIntegerv(GL_VIEWPORT, Viewport);
 gluProject(m_dCenter_planar[0], m_dCenter_planar[1], m_dCenter_planar[2], Modelview, Projection, Viewport, &ScreenCoords[0], &ScreenCoords[1], &ScreenCoords[2]);
 TextPoint.setX(ScreenCoords[0]);
 TextPoint.setY(m_pGLWidget->height() - ScreenCoords[1] - m_nVTextBlockOffset - (LineCount - 1) * m_Font.pixelSize()); //m_FontSize.height());
 
 //glColor3ub(0, 192, 0);
 glColor3ub(m_TextColors[m_nTextColorIndex].red(), m_TextColors[m_nTextColorIndex].green(), m_TextColors[m_nTextColorIndex].blue());
 
 sprintf(tstr, "%s", m_CallSign);
 m_pGLWidget->renderText(TextPoint.x(), TextPoint.y(), QString::fromLocal8Bit(tstr));

 TextPoint.setY(TextPoint.y() + m_Font.pixelSize()); //m_FontSize.height());
 sprintf(tstr, "%s", m_PlaneType);
 m_pGLWidget->renderText(TextPoint.x(), TextPoint.y(), QString::fromLocal8Bit(tstr));

 //TextPoint.setY(TextPoint.y() + m_Font.pixelSize()); //m_FontSize.height());
 //sprintf(tstr, "%.0lf deg", m_fHeading);
 m_pGLWidget->renderText(TextPoint.x(), TextPoint.y(), QString::fromLocal8Bit(tstr));
}

void CPrimitive_Plane::SetTextColorIndex(short p_nIndex)
{
 if(p_nIndex < 0 || p_nIndex >= m_TextColorCount)
   m_nTextColorIndex = 0;
 else
   m_nTextColorIndex = p_nIndex;
}

/************************************************/

CPrimitive_UnlimitedLine::CPrimitive_UnlimitedLine()
{
 m_lPointCount = m_lAvlPointCount = 0;
 m_pPointData = NULL;
 //m_pPointsGeo = NULL;
 //m_pdX = m_pdY = m_pdZ = NULL;
 Reset(true);
}

CPrimitive_UnlimitedLine::~CPrimitive_UnlimitedLine()
{
 //qDebug() << "~CPrimitive_UnlimitedLine";
 Reset(true);
}

void CPrimitive_UnlimitedLine::Reset(bool p_bDeleteVectors)
{
 if(p_bDeleteVectors) {
   if(m_pPointData) delete [] m_pPointData;
   m_lAvlPointCount = 0; }
 m_lPointCount = 0;
 m_fLineWidth = 1;
 m_Color[0] = 255;
 m_Color[1] = 255;
 m_Color[2] = 255;
 m_SurfaceElevation = 0;
}

/*
void CPrimitive_UnlimitedLine::DeleteVectors()
{
 if(m_pPointsGeo) { delete [] m_pPointsGeo; m_pPointsGeo = NULL; }
 if(m_pdX) { delete [] m_pdX; m_pdX = NULL; }
 if(m_pdY) { delete [] m_pdY; m_pdY = NULL; }
 if(m_pdZ) { delete [] m_pdZ; m_pdZ = NULL; }
 m_lAvlPointCount = m_lPointCount = 0;
}
*/

void CPrimitive_UnlimitedLine::Draw()
{
 unsigned long li;
 glLineWidth(m_fLineWidth); 
 glColor3ub(m_Color[0], m_Color[1], m_Color[2]);
 glBegin(GL_LINES);
 for(li = 0; li < m_lPointCount - 1; li ++) {
   glVertex3f(m_pPointData[li].m_dX, m_pPointData[li].m_dY, m_pPointData[li].m_dZ);
   glVertex3f(m_pPointData[li + 1].m_dX, m_pPointData[li + 1].m_dY, m_pPointData[li + 1].m_dZ); }
 glEnd();     

 //glColor3ub(128, 128, 128);
 //glBegin(GL_LINES);
 //for(li = 0; li < m_lPointCount - 1; li ++) {
 //  glVertex3f(m_pPointData[li].m_dX, m_SurfaceElevation, m_pPointData[li].m_dZ);
 //  glVertex3f(m_pPointData[li + 1].m_dX, m_SurfaceElevation, m_pPointData[li + 1].m_dZ); }
 //glEnd();     

 //glColor3ub(128, 128, 128);
 //glBegin(GL_LINES);
 //for(li = 0; li < m_lPointCount; li ++) {
 //  if(!(li % 10) || li == m_lPointCount - 1) {
 //    glVertex3f(m_pPointData[li].m_dX, m_pPointData[li].m_dY, m_pPointData[li].m_dZ);
 //    glVertex3f(m_pPointData[li].m_dX, m_SurfaceElevation, m_pPointData[li].m_dZ); } }
 //glEnd();     
}

void CPrimitive_UnlimitedLine::Recalc(GeographicLib::LocalCartesian *p_pLC)
// p_pLC: X - East, Y - North
// computes planar xyz of the circle center from the geo center. The geo center is assumed to be set to the proper value;
{
 unsigned long li;
 GeographicLib::Math::real LCX, LCY, LCZ;
 if(!p_pLC || !m_bRecalcFromGeo) return;
 
 for(li = 0; li < m_lPointCount; li ++) {
   p_pLC->Forward(m_pPointData[li].m_PointGeo.m_dLat, m_pPointData[li].m_PointGeo.m_dLong, m_pPointData[li].m_PointGeo.m_dH, LCX, LCY, LCZ);
   m_pPointData[li].m_dZ = LCX; // LCX is East, so it's our Z
   m_pPointData[li].m_dX = LCY; // LCY is North, so it's our X
   m_pPointData[li].m_dY = LCZ; // LCZ is up, so it's our Y
   }
}

bool CPrimitive_UnlimitedLine::AddPoints(rWVSSGeoPoint *m_pPointsGeo, unsigned long p_lPointCount, GeographicLib::LocalCartesian *p_pLC)
{
 unsigned long li;
 rWVSSGeoPoint *pCurPointGeo;

 m_lPointCount = 0;
 if(p_lPointCount > m_lAvlPointCount) {
   if(m_pPointData) delete [] m_pPointData;
   m_lAvlPointCount = 0;
   m_pPointData = new rPointData[p_lPointCount];
   if(!m_pPointData) return(false);
   memset(m_pPointData, 0, sizeof(rPointData) * p_lPointCount);
   m_lAvlPointCount = p_lPointCount; }

 pCurPointGeo = m_pPointsGeo;
 for(li = 0; li < p_lPointCount; li ++) {
   m_pPointData[li].m_PointGeo = *pCurPointGeo;
   ++ pCurPointGeo;
   }
 m_lPointCount = p_lPointCount;
 CPrimitive_UnlimitedLine::Recalc(p_pLC);
 return(true);
}

/************************************************/

CPrimitive_TrackLine::CPrimitive_TrackLine()
{
 m_pSurfacePointData = NULL;
 //m_lAvlSurfacePointCount = 0;
}

CPrimitive_TrackLine::~CPrimitive_TrackLine()
{
 Reset(true);
}

void CPrimitive_TrackLine::Reset(bool p_bDeleteVectors)
{
 if(p_bDeleteVectors) {
   if(m_pSurfacePointData) delete [] m_pSurfacePointData;
   //m_lAvlSurfacePointCount = 0;
   }
}

void CPrimitive_TrackLine::Draw()
{
 unsigned long li;
 
 glLineWidth(m_fLineWidth); 
 
 CPrimitive_UnlimitedLine::Draw();

 glColor3ub(128, 128, 128);
 glBegin(GL_LINES);
 for(li = 0; li < m_lPointCount - 1; li ++) {
   glVertex3f(m_pSurfacePointData[li].m_dX, m_pSurfacePointData[li].m_dY, m_pSurfacePointData[li].m_dZ);
   glVertex3f(m_pSurfacePointData[li + 1].m_dX, m_pSurfacePointData[li + 1].m_dY, m_pSurfacePointData[li + 1].m_dZ); }
 glEnd();     

 glColor3ub(128, 128, 128);
 glBegin(GL_LINES);
 for(li = 0; li < m_lPointCount; li ++) {
   if(!(li % 10) || li == m_lPointCount - 1) {
     glVertex3f(m_pPointData[li].m_dX, m_pPointData[li].m_dY, m_pPointData[li].m_dZ);
     glVertex3f(m_pSurfacePointData[li].m_dX, m_pSurfacePointData[li].m_dY, m_pSurfacePointData[li].m_dZ); } }
 glEnd();     
}

void CPrimitive_TrackLine::Recalc(GeographicLib::LocalCartesian *p_pLC)
// p_pLC: X - East, Y - North
// computes planar xyz of the circle center from the geo center. The geo center is assumed to be set to the proper value;
{
 unsigned long li;
 GeographicLib::Math::real LCX, LCY, LCZ;
 if(!p_pLC || !m_bRecalcFromGeo) return;
 
 //CPrimitive_UnlimitedLine::Recalc(p_pLC);

 for(li = 0; li < m_lPointCount; li ++) {
   p_pLC->Forward(m_pSurfacePointData[li].m_PointGeo.m_dLat, m_pSurfacePointData[li].m_PointGeo.m_dLong, m_pSurfacePointData[li].m_PointGeo.m_dH, LCX, LCY, LCZ);
   m_pSurfacePointData[li].m_dZ = LCX; // LCX is East, so it's our Z
   m_pSurfacePointData[li].m_dX = LCY; // LCY is North, so it's our X
   m_pSurfacePointData[li].m_dY = LCZ; // LCZ is up, so it's our Y
   }
}

bool CPrimitive_TrackLine::AddPoints(rWVSSGeoPoint *m_pPointsGeo, unsigned long p_lPointCount, GeographicLib::LocalCartesian *p_pLC)
{
 unsigned long li, OldAvlPointCount;

 OldAvlPointCount = m_lAvlPointCount;
 if(!CPrimitive_UnlimitedLine::AddPoints(m_pPointsGeo, p_lPointCount, p_pLC)) return(false);

 // we need to synchronise m_pSurfacePointData to m_pPointData tracking m_lAvlPointCount changes
 if(m_lAvlPointCount > OldAvlPointCount) {
   if(m_pSurfacePointData) delete [] m_pSurfacePointData;
   m_pSurfacePointData = new rPointData[m_lAvlPointCount];
   if(!m_pSurfacePointData) return(false);
   memset(m_pSurfacePointData, 0, sizeof(rPointData) * m_lAvlPointCount); }
 
 for(li = 0; li < p_lPointCount; li ++) {
   m_pSurfacePointData[li].m_PointGeo = m_pPointData[li].m_PointGeo;
   // set H = 0p to denote a surface point below a track point. This is actually wrong because surface elevation at the area center is nonzero but somewhere else is something different (say 0).
   // We neeed to account for surface elevation at the area center. Otherise tracks will go undeground. Bu elsehwre this value is meaningless and wrong, especially if it's very high.
   m_pSurfacePointData[li].m_PointGeo.m_dH = 0;
   }

 Recalc(p_pLC);
 return(true);
}
