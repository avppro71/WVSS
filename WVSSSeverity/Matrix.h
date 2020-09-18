#pragma once
struct CMatrix
{
public:
 double M[4][4];
public:
 CMatrix() { Identity(); }
 void Identity() { Reset(); M[0][0] = M[1][1] = M[2][2] = M[3][3] = 1; }
 void Reset() { memset(&M, 0, sizeof(M)); }
 double * operator [](short p_nRowIndex) { return(M[p_nRowIndex]); }
 void Invert(CMatrix *p_pDest);
 double Determinant();
};
