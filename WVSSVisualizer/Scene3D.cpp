#include <QtGui>
#include <math.h>
#include "scene3D.h"
#include "glext.h"
#include <GL/glu.h>
#include "qdir.h"

const GLdouble pi=3.141593, k=pi/180;

GLint viewport[4];

GLfloat VertexArray[6][3];
GLfloat TextureArray[6][2];
GLuint IndexArray[8][3];
GLuint textureID[2];

GLint signs[2]={1, 1};
bool motionParameters[2]={1, 1};
int textureParameters[2]={0, 1};

Scene3D::Scene3D(QWidget* parent) : QGLWidget(QGLFormat(QGL::SampleBuffers), parent)
{  
   xRot1=-90.0f; yRot1=0.0f; zRot1=0.0f; zTra1=0.0f;
   xRot2=-90.0f; yRot2=0.0f; zRot2=0.0f; zTra2=0.0f;

   timer = new QTimer(this);
   connect(timer, SIGNAL(timeout()), this, SLOT(change())); 
   timer->start(10);

   QGLFormat frmt;
   frmt.setSwapInterval(1);
   setFormat(frmt);
}

Scene3D::~Scene3D()
{

}

void Scene3D::initializeGL()
{

// возвращаем указатель на строку с информацией в памяти  
   //const GLubyte* str = glGetString(GL_VERSION);
   //const GLubyte* str2 = glGetString(GL_VENDOR);

   qglClearColor(Qt::black);
   glEnable(GL_DEPTH_TEST);  
   glEnable(GL_CULL_FACE); 
   glEnable(GL_TEXTURE_2D); 
   glEnable(GL_MULTISAMPLE); 

   getVerTexArrays();
   getIndexArray();
   genTextures();

   glEnableClientState(GL_VERTEX_ARRAY); 
   glEnableClientState(GL_TEXTURE_COORD_ARRAY); 
}

void Scene3D::resizeGL(int nWidth, int nHeight)
{ 
   glMatrixMode(GL_PROJECTION); 
   glLoadIdentity();            
 
   ratio=(GLfloat)nHeight/(GLfloat)nWidth;
   
   if (nWidth>=nHeight)
      glOrtho(-2.0/ratio, 2.0/ratio, -2.0, 2.0, -10.0, 10.0); 
   else
      glOrtho(-2.0, 2.0, -2.0*ratio, 2.0*ratio, -10.0, 10.0);
  
   glViewport(0, 0, (GLint)nWidth, (GLint)nHeight);
   glGetIntegerv(GL_VIEWPORT, viewport);
}

void Scene3D::paintGL()
{ 
   glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); 

   glMatrixMode(GL_MODELVIEW); 
   glLoadIdentity();           

   glPushMatrix(); 
      glTranslatef(-1.0f/ratio, zTra1, 0.0f);
      glRotatef(xRot1, 1.0f, 0.0f, 0.0f);
      glRotatef(yRot1, 0.0f, 1.0f, 0.0f);
      glRotatef(zRot1, 0.0f, 0.0f, 1.0f);
      glBindTexture(GL_TEXTURE_2D, textureID[textureParameters[0]]);
      drawFigure();
   glPopMatrix();

   glPushMatrix();
      glTranslatef(1.0f/ratio, zTra2, 0.0f);
      glRotatef(xRot2, 1.0f, 0.0f, 0.0f);
      glRotatef(yRot2, 0.0f, 1.0f, 0.0f);
      glRotatef(zRot2, 0.0f, 0.0f, 1.0f);
      glBindTexture(GL_TEXTURE_2D, textureID[textureParameters[1]]);
      drawFigure();
   glPopMatrix();
}

void Scene3D::mousePressEvent(QMouseEvent* pe)
{
   QPoint mousePosition = pe->pos();
   this->selectFigures(mousePosition);
}  

void Scene3D::genTextures()
{
 QDir Dir;
 QString Str = Dir.absolutePath();
 
 QPixmap Pixmap1 = QPixmap(QString("textures/picture1.jpg"));
 //int w = Pixmap1.width();

 textureID[0]=bindTexture(QPixmap(QString("textures/texture1.jpg")), GL_TEXTURE_2D);
 glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE); 
 glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
 
 textureID[1]=bindTexture(QPixmap(QString("textures/picture2.jpg")), GL_TEXTURE_2D);
 glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
 glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE); 
}

void Scene3D::getVerTexArrays()
{
   GLfloat R=1.0f;
   GLfloat alpha=pi/2;

   VertexArray[0][0]=0.0f;
   VertexArray[0][1]=0.0f;
   VertexArray[0][2]=R;
   TextureArray[0][0]=0.5f;
   TextureArray[0][1]=1.0f;

   VertexArray[1][0]=R*sin(alpha)*sin(0);
   VertexArray[1][1]=R*sin(alpha)*cos(0);
   VertexArray[1][2]=R*cos(alpha);
   TextureArray[1][0]=1.0f;
   TextureArray[1][1]=0.0f;

   VertexArray[2][0]=R*sin(alpha)*sin(pi/2);
   VertexArray[2][1]=R*sin(alpha)*cos(pi/2);
   VertexArray[2][2]=R*cos(alpha);
   TextureArray[2][0]=0.0f;
   TextureArray[2][1]=0.0f;

   VertexArray[3][0]=R*sin(alpha)*sin(pi);
   VertexArray[3][1]=R*sin(alpha)*cos(pi);
   VertexArray[3][2]=R*cos(alpha);
   TextureArray[3][0]=1.0f;
   TextureArray[3][1]=0.0f;

   VertexArray[4][0]=R*sin(alpha)*sin(3*pi/2);
   VertexArray[4][1]=R*sin(alpha)*cos(3*pi/2);
   VertexArray[4][2]=R*cos(alpha);
   TextureArray[4][0]=0.0f;
   TextureArray[4][1]=0.0f;

   VertexArray[5][0]=0.0f;
   VertexArray[5][1]=0.0f;
   VertexArray[5][2]=-R;
   TextureArray[5][0]=0.5f;
   TextureArray[5][1]=1.0f;
}

