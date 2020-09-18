#ifndef CCOMPASS_H
#define CCOMPASS_H

class QPainter;
class QPoint;

class CCompass
{
private:
protected:
public:
 double m_dHeading_deg;
 short m_nRadius;

private:
protected:
public:
 CCompass();
 ~CCompass();
 void Draw(QPainter *p_pPainter, QPoint *p_pCenter);
 static void RotatePoints(QPoint *p_pPoints, short p_nPoints, QPoint *p_pCenter, double p_dAngleDeg);
};

#endif
