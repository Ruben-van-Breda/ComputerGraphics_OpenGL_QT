#ifndef ANIMATION_H
#define ANIMATION_H

#include <QWidget>
#include <QtOpenGL>
#include <QOpenGLWidget>
#include <QGLWidget>
#include "objects.h"

class animation
{
public:
    animation();
    void computePosition(int timeStep);

};


#endif // ANIMATION_H
