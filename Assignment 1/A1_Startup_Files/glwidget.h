#pragma once

#include <QGLWidget>
#include <QKeyEvent>
#include <QMainWindow>
#include <QtOpenGL>
#include <vector>

#include "PPMImage.h"
#include "RGBValue.h"
#include "point.h"

class GLWidget : public QOpenGLWidget {
    Q_OBJECT
public:
    GLWidget(QWidget *parent = nullptr);

    void initializeGL();
    void paintGL();
    void resizeGL(int w, int h);

    virtual ~GLWidget();

private:
    // image routines
    void saveImage();
    void clearImage();

protected:
    void keyPressEvent(QKeyEvent *event);
    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);

private:
    QWidget *mainWindow;
    PPMImage image;
    int width, height;

private:
    // draws vertices
    void drawVertices();
    void drawVertex(Point p, int brushSize, RGBValue color);
    // draws edge
    void drawEdge(Point fromPoint, Point toPoint, RGBValue color);
    // draws ine segments between control points
    void drawControlPolygon();
    // draws using deCasteljau's algorithm for parameter value t
    void drawDeCasteljau(float t);
    // draws the bezier curve
    void drawBezierCurve();
    // calculates point for t using control points
    Point DeCasteljau(float t);
    Point DeCasteljauTableMethod(float t);
    void drawDeCasteljauTable(float t);

private:
    std::vector<Point> vertices;   // vector of vertices storing control points
    int selectedVertex = 1;        // no vertex selected
    bool joinControlPoints = true; // toggle line segments between control points
    bool showVertices = true;      // toggle control points
    bool showDeCasteljau = true;   // toggle deCasteljau's construction
    float deCasteljauParameter = 0.5f; // deCasteljau parameter value
    bool showBezier = true;            // show Bezier curve
};
