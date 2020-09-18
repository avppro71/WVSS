#include "stdafx.h"
#include "GeometricVector.h"
CGeometricVector3D CGeometricVector3D::VectorProduct(CGeometricVector3D p_Vector, bool p_bLeft)
{
 CGeometricVector3D ResGV;
 ResGV.X = (Y * p_Vector.Z - Z * p_Vector.Y) * (p_bLeft ? -1 : 1);
 ResGV.Y = -(X * p_Vector.Z - Z * p_Vector.X) * (p_bLeft ? -1 : 1);
 ResGV.Z = (X * p_Vector.Y - Y * p_Vector.X) * (p_bLeft ? -1 : 1);
 return(ResGV);
}
CGeometricVector3D CGeometricVector3D::VectorProduct(double p_dX, double p_dY, double p_dZ, bool p_bLeft)
{
 CGeometricVector3D GVector(p_dX, p_dY, p_dZ);
 return(VectorProduct(GVector, p_bLeft));
}
CGeometricVector3D CGeometricVector3D::VectorProduct(CGeometricVector3D p_Vector1, CGeometricVector3D p_Vector2, bool p_bLeft)
{
 CGeometricVector3D GVector1(p_Vector1), GVector2(p_Vector2);
 return(GVector1.VectorProduct(GVector2, p_bLeft));
}
double CGeometricVector3D::AngleDeg(CGeometricVector3D p_Vector)
{ 
 double dVal, dVal2;
 dVal = ScalarProduct(p_Vector) / (Module() * p_Vector.Module());
 dVal2 = acos(dVal);
 return(dVal2 * (dVal < 0 ? -1 : 1) * 180 / M_PI);
}