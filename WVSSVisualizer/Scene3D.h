#ifndef SCENE3D_H 
#define SCENE3D_H

#include "QTOpenGL/QGLWidget"
//#include <QGLWidget>

class Scene3D : public QGLWidget
{ 
   Q_OBJECT

   private:   
      GLfloat ratio;

      GLfloat xRot1;
      GLfloat yRot1;
      GLfloat zRot1;
      GLfloat zTra1;

      GLfloat xRot2;
      GLfloat yRot2;
      GLfloat zRot2;
      GLfloat zTra2;

      QTimer *timer;

      void getVerTexArrays();
      void getIndexArray();
      void genTextures();
      void drawFigure();

      void selectFigures(QPoint mp);   
        
   protected:
      void initializeGL();           
      void resizeGL(int nWidth, int nHeight); 
      void paintGL();  
      void mousePressEvent(QMouseEvent* pe);   

   private slots:
      void change();
      void changeTex();
      void stopTmr();
      void startTmr();

   public: 
      Scene3D(QWidget* parent = 0);
      ~Scene3D();
}; 
#endif
