#include <qmath.h>
#include "qdir.h"
#include "AVP3D.h"
#include <GL/glu.h>
#define GEOGRAPHICLIB_SHARED_LIB 1
#include "GeographicLib/LocalCartesian.hpp"
#include "GeographicLib\geodesic.hpp"
#include "DrawThread.h"
#include <QXmlStreamReader>
#include "TexturedPlane.h"
#include "Compass.h"

#pragma warning(disable : 4996)

const static double k = M_PI / 180; // глобальная переменная

PFNGLGENVERTEXARRAYSPROC glGenVertexArrays;

AVP3D::AVP3D(QWidget* parent/*= 0*/) : QGLWidget(parent) 
{ 
 //bool bVal;
 QFont Font;
 CPrimitive_Plane::m_pPlaneLibrary = &m_PlaneLibrary;

 setAutoFillBackground(false);

 m_GeoCenter.m_dLat = m_GeoCenter.m_dLong = m_GeoCenter.m_dH = 0;
 GeographicLib::Geocentric Earth = GeographicLib::Geocentric::WGS84();
 //GeographicLib::Geocentric Earth = GeographicLib::Geocentric(30000, 0); //63781.37, 0);
 m_pLC = new GeographicLib::LocalCartesian(m_GeoCenter.m_dLat, m_GeoCenter.m_dLong, 0, Earth);
 //m_pGeo = new GeographicLib::Geodesic(GeographicLib::Constants::WGS84_a(), GeographicLib::Constants::WGS84_f());
 m_pGeo = new GeographicLib::Geodesic(Earth.MajorRadius(), Earth.Flattening());
 CTexturedPlane::m_pGeo = m_pGeo;
 CTexturedPlane::m_pLC = m_pLC;
 CTexturedPlane::m_EarthColor = QColor(255, 255, 255);
 CSurfaceArea::m_pLC = m_pLC;
 CSurfaceArea::m_nBlankEarthTiles = 0;

 m_bDebug = false;
 m_bDrawAxes = false;
 m_bDrawHelp = false;
 m_bDepthTest = true;
 m_bDebugText = false;
 m_SkyColor = QColor(0, 0, 128);
 strcpy(m_DefaultSurfaceArea, "MEM");
 m_nCurAreaIndex = 0;
 GetSettings();

 //bVal = GetOpenGLProcAddress((PROC&)glGenVertexArrays, "glGenVertexArrays");

 QObject::connect(&m_FPSTimer, SIGNAL(timeout()), this, SLOT(OnUpdateFps()));
 QObject::connect(&m_DrawTimer, SIGNAL(timeout()), this, SLOT(OnDrawTimer()));
 QObject::connect(&m_MouseTimer, SIGNAL(timeout()), this, SLOT(OnMouseTimer()));

 //m_pDrawThread = new CDrawThread(this);
 //connect(m_pDrawThread, &CDrawThread::finished, m_pDrawThread, &QObject::deleteLater);
 //m_pDrawThread->start();

 QObject::connect(this, SIGNAL(CheckMouseQ()), this, SLOT(OnCheckMouseQ()));

 m_nCurAircraftModel = 10000;
 
 m_lCurFace = 104;
 m_nCurFaceDir = 1;
 m_lStartTextCoord = 0;
 m_bTransparentObject = false;
 m_bWireframe = false;

 //CPrimitive_Line *pLine = new CPrimitive_Line;
 //if(pLine) {
 //  pLine->m_bRecalcFromGeo = false;
 //  m_PlanarLines.push_back(QSharedPointer<CPrimitive_Line>(pLine)); }

 m_bDrawTexture = true;
 m_pCircles = NULL;
 m_lCircles = 0;
 m_pLines = NULL;
 m_lLines = 0;
 m_pRectangles = NULL;
 m_lRectangles = 0;
 m_pPlanes = NULL;
 m_nPlanes = 0;
 //m_pUnlimitedLines = NULL;
 //m_lUnlimitedLines = m_lAvlUnlimitedLines = 0;

 m_pTrackLines = NULL;
 m_lTrackLines = m_lAvlTrackLines = 0;

 m_pCurTexturedPlane = NULL;

 /*
 CPrimitive_Circle *pCircle = new CPrimitive_Circle;
 if(pCircle) {
   pCircle->m_bRecalcFromGeo = false;
   m_PlanarCircles.push_back(QSharedPointer<CPrimitive_Circle>(pCircle)); }
 */

 CDrawPrimitive::m_pGLWidget = this;

 defaultScene();
 InitComms();
 m_FPSTimer.start(1000);
 //qDebug() << "constructor done";
 m_bDrawCompass = true;

 m_Font.setPixelSize(12);
 //setFont(m_Font);
 m_KeyboardMode = KPPV_KBMODE_NONE;
 
 Font.setPixelSize(20);
 CPrimitive_Plane::SetFont(Font);
} 

AVP3D::~AVP3D() 
{
 if(m_pLC) delete m_pLC;
 if(m_pGeo) delete m_pGeo;
 if(m_pCircles) delete [] m_pCircles;
 if(m_pRectangles) delete [] m_pRectangles;
 if(m_pLines) delete [] m_pLines;
 if(m_pPlanes) delete [] m_pPlanes;
 //if(m_pUnlimitedLines) delete [] m_pUnlimitedLines;
 if(m_pTrackLines) delete [] m_pTrackLines;

 //ClearTexturedPlanes();
}

void AVP3D::closeEvent(QCloseEvent *p_pEvent)
{
 //m_pDrawThread->Stop();
 /*
 AddToInfo("MainWindow: Close event"); 

 m_RawDataCxn.Stop();
 m_ARMCxn.Stop();

 QApplication::processEvents(QEventLoop::AllEvents, 1000); // to allow messages from other threads to be displayed ahead of the "exiting"
 AddToInfo("MainWindow: Application exiting...");
 QApplication::processEvents(QEventLoop::AllEvents, 100);
 QThread::msleep(1000);

 //p_pEvent->ignore();
 */
 m_3DObj.Unload(context());
 ClearSurfaceAreas();
 m_DataCxn.Stop();
 p_pEvent->accept();
}

void AVP3D::AddToLog(const char *p_pFormat, ...)
{
 size_t Len;
 va_list ArgList;
 if(!p_pFormat) return;
 va_start(ArgList, p_pFormat);
 vsprintf(m_LogFileMessage, p_pFormat, ArgList);
 va_end(ArgList);
 Len = strlen(m_LogFileMessage);
 strcpy(m_LogFileMessage + Len, "\n");
 Len ++;
 //m_LogFile.write(m_LogFileMessage, Len);
 //MainForm->AddToStatus(m_Messsge);
}

void AVP3D::AddToInfo(QString p_Str)
{
 setWindowTitle(p_Str);
 //if(ui.MessageListWidget->count() > (int)m_lMessageLines) ui.MessageListWidget->clear();
 //ui.MessageListWidget->addItem(p_Str);
 //ui.MessageListWidget->scrollToBottom();
}

void AVP3D::OnUpdateFps()
{
 char tstr[256];
 //CPathParts FileParts;
 
 if(!m_bDebug) return;
 //
 //FileParts = m_3DObj.GetFileParts();
 //sprintf(tstr, "3DObj File=%s", FileParts.m_FileName);
 //if(m_bDebug)
 //  setWindowTitle(tstr);

 float fDX, fDZ, fElevation;

 fDX = fDZ = fElevation = 0;
 //if(m_TexturedPLanes.size())
 //  m_TexturedPLanes[m_nCurTexturedObject]->GetOffsetNumbersFromOldVis(&fDX, &fDZ, &fElevation);
 //sprintf(tstr, "%ld FPS, CurTexturedObj=%d, DX=%.0f, DZ=%.0f, Elevation=%.0f, Face #%ld, StartTexCoord=%ld", m_lFrameCount, m_nCurTexturedObject, fDX, fDZ, fElevation, m_lCurFace, m_lStartTextCoord);
 QString Str = m_DataCxn.GetTCPIPSettings();
 sprintf(tstr, "WVSS Visualizer on %s. %ld FPS", (LPCSTR)Str.toLocal8Bit(), m_lFrameCount);
 setWindowTitle(tstr);
 m_lFrameCount = 0;
}

void AVP3D::OnDrawTimer()
{
 AVPRotate(5, 0);
 repaint();
}

void AVP3D::initializeGL()
{
 QGLFormat fmt;
 fmt.setSampleBuffers(true);
 fmt.setSamples(8); //2, 4, 8, 16
 QGLFormat::setDefaultFormat(fmt);

 //glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);

 //qglClearColor(QColor(127, 199, 255)); 
 qglClearColor(m_SkyColor);
 //glEnable(GL_DEPTH_TEST);  // устанавливает режим проверки глубины пикселей
 glShadeModel(GL_FLAT);    // отключает режим сглаживания цветов 
 
 //glDisable(GL_CULL_FACE);  // устанавливаем режим, когда строятся только внешние поверхности. 180627 don't enable it here, it will disallow standard painter calls

 //glEnable(GL_TEXTURE_2D); 
 //glEnable(GL_MULTISAMPLE); 
 //glDisable(GL_POLYGON_SMOOTH);

 //getVertexAndTextureArray(); // определить массив вершин
 //getIndexArray();  // определить массив индексов вершин
 //genTextures();

	// good
 //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR); //GL_LINEAR);
 //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_NEAREST); //GL_LINEAR);
 //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
 //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE); 
 // end good

 //glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL); //GL_MODULATE);

 UseSurfaceArea(m_DefaultSurfaceArea);
   
 glEnableClientState(GL_VERTEX_ARRAY); // активизация массива вершин
 //glEnableClientState(GL_COLOR_ARRAY);  // активизация массива цветов вершин
 glEnableClientState(GL_TEXTURE_COORD_ARRAY);
 glEnableClientState(GL_INDEX_ARRAY);
 //LoadNextAircraftModel();
}

void AVP3D::resizeGL(int nWidth, int nHeight)
{ 
 //GLfloat fFrustumVal = 1.0; //0.3;
 m_fFrustumVal = (GLfloat)0.3;

 glMatrixMode(GL_PROJECTION); // устанавливает текущей проекционную матрицу
 glLoadIdentity();            // присваивает проекционной матрице единичную матрицу
 
 // отношение высоты окна виджета к его ширине
 GLfloat ratio=(GLfloat)nHeight/(GLfloat)nWidth;
 //GLfloat BaseCutoffValue = 1.0f;
   
 // мировое окно
 //if (nWidth>=nHeight)
 //   // параметры видимости ортогональной проекции
 //   glOrtho(-BaseCutoffValue/ratio, BaseCutoffValue/ratio, -BaseCutoffValue, BaseCutoffValue, -BaseCutoffValue, BaseCutoffValue); 
 //else
 //   glOrtho(-BaseCutoffValue, BaseCutoffValue, -BaseCutoffValue*ratio, BaseCutoffValue*ratio, -BaseCutoffValue, BaseCutoffValue);      
 // плоскости отсечения (левая, правая, верхняя, нижняя, передняя, задняя)
   
 m_nWidth = nWidth;
 m_nHeight = nHeight;
 // параметры видимости перспективной проекции
 if (nWidth >= nHeight) {
   m_dWidthRatio = 1.0/ratio;
   m_dHeightRatio = 1,0;
   glFrustum(-m_fFrustumVal/ratio, m_fFrustumVal/ratio, -m_fFrustumVal, m_fFrustumVal, 1, 10000000.0);
   //glOrtho(-1.0/ratio, 1.0/ratio, -1.0, 1.0, -1, 1);
   }
 else {
   m_dWidthRatio = 1.0;
   m_dHeightRatio = 1.0/ratio;
   glFrustum(-m_fFrustumVal, m_fFrustumVal, -m_fFrustumVal*ratio, m_fFrustumVal*ratio, 1, 10000000.0);
   //glOrtho(-1.0, 1.0, -1.0*ratio, 1.0*ratio, -1, 1);
   // плоскости отсечения (левая, правая, верхняя, нижняя, ближняя, дальняя)
   }
  
 // поле просмотра
 glViewport(0, 0, (GLint)nWidth, (GLint)nHeight);
}

//void AVP3D::paintGL()
//{ 
// Paint3DScene();
//}

void AVP3D::Paint3DScene()
{
 glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // очистка буфера изображения и глубины

 glMatrixMode(GL_MODELVIEW); 
 glLoadIdentity();
 gluLookAt(m_dEye[0], m_dEye[1], m_dEye[2], m_dCenter[0], m_dCenter[1], m_dCenter[2], 0.0, 1.0, 0.0); // good
 glDisable(GL_TEXTURE_2D);
 if(m_bDrawAxes) drawAxis();
 
 DrawObject();
 
 glEnable(GL_DEPTH_TEST);
 glEnable(GL_CULL_FACE);
 glDepthFunc(GL_ALWAYS);

 DrawSurfaceArea();

 glDepthFunc(GL_LESS);
 
 if(m_bDepthTest) glEnable(GL_DEPTH_TEST);
 else glDisable(GL_DEPTH_TEST);

 AVPDrawAllObjects();

 glDisable(GL_CULL_FACE);
 glDisable(GL_DEPTH_TEST);

 if(m_bDebugText)
   DrawDebugText(QPoint(5, 15), 15);
 
 m_lFrameCount ++;
}  

void AVP3D::defaultScene() // наблюдение сцены по умолчанию
{
 // начальные значения данных-членов класса
 xRot=0; yRot=0; zRot=0; zTra=0; nSca=1; 

 // передает дальше указатель на объект parent
 m_dEye[0] = 0; // will be recomputed
 m_dEye[1] = 0; // will be recomputed
 m_dEye[2] = 0; // will be recomputed
 m_dCenter[0] = -18.990526421676059;
 m_dCenter[1] = 0;
 m_dCenter[2] = 18.153069371726712;

 m_dHeading_deg = 0; //143.74249958992004;
 m_dPitchAngle_deg = -51.529183804988861;
 m_dDistanceToCenter = 370187.31178959907;
 
 RecalcEye();

 //m_dDistanceToCenter = (m_dCenter[1] - m_dEye[1]) / sin(m_dPitchAngle_deg * M_PI / 180);

 //m_fCircleHeading = 0;
 //m_fCircleBank = 0;
 //m_fCirclePitch = 0;

 /*
 m_PlanarLines[0].data()->m_dX[0] = 0;
 m_PlanarLines[0].data()->m_dY[0] = 0;
 m_PlanarLines[0].data()->m_dZ[0] = 0;
 m_PlanarLines[0].data()->m_dX[1] = 1;
 m_PlanarLines[0].data()->m_dY[1] = 0.2;
 m_PlanarLines[0].data()->m_dZ[1] = 0;

 m_PlanarCircles[0].data()->m_dCenter_planar[0] = 1;
 m_PlanarCircles[0].data()->m_dCenter_planar[1] = 0;
 m_PlanarCircles[0].data()->m_dCenter_planar[2] = 0;
 m_PlanarCircles[0].data()->m_fHeading = 0;
 */

 memset(m_dTestVal, 0, sizeof(m_dTestVal));
}

void AVP3D::mousePressEvent(QMouseEvent *p_pEvent) // нажатие клавиши мыши
{
 //qDebug() << "mousePressEvent";

 if(m_MouseEvents.size()) {
   //qDebug() << "m_MouseEvents not empty! " << m_MouseEvents.size() << " elements";
   while(m_MouseEvents.size())
     m_MouseEvents.pop();
   }
 
 m_ptrMousePosition = p_pEvent->pos();  
 m_ulMouseOperStart = p_pEvent->timestamp();
 m_MouseTime.start();
 m_MouseTimer.start(10);
} 

void AVP3D::mouseReleaseEvent(QMouseEvent *p_pEvent) // отжатие клавиши мыши
{ 
 //qDebug() << "mouseReleaseEvent";
 m_MouseTimer.stop();
 //if(m_bLastMouseEventRemoved)
 //  ProcessMouseMoveEvent(p_pEvent);
 m_ulMouseOperStart = 0;
} 

// изменение положения стрелки мыши
void AVP3D::mouseMoveEvent(QMouseEvent *p_pEvent) 
{   
 //qDebug() << "mouseMoveEvent " << p_pEvent->timestamp();
 if(m_ulMouseOperStart) {
   //ProcessMouseMoveEvent(p_pEvent);
   m_MouseEvents.push(QMouseEvent(*p_pEvent));
   }

 //OnMouseTimer();
 //emit(CheckMouseQ());
 //ProcessMouseMoveEvent(p_pEvent);
}

void AVP3D::OnCheckMouseQ()
{
 ulong MouseEventTime, TimeElapsed;
 static bool InHere = false;
 bool bVal;

 if(InHere)
   return;
 InHere = true;
 //qDebug() << "OnCheckMouseQ";

 while(!m_MouseEvents.empty()) {
   MouseEventTime = m_MouseEvents.front().timestamp();
   TimeElapsed = m_MouseTime.elapsed();
   //qDebug() << "OnCheckMouseQ: m_ulMouseOperStart=" << m_ulMouseOperStart << ", MouseEventTime=" << MouseEventTime << ", TimeElapsed=" << TimeElapsed << ", diff=" << MouseEventTime - m_ulMouseOperStart;
   bVal = false;
   //if(m_MouseEvents.front().timestamp() - m_ulMouseOperStart < m_MouseTime.elapsed() && m_MouseEvents.size() > 1) {
   if(m_MouseEvents.size() > 1) {
     m_bLastMouseEventRemoved = (m_MouseEvents.size() == 1);
     //qDebug() << "Mouse event skipped";
     }
   else {
     bVal = true;
     ProcessMouseMoveEvent(&m_MouseEvents.front());
     }
   m_MouseEvents.pop();
   if(bVal) break;
   //emit(CheckMouseQ());
   }
 InHere = false;
}

void AVP3D::OnMouseTimer()
{
 OnCheckMouseQ();
}

void AVP3D::ProcessMouseMoveEvent(QMouseEvent *p_Event)
{
 bool bRotate = false;
 int iVal[2];

 //qDebug() << "ProcessMouseMoveEvent";
 bRotate = (p_Event->modifiers() & Qt::ShiftModifier);
 iVal[0] = p_Event->x() - m_ptrMousePosition.x();
 iVal[1] = p_Event->y() - m_ptrMousePosition.y();
 if(bRotate)
   AVPRotate((GLfloat)360 * iVal[0] / width(), (GLfloat)-360 * iVal[1] / height());
 else
   AVPPan(iVal[0], iVal[1], p_Event->x(), p_Event->y());
 repaint(); // обновление изображения
 //m_pDrawThread->Do();
 m_ptrMousePosition = p_Event->pos();
}

// вращение колёсика мыши
void AVP3D::wheelEvent(QWheelEvent* pe) 
{ 
 //if ((pe->delta())>0) scale_plus(); else if ((pe->delta())<0) scale_minus();   
 if((pe->delta()) > 0) AVPScalePlus();
 else if((pe->delta()) < 0) AVPScaleMinus();    
 repaint();
}

// нажатие определенной клавиши
void AVP3D::keyPressEvent(QKeyEvent *p_pEvent) 
{  
 bool bRotate = false;
 float fDX, fDZ, fElevation;
 double dAreaCenterLat, dAreaCenterLong;
 bool Shift, Control;
 QFont Font;

 Control = (p_pEvent->modifiers() & Qt::ControlModifier);
 Shift = (p_pEvent->modifiers() & Qt::ShiftModifier);

 if(m_SurfaceAreas.size() > m_nCurAreaIndex) {
   dAreaCenterLat = m_SurfaceAreas[m_nCurAreaIndex]->GetAreaCenterLat();
   dAreaCenterLong = m_SurfaceAreas[m_nCurAreaIndex]->GetAreaCenterLong(); }

 bRotate = (p_pEvent->modifiers() & Qt::ShiftModifier);

 int iVal = p_pEvent->key();
 switch(p_pEvent->key()) {
   case Qt::Key_Space:
     defaultScene();   // возвращение значений по умолчанию
   break;
   case Qt::Key_Escape:
     if(m_KeyboardMode == KPPV_KBMODE_NONE)
       close();
     m_KeyboardMode = KPPV_KBMODE_NONE;
   break;                                                           
   case Qt::Key_A:
     if(Control && Shift) {
       m_bDrawAxes = !m_bDrawAxes;
       CTexturedPlane::m_bDrawAxes = m_bDrawAxes; }
     else if(m_bDebug && Control)
       CPrimitive_Plane::m_bShowAirblock = !CPrimitive_Plane::m_bShowAirblock;
     else if(bRotate)
       AVPRotate(-5, 0); 
     //else
       //AVPPan(-1, 0);
     break;
   case Qt::Key_S:
     //if(!m_bDebug) return;
     if(bRotate)
       AVPRotate(5, 0); 
     //else
       //AVPPan(1, 0);
     break;
   case Qt::Key_Plus:
     if(m_KeyboardMode == KPPV_KBMODE_TEXT) {
       Font = CPrimitive_Plane::GetFont();
       Font.setPixelSize(Font.pixelSize() + 1);
       CPrimitive_Plane::SetFont(Font);
       m_KeyboardMode = KPPV_KBMODE_NONE; }
     else
       AVPScalePlus();
     break;
   case Qt::Key_Minus:
     if(m_KeyboardMode == KPPV_KBMODE_TEXT) {
       Font = CPrimitive_Plane::GetFont();
       Font.setPixelSize(Font.pixelSize() - 1);
       CPrimitive_Plane::SetFont(Font);
       m_KeyboardMode = KPPV_KBMODE_NONE; }
     else
       AVPScaleMinus();
     break;
   case Qt::Key_1:
   case Qt::Key_2:
   case Qt::Key_3:
   case Qt::Key_4:
   case Qt::Key_5:
     if(!m_bDebug) break;
     if(m_KeyboardMode == KPPV_KBMODE_TEXT)
       CPrimitive_Plane::SetTextColorIndex(p_pEvent->key() - Qt::Key_1);
     break;
   case Qt::Key_Q:
     if(!m_bDebug) break;
     //m_PlanarCircles[0].data()->m_fHeading ++;
     if(m_pCurTexturedPlane) m_pCurTexturedPlane->m_fSurfaceRotateX ++;
     break;
   case Qt::Key_W:
     if(!m_bDebug) break;
     //m_PlanarCircles[0].data()->m_fHeading --;
     if(m_pCurTexturedPlane) m_pCurTexturedPlane->m_fSurfaceRotateX --;
     break;
   case Qt::Key_E:
     if(!m_bDebug) break;
     if(m_pCurTexturedPlane) m_pCurTexturedPlane->m_fSurfaceRotateZ ++;
     //m_PlanarCircles[0].data()->m_fBank ++;
     break;
   case Qt::Key_R:
     if(!m_bDebug) break;
     if(m_pCurTexturedPlane) m_pCurTexturedPlane->m_fSurfaceRotateZ --;
     //m_PlanarCircles[0].data()->m_fBank --;
     break;
   case Qt::Key_T:
     if(!m_bDebug) break;
     //m_PlanarCircles[0].data()->m_fPitch ++;
     if(Control)
       m_KeyboardMode = KPPV_KBMODE_TEXT;
     break;
   case Qt::Key_Y:
     if(!m_bDebug) break;
     //m_PlanarCircles[0].data()->m_fPitch --;
     m_dCenter[1] = -184000;
     //m_dEye[1] = m_dCenter[1] - m_dDistanceToCenter * sin(m_dPitchAngle_deg * M_PI / 180);
     m_dDistanceToCenter = (m_dCenter[1] - m_dEye[1]) / sin(m_dPitchAngle_deg * M_PI / 180);
     //m_dDistanceToCenter = sqrt(pow(m_dEye[0] - m_dCenter[0], 2) + pow(m_dEye[1] - m_dCenter[1], 2) + pow(m_dEye[2] - m_dCenter[2], 2));
     RecalcEye();
     break;
   case Qt::Key_O:
     if(m_DrawTimer.isActive())
       m_DrawTimer.stop();
     else
       m_DrawTimer.start();
     break;
   case Qt::Key_F1:
     //if(!m_bDebug) break;
     if(!Control && !Shift)
       m_bDrawTexture = !m_bDrawTexture;
     else if(Control && Shift && m_bDebug)
      m_bDebugText = !m_bDebugText;
     break;
   case Qt::Key_F2:
     if(!m_bDebug) break;
     if(Control)
       GetNextTexturedPlane(true);
     else
       GetNextTexturedPlane();
     break;
   case Qt::Key_F3:
     if(!m_bDebug) break;
     m_lStartTextCoord += 2 * m_nCurFaceDir;
     break;
   case Qt::Key_F4:
     if(!m_bDebug) break;
     m_bTransparentObject = !m_bTransparentObject;
     Set3DObjDrawFlags();
     break;
   case Qt::Key_F5:
     if(!m_bDebug) break;
     m_bWireframe = !m_bWireframe;
     Set3DObjDrawFlags();
     break;
   case Qt::Key_F6:
     if(!m_bDebug) break;
     LoadNextAircraftModel();
     break;
   /*
   case Qt::Key_F7:
     if(!m_bDebug) break;
     if(m_pCurTexturedPlane)
       m_pCurTexturedPlane->m_bAlternateSurface = !m_pCurTexturedPlane->m_bAlternateSurface;
     break;
   */
   case Qt::Key_F8:
     if(!m_bDebug) break;
     m_bDepthTest = !m_bDepthTest;
     //if(glIsEnabled(GL_DEPTH_TEST)) glDisable(GL_DEPTH_TEST);
     //else glEnable(GL_DEPTH_TEST);
     break;
   case Qt::Key_Up:
     if(m_KeyboardMode == KPPV_KBMODE_TEXT) {
       ToggleAirblocks();
       m_KeyboardMode = KPPV_KBMODE_NONE;
       break; }
     else if(m_pCurTexturedPlane) {
       m_pCurTexturedPlane->GetOffsetNumbersFromOldVis(&fDX, &fDZ, &fElevation);
       fDZ ++;
       m_pCurTexturedPlane->SetOffsetNumbersFromOldVis(fDX, fDZ, fElevation);
       m_pCurTexturedPlane->Recalc(dAreaCenterLat, dAreaCenterLong); }
     else {
       if(bRotate)
         AVPRotate(0, -5); 
       else
       AVPPan(0, 10, m_nWidth / 2, m_nHeight / 2);
       }
     break;
   case Qt::Key_Down:
     if(m_KeyboardMode == KPPV_KBMODE_TEXT) {
       ToggleAirblocks(false);
       m_KeyboardMode = KPPV_KBMODE_NONE;
       break; }
     else if(m_pCurTexturedPlane) {
       m_pCurTexturedPlane->GetOffsetNumbersFromOldVis(&fDX, &fDZ, &fElevation);
       fDZ --;
       m_pCurTexturedPlane->SetOffsetNumbersFromOldVis(fDX, fDZ, fElevation);
       m_pCurTexturedPlane->Recalc(dAreaCenterLat, dAreaCenterLong); }
     else {
       if(bRotate)
         AVPRotate(0, 5); 
       else
         AVPPan(0, -10, m_nWidth / 2, m_nHeight / 2);
       }
     break;         
   case Qt::Key_Left:
     if(m_pCurTexturedPlane) {
       m_pCurTexturedPlane->GetOffsetNumbersFromOldVis(&fDX, &fDZ, &fElevation);
       fDX --;
       m_pCurTexturedPlane->SetOffsetNumbersFromOldVis(fDX, fDZ, fElevation);
       m_pCurTexturedPlane->Recalc(dAreaCenterLat, dAreaCenterLong); }
     else {
       if(bRotate)
         AVPRotate(5, 0); 
       else
         AVPPan(-10, 0, m_nWidth / 2, m_nHeight / 2);
       }
     break;
   case Qt::Key_Right:
     if(m_pCurTexturedPlane) {
       m_pCurTexturedPlane->GetOffsetNumbersFromOldVis(&fDX, &fDZ, &fElevation);
       fDX ++;
       m_pCurTexturedPlane->SetOffsetNumbersFromOldVis(fDX, fDZ, fElevation);
       m_pCurTexturedPlane->Recalc(dAreaCenterLat, dAreaCenterLong); }
     else {
       if(bRotate)
         AVPRotate(-5, 0); 
       else
         AVPPan(10, 0, m_nWidth / 2, m_nHeight / 2);
       }
     break;                           
   case Qt::Key_PageUp:
     if(m_pCurTexturedPlane) {
       m_pCurTexturedPlane->GetOffsetNumbersFromOldVis(&fDX, &fDZ, &fElevation);
       fElevation ++;
       m_pCurTexturedPlane->SetOffsetNumbersFromOldVis(fDX, fDZ, fElevation);
       m_pCurTexturedPlane->Recalc(dAreaCenterLat, dAreaCenterLong); }
     break;
   case Qt::Key_PageDown:
     if(m_pCurTexturedPlane) {
       m_pCurTexturedPlane->GetOffsetNumbersFromOldVis(&fDX, &fDZ, &fElevation);
       fElevation --;
       m_pCurTexturedPlane->SetOffsetNumbersFromOldVis(fDX, fDZ, fElevation);
       m_pCurTexturedPlane->Recalc(dAreaCenterLat, dAreaCenterLong); }
     break;                           
   case Qt::Key_N:
     if(!m_bDebug) break;
     if(p_pEvent->modifiers() & Qt::ControlModifier)
       m_nCurFaceDir *= -1;
     else  
       m_lCurFace = 0;
     Set3DObjDrawFlags();
     break;
   case Qt::Key_M:
     if(!m_bDebug) break;
     m_lCurFace += m_nCurFaceDir;
     Set3DObjDrawFlags();
     break;
   case Qt::Key_V:
     if(!m_bDebug) break;
     if(m_pCurTexturedPlane)
       m_pCurTexturedPlane->m_bVisible = !m_pCurTexturedPlane->m_bVisible;
     break;
   case Qt::Key_U:
     if(bRotate) {
       m_dHeading_deg = 0;
       RecalcEye(); }
     break;
   case Qt::Key_C:
     if(Control)
       m_bDrawCompass = !m_bDrawCompass;
     break;
   case Qt::Key_H:
     if(Control)
       m_bDrawHelp = !m_bDrawHelp;
     break;
   case Qt::Key_D:
    if(Control && Shift)
      m_bDebug = !m_bDebug;
   }
   
 repaint(); // обновление изображения
}

void AVP3D::drawAxis() // построить оси координат
{
 glLineWidth(3.0f); 
 glBegin(GL_LINES); // построение линии
    glColor4f(1.00f, 0.00f, 0.00f, 1.0f);
    glVertex3f(1000.0f,  0.0f,  0.0f); // первая точка
    glVertex3f(0.0f,  0.0f,  0.0f); // вторая точка
    glColor4f(0.00f, 1.00f, 0.00f, 1.0f);
    glVertex3f(0.0f,  1000.0f,  0.0f);
    glVertex3f(0.0f, 0.0f,  0.0f);
    glColor4f(0.00f, 0.00f, 1.00f, 1.0f);
    glVertex3f(0.0f,  0.0f,  1000.0f);
    glVertex3f(0.0f,  0.0f, 0.0f);
 glEnd();  
}

void AVP3D::DrawObject()
{
 //m_3DObj.m_pObjDescArray[0].Material.m_bTexture = true;
 //m_3DObj.m_pObjDescArray[0].Material.m_TextureID = m_TextureID[2];

 if(m_3DObj.m_lObjDescCount) {
   m_3DObj.m_pObjDescArray[0].DrawFlags.m_bSelectFace = true;
   m_3DObj.m_pObjDescArray[0].DrawFlags.m_lSelectFaceNum = m_lCurFace;
   m_3DObj.m_pObjDescArray[0].DrawFlags.m_Alpha = 128;
   m_3DObj.m_pObjDescArray[0].DrawFlags.m_bTransparent = m_bTransparentObject;
   m_3DObj.m_pObjDescArray[0].DrawFlags.m_bWireframe = m_bWireframe; }
 m_3DObj.Draw();
}

GLfloat AVP3D::GetSurfaceElevation()
{
 size_t i;
 GLfloat fMaxElevation;
 float fCurElvation;

 if(!m_bDrawTexture) return(0);

 fMaxElevation = -10000;
 for(i = 0; i < m_SurfaceAreas.size(); i ++) {
   if(!m_SurfaceAreas[i]->m_bOn) continue;
   fCurElvation = m_SurfaceAreas[i]->GetMaxSurfaceElevation();
   if(fCurElvation > fMaxElevation)
     fMaxElevation = fCurElvation; }
 return(fMaxElevation);
}

void AVP3D::DrawSurfaceArea()
{
 size_t i;

 if(!m_bDrawTexture) return;

 for(i = 0; i < m_SurfaceAreas.size(); i ++) {
   if(m_SurfaceAreas[i]->m_bOn)
     m_SurfaceAreas[i]->Draw();
   }
}

