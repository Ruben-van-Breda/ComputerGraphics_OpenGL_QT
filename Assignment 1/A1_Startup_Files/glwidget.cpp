#include "glwidget.h"
#include "vector.h"
#include <QDebug>
#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <iostream>

/**
 * Ruben van Breda
 * COMP30020 : Graphics
 * Assignment 1
 * 19200704
 * 1 Nov 2021
 */
GLWidget::GLWidget(QWidget *parent) : QOpenGLWidget(parent) {

    setFocusPolicy(Qt::StrongFocus);
    mainWindow = nullptr;

    // Add Bezier control points
    this->vertices.push_back(Point(100, 300, 0));
    this->vertices.push_back(Point(200, 100, 0));
    this->vertices.push_back(Point(400, 100, 0));
    this->vertices.push_back(Point(500, 300, 0));
}

void GLWidget::initializeGL() {
    // default behaviour clears the buffer
    glClearColor(1.0, 1.0, 1.0, 1.0); // set the colour of the canvas
    glClear(GL_COLOR_BUFFER_BIT);     // clean the canvas (draw buffer)
}

void GLWidget::paintGL() {
    // clear the buffer
    glClear(GL_COLOR_BUFFER_BIT);

    // start by clearing the image
    this->clearImage();

    // call subroutines to do the actual drawing
    if (this->joinControlPoints)
        this->drawControlPolygon(); // show line segments between control points
    if (this->showVertices)
        this->drawVertices(); // show control points
    if (this->showDeCasteljau)
        this->drawDeCasteljau(this->deCasteljauParameter); // show deCasteljau's construction
    if (this->showBezier)
        this->drawBezierCurve(); // show Bezier curve

    // call the PPM Image display function
    image.draw();

    // flush everything from the draw buffer
    glFlush();
}

void GLWidget::resizeGL(int w, int h) {
    image.zoom = devicePixelRatio(); // to support high res diplays
    // resize the PPM image to the new width and height
    width = w;
    height = h;
    image.resize(static_cast<unsigned int>(width), static_cast<unsigned int>(height));

    // and sets to a standard 2D set of coordinates
    glMatrixMode(GL_PROJECTION);         // switch to proj. mat.
    glLoadIdentity();                    // set to identity
    glOrtho(0, width, 0, height, -1, 1); // with orthog proj
    glViewport(0, 0, width, height);     // set the viewport
}

void GLWidget::keyPressEvent(QKeyEvent *event) {
    switch (event->key()) {

    // save PPM image
    case Qt::Key_S:
        this->saveImage();
        break;
    case Qt::Key_E:
        this->clearImage();
        this->update();
        break;

        // drawing modes
    case Qt::Key_1:
        this->showVertices = !this->showVertices;
        this->update();
        break;
    case Qt::Key_2:
        this->joinControlPoints = !this->joinControlPoints;
        this->update();
        break;
    case Qt::Key_3:
        this->showDeCasteljau = !this->showDeCasteljau;
        this->update();
        break;
    case Qt::Key_4:
        this->showBezier = !this->showBezier;
        this->update();
        break;

        // backspace/delete keys: remove selected vertex
    case Qt::Key_Delete:
    case Qt::Key_Backspace:
        if ((vertices.size() < 2) || (selectedVertex == -1)) {
            return;
        }
        // delete the selected vertex (if there is more than 1 left)
        this->vertices.erase(this->vertices.begin() + this->selectedVertex);
        selectedVertex = -1;
        this->update();
        break;

        // change deCasteljau Parameter
    case Qt::Key_Minus:
        this->deCasteljauParameter -= 0.01f;
        if (this->deCasteljauParameter < 0.0f)
            this->deCasteljauParameter = 0.0f;
        this->update();
        break;
    case Qt::Key_Plus:
        std::cout << "++++" << std::endl;
        this->deCasteljauParameter += 0.01f;
        if (this->deCasteljauParameter > 1.0f)
            this->deCasteljauParameter = 1.0f;
        this->update();
        break;

        // quit
    case Qt::Key_Q:
        qApp->exit();
        break;
    default:
        event->ignore();
        break;
    }
}

void GLWidget::mousePressEvent(QMouseEvent *event) {
    Point p(event->x(), event->y(), 0);

    // right mouse click to add vertex
    if (event->button() == Qt::RightButton) {

        // TODO : Done
        vertices.push_back(Point(event->x(), event->y(),0));

    }
    // left mouse click to select vertex
    else {
        // reset selection
        this->selectedVertex = -1;
        // search through all vertices
        for (size_t vertexIndex = 0; vertexIndex < vertices.size(); vertexIndex++) {
            // create vector from Point p to the current vertex
            Vector vec = this->vertices[vertexIndex] - p;
            // Check the length of that vector
            if (vec.norm() < 28) { // close enough
                this->selectedVertex = static_cast<int>(vertexIndex);
                break;
            }
        }
    }

    this->update();
}

