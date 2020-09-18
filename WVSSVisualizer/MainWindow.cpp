#include <QtGui>
#include "mainwindow.h"
#include "scene3D.h"
#include "qaction.h"
#include "qmenubar.h"

MainWindow::MainWindow()
{ 
   scene1 = new Scene3D;
   setCentralWidget(scene1);
     
   this->setWindowTitle(tr("lecture2"));
   
   createActions();
   createMenus();
}

void MainWindow::createActions()
{
   changeTexAct = new QAction(tr("Change"), this);
   connect(changeTexAct, SIGNAL(triggered()), scene1, SLOT(changeTex()));

   stopTimAct = new QAction(tr("Stop"), this);
   connect(stopTimAct, SIGNAL(triggered()), scene1, SLOT(stopTmr()));

   startTimAct = new QAction(tr("Start"), this);
   connect(startTimAct, SIGNAL(triggered()), scene1, SLOT(startTmr()));
}

void MainWindow::createMenus()
{
   texture_menu = menuBar()->addMenu(tr("Texture"));
   texture_menu->addAction(changeTexAct);

   timer_menu = menuBar()->addMenu(tr("Animation"));
   timer_menu->addAction(stopTimAct);
   timer_menu->addAction(startTimAct);
}
