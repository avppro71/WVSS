#pragma once
#define _USE_MATH_DEFINES
#include "math.h"
class CGeometricVector3D
{
public:
 double X, Y, Z;
public:
 CGeometricVector3D() { Set(0, 0, 0); }
 CGeometricVector3D(double p_dX, double p_dY, double p_dZ) { Set(p_dX, p_dY, p_dZ); }
 CGeometricVector3D(CGeometricVector3D &p_GV) { Set(p_GV.X, p_GV.Y, p_GV.Z); }
 ~CGeometricVector3D() { }
 void Set(double p_dX, double p_dY, double p_dZ) { X = p_dX; Y = p_dY; Z = p_dZ; }
 CGeometricVector3D VectorProduct(CGeometricVector3D p_Vector, bool p_bLeft = false); 
 CGeometricVector3D VectorProduct(double p_dX, double p_dY, double p_dZ, bool p_bLeft = false); 
 static CGeometricVector3D VectorProduct(CGeometricVector3D p_Vector1, CGeometricVector3D p_Vector2, bool p_bLeft = false);
 double Module() { return(sqrt(X * X + Y * Y + Z * Z)); }
 CGeometricVector3D Normalize() { double dModule = Module(); CGeometricVector3D GV(X / dModule, Y / dModule, Z / dModule); return(GV); }
 void NormalizeMe() { double dModule = Module(); X /= dModule; Y /= dModule; Z /= dModule; }
 double ScalarProduct(CGeometricVector3D p_Vector) { return(X * p_Vector.X + Y * p_Vector.Y + Z * p_Vector.Z); }
 double AngleDeg(CGeometricVector3D p_Vector);
 CGeometricVector3D operator - (CGeometricVector3D p_Vector) { CGeometricVector3D GV(X - p_Vector.X, Y - p_Vector.Y, Z - p_Vector.Z); return(GV); }
 double Distance(CGeometricVector3D p_Vector) { return(sqrt((p_Vector.X - X) * (p_Vector.X - X) + (p_Vector.Y - Y) * (p_Vector.Y - Y) + (p_Vector.Z - Z) * (p_Vector.Z - Z))); }
};
