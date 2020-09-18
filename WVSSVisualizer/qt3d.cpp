#include "qt3d.h"

QT3D::QT3D(QWidget *parent) : QMainWindow(parent)
{
 ui.setupUi(this);
 //move(0, 0);
 //resize(100, 100);
 setGeometry(0, 0, 100, 100);
}

QT3D::~QT3D()
{

}