void GLWidget::mouseMoveEvent(QMouseEvent *event) {
    Point p(event->x(), event->y(), 0);

    // if a vertex is select move it to where the mouse was moved
    if (selectedVertex != -1) {
        vertices[static_cast<size_t>(selectedVertex)] = p;
        this->update();
    }
}

void GLWidget::saveImage() {
    QString filePath = QDir::homePath() + "/Desktop/comp30020-Lab4.ppm";
    QFile file(filePath);
    file.remove();
    image.writeFile(filePath.toStdString());
}

void GLWidget::clearImage() {
    image.clear();
}

GLWidget::~GLWidget() {
    // Auto-generated destructor stub
}

// draw a disc at vertex location
void GLWidget::drawVertex(Point p, int brushSize, RGBValue color) {
    int size = brushSize / 2;
    Point vertex = p;
    vertex.y = height - 1 - vertex.y; // Flip y vertex

    for (int i = -size; i < size; i++) {
        for (int j = -size; j < size; j++) {
            // draw disc with radius size
            if (i * i + j * j <= size * size) {
                image.setPixel(static_cast<Index>(vertex.y + i), static_cast<Index>(vertex.x + j), color);
            }
        }
    }
}

//	draw all vertices
void GLWidget::drawVertices() {
    for (size_t vertexIndex = 0; vertexIndex < vertices.size(); vertexIndex++) {
        Point p = vertices[vertexIndex];
        if (static_cast<int>(vertexIndex) != selectedVertex){ // just draw disc
            this->drawVertex(p, 9, RGBValue(80, 80, 80));
        } else { // selected: draw disk + circle (by drawing three disks of different sizes)
            this->drawVertex(p, 17, RGBValue(0, 0, 0));
            this->drawVertex(p, 13, RGBValue(255, 255, 255));
            this->drawVertex(p, 7, RGBValue(0, 0, 0));
        }
    }
}

void GLWidget::drawEdge(Point fromPoint, Point toPoint, RGBValue color) {
    Point pointP = fromPoint;
    Point pointQ = toPoint;
    Vector vec_PQ = (pointQ - pointP);
    float norm_vec_PQ = vec_PQ.norm();    // compute vector length
    float stepDelta = 1.0f / norm_vec_PQ; // compute parameter step

    for (float t = 0.0f; t <= 1.0f; t += stepDelta) {
        Point r = (pointP + t * vec_PQ);
        image.setPixel(static_cast<Index>(this->height - 1 - r.y), static_cast<Index>(r.x), color);
    }
}

// draws line segments between control points
void GLWidget::drawControlPolygon() {
    // TODO

    for ( size_t i = 0; i < vertices.size()-1; i++){
        drawEdge(vertices[i], vertices[i+1], RGBValue(0,0,0));
    }

}

// draws using deCasteljau's algorithm for parameter value t
void GLWidget::drawDeCasteljau(float t) {
    // TODO
    drawDeCasteljauTable(t);
    return;

    /* int N = vertices.size();
    Point table [N][N];
    Point a,b,c,d,C;
    RGBValue color(255,0,0);
    RGBValue grey(155,155,155);
    switch (N) {
    case (2) :
        a.x = (1.0 - t)*vertices[0].x + t*vertices[1].x;
        a.y = (1.0 - t)*vertices[0].y + t*vertices[1].y;
//        this->drawVertex(a, 9, RGBValue(155, 60, 155));

        this->image.setPixel(this->height - a.y -1, a.x, color);
        break;

    case 3:

        a.x = (1.0 - t)*vertices[0].x + t*vertices[1].x;
        a.y = (1.0 - t)*vertices[0].y + t*vertices[1].y;
        this->drawVertex(a, 7, grey);
        b.x = (1.0 - t)*vertices[1].x + t*vertices[2].x;
        b.y = (1.0 - t)*vertices[1].y + t*vertices[2].y;
        this->drawVertex(b, 7, grey);
        drawEdge(a,b,grey);

        c.x = (1.0-t)*a.x + t*b.x;
        c.y = (1.0-t)*a.y + t*b.y;
        drawEdge(b,c,grey);

        this->drawVertex(c, 9, color);
//        this->image.setPixel(this->height - c.y -1, c.x, color);

    break;

    case 4:

        a.x = (1.0 - t)*vertices[0].x + t*vertices[1].x;
        a.y = (1.0 - t)*vertices[0].y + t*vertices[1].y;
        this->drawVertex(a, 7, grey);

        b.x = (1.0 - t)*vertices[1].x + t*vertices[2].x;
        b.y = (1.0 - t)*vertices[1].y + t*vertices[2].y;
        this->drawVertex(b, 7, grey);
        drawEdge(a,b,grey);

        c.x = (1.0-t)*a.x + t*b.x;
        c.y = (1.0-t)*a.y + t*b.y;
        drawEdge(c,b,grey);
        this->drawVertex(c, 7, grey);

        d.x = pow(1.0-t,3)*vertices[0].x + 3*pow(1.0 - t,2)*t*vertices[1].x + 3*(1.0-t)*(t*t)*vertices[2].x + pow(t,3)*vertices[3].x;
        d.y = pow(1.0-t,3)*vertices[0].y + 3*pow(1.0 - t,2)*t*vertices[1].y + 3*(1.0-t)*(t*t)*vertices[2].y + pow(t,3)*vertices[3].y;
        drawEdge(c,d,grey);

//        drawEdge(b,d,color);

        this->drawVertex(d, 9, color);


      break;
    default:{
        drawDeCasteljauTable(t);
        return;
    }



}
*/


}
Point GLWidget::DeCasteljauTableMethod(float t){
    int N = vertices.size();


    Vector table[N];

    for(int i = 0; i < N; i++){
        table[i] = Vector(vertices[i].x,vertices[i].y,vertices[i].z);;

    }

    for(int k = 1;  k < N; k++){
        for (int i = 0; i < N-k ; i++ ) {
            table[i] = (1-t)*table[i] + t*table[i + 1];
        }


    }

    Point p(table[0].dx, table[0].dy, table[0].dz);
    return p;

}