void AVP3D::AVPPan(int p_nPanByX_pix, int p_nPanByY_pix, int p_nX, int p_nY)
{
 GLdouble dVal, dHeading, SumMove[3];
 
 //dVal = (GLdouble)p_nPanByX_pix * m_dWidthRatio / m_nWidth * m_dDistanceToCenter * 2;
 dVal = (GLdouble)p_nPanByX_pix * m_dWidthRatio / m_nWidth * m_dDistanceToCenter * 2 * m_fFrustumVal;
 dHeading = m_dHeading_deg;
 m_dEye[2] += dVal * cos(dHeading * M_PI / 180) * (-1);
 m_dCenter[2] += dVal * cos(dHeading * M_PI / 180) * (-1); 
 m_dEye[0] += dVal * sin(dHeading * M_PI / 180);
 m_dCenter[0] += dVal * sin(dHeading * M_PI / 180); 

 SumMove[0] = dVal * cos(dHeading * M_PI / 180) * (-1);
 SumMove[1] = dVal * sin(dHeading * M_PI / 180);

 // test
 //dVal /= abs(sin(m_dPitchAngle_deg));
 double dVal2 = m_ptrMousePosition.y() / ((double)m_nHeight / 2);
 //dVal2 = ((double)m_nHeight - p_nY) * 2 / (double)m_nHeight;
 //dVal2 = atan2(m_dDistanceToCenter, dVal2) * 180 / pi;
 //dVal2 = dVal2;
 m_dTestVal[0] = ((double)m_nHeight - p_nY) * 2 / (double)m_nHeight;
 m_dTestVal[1] = cos(m_dPitchAngle_deg * M_PI / 180) * m_dTestVal[0];
 //m_dTestVal = dVal2;
 //dVal *= (2 * dVal2);
 //dVal /= abs(sin(m_dPitchAngle_deg));
 //dVal /= (abs(sin(m_dPitchAngle_deg * M_PI / 180)) / m_dTestVal[0]);
 //dVal = dVal * abs((1 - m_dTestVal[1]));
 //dVal *= 2 * m_dTestVal[1];
 // end test
 
 //  * m_fFrustumVal
 //dVal = (GLdouble)p_nPanByY_pix * m_dWidthRatio / m_nWidth * m_dDistanceToCenter * 2;
 dVal = (GLdouble)p_nPanByY_pix * m_dWidthRatio / m_nWidth * m_dDistanceToCenter * 2 * m_fFrustumVal;
 dHeading = m_dHeading_deg + 90;
 if(dHeading > 360) dHeading -= 360;
 m_dEye[2] += dVal * cos(dHeading * M_PI / 180) * (-1);
 m_dCenter[2] += dVal * cos(dHeading * M_PI / 180) * (-1);
 m_dEye[0] += dVal * sin(dHeading * M_PI / 180);
 m_dCenter[0] += dVal * sin(dHeading * M_PI / 180);
 //AVPRecalcPitchAndDistance();

 SumMove[0] += dVal * cos(dHeading * M_PI / 180) * (-1);
 SumMove[1] += dVal * sin(dHeading * M_PI / 180);
 SumMove[2] = sqrt(pow(SumMove[0], 2) + pow(SumMove[1], 2));
 SumMove[2] = SumMove[2];

 /*
 SumMove[2] = sqrt(pow(m_dCenter[0], 2) + pow(m_dCenter[1], 2));
 dVal = 2 * M_PI * (SumMove[2] / 40000000);
 dVal = SumMove[2] / 6378137;
 dVal = 6378137 * (1 - cos(dVal));
 m_dCenter[1] = -dVal;
 m_dDistanceToCenter = (m_dCenter[1] - m_dEye[1]) / sin(m_dPitchAngle_deg * M_PI / 180);
 */
}

void AVP3D::RecalcEye()
{
 m_dEye[0] = m_dCenter[0] - m_dDistanceToCenter * cos(m_dHeading_deg * M_PI / 180) * cos(m_dPitchAngle_deg * M_PI / 180);
 m_dEye[2] = m_dCenter[2] - m_dDistanceToCenter * sin(m_dHeading_deg * M_PI / 180) * cos(m_dPitchAngle_deg * M_PI / 180);
 m_dEye[1] = m_dCenter[1] - m_dDistanceToCenter * sin(m_dPitchAngle_deg * M_PI / 180);
}

void AVP3D::AVPScalePlus()
{
 m_dDistanceToCenter /= 1.1;
 RecalcEye();
}

void AVP3D::AVPScaleMinus()
{
 m_dDistanceToCenter *= 1.1;
 RecalcEye();
}

void AVP3D::AVPRotate(GLfloat p_dRotateBy_degH, GLfloat p_dRotateBy_degV)
{
 m_dHeading_deg += p_dRotateBy_degH; 
 if(m_dHeading_deg > 360) m_dHeading_deg -= 360; 
 if(m_dHeading_deg < 0) m_dHeading_deg += 360; 

 m_dPitchAngle_deg += p_dRotateBy_degV;
 if(m_dPitchAngle_deg < -89) m_dPitchAngle_deg = -89;
 if(m_dPitchAngle_deg > 89) m_dPitchAngle_deg = 89;

 RecalcEye();
}

void AVP3D::DrawDebugText(QPoint p_StartPoint, short p_nTextHeight)
{
 char tstr[128];
 QPoint TextPoint;

 TextPoint.setX(p_StartPoint.x());
 TextPoint.setY(p_StartPoint.y());
 
 setFont(m_Font);
 glColor3ub(255, 255, 255);
 sprintf(tstr, "Heading=%.1lf", m_dHeading_deg);
 renderText(TextPoint.x(), TextPoint.y(), QString::fromLocal8Bit(tstr));
 
 TextPoint.setY(TextPoint.y() + p_nTextHeight);
 sprintf(tstr, "PitchAngle=%.1lf", m_dPitchAngle_deg);
 renderText(TextPoint.x(), TextPoint.y(), tstr);
 
 TextPoint.setY(TextPoint.y() + p_nTextHeight);
 sprintf(tstr, "Distance to center=%.1lf", m_dDistanceToCenter);
 renderText(TextPoint.x(), TextPoint.y(), tstr);
 
 TextPoint.setY(TextPoint.y() + p_nTextHeight);
 sprintf(tstr, "Camera=[%.1lf,%.1lf,%.1lf]", m_dEye[0], m_dEye[1], m_dEye[2]);
 renderText(TextPoint.x(), TextPoint.y(), tstr);
 
 TextPoint.setY(TextPoint.y() + p_nTextHeight);
 sprintf(tstr, "Center=[%.1lf,%.1lf,%.1lf]", m_dCenter[0], m_dCenter[1], m_dCenter[2]);
 renderText(TextPoint.x(), TextPoint.y(), tstr);
 
 TextPoint.setY(TextPoint.y() + 2 * p_nTextHeight);
 sprintf(tstr, "TestVal [%.3lf, %.3lf]", m_dTestVal[0], m_dTestVal[1]);
 renderText(TextPoint.x(), TextPoint.y(), tstr);

 (*tstr = '\0');
 switch(m_KeyboardMode) {
   case KPPV_KBMODE_TEXT: sprintf(tstr, "TEXT KEYBOARD MODE"); break;
   default: break; }
 if(*tstr) {
   TextPoint.setY(TextPoint.y() + p_nTextHeight);
   renderText(TextPoint.x(), TextPoint.y(), tstr); }

 TextPoint.setY(TextPoint.y() + p_nTextHeight);
 sprintf(tstr, "Air block font size: %d", CPrimitive_Plane::GetFont().pixelSize());
 renderText(TextPoint.x(), TextPoint.y(), tstr);
}

void AVP3D::AVPDrawAllObjects()
{
 size_t i;
 CPrimitive_Circle *pCircleDest;
 CPrimitive_Line *pLineDest;
 CPrimitive_Rectangle *pRectangleDest;
 CPrimitive_Plane *pPlaneDest;
 //CPrimitive_UnlimitedLine *pUnlimitedLineDest;
 CPrimitive_TrackLine *pTrackLineDest;

 //for(i = 0; i < m_Lines.size(); i ++)
 //  m_Lines[i].data()->Draw();
 //for(i = 0; i < m_Circles.size(); i ++)
 //  m_Circles[i].data()->Draw();
 for(i = 0; i < m_PlanarCircles.size(); i ++)
   m_PlanarCircles[i].data()->Draw();

 pCircleDest = (CPrimitive_Circle *)m_pCircles;
 for(i = 0; i < m_lCircles; i ++) {
   pCircleDest->Draw();
   ++pCircleDest; }

 pLineDest = (CPrimitive_Line *)m_pLines;
 for(i = 0; i < m_lLines; i ++) {
   pLineDest->Draw();
   ++pLineDest; }

 pRectangleDest = (CPrimitive_Rectangle *)m_pRectangles;
 for(i = 0; i < m_lRectangles; i ++) {
   pRectangleDest->Draw();
   ++pRectangleDest; }

 pPlaneDest = (CPrimitive_Plane *)m_pPlanes;
 for(i = 0; i < m_nPlanes; i ++) {
   pPlaneDest->Draw();
   ++pPlaneDest; }

 pTrackLineDest = (CPrimitive_TrackLine *)m_pTrackLines;
 for(i = 0; i < m_lTrackLines; i ++) {
   pTrackLineDest->Draw();
   ++pTrackLineDest; }

 //pPlaneDest = (CPrimitive_Plane *)m_pPlanes;
 //for(i = 0; i < m_nPlanes; i ++) {
 //  RenderTextBlock(pPlaneDest->m_dCenter_planar, NULL, 15, pPlaneDest->m_CallSign, 15);
 //  ++pPlaneDest; }
}

bool AVP3D::InitComms()
{
 bool bVal;

 m_DataCxn.SetThreadName("DataCxn");
 connect(&m_DataCxn, SIGNAL(Message(QString)), this, SLOT(OnDataCxnMessage(QString)));
 connect(&m_DataCxn, SIGNAL(Process(QSharedPointer<char>)), this, SLOT(OnDataCxnProcess(QSharedPointer<char>)));
 m_DataCxn.SetCxnType(true, CWVSSGenericCxn::SERVER, false, CWVSSGenericCxn::SERVER);
 //m_DataCxn.SetAddressPort("127.0.0.1", 1701, "127.0.0.1", 1702, "127.0.0.1", 1703);
 bVal = m_DataCxn.Start();
 return(bVal);
}

void AVP3D::OnDataCxnMessage(QString p_Str)
{
 AddToInfo(p_Str);
}

void AVP3D::OnDataCxnProcess(QSharedPointer<char>p_Data)
{
 CWVSSMsgHeader *pMsg = (CWVSSMsgHeader *)p_Data.data();
 
 switch(pMsg->msg_type) {
   case WVSS_msg_area:
     OnAreaMessage((CWVSSArea *)pMsg);
     break;
   case WVSS_msg_data:
     OnDataMessage((CWVSSData *)pMsg);
     break;
   case WVSS_msg_planepos:
     OnPlanePositionMessage((CWVSSPlanePosition *)pMsg);
     break;
   case WVSS_msg_loadplane:
     OnPlaneListMessage((CWVSSPlaneTypeList *)pMsg);
     break;
   case WVSS_msg_centertoplane:
     OnCenterToPlaneMessage((CWVSSCenterToPlane *)pMsg, true);
     break;
   case WVSS_msg_track:
     OnTrackMessage((CWVSSTrack *)pMsg);
     break;
   default: break; }
}

