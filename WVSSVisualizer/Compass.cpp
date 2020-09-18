#include <qmath.h>
#include "qPainter.h"
#include "qsize.h"
#include "Compass.h"

CCompass::CCompass()
{
 m_dHeading_deg = 0;
 m_nRadius = 25;
}

CCompass::~CCompass()
{

}

void CCompass::Draw(QPainter *p_pPainter, QPoint *p_pCenter)
{
 QBrush BkBrush, Brush[2], TextBrush;
 QPen Pen;
 QRect DrawRect;
 QColor Color;
 QPoint Points[3], TickPoints[24];
 QFont Font;
 short i, Offset = 5, FontPixelSize = 15, TextWidth;
 char tstr[128];
 double dCompassPosition;

 for(i = 0; i < 24; i += 2) {
   TickPoints[i].setX(p_pCenter->x() + 0); 
   TickPoints[i].setY(p_pCenter->y() - m_nRadius); 
   TickPoints[i + 1].setX(p_pCenter->x() + 0); 
   TickPoints[i + 1].setY(p_pCenter->y() - m_nRadius + 2 * Offset + ((!(i % 3)) ? Offset : 0)); }
 
 DrawRect.setCoords(p_pCenter->x() - m_nRadius, p_pCenter->y() - m_nRadius, p_pCenter->x() + m_nRadius, p_pCenter->y() + m_nRadius);

 BkBrush = QBrush(QColor(255, 0, 0, 128));
 TextBrush = QBrush(QColor(255, 255, 0, 255));
 Brush[0] = QBrush(QColor(255, 0, 0, 255));
 Brush[1] = QBrush(QColor(255, 255, 255, 255));
 Pen = QPen(Qt::black);
 Pen.setWidth(1);
 
 Pen.setStyle(Qt::NoPen);
 p_pPainter->setPen(Pen);
 BkBrush.setStyle(Qt::SolidPattern);
 p_pPainter->setBrush(BkBrush);
 p_pPainter->drawEllipse(DrawRect.center(), m_nRadius - Offset, m_nRadius - Offset);

 BkBrush.setStyle(Qt::NoBrush);
 p_pPainter->setBrush(BkBrush);
 Pen.setStyle(Qt::SolidLine);
 p_pPainter->setPen(Pen);
 p_pPainter->drawEllipse(DrawRect.center(), m_nRadius, m_nRadius);

 for(i = 0; i < 12; i ++)
   RotatePoints(&(TickPoints[i * 2]), 2, p_pCenter, 30 * i);
 
 p_pPainter->drawLines(TickPoints, 12);

 Pen.setStyle(Qt::NoPen);
 p_pPainter->setPen(Pen);

 p_pPainter->setBrush(Brush[0]);
 Points[0].setX(p_pCenter->x() + Offset);
 Points[0].setY(p_pCenter->y());
 Points[1].setX(p_pCenter->x() - Offset);
 Points[1].setY(p_pCenter->y());
 Points[2].setX(p_pCenter->x());
 Points[2].setY(p_pCenter->y() - m_nRadius + Offset);
 
 dCompassPosition = 360.0 - m_dHeading_deg;
 if(dCompassPosition < 0) dCompassPosition += 360;

 RotatePoints(Points, 3, p_pCenter, dCompassPosition);
 p_pPainter->drawConvexPolygon(Points, 3);

 p_pPainter->setBrush(Brush[1]);
 Points[0].setX(p_pCenter->x() + Offset);
 Points[0].setY(p_pCenter->y());
 Points[1].setX(p_pCenter->x() - Offset);
 Points[1].setY(p_pCenter->y());
 Points[2].setX(p_pCenter->x());
 Points[2].setY(p_pCenter->y() + m_nRadius - Offset);

 RotatePoints(Points, 3, p_pCenter, dCompassPosition);
 p_pPainter->drawConvexPolygon(Points, 3);

 Pen.setColor(Qt::black);
 Pen.setStyle(Qt::SolidLine);
 p_pPainter->setPen(Pen);
 p_pPainter->setBrush(TextBrush);
 Font.setPixelSize(FontPixelSize);
 p_pPainter->setFont(Font);
 sprintf_s(tstr, sizeof(tstr), "%.0lf", m_dHeading_deg);
 QFontMetrics FontMetrics(Font);
 TextWidth = FontMetrics.width(tstr) + 4;

 DrawRect.setCoords(p_pCenter->x() - TextWidth / 2, p_pCenter->y() - FontPixelSize / 2, p_pCenter->x() + TextWidth / 2, p_pCenter->y() + FontPixelSize / 2 + 2);
 p_pPainter->drawRect(DrawRect);
 p_pPainter->drawText(DrawRect, Qt::AlignCenter, QString::fromLocal8Bit(tstr));
}

void CCompass::RotatePoints(QPoint *p_pPoints, short p_nPoints, QPoint *p_pCenter, double p_dAngleDeg)
{
 short i;
 double dX, dY;

 float s = sin(p_dAngleDeg * M_PI / 180);
 float c = cos(p_dAngleDeg * M_PI / 180);

 for(i = 0; i < p_nPoints; i ++) {
   dX = p_pPoints[i].x() - p_pCenter->x();
   dY = p_pPoints[i].y() - p_pCenter->y();
   p_pPoints[i].setX(dX * c - dY * s + p_pCenter->x());
   p_pPoints[i].setY(dX * s + dY * c + p_pCenter->y());
   }
}