void GLWidget::drawDeCasteljauTable(float t){
    int N = vertices.size();

    RGBValue grey(155,155,155);
    RGBValue red(255,0,0);
    RGBValue blue(0,0,255);
    RGBValue green(0,255,0);
    Vector table[N];
    Point pi, pi1;
    Point K;
    Point k1;
    Point A, iP1;

    // populate table with control points
    for(int i = 0; i < N; i++){
        table[i] = Vector(vertices[i].x,vertices[i].y,vertices[i].z);;
    }
    for(int k = 1;  k < N; k++){
           k1 = Point(table[k-1].dx, table[k-1].dy, table[k-1].dz);
           K = Point(table[k].dx, table[k].dy, table[k].dz);
           drawEdge(k1,K,grey);

            for (int i = 0; i < N - k; i++ ) {



                table[i] = (1.0-t)*table[i] + t*table[i + 1];
                k1 = Point(table[k-1].dx, table[k-1].dy, table[k-1].dz);
                K = Point(table[k].dx, table[k].dy, table[k].dz);



                drawEdge(K,k1,grey);

                iP1 = Point(table[i+1].dx, table[i+1].dy, table[i+1].dz);
                pi = Point(table[i].dx, table[i].dy, table[i].dz);

                drawVertex(pi, 7, grey);
                drawEdge(pi,iP1,grey);

                if(i > 0){ // connect prev
                    A = Point(table[i-1].dx, table[i-1].dy, table[i-1].dz);
                    drawEdge(A,pi,grey);
                }

        }




    }

    Point p(table[0].dx, table[0].dy, table[0].dz);
    drawVertex(p,9,green); // draw current point







}

Point GLWidget::DeCasteljau(float t){
    int N = vertices.size();

    Point a,b,c,d,C;
    RGBValue color(255,0,0);
    RGBValue grey(155,155,155);
    switch (N) {
    case (2) :
        a.x = (1.0 - t)*vertices[0].x + t*vertices[1].x;
        a.y = (1.0 - t)*vertices[0].y + t*vertices[1].y;



        return a;
        break;

    case 3:

        a.x = (1.0 - t)*vertices[0].x + t*vertices[1].x;
        a.y = (1.0 - t)*vertices[0].y + t*vertices[1].y;

        b.x = (1.0 - t)*vertices[1].x + t*vertices[2].x;
        b.y = (1.0 - t)*vertices[1].y + t*vertices[2].y;

        c.x = (1.0-t)*a.x + t*b.x;
        c.y = (1.0-t)*a.y + t*b.y;


        return c;

    break;

    case 4:

        a.x = (1.0 - t)*vertices[0].x + t*vertices[1].x;
        a.y = (1.0 - t)*vertices[0].y + t*vertices[1].y;


        b.x = (1.0 - t)*vertices[1].x + t*vertices[2].x;
        b.y = (1.0 - t)*vertices[1].y + t*vertices[2].y;

        c.x = (1.0-t)*a.x + t*b.x;
        c.y = (1.0-t)*a.y + t*b.y;

        d.x = pow(1.0-t,3)*vertices[0].x + 3*pow(1.0 - t,2)*t*vertices[1].x + 3*(1.0-t)*t*t*vertices[2].x + pow(t,3)*vertices[3].x;
        d.y = pow(1.0-t,3)*vertices[0].y + 3*pow(1.0 - t,2)*t*vertices[1].y + 3*(1.0-t)*t*t*vertices[2].y + pow(t,3)*vertices[3].y;


        return d;

      break;
    default:{
        return DeCasteljauTableMethod(t);
    }
}
}
// draws the bezier curve
void GLWidget::drawBezierCurve() {
    // TODO
    for (float u = 0.0; u <= 1.0; u+=0.001 ) {


            Point p = DeCasteljau(u);
            this->image.setPixel(this->height -p.y -1, p.x, RGBValue(0,255,0));

    }

}