void AVP3D::OnAreaMessage(CWVSSArea *p_pMsg)
// called when we get a packet that with a new geo center. Recompute all appropriate objects from lat long to xyz.
{
 size_t i;
 CPrimitive_Circle *pCircleDest;
 CPrimitive_Line *pLineDest;
 CPrimitive_Rectangle *pRectangleDest;
 CPrimitive_Plane *pPlaneDest;
 //CPrimitive_UnlimitedLine *pUnlimitedLineDest;
 CPrimitive_TrackLine *pTrackLineDest;
 
 //CTexturedPlane *pTexturedPlane;
 //CTexturedPlane::rData TPData;
 //char tstr[256];

 //ClearTexturedPlanes();

 //qDebug() << "OnGeoCenterMessage";

 //m_GeoCenter = p_pMsg->m_Point;
 //m_pLC->Reset(m_GeoCenter.m_dLat, m_GeoCenter.m_dLong, 0); //m_GeoCenter.m_dH);
 // the line above does not use m_GeoCenter.m_dH because it's easier to reference 0 altitude
 // The value of m_GeoCenter.m_dH is used to position the textured surface object.
 //pTexturedPlane = new CTexturedPlane(context());
 //sprintf(tstr, "/textures/%s", p_pMsg->m_ImageName);
 //pTexturedPlane->Load(tstr, p_pMsg->m_lAreaSize_m);
 //pTexturedPlane->SetOffsetNumbersFromOldVis(p_pMsg->m_dX, p_pMsg->m_dZ, p_pMsg->m_Point.m_dH);
 
/*
 sprintf(tstr, "/textures/%s", p_pMsg->m_ImageName);
 pTexturedPlane = new CTexturedPlane;
 strcpy(TPData.m_FileName, tstr);
 TPData.m_fSize = p_pMsg->m_lAreaSize_m;
 TPData.m_fDX = p_pMsg->m_dX;
 TPData.m_fDZ = p_pMsg->m_dZ;
 TPData.m_fElevation = p_pMsg->m_Point.m_dH;
 if(pTexturedPlane) {
   pTexturedPlane->Set(&TPData);
   m_TexturedPLanes.push_back(pTexturedPlane); }

 if(pTexturedPlane)
   pTexturedPlane->Load(context());
*/

 
 //qDebug() << p_pMsg->m_AreaName;

 UseSurfaceArea(p_pMsg->m_AreaName); // this will recalculate m_pLC

 pCircleDest = (CPrimitive_Circle *)m_pCircles;
 for(i = 0; i < m_lCircles; i ++) {
   pCircleDest->Recalc(m_pLC);
   ++pCircleDest; }
 
 pLineDest = (CPrimitive_Line *)m_pLines;
 for(i = 0; i < m_lLines; i ++) {
   pLineDest->Recalc(m_pLC);
   ++pLineDest; }

 pRectangleDest = (CPrimitive_Rectangle *)m_pRectangles;
 for(i = 0; i < m_lRectangles; i ++) {
   pRectangleDest->Recalc(m_pLC);
   ++pRectangleDest; }

 pPlaneDest = (CPrimitive_Plane *)m_pPlanes;
 for(i = 0; i < m_nPlanes; i ++) {
   pPlaneDest->Recalc(m_pLC);
   ++pPlaneDest; }

 //pUnlimitedLineDest = (CPrimitive_UnlimitedLine *)m_pUnlimitedLines;
 //for(i = 0; i < m_lUnlimitedLines; i ++) {
 //  pUnlimitedLineDest->Recalc(m_pLC);
 //  ++pUnlimitedLineDest; }

 pTrackLineDest = (CPrimitive_TrackLine *)m_pTrackLines;
 for(i = 0; i < m_lTrackLines; i ++) {
   pTrackLineDest->Recalc(m_pLC);
   ++pTrackLineDest; }

 //qDebug() << "OnGeoCenterMessage before repaint()";
 repaint();
 //qDebug() << "OnGeoCenterMessage after repaint()";
}

void AVP3D::OnDataMessage(CWVSSData *p_pMsg)
{
 size_t i;
 rWVSSToVisCircle *pMsgCircle;
 rWVSSToVisLine *pMsgLine;
 rWVSSToVisRectangle *pMsgRectangle;
 CPrimitive_Circle Circle;
 CPrimitive_Line Line;
 CPrimitive_Rectangle Rectangle;
 QTime Time;
 int iVal;
 CPrimitive_Circle *pCircleDest;
 CPrimitive_Line *pLineDest;
 CPrimitive_Rectangle *pRectangleDest;

 Time.start();

 if(m_lLines < p_pMsg->m_ulWVSSToVisLineCount) {
   if(m_pLines) delete [] m_pLines;
   m_pLines = new char [p_pMsg->m_ulWVSSToVisLineCount * sizeof(CPrimitive_Line)];
   if(!m_pLines) { m_lLines = 0; return; }
   memset(m_pLines, 0, p_pMsg->m_ulWVSSToVisLineCount * sizeof(CPrimitive_Line)); }

 m_lLines = p_pMsg->m_ulWVSSToVisLineCount;

 pMsgLine = (rWVSSToVisLine *)(((char *)p_pMsg) + sizeof(CWVSSData));
 pLineDest = (CPrimitive_Line *)m_pLines;
 for(i = 0; i < p_pMsg->m_ulWVSSToVisLineCount; i ++) {
   Line.Reset();
   Line.m_PointsGeo[0].m_dLat = pMsgLine->m_Points[0].m_dLat;
   Line.m_PointsGeo[0].m_dLong = pMsgLine->m_Points[0].m_dLong;
   Line.m_PointsGeo[0].m_dH = pMsgLine->m_Points[0].m_dH;
   Line.m_PointsGeo[1].m_dLat = pMsgLine->m_Points[1].m_dLat;
   Line.m_PointsGeo[1].m_dLong = pMsgLine->m_Points[1].m_dLong;
   Line.m_PointsGeo[1].m_dH = pMsgLine->m_Points[1].m_dH;
   Line.m_fLineWidth = pMsgLine->m_fLineWidth;
   Line.m_Color[0] = pMsgLine->m_Color[0];
   Line.m_Color[1] = pMsgLine->m_Color[1];
   Line.m_Color[2] = pMsgLine->m_Color[2];
   Line.Recalc(m_pLC);
   memcpy(pLineDest, &Line, sizeof(CPrimitive_Line));
   ++pLineDest;
   ++pMsgLine;
   }

#pragma message("An already allocated buffer is not reallocated if the new count < old count, but the value of originally allocated count is lost. Jump back up to values < old count and > new count is impossible. Realloc happens. No memory leak, but BAD")
#pragma message("CPrimitive_UnlimitedLine does it right")

 if(m_lCircles < p_pMsg->m_ulWVSSToVisCircleCount) {
   if(m_pCircles) delete [] m_pCircles;
   m_pCircles = new char [p_pMsg->m_ulWVSSToVisCircleCount * sizeof(CPrimitive_Circle)];
   if(!m_pCircles) { m_lCircles = 0; return; }
   memset(m_pCircles, 0, p_pMsg->m_ulWVSSToVisCircleCount * sizeof(CPrimitive_Circle)); }
 else if(p_pMsg->m_ulWVSSToVisCircleCount < m_lCircles)
   m_lCircles = m_lCircles;

 m_lCircles = p_pMsg->m_ulWVSSToVisCircleCount;

 pMsgCircle = (rWVSSToVisCircle *)(((char *)p_pMsg) + sizeof(CWVSSData) + p_pMsg->m_ulWVSSToVisLineCount * sizeof(rWVSSToVisLine));
 pCircleDest = (CPrimitive_Circle *)m_pCircles;
 for(i = 0; i < p_pMsg->m_ulWVSSToVisCircleCount; i ++) {
   Circle.Reset();
   Circle.m_CenterGeo = pMsgCircle->m_Center;
   if(Circle.m_CenterGeo.m_dLat > 100) { // this sets the center to the area center, not otherwise known to the Player.
     Circle.m_CenterGeo.m_dLat = m_GeoCenter.m_dLat;
     Circle.m_CenterGeo.m_dLong = m_GeoCenter.m_dLong;
     Circle.m_CenterGeo.m_dH = m_GeoCenter.m_dH + 100; }
   Circle.m_fRadius = pMsgCircle->m_fRadius;
   Circle.m_Color[0] = pMsgCircle->m_Color[0];
   Circle.m_Color[1] = pMsgCircle->m_Color[1];
   Circle.m_Color[2] = pMsgCircle->m_Color[2];
   Circle.m_fLineWidth = pMsgCircle->m_fLineWidth;
   Circle.m_fHeading = pMsgCircle->m_fHeading;
   Circle.m_fPitch = pMsgCircle->m_fPitch;
   Circle.m_fBank = pMsgCircle->m_fBank;
   Circle.Recalc(m_pLC);
   memcpy(pCircleDest, &Circle, sizeof(CPrimitive_Circle));
   ++pCircleDest;
   ++pMsgCircle;
   }
 
 if(m_lRectangles < p_pMsg->m_ulWVSSToVisRectangleCount) {
   if(m_pRectangles) delete [] m_pRectangles;
   m_pRectangles = new char [p_pMsg->m_ulWVSSToVisRectangleCount * sizeof(CPrimitive_Rectangle)];
   if(!m_pRectangles) { m_lRectangles = 0; return; }
   memset(m_pRectangles, 0, p_pMsg->m_ulWVSSToVisRectangleCount * sizeof(CPrimitive_Rectangle)); }

 m_lRectangles = p_pMsg->m_ulWVSSToVisRectangleCount;

 pMsgRectangle = (rWVSSToVisRectangle *)(((char *)p_pMsg) + sizeof(CWVSSData) + p_pMsg->m_ulWVSSToVisLineCount * sizeof(rWVSSToVisLine) + p_pMsg->m_ulWVSSToVisCircleCount * sizeof(rWVSSToVisCircle));
 pRectangleDest = (CPrimitive_Rectangle *)m_pRectangles;
 for(i = 0; i < p_pMsg->m_ulWVSSToVisRectangleCount; i ++) {
   Rectangle.Reset();
   Rectangle.m_CenterGeo = pMsgRectangle->m_Center;
   if(Rectangle.m_CenterGeo.m_dLat > 100) { // this sets the center to the area center, not otherwise known to the Player.
     Rectangle.m_CenterGeo.m_dLat = m_GeoCenter.m_dLat;
     Rectangle.m_CenterGeo.m_dLong = m_GeoCenter.m_dLong;
     Rectangle.m_CenterGeo.m_dH = m_GeoCenter.m_dH + 100; }
   Rectangle.m_fHorzExtent = pMsgRectangle->m_fHorzExtent;
   Rectangle.m_fVertExtent = pMsgRectangle->m_fVertExtent;
   Rectangle.m_Color[0] = pMsgRectangle->m_Color[0];
   Rectangle.m_Color[1] = pMsgRectangle->m_Color[1];
   Rectangle.m_Color[2] = pMsgRectangle->m_Color[2];
   Rectangle.m_fLineWidth = pMsgRectangle->m_fLineWidth;
   Rectangle.m_fHeading = pMsgRectangle->m_fHeading;
   Rectangle.m_fPitch = pMsgRectangle->m_fPitch;
   Rectangle.m_fBank = pMsgRectangle->m_fBank;
   Rectangle.Recalc(m_pLC);
   memcpy(pRectangleDest, &Rectangle, sizeof(CPrimitive_Rectangle));
   ++pRectangleDest;
   ++pMsgRectangle;
   }

 iVal = Time.elapsed();
 iVal = iVal;
 
 repaint();
}

