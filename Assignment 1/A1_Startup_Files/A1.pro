QT += core gui opengl

TARGET = A1
TEMPLATE = app

SOURCES += main.cpp\
    mainwindow.cpp \
    glwidget.cpp \
    PPMImage.cpp \
    RGBValue.cpp \
    point.cpp \
    vector.cpp

HEADERS  += mainwindow.h \
    glwidget.h \
    PPMImage.h \
    RGBValue.h \
    point.h \
    vector.h

FORMS += \
    mainwindow.ui

RESOURCES += \
    resources.qrc

win32: LIBS += -lopengl32
