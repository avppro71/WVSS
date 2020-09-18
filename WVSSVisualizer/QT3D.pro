TEMPLATE = app
TARGET = KPPV
QT += core widgets gui opengl

INCLUDEPATH += ./GeneratedFiles
DEPENDPATH += .
UI_DIR += ./GeneratedFiles
RCC_DIR += ./GeneratedFiles

HEADERS += \
  ./qt3d.h \
  ./MainWindow.h \
  ./Scene3D.h \
  ./glext.h \
  ./AVP3D.h \
    DrawPrimitive.h
SOURCES += \
  ./main.cpp \
  ./qt3d.cpp \
  ./MainWindow.cpp \
  ./Scene3D.cpp \
  ./AVP3D.cpp \
    DrawPrimitive.cpp
FORMS += qt3d.ui