/*
void AVP3D::ClearTexturedPlanes()
{
 size_t i;

 for(i = 0; i < m_TexturedPLanes.size(); i ++) {
   m_TexturedPLanes[i]->Unload();
   delete m_TexturedPLanes[i];
   }
 m_TexturedPLanes.clear();
}
*/

void AVP3D::ClearSurfaceAreas()
{
 size_t i;

 for(i = 0; i < m_SurfaceAreas.size(); i ++) {
   m_SurfaceAreas[i]->Unload();
   delete m_SurfaceAreas[i];
   }
 m_SurfaceAreas.clear();
}

bool AVP3D::GetOpenGLProcAddress(PROC &funcPtr, LPCSTR funcName)
{
 if(!wglGetProcAddress(funcName)) {
   //qDebug() << "Unsupported capability: function " << funcName << " is not supported by the videocard.";
   return(false); }
 return true;
}

bool AVP3D::LoadNextAircraftModel()
{
 bool bVal;
 const char *pModelFileName;

 m_nCurAircraftModel ++;
 if(m_nCurAircraftModel > (short)m_PlaneLibrary.GetPlaneCount() - 1)
   m_nCurAircraftModel = 0;
 pModelFileName = m_PlaneLibrary.GetPlaneFileNameByIndex(m_nCurAircraftModel);

 m_3DObj.Unload(context());
 bVal = m_3DObj.Load(pModelFileName);
 if(bVal)
   m_3DObj.LoadTextures(context());
 return(bVal);
}

bool AVP3D::GetSettings()
{
 QXmlStreamReader xml;
 QFile ConfigFile;
 QXmlStreamReader::TokenType TT;
 QString Str, CurXMLPath;
 QXmlStreamAttributes Attributes;
 QStringList StrList;
 CPathParts FileParts;
 CSurfaceArea *pSurfaceArea;
 CTexturedPlane *pTexturedPlane;
 CSurfaceArea::rData SurfaceAreaData;
 CTexturedPlane::rData TexturedPlaneData;
 bool bNewSurfaceArea;
 bool bSkyColor, bEarthColor, bBlankEarthTiles, bDrawAxes, bDebug, bBlankEarthTileSize, bBlankEarthTileStep;
 bool bVal;
 double dVal;
 // connection parameters
 QString p_TCPIPAddress = "127.0.0.1";
 int p_nTCPIPPort = 1701;
 QString p_ZMQSendAddress = "127.0.0.1";
 int p_nZMQSendPort = 2104;
 QString p_ZMQRecvAddress = "127.0.0.1";
 int p_nZMQRecvPort = 2105;
 // END connection parameters

 FileParts.ParseFileName("WVSS Visualizer Config.xml");
 ConfigFile.setFileName(FileParts.FullName());
 if(!ConfigFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
   //qDebug() << "config file " << FileParts.FullName() << " did not open";
   return(false); }

 xml.setDevice(&ConfigFile);

 bNewSurfaceArea = false;
 bSkyColor = bEarthColor = bBlankEarthTiles = bDrawAxes = bDebug = bBlankEarthTileStep = bBlankEarthTileSize = false;

 while(!xml.atEnd()) {
   TT = xml.readNext();
   if(TT == QXmlStreamReader::StartElement) {
     Str = xml.name().toString();
     StrList.append(Str);
     CurXMLPath = StrList.join('/');
     if(CurXMLPath == "MainConfig/PlaneTypes/PlaneType") {
       Attributes = xml.attributes();
       bVal = false;
       dVal = 0;
       if(Attributes.hasAttribute("Name") && Attributes.hasAttribute("File")) {
         if(Attributes.hasAttribute("Default"))
           bVal = (Attributes.value("Default").toInt() == 1);
         if(Attributes.hasAttribute("Wingspan"))
           dVal = Attributes.value("Wingspan").toDouble();
         m_PlaneLibrary.Add(Attributes.value("Name").toString().toLocal8Bit(), Attributes.value("File").toString().toLocal8Bit(), bVal, dVal); }
       }
     else if(CurXMLPath == "MainConfig/PlaneProxies/PlaneType") {
       Attributes = xml.attributes();
       if(Attributes.hasAttribute("Name") && Attributes.hasAttribute("ModelName"))
         m_PlaneLibrary.AddProxy(Attributes.value("Name").toString().toLocal8Bit(), Attributes.value("ModelName").toString().toLocal8Bit());
       }
     else if(CurXMLPath == "MainConfig/Areas") {
       Attributes = xml.attributes();
       if(Attributes.hasAttribute("Default"))
         strcpy(m_DefaultSurfaceArea, Attributes.value("Default").toString().toLocal8Bit());
       }
     else if(CurXMLPath == "MainConfig/Areas/Area") {
       pSurfaceArea = new CSurfaceArea();
       if(!pSurfaceArea) return(false);
       bNewSurfaceArea = true;
       Attributes = xml.attributes();
       memset(&SurfaceAreaData, 0, sizeof(SurfaceAreaData));
       if(Attributes.hasAttribute("Name"))
         strcpy(SurfaceAreaData.Name, Attributes.value("Name").toString().toLocal8Bit());
       if(Attributes.hasAttribute("CenterLat"))
         SurfaceAreaData.m_dCenterLat = Attributes.value("CenterLat").toDouble();
       if(Attributes.hasAttribute("CenterLong"))
         SurfaceAreaData.m_dCenterLong = Attributes.value("CenterLong").toDouble();
       pSurfaceArea->Set(&SurfaceAreaData);
       }
     else if(CurXMLPath == "MainConfig/Areas/Area/Texture") {
       if(bNewSurfaceArea) {
         Attributes = xml.attributes();
         memset(&TexturedPlaneData, 0, sizeof(TexturedPlaneData));
         if(Attributes.hasAttribute("File"))
           strcpy(TexturedPlaneData.m_FileName, Attributes.value("File").toString().toLocal8Bit());
         if(Attributes.hasAttribute("RefPointLat"))
           TexturedPlaneData.m_dRefPointLat = Attributes.value("RefPointLat").toDouble();
         if(Attributes.hasAttribute("RefPointLong"))
           TexturedPlaneData.m_dRefPointLong = Attributes.value("RefPointLong").toDouble();
         if(Attributes.hasAttribute("Size"))
           TexturedPlaneData.m_fSizeXM = TexturedPlaneData.m_fSizeZM = Attributes.value("Size").toDouble();
         if(Attributes.hasAttribute("DX"))
           TexturedPlaneData.m_fDX = Attributes.value("DX").toDouble();
         if(Attributes.hasAttribute("DZ"))
           TexturedPlaneData.m_fDZ = Attributes.value("DZ").toDouble();
         if(Attributes.hasAttribute("Elevation"))
           TexturedPlaneData.m_fElevation = Attributes.value("Elevation").toDouble();
         pTexturedPlane = new CTexturedPlane();
         if(!pTexturedPlane) return(false);
         pTexturedPlane->Set(&TexturedPlaneData);
         pSurfaceArea->AddTexturedPlane(pTexturedPlane);
         //Stop point
         }
       }
     else if(CurXMLPath == "MainConfig/SkyColor") bSkyColor = true;
     else if(CurXMLPath == "MainConfig/EarthColor") bEarthColor = true;
     else if(CurXMLPath == "MainConfig/BlankEarthTiles") bBlankEarthTiles = true;
     else if(CurXMLPath == "MainConfig/DrawAxes") bDrawAxes = true;
     else if(CurXMLPath == "MainConfig/Debug") bDebug = true;
     else if(CurXMLPath == "MainConfig/BlankEarthTileStep") bBlankEarthTileStep = true;
     else if(CurXMLPath == "MainConfig/BlankEarthTileSize") bBlankEarthTileSize = true;
     else if(CurXMLPath == "MainConfig/Connection/TCPIP") {
       Attributes = xml.attributes();
       if(Attributes.hasAttribute("ListenAddress"))
         p_TCPIPAddress = Attributes.value("ListenAddress").toString();
       if(Attributes.hasAttribute("ListenPort"))
         p_nTCPIPPort = Attributes.value("ListenPort").toInt();
       }
     else if(CurXMLPath == "MainConfig/Connection/ZMQ") {
       Attributes = xml.attributes();
       if(Attributes.hasAttribute("PullAddress"))
         p_ZMQRecvAddress = Attributes.value("PullAddress").toString();
       if(Attributes.hasAttribute("PullPort"))
         p_nZMQRecvPort = Attributes.value("PullPort").toInt();
       if(Attributes.hasAttribute("PubAddress"))
         p_ZMQSendAddress = Attributes.value("PubAddress").toString();
       if(Attributes.hasAttribute("PubPort"))
         p_nZMQSendPort = Attributes.value("PubPort").toInt();
       }
     }
   else if(TT == QXmlStreamReader::Characters) {
     Str = xml.text().toString();
     if(bSkyColor) m_SkyColor = ParseColorStr(Str);
     else if(bEarthColor) CTexturedPlane::m_EarthColor = ParseColorStr(Str);
     else if(bBlankEarthTiles) CSurfaceArea::m_nBlankEarthTiles = atoi(Str.toLocal8Bit());
     else if(bDrawAxes) CTexturedPlane::m_bDrawAxes = m_bDrawAxes = atoi(Str.toLocal8Bit());
     else if(bDebug) m_bDebug = atoi(Str.toLocal8Bit());
     else if(bBlankEarthTileStep) CTexturedPlane::m_dGeographicTileStep = atof(Str.toLocal8Bit());
     else if(bBlankEarthTileSize) CSurfaceArea::m_dGeographicTileSize = atof(Str.toLocal8Bit());
     }
   else if(TT == QXmlStreamReader::EndElement) {
     if(CurXMLPath == "MainConfig/Areas/Area") {
       // pSurfaceArea->Recalc(m_pLC); // textures not loaded yet, don't do this here
       //pSurfaceArea->AddEarthTiles();
       m_SurfaceAreas.push_back(pSurfaceArea);
       bNewSurfaceArea = false;
       //m_RawDataCxn.SetCxnType(bVal[0], CLidarGenericCxn::SERVER, bVal[1], CLidarGenericCxn::SERVER);
       //m_RawDataCxn.SetAddressPort(StrVal[0], IntVal[0], StrVal[1], IntVal[1], StrVal[2], IntVal[2]);
       }
     else if(CurXMLPath == "MainConfig/SkyColor") bSkyColor = false;
     else if(CurXMLPath == "MainConfig/EarthColor") bEarthColor = false;
     else if(CurXMLPath == "MainConfig/BlankEarthTiles") bBlankEarthTiles = false;
     else if(CurXMLPath == "MainConfig/DrawAxes") bDrawAxes = false;
     else if(CurXMLPath == "MainConfig/Debug") bDebug = false;
     else if(CurXMLPath == "MainConfig/BlankEarthTileStep") bBlankEarthTileStep = false;
     else if(CurXMLPath == "MainConfig/BlankEarthTileSize") bBlankEarthTileSize = false;
     StrList.removeLast();
     CurXMLPath = StrList.join('/');
     }
   if(xml.hasError()) { // do error handling
     //qDebug() << "Configuration file error" << xml.errorString();
     return(false); }
   }

 m_DataCxn.SetAddressPort(p_TCPIPAddress, p_nTCPIPPort, p_ZMQSendAddress, p_nZMQSendPort, p_ZMQRecvAddress, p_nZMQRecvPort);
 ConfigFile.close();
 //qDebug() << "Configuration from file " << FileParts.FullName() << " processed";
 return(true);
}

