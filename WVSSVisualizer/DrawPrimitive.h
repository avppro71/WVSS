#ifndef CDRAWPRIMITIVE_H
#define CDRAWPRIMITIVE_H

#include <QTOpenGL/QGLWidget>
#include "WVSSIPProtocol.h"

namespace GeographicLib {
class LocalCartesian;
}

class m_pGLWidget;

class CDrawPrimitive
{
public:
 //enum Type { AVP_DP_NONE, AVP_DP_CIRCLE, AVP_DP_LINE };

private:
protected:
  //Type m_Type;

public:
 static QGLWidget *m_pGLWidget;
 bool m_bRecalcFromGeo; // some items we don't want to recalc, hence the flag

private:
protected:
public:
 CDrawPrimitive();
 ~CDrawPrimitive();
 virtual void Reset();
 virtual void Draw() = 0;
 virtual void Recalc(GeographicLib::LocalCartesian *p_pLC) = 0; // p_pLC: X - East, Y - North
};

class CPrimitive_Line : public CDrawPrimitive
{
private:
protected:
public:
 rWVSSGeoPoint m_PointsGeo[2];
 GLdouble m_dX[2], m_dY[2], m_dZ[2];
 GLfloat m_fLineWidth; 
 GLubyte m_Color[3];

private:
protected:
public:
 CPrimitive_Line();
 ~CPrimitive_Line();
 virtual void Reset();
 virtual void Draw();
 virtual void Recalc(GeographicLib::LocalCartesian *p_pLC); // p_pLC: X - East, Y - North
};

class CPrimitive_Circle : public CDrawPrimitive
{
private:
protected:
public:
 rWVSSGeoPoint m_CenterGeo;
 GLdouble m_dCenter_planar[3]; // x, y, and z in OpenGL coordinates
 GLfloat m_fRadius; 
 GLubyte m_Color[3];
 GLfloat m_fLineWidth;
 GLfloat m_fHeading, m_fPitch, m_fBank;

private:
protected:
public:
 CPrimitive_Circle();
 ~CPrimitive_Circle();
 virtual void Reset();
 virtual void Draw();
 virtual void Recalc(GeographicLib::LocalCartesian *p_pLC); // p_pLC: X - East, Y - North
};

class CPrimitive_Rectangle : public CDrawPrimitive
{
private:
protected:
public:
 rWVSSGeoPoint m_CenterGeo;
 GLdouble m_dCenter_planar[3]; // x, y, and z in OpenGL coordinates
 GLfloat m_fHorzExtent, m_fVertExtent;
 GLubyte m_Color[3];
 GLfloat m_fLineWidth;
 GLfloat m_fHeading, m_fPitch, m_fBank;

private:
protected:
public:
 CPrimitive_Rectangle();
 ~CPrimitive_Rectangle();
 virtual void Reset();
 virtual void Draw();
 virtual void Recalc(GeographicLib::LocalCartesian *p_pLC); // p_pLC: X - East, Y - North
};

class CPlaneLibrary;

class CPrimitive_Plane : public CDrawPrimitive
{
private:
protected:
 static QFont m_Font;
 static const short m_TextColorCount = 5;
 static QColor m_TextColors[m_TextColorCount];
 static short m_nTextColorIndex;

public:
 static bool m_bShowAirblock;
 static CPlaneLibrary *m_pPlaneLibrary;
 char m_PlaneType[16];
 char m_CallSign[32];
 rWVSSGeoPoint m_CenterGeo;
 GLdouble m_dCenter_planar[3]; // x, y, and z in OpenGL coordinates
 GLfloat m_fHeading, m_fPitch, m_fBank;
 short m_nScale; // default - 1. This is arbitrary scaling for visibility purposes, settable by the player
 double m_dWingspan; // as passed by the Player. The wingspan of the actual plane. Used when scaling the default model to actual size. Only used when the default model is chosen.
 short m_nVTextBlockOffset; // vertical offset of the text block relative to the center point of the plane
 
private:
protected:
public:
 CPrimitive_Plane();
 ~CPrimitive_Plane();
 virtual void Reset();
 virtual void Draw();
 void RenderTextBlock();
 virtual void Recalc(GeographicLib::LocalCartesian *p_pLC); // p_pLC: X - East, Y - North
 static void SetFont(QFont p_Font);
 static QFont GetFont() { return(m_Font); }
 static void SetTextColorIndex(short p_nIndex);
};

class CPrimitive_UnlimitedLine : public CDrawPrimitive
{
private:
protected:
 struct rPointData {
   rWVSSGeoPoint m_PointGeo;
   GLdouble m_dX, m_dY, m_dZ; };

protected:
public:
 rPointData *m_pPointData;
 unsigned long m_lPointCount, m_lAvlPointCount;
 GLfloat m_fLineWidth; 
 GLubyte m_Color[3];
 GLfloat m_SurfaceElevation;

private:
protected:
 //void DeleteVectors();

public:
 CPrimitive_UnlimitedLine();
 ~CPrimitive_UnlimitedLine();
 virtual void Reset(bool p_bDeleteVectors);
 virtual void Draw();
 virtual void Recalc(GeographicLib::LocalCartesian *p_pLC); // p_pLC: X - East, Y - North
 virtual bool AddPoints(rWVSSGeoPoint *m_pPointsGeo, unsigned long p_lPointCount, GeographicLib::LocalCartesian *p_pLC);
};

class CPrimitive_TrackLine : public CPrimitive_UnlimitedLine
{
private:
protected:
public:
 rPointData *m_pSurfacePointData;
 //unsigned long m_lAvlSurfacePointCount; // should be the same as m-lAvlPointCount, but the value has to be lept because the base class doesn't know about it and can reallocate to a higher number

private:
protected:
public:
 CPrimitive_TrackLine();
 ~CPrimitive_TrackLine();
 virtual void Reset(bool p_bDeleteVectors);
 virtual void Draw();
 virtual void Recalc(GeographicLib::LocalCartesian *p_pLC); // p_pLC: X - East, Y - North
 virtual bool AddPoints(rWVSSGeoPoint *m_pPointsGeo, unsigned long p_lPointCount, GeographicLib::LocalCartesian *p_pLC);
};

#endif // CDRAWPRIMITIVE_H
