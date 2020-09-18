#ifndef QT3D_H
#define QT3D_H

#include <QtWidgets/QMainWindow>
#include "ui_qt3d.h"

class QT3D : public QMainWindow
{
 Q_OBJECT

public:
 QT3D(QWidget *parent = 0);
 ~QT3D();

private:
 Ui::QT3DClass ui;
};

#endif // QT3D_H