void AVP3D::OnPlaneListMessage(CWVSSPlaneTypeList *p_pMsg)
{
 short i;
 rWVSSPlaneType *pPlaneType;

 pPlaneType = (rWVSSPlaneType *)((char *)p_pMsg + sizeof(CWVSSPlaneTypeList));
 for(i = 0; i < p_pMsg->m_nPlaneTypeCount; i ++) {
   m_PlaneLibrary.Load(pPlaneType->m_PlaneType, context());
   pPlaneType ++;
   }
}

void AVP3D::OnPlanePositionMessage(CWVSSPlanePosition *p_pMsg)
{
 short i, nCenterToPlaneIndex = -1;
 rWVSSToVisPlane *pMsgPlane;
 CPrimitive_Plane *pPlaneDest, Plane;
 CPrimitive_Plane P2;
 CWVSSCenterToPlane CenterToPlaneMsg;

 if(m_nPlanes < p_pMsg->p_nWVSSToVisPlaneCount) {
   if(m_pPlanes) delete [] m_pPlanes;
   m_pPlanes = new char [p_pMsg->p_nWVSSToVisPlaneCount * sizeof(CPrimitive_Plane)];
   if(!m_pPlanes) { m_nPlanes = 0; return; }
   memset(m_pPlanes, 0, p_pMsg->p_nWVSSToVisPlaneCount * sizeof(CPrimitive_Plane));
   }

 m_nPlanes = p_pMsg->p_nWVSSToVisPlaneCount;

 pMsgPlane = (rWVSSToVisPlane *)(((char *)p_pMsg) + sizeof(CWVSSPlanePosition));
 pPlaneDest = (CPrimitive_Plane *)m_pPlanes;
 for(i = 0; i < p_pMsg->p_nWVSSToVisPlaneCount; i ++) {
   Plane.Reset();
   strcpy(Plane.m_PlaneType, pMsgPlane->m_PlaneType.m_PlaneType);
   strcpy(Plane.m_CallSign, pMsgPlane->m_CallSign.m_CallSign);
   Plane.m_CenterGeo = pMsgPlane->m_Position;
   Plane.m_fHeading = pMsgPlane->m_dHeading;
   Plane.m_fPitch = pMsgPlane->m_dPitch;
   Plane.m_fBank = pMsgPlane->m_dBank;
   Plane.m_nScale = pMsgPlane->m_nScale;
   Plane.m_dWingspan = pMsgPlane->m_dWingspan;
   Plane.m_bShowAirblock = pMsgPlane->m_bShowAirBlock;
   if(pMsgPlane->m_bCenterTo)
     nCenterToPlaneIndex = i;
   Plane.Recalc(m_pLC);
   memcpy(pPlaneDest, &Plane, sizeof(CPrimitive_Plane));
   ++pPlaneDest;
   ++pMsgPlane;
   }

 if(nCenterToPlaneIndex == -1) { repaint(); return; }
 memset(&CenterToPlaneMsg, 0, sizeof(CenterToPlaneMsg));
 strncpy(CenterToPlaneMsg.m_CallSign.m_CallSign, ((CPrimitive_Plane *)m_pPlanes + nCenterToPlaneIndex)->m_CallSign, sizeof(CenterToPlaneMsg.m_CallSign.m_CallSign) - 1);
 OnCenterToPlaneMessage(&CenterToPlaneMsg, false);
 repaint();
}

void AVP3D::UseSurfaceArea(const char *p_pAreaName)
{
 size_t i, AreaIndex = m_SurfaceAreas.size() + 1;

 for(i = 0; i < m_SurfaceAreas.size(); i ++) {
   if(!stricmp(m_SurfaceAreas[i]->GetAreaName(), p_pAreaName)) {
     m_SurfaceAreas[i]->m_bOn = true;
     AreaIndex = i; }
   else
     m_SurfaceAreas[i]->m_bOn = false; }
 if(AreaIndex == m_SurfaceAreas.size() + 1) {
   m_nCurAreaIndex = 0;
   return; }
 //m_pLC->Reset(m_SurfaceAreas[AreaIndex]->m_Data.m_dCenterLat, m_SurfaceAreas[AreaIndex]->m_Data.m_dCenterLong, 0);
 
 m_GeoCenter.m_dLat = m_SurfaceAreas[AreaIndex]->GetAreaCenterLat();
 m_GeoCenter.m_dLong = m_SurfaceAreas[AreaIndex]->GetAreaCenterLong();
 m_GeoCenter.m_dH = 0;
 m_pLC->Reset(m_GeoCenter.m_dLat, m_GeoCenter.m_dLong, m_GeoCenter.m_dH);

 //qDebug() << "before m_SurfaceAreas[AreaIndex]->Load";
 bool bVal = m_SurfaceAreas[AreaIndex]->Load(context());
 //qDebug() << "after m_SurfaceAreas[AreaIndex]->Load. return " << QString::number(bVal);

 m_SurfaceAreas[AreaIndex]->Recalc();
 m_nCurAreaIndex = AreaIndex;
}

CTexturedPlane *AVP3D::GetNextTexturedPlane(bool p_bFirst)
{
 size_t i, j, TexturedPlanesCount;
 CTexturedPlane *pCurTexturedPlane, *pLastTexturePlane;
 bool bPlaneSet = false;

 pLastTexturePlane = NULL;
 for(i = 0; i < m_SurfaceAreas.size(); i ++) {
   if(!m_SurfaceAreas[i]->m_bOn) continue;
   TexturedPlanesCount = m_SurfaceAreas[i]->GetTexturedPlanesCount();
   for(j = 0; j < TexturedPlanesCount; j ++) {
     pCurTexturedPlane = m_SurfaceAreas[i]->GetTexturedPlaneByIndex(j);
     if(p_bFirst) {
       if(!bPlaneSet) {
         m_pCurTexturedPlane = pCurTexturedPlane;
         m_pCurTexturedPlane->m_bSelected = true;
         bPlaneSet = true; }
       else
         pCurTexturedPlane->m_bSelected = false; }
     else {
       if(pCurTexturedPlane == m_pCurTexturedPlane) {
         m_pCurTexturedPlane->m_bSelected = false;
         pLastTexturePlane = m_pCurTexturedPlane; }
       else if(pLastTexturePlane == m_pCurTexturedPlane) {
         m_pCurTexturedPlane = pCurTexturedPlane;
         m_pCurTexturedPlane->m_bSelected = true;
         return(m_pCurTexturedPlane);
         }
       }
     }
   }
 if(!p_bFirst)
   m_pCurTexturedPlane = NULL;
 return(m_pCurTexturedPlane);
}

void AVP3D::Set3DObjDrawFlags()
{
 C3DObj::rObjDraw ObjDraw;

 memset(&ObjDraw, 0, sizeof(ObjDraw));
 ObjDraw.m_bTransparent = m_bTransparentObject;
 ObjDraw.m_Alpha = 128;
 ObjDraw.m_bWireframe = m_bWireframe;
 ObjDraw.m_bSelectFace = true;
 ObjDraw.m_bSelectFace = m_lCurFace;
 m_PlaneLibrary.SetDrawFlags(&ObjDraw);
}

QColor AVP3D::ParseColorStr(QString Str)
{
 char *pWork, tstr[32];
 unsigned char R = 255, G = 255, B = 255;
 
 memset(tstr, 0, sizeof(tstr));
 strncpy(tstr, Str.toLocal8Bit(), sizeof(tstr) - 1);
 pWork = strtok(tstr, " ,");
 if(pWork) R = atoi(pWork);
 pWork = strtok(NULL, " ,");
 if(pWork) G = atoi(pWork);
 pWork = strtok(NULL, " ,");
 if(pWork) B = atoi(pWork);
 return(QColor(R, G, B));
}

void AVP3D::OnCenterToPlaneMessage(CWVSSCenterToPlane *p_pMsg, bool p_bRedraw)
{
 short i;
 CPrimitive_Plane *pPlane;

 pPlane = (CPrimitive_Plane *)m_pPlanes;
 for(i = 0; i < m_nPlanes; i ++) {
   if(!stricmp(pPlane->m_CallSign, p_pMsg->m_CallSign.m_CallSign))
     break;
   ++pPlane; }
 if(i == m_nPlanes) {
   m_dCenter[0] = 0;
   m_dCenter[1] = 0;
   m_dCenter[2] = 0; }
 else {  
   m_dCenter[0] = pPlane->m_dCenter_planar[0];
   m_dCenter[1] = pPlane->m_dCenter_planar[1];
   m_dCenter[2] = pPlane->m_dCenter_planar[2]; }
 RecalcEye();
 if(p_bRedraw)
   repaint();
}

//void AVP3D::OnTrackMessage(CWVSSTrack *p_pMsg)
//{
// unsigned long li;
// rWVSSToVisUnlimitedLine *pMsgUnlimitedLine;
// CPrimitive_UnlimitedLine *pUnlimitedLineDest;
// CPrimitive_UnlimitedLine UnlimitedLine;
// GLfloat SurfaceElevation;
//
// if(p_pMsg->m_ulWVSSToVisUnlimitedLineCount > m_lAvlUnlimitedLines) {
//   if(m_pUnlimitedLines) delete [] m_pUnlimitedLines;
//   m_pUnlimitedLines = new CPrimitive_UnlimitedLine [p_pMsg->m_ulWVSSToVisUnlimitedLineCount];
//   if(!m_pUnlimitedLines) { m_lUnlimitedLines = m_lAvlUnlimitedLines = 0; return; }
//   m_lAvlUnlimitedLines = p_pMsg->m_ulWVSSToVisUnlimitedLineCount; }
//
// SurfaceElevation = GetSurfaceElevation();
// m_lUnlimitedLines = 0;
// pMsgUnlimitedLine = (rWVSSToVisUnlimitedLine *)(((char *)p_pMsg) + sizeof(CWVSSTrack));
// pUnlimitedLineDest = m_pUnlimitedLines;
// for(li = 0; li < p_pMsg->m_ulWVSSToVisUnlimitedLineCount; li ++) {
//   pUnlimitedLineDest->Reset(false);
//   pUnlimitedLineDest->m_fLineWidth = pMsgUnlimitedLine->m_fLineWidth;
//   pUnlimitedLineDest->m_Color[0] = pMsgUnlimitedLine->m_Color[0];
//   pUnlimitedLineDest->m_Color[1] = pMsgUnlimitedLine->m_Color[1];
//   pUnlimitedLineDest->m_Color[2] = pMsgUnlimitedLine->m_Color[2];
//   pUnlimitedLineDest->AddPoints((rWVSSGeoPoint *)(((char *)pMsgUnlimitedLine) + sizeof(rWVSSToVisUnlimitedLine)), pMsgUnlimitedLine->p_lPointCount, m_pLC);
//   pUnlimitedLineDest->m_SurfaceElevation = SurfaceElevation + 1;
//   pMsgUnlimitedLine = (rWVSSToVisUnlimitedLine *)(((char *)pMsgUnlimitedLine) + sizeof(rWVSSToVisUnlimitedLine) + (pMsgUnlimitedLine->p_lPointCount * sizeof(rWVSSGeoPoint)));
//   ++pUnlimitedLineDest; }
// m_lUnlimitedLines = p_pMsg->m_ulWVSSToVisUnlimitedLineCount;
//}

