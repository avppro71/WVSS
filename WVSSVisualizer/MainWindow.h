#ifndef MAINWINDOW_H 
#define MAINWINDOW_H 

#include <QMainWindow> 
#include "scene3D.h"

class MainWindow : public QMainWindow
{ 
   Q_OBJECT

   private:
      Scene3D* scene1;
      QMenu* texture_menu;          
      QAction* changeTexAct;      
      QMenu* timer_menu;  
      QAction* stopTimAct;
      QAction* startTimAct;

      void createActions();
      void createMenus();
              
   public:   
      MainWindow();
}; 
#endif
