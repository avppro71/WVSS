#ifndef AVP3D_H 
#define AVP3D_H

#include <QTOpenGL/QGLWidget>
#include "DrawPrimitive.h"
#include <vector>
#include <queue>
#include "WVSSGenericCxn.h"
#include "qevent.h"
#include <QTime>
#include "3DObj.h"
#include "PlaneLibrary.h"
#include "SurfaceArea.h"

// IMPORTANT! Local Coordinate System: X - North, Z - East, Y - up.

namespace GeographicLib {
class LocalCartesian;
class Geodesic;
}

class CDrawThread;
class AVP3D : public QGLWidget
{ 
 Q_OBJECT

private:
 enum eKeyboardMode { KPPV_KBMODE_NONE, KPPV_KBMODE_TEXT };

private: 
 GLfloat xRot;
 GLfloat yRot; 
 GLfloat zRot;
 GLfloat zTra;
 GLfloat nSca;
 QPoint m_ptrMousePosition;
 GLenum m_GLError;
 GLdouble m_dCenter[3], m_dEye[3];
 GLdouble m_dHeading_deg; // OpenGL X points North, Z points East, Y points up
 GLdouble m_dPitchAngle_deg; // 0 - horizontal
 GLdouble m_dDistanceToCenter;
 int m_nWidth, m_nHeight;
 GLdouble m_dWidthRatio, m_dHeightRatio;
 //GLfloat m_fCircleHeading, m_fCircleBank, m_fCirclePitch;
 double m_dTestVal[2];
 GLfloat m_fFrustumVal;
 char m_LogFileMessage[1024];
 CWVSSGenericCxn m_DataCxn;
 rWVSSGeoPoint m_GeoCenter;
 GeographicLib::LocalCartesian *m_pLC; // geo to planar compurations. X - East, Y - North
 GeographicLib::Geodesic *m_pGeo;
 
 // fps
 unsigned long m_lFrameCount;
 QTimer m_FPSTimer, m_DrawTimer, m_MouseTimer;
 std::vector<CSurfaceArea *>m_SurfaceAreas;
 
 bool m_bDrawTexture;
 // mouse
 std::queue<QMouseEvent>m_MouseEvents;
 ulong m_ulMouseOperStart;
 QTime m_MouseTime;
 bool m_bLastMouseEventRemoved;
 // thread
 //CDrawThread *m_pDrawThread;
 // obj load
 C3DObj m_3DObj;