void AVP3D::OnTrackMessage(CWVSSTrack *p_pMsg)
{
 unsigned long li;
 rWVSSToVisUnlimitedLine *pMsgUnlimitedLine;
 CPrimitive_TrackLine *pTrackLineDest;
 CPrimitive_TrackLine TrackLine;
 GLfloat SurfaceElevation;

 if(p_pMsg->m_ulWVSSToVisUnlimitedLineCount > m_lAvlTrackLines) {
   if(m_pTrackLines) delete [] m_pTrackLines;
   m_pTrackLines = new CPrimitive_TrackLine [p_pMsg->m_ulWVSSToVisUnlimitedLineCount];
   if(!m_pTrackLines) { m_lTrackLines = m_lAvlTrackLines = 0; return; }
   m_lAvlTrackLines = p_pMsg->m_ulWVSSToVisUnlimitedLineCount; }

 SurfaceElevation = GetSurfaceElevation();
 m_lTrackLines = 0;
 pMsgUnlimitedLine = (rWVSSToVisUnlimitedLine *)(((char *)p_pMsg) + sizeof(CWVSSTrack));
 pTrackLineDest = m_pTrackLines;
 for(li = 0; li < p_pMsg->m_ulWVSSToVisUnlimitedLineCount; li ++) {
   pTrackLineDest->Reset(false);
   pTrackLineDest->m_fLineWidth = pMsgUnlimitedLine->m_fLineWidth;
   pTrackLineDest->m_Color[0] = pMsgUnlimitedLine->m_Color[0];
   pTrackLineDest->m_Color[1] = pMsgUnlimitedLine->m_Color[1];
   pTrackLineDest->m_Color[2] = pMsgUnlimitedLine->m_Color[2];
   pTrackLineDest->AddPoints((rWVSSGeoPoint *)(((char *)pMsgUnlimitedLine) + sizeof(rWVSSToVisUnlimitedLine)), pMsgUnlimitedLine->p_lPointCount, m_pLC);
   pTrackLineDest->m_SurfaceElevation = SurfaceElevation + 1;
   pMsgUnlimitedLine = (rWVSSToVisUnlimitedLine *)(((char *)pMsgUnlimitedLine) + sizeof(rWVSSToVisUnlimitedLine) + (pMsgUnlimitedLine->p_lPointCount * sizeof(rWVSSGeoPoint)));
   ++pTrackLineDest; }
 m_lTrackLines = p_pMsg->m_ulWVSSToVisUnlimitedLineCount;
 repaint();
}

void AVP3D::paintEvent(QPaintEvent *p_pEvent)
{
 QPainter Painter;
 
 Paint3DScene();

 Painter.begin(this);

 if(m_bDrawCompass)
   DrawCompass(p_pEvent, &Painter);
 if(m_bDrawHelp)
   DrawHelp(&Painter);

 Painter.end();
}

void AVP3D::DrawTextOverlay(QString p_Str, QPainter *p_pPainter, QPoint p_LeftTop, QRect p_MarginRect)
// p_MarginRect contains both margin and padding as such: left margin, top margin, x padding, y padding
{
 //const int Margin = 11;
 //const int Padding = 6;
 QTextDocument TextDocument;
 QRect Rect;
 QFont Font;
 QTextOption TextOption;
 QString StyleSheetStr;

 StyleSheetStr = TextDocument.defaultStyleSheet();
 //StyleSheetStr += "*{ color:  RGB(255, 255, 255); } ";
 StyleSheetStr += "*{ color: #FFFFFF; }";
 TextDocument.setDefaultStyleSheet(StyleSheetStr);

 TextOption = TextDocument.defaultTextOption();
 TextOption.setTabStop(300);
 TextDocument.setDefaultTextOption(TextOption);

 //TextDocument.setPlainText(p_Str);
 TextDocument.setHtml(p_Str);
 Font.setPixelSize(12);
 TextDocument.setDefaultFont(Font);
 TextDocument.setTextWidth(TextDocument.size().width());

 Rect.setCoords(p_LeftTop.x(), p_LeftTop.y(), TextDocument.size().width() + 2 * p_MarginRect.right(), TextDocument.size().height() + 2 * p_MarginRect.bottom());
 //p_pPainter->translate(width() - Rect.width() - Margin, height() - Rect.height() - Margin);
 p_pPainter->translate(p_MarginRect.left(), p_MarginRect.top());
 p_pPainter->setPen(QColor(255, 239, 239));
 p_pPainter->setBrush(QColor(0, 0, 255, 64));
 Font = p_pPainter->font();
 p_pPainter->drawRect(Rect);
 p_pPainter->translate(p_MarginRect.right(), p_MarginRect.bottom());
 TextDocument.drawContents(p_pPainter);
}

void AVP3D::DrawHelp(QPainter *p_pPainter)
{
 QString Str;

 Str = "<html><body><table border=0 cellpadding=2 cellspacing=0>";
 Str += "<tr><td colspan=2 style=\"background-color:#555555\">Keyboard shortcuts</td></tr>";
 Str += "<tr><td>Escape</td><td>quit</td></tr>";
 Str += "<tr><td>Space</td><td>restore defaults</td></tr>";
 Str += "<tr><td>Ctrl+H</td><td>Keyboard shortcuts</td></tr>";
 Str += "<tr><td>Up</td><td>pan forward</td></tr>";
 Str += "<tr><td>Down</td><td>pan back</td></tr>";
 Str += "<tr><td>Left</td><td>pan right</td></tr>";
 Str += "<tr><td>Right</td><td>pan left</td></tr>";
 Str += "<tr><td>Shift+[Right|A]</td><td>rotate clockwise</td></tr>";
 Str += "<tr><td>Shift+[Left|S]</td><td>rotate counter clockwise</td></tr>";
 Str += "<tr><td>Plus</td><td>Zoom in</td></tr>";
 Str += "<tr><td>Minus</td><td>Zoom out</td></tr>";
 Str += "<tr><td>Shift+U</td><td>set heading to North</td></tr>";
 Str += "<tr><td>Ctrl+C</td><td>compass on/off</td></tr>";
 Str += "<tr><td>O</td><td>rotation timer on/off</td></tr>";
 Str += "<tr><td>F1</td><td>satellite texture on/off</td></tr>";
 if(m_bDebug) Str += "<tr><td>Ctrl+Shift+F1</td><td>debug text</td></tr>";
 if(m_bDebug) Str += "<tr><td>Ctrl+F2</td><td>select first textured plane</td></tr>";
 if(m_bDebug) Str += "<tr><td>F2</td><td>select next textured plane</td></tr>";
 if(m_bDebug) Str += "<tr><td>V</td><td>selected textured plane on/off</td></tr>";
 if(m_bDebug) Str += "<tr><td>F4</td><td>transparent objects</td></tr>";
 if(m_bDebug) Str += "<tr><td>F5</td><td>wireframe objects</td></tr>";
 if(m_bDebug) Str += "<tr><td>F6</td><td>load next aircraft model</td></tr>";
 if(m_bDebug) Str += "<tr><td>F8</td><td>depth test on/off</td></tr>";
 if(m_bDebug) Str += "<tr><td>N</td><td>first face</td></tr>";
 if(m_bDebug) Str += "<tr><td>Ctrl+N</td><td>change face direction</td></tr>";
 if(m_bDebug) Str += "<tr><td>M</td><td>next face</td></tr>";
 if(m_bDebug) Str += "<tr><td>Ctrl+Shift+A</td><td>axes on/off</td></tr>";
 if(m_bDebug) Str += "<tr><td>Ctrl+T+[Up|Down]</td><td>airblock text on/off</td></tr>";
 if(m_bDebug) Str += "<tr><td>Ctrl+T+[+|-]</td><td>airblock text size larger/smaller</td></tr>";
 if(m_bDebug) Str += "<tr><td>Ctrl+T+[1|2|3|4|5]</td><td>airblock text color: white, red, green, blue, black</td></tr>";
 Str += "</table></body></html>";
 DrawTextOverlay(Str, p_pPainter, QPoint(0, 0), QRect(11, 11, 6, 6));
}

void AVP3D::DrawCompass(QPaintEvent *p_pEvent, QPainter *p_pPainter)
{
 CCompass Compass;
 QPoint Point;

 Compass.m_dHeading_deg = m_dHeading_deg;
 Compass.m_nRadius = 50;
 Point.setX(p_pEvent->rect().right() - 5 - Compass.m_nRadius);
 Point.setY(p_pEvent->rect().bottom() - 5 - Compass.m_nRadius);
 Compass.Draw(p_pPainter, &Point);
}

void AVP3D::RenderTextBlock(GLdouble *p_pdCenter_planar, QPoint *p_pStartPoint, short p_nTextHeight, char *p_pText, short p_nVOffset)
{
 char *pWork;
 QPoint TextPoint;
 GLdouble Projection[16];
 GLdouble Modelview[16];
 GLint Viewport[4];
 GLdouble ScreenCoords[3];

 //glPushMatrix();
 //glLoadIdentity();

 if(p_pStartPoint) {
   TextPoint.setX(p_pStartPoint->x());
   TextPoint.setY(p_pStartPoint->y()); }
 else { // p_pdCenter_planar is set, convert from world to screen
   glGetDoublev(GL_PROJECTION_MATRIX, Projection);
   glGetDoublev(GL_MODELVIEW_MATRIX, Modelview);
   glGetIntegerv(GL_VIEWPORT, Viewport);
   gluProject(p_pdCenter_planar[0], p_pdCenter_planar[1], p_pdCenter_planar[2], Modelview, Projection, Viewport, &ScreenCoords[0], &ScreenCoords[1], &ScreenCoords[2]);
   TextPoint.setX(ScreenCoords[0]);
   TextPoint.setY(height() - ScreenCoords[1] - p_nVOffset);
   }
 
 glColor3ub(255, 255, 255);
 
 pWork = strtok(p_pText, "\n");
 while(pWork) {
   renderText(TextPoint.x(), TextPoint.y(), QString::fromLocal8Bit(pWork));
   TextPoint.setY(TextPoint.y() + p_nTextHeight);
   pWork = strtok(NULL, "\n");
   }

 //glPopMatrix();
}

void AVP3D::ToggleAirblocks(bool p_bOn)
{
 CPrimitive_Plane *pPlaneDest;
 size_t i;

 pPlaneDest = (CPrimitive_Plane *)m_pPlanes;
 for(i = 0; i < m_nPlanes; i ++) {
   pPlaneDest->m_bShowAirblock = p_bOn;
   ++pPlaneDest; }
 repaint();
}
