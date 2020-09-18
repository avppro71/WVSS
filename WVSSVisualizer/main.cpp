#include "qt3d.h"
#include <QtWidgets/QApplication>
//#include "mainwindow.h"
#include "AVP3D.h"
#include "WVSSIPCommonFunctions.h"

int main(int argc, char *argv[])
{
 QApplication::addLibraryPath(".");
 QApplication app(argc, argv);
 AVP3D AVPScene;
 short Width = 800, Height = 600;
 QRect WndFrameRect, AvlScreenRect;

 GlobalFillMessageNames();

 AVPScene.setWindowTitle("AVP"); 
 AVPScene.resize(Width, Height);
 WndFrameRect = AVPScene.frameGeometry();
 AvlScreenRect = QApplication::desktop()->availableGeometry();
 AVPScene.move(AvlScreenRect.width() - WndFrameRect.width(), AvlScreenRect.height() - WndFrameRect.height());
 AVPScene.show();
 
 return app.exec();
}