void Scene3D::getIndexArray()
{
   IndexArray[0][0]=0;
   IndexArray[0][1]=2;
   IndexArray[0][2]=1;

   IndexArray[1][0]=0;
   IndexArray[1][1]=3;
   IndexArray[1][2]=2;

   IndexArray[2][0]=0;
   IndexArray[2][1]=4;
   IndexArray[2][2]=3;

   IndexArray[3][0]=0;
   IndexArray[3][1]=1;
   IndexArray[3][2]=4;

   IndexArray[4][0]=5;
   IndexArray[4][1]=1;
   IndexArray[4][2]=2;

   IndexArray[5][0]=5;
   IndexArray[5][1]=2;
   IndexArray[5][2]=3;

   IndexArray[6][0]=5;
   IndexArray[6][1]=3;
   IndexArray[6][2]=4;

   IndexArray[7][0]=5;
   IndexArray[7][1]=4;
   IndexArray[7][2]=1;
}

void Scene3D::drawFigure()
{
   glVertexPointer(3, GL_FLOAT, 0, VertexArray); 
   glTexCoordPointer(2, GL_FLOAT, 0, TextureArray); 
   glDrawElements(GL_TRIANGLES, 24, GL_UNSIGNED_INT, IndexArray); 
}

void Scene3D::selectFigures(QPoint mp)
{
   GLuint selectBuffer[4];
   GLint hits;
   glSelectBuffer(4, selectBuffer);

   glMatrixMode(GL_PROJECTION);
   glPushMatrix();
   glRenderMode(GL_SELECT);
   glLoadIdentity();

   gluPickMatrix((GLdouble)mp.x(), (GLdouble)(viewport[3]-mp.y()), 1.0, 1.0, viewport);
   if (width()>=height())
      glOrtho(-2.0/ratio, 2.0/ratio, -2.0, 2.0, -10.0, 10.0); 
   else
      glOrtho(-2.0, 2.0, -2.0*ratio, 2.0*ratio, -10.0, 10.0);

   glMatrixMode(GL_MODELVIEW);
   glLoadIdentity();

   glInitNames();
   glPushName(0);

   glPushMatrix();
      glTranslatef(-1.0f/ratio, zTra1, 0.0f);
      glRotatef(xRot1, 1.0f, 0.0f, 0.0f);
      glRotatef(yRot1, 0.0f, 1.0f, 0.0f);
      glRotatef(zRot1, 0.0f, 0.0f, 1.0f);
      glLoadName(1);
      drawFigure();
   glPopMatrix();

   glPushMatrix();
      glTranslatef(1.0f/ratio, zTra2, 0.0f);
      glRotatef(xRot2, 1.0f, 0.0f, 0.0f);
      glRotatef(yRot2, 0.0f, 1.0f, 0.0f);
      glRotatef(zRot2, 0.0f, 0.0f, 1.0f);
      glLoadName(2);
      drawFigure();
   glPopMatrix();

   hits=glRenderMode(GL_RENDER); 

   if (hits>0)
   {
      int figureName=selectBuffer[3];

      if (motionParameters[figureName-1]) 
         motionParameters[figureName-1]=0; 
      else
         motionParameters[figureName-1]=1;
   }

   glMatrixMode(GL_PROJECTION);
   glPopMatrix();
}

void Scene3D::change() 
{
   if (motionParameters[0])
   {
      xRot1 -=0.05f;
      yRot1 -=0.05f;
      zRot1 +=0.05f;

      if ((xRot1>360)||(xRot1<-360)) xRot1=0.0f;
      if ((yRot1>360)||(yRot1<-360)) yRot1=0.0f;
      if ((zRot1>360)||(zRot1<-360)) zRot1=0.0f;

      if (abs(zTra1)>0.5f) signs[0] *=-1;
      zTra1 -=signs[0]*0.005f;
   }

   if (motionParameters[1])
   {
      xRot2 +=0.05f;
      yRot2 +=0.05f;
      zRot2 -=0.05f;

      if ((xRot2>360)||(xRot2<-360)) xRot2=0.0f;
      if ((yRot2>360)||(yRot2<-360)) yRot2=0.0f;
      if ((zRot2>360)||(zRot2<-360)) zRot2=0.0f;

      if (abs(zTra2)>0.5f) signs[1] *=-1;
      zTra2 +=signs[1]*0.005f;
   }

   updateGL();
}

void Scene3D::changeTex()
{
   if (textureParameters[0]==0)
   {
      textureParameters[0]=1;
      textureParameters[1]=0;
   }
   else
   {
      textureParameters[0]=0;
      textureParameters[1]=1;
   }

   updateGL();
}

void Scene3D::stopTmr()
{
   timer->stop();
}

void Scene3D::startTmr()
{
   timer->start();
}