 unsigned long m_lCurFace; // test
 short m_nCurFaceDir; // test
 unsigned long m_lStartTextCoord;
 unsigned long m_lObjIndexCount;
 bool m_bTransparentObject;
 bool m_bWireframe;
 short m_nCurAircraftModel; // test
 CPlaneLibrary m_PlaneLibrary;
 char m_DefaultSurfaceArea[32];
 QColor m_SkyColor;
 bool m_bDrawAxes;
 size_t m_nCurAreaIndex;
 bool m_bDebug;
 bool m_bDrawCompass;
 bool m_bDrawHelp;
 bool m_bDepthTest;
 bool m_bDebugText;
 QFont m_Font;
 eKeyboardMode m_KeyboardMode; // current operations mode. Introduced to handle complex shortcuts like in the Visual Studio

protected:
public:
 std::vector<QSharedPointer<CPrimitive_Line> >m_PlanarLines;
 std::vector<QSharedPointer<CPrimitive_Circle> >m_PlanarCircles;
 // circles other approach
 char *m_pCircles;
 unsigned long m_lCircles;
 char *m_pLines;
 unsigned long m_lLines;
 char *m_pRectangles;
 unsigned long m_lRectangles;
 char *m_pPlanes;
 unsigned short m_nPlanes;
 //CPrimitive_UnlimitedLine *m_pUnlimitedLines;
 //unsigned long m_lUnlimitedLines, m_lAvlUnlimitedLines;
 CPrimitive_TrackLine *m_pTrackLines;
 unsigned long m_lTrackLines, m_lAvlTrackLines;

private:
 bool GetSettings();
 QColor ParseColorStr(QString Str);
 //void scale_plus();
 //void scale_minus();
 //void rotate_up();
 //void rotate_down();
 //void rotate_left();
 //void rotate_right();
 //void translate_down();
 //void translate_up();     
 void defaultScene();  
 void drawAxis();
 //void getVertexAndTextureArray();
 //void getIndexArray();     
 //void drawFigure();
 // avp
 void DrawSurfaceArea();
 GLfloat GetSurfaceElevation();
 //void genTextures();
 void AVPPan(int p_nPanByX_deg, int p_nPanByY_deg, int p_nX, int p_nY);
 void RecalcEye();
 //void AVPRecalcPitchAndDistance();
 void AVPScalePlus();
 void AVPScaleMinus();
 void AVPRotate(GLfloat p_dRotateBy_degH, GLfloat p_dRotateBy_degV);
 void DrawDebugText(QPoint p_StartPoint, short p_nTextHeight);
 void AVPDrawAllObjects();
 void AddToLog(const char *p_pFormat, ...);
 void AddToInfo(QString p_Str);
 bool InitComms();
 // WVSS message handlers
 void OnAreaMessage(CWVSSArea *p_pMsg);
 // wake and aux data message: circles and lines
 void OnDataMessage(CWVSSData *p_pMsg);
 // aircraft position message
 void OnPlanePositionMessage(CWVSSPlanePosition *p_pMsg);
 // airplane list to load into view
 void OnPlaneListMessage(CWVSSPlaneTypeList *p_pMsg);
 // a handler to WVSS_msg_centertoplane
 void OnCenterToPlaneMessage(CWVSSCenterToPlane *p_pMsg, bool p_bRedraw);
 // a handler to WVSS_msg_track
 void OnTrackMessage(CWVSSTrack *p_pMsg);
 // mouse
 void ProcessMouseMoveEvent(QMouseEvent *p_Event);
 // textured planes
 void ClearSurfaceAreas();
 // openGL supported features
 bool GetOpenGLProcAddress(PROC &funcPtr, LPCSTR funcName);
 // draw object (plane)
 void DrawObject();
 // help overlay window
 void DrawHelp(QPainter *p_pPainter);
 // compass picture
 void DrawCompass(QPaintEvent *p_pEvent, QPainter *p_pPainter);
 // draw flat text in 3D. Text lines are separated by \n
 // set either p_pdCenter_planar to GLdouble[3] OR p_pStartPoint to a non NULL value
 void RenderTextBlock(GLdouble *p_pdCenter_planar, QPoint *p_pStartPoint, short p_nTextHeight, char *p_pText, short p_nVOffset);
 // air blocks on/off for all planes
 void ToggleAirblocks(bool p_bOn = true);

 // test
 bool LoadNextAircraftModel();
 void UseSurfaceArea(const char *p_pAreaName);
 // select stuff
 CTexturedPlane *m_pCurTexturedPlane;
 CTexturedPlane *GetNextTexturedPlane(bool p_bFirst = false);
 void Set3DObjDrawFlags();
 void Paint3DScene();
 void DrawTextOverlay(QString p_Str, QPainter *p_pPainter, QPoint p_LeftTop, QRect p_MarginRect);

protected:
 void initializeGL();         
 void resizeGL(int nWidth, int nHeight);  
 //void paintGL();                          
 void mousePressEvent(QMouseEvent* pe); 
 void mouseMoveEvent(QMouseEvent* pe);  
 void mouseReleaseEvent(QMouseEvent* pe); 
 void wheelEvent(QWheelEvent* pe);  
 void keyPressEvent(QKeyEvent* pe);       
 virtual void closeEvent(QCloseEvent *p_pEvent);
 virtual void paintEvent(QPaintEvent *p_pEvent);

public: 
 AVP3D(QWidget* parent = 0);
 ~AVP3D();

signals:
 void CheckMouseQ();

private slots:
 void OnCheckMouseQ();

public slots:
 void OnDataCxnMessage(QString p_Str);
 void OnDataCxnProcess(QSharedPointer<char>p_Data);
 void OnUpdateFps();
 void OnDrawTimer();
 void OnMouseTimer();
}; 
#endif 
