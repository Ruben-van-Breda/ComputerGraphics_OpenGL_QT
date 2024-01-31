#include "glwidget.h"
#include <QDebug>
#include <iostream>
//#include "animation.h"
//#include "glu.h"
#include <QGLWidget>

static float PI = 3.141592f;

static GLfloat light_position[] = {0.0, 100.0, 100.0, 0.0};

// basic colours
static GLfloat black[] = {0.0, 0.0, 0.0, 1.0};
static GLfloat white[] = {1.0, 1.0, 1.0, 1.0};
static GLfloat grey[] = {0.5, 0.5, 0.5, 1.0};

// primary colours
static GLfloat red[] = {1.0, 0.0, 0.0, 1.0};
static GLfloat green[] = {0.0, 1.0, 0.0, 1.0};
static GLfloat blue[] = {0.0, 0.0, 1.0, 1.0};

// secondary colours
static GLfloat yellow[] = {1.0, 1.0, 0.0, 1.0};
static GLfloat magenta[] = {1.0, 0.0, 1.0, 1.0};
static GLfloat cyan[] = {0.0, 1.0, 1.0, 1.0};

// other colours
static GLfloat orange[] = {1.0, 0.5, 0.0, 1.0};
static GLfloat brown[] = {0.5, 0.25, 0.0, 1.0};
static GLfloat dkgreen[] = {0.0, 0.5, 0.0, 1.0};
static GLfloat pink[] = {1.0, 0.6f, 0.6f, 1.0};


GLWidget::GLWidget(QWidget *parent) : QOpenGLWidget(parent) {
    setFocusPolicy(Qt::StrongFocus);

    m_lookAt.eye =    {+0.0f, +5.0f, 15.0f};
    m_lookAt.center =  {+0.0f, +0.0f, -1.0f};
    m_lookAt.up =     {+0.0f, +1.0f, +0.0f};

    m_showGrid = false;
    m_showWireFrame = false;
    m_runAnimation = false;

    viewWidth = this->width();
    viewHeight = this->height();

    m_time = 0;
    m_nframes = 550;

    m_objects = std::map<std::string, Object*>();




    //    m_nframes = 10;
    //    m_animation.posn_x = 100;

    // Timer for animation
    QTimer *timer = new QTimer(this);
    timer->start(0);
    connect(timer, SIGNAL(timeout()), this, SLOT(idle()));

    // Set frame 0 of the animation
    //    animate(0);
}

void GLWidget::initializeGL() {
    glClearColor(0.0, 0.0, 0.0, 0.0);
    glShadeModel(GL_SMOOTH);
    glLightfv(GL_LIGHT0, GL_POSITION, light_position); // specify the position of the light
    glEnable(GL_LIGHT0);       // switch light #0 on
    glEnable(GL_LIGHTING);     // switch lighting on
    glEnable(GL_DEPTH_TEST);   // make sure depth buffer is switched on
    glEnable(GL_NORMALIZE);    // normalize normal vectors for safety

    // Create storage for 10 textures - use those slots in turn
    glMatrixMode(GL_TEXTURE);
    glLoadIdentity();
    glGenTextures(10, m_textures);
    glMatrixMode(GL_MODELVIEW);

    setupObjects();

}

void GLWidget::setProjection() {
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    if (m_orthoProjection){
        double aspectRatio = static_cast<double>(viewWidth) / static_cast<float>(viewHeight);
        if (aspectRatio > 1.0) {
            glOrtho(static_cast<GLdouble>(-m_ortho.extent),
                    static_cast<GLdouble>(m_ortho.extent),
                    static_cast<GLdouble>(-m_ortho.extent) / aspectRatio,
                    static_cast<GLdouble>(m_ortho.extent) / aspectRatio, m_ortho.nearPlane, m_ortho.farPlane);
        } else {
            glOrtho(static_cast<GLdouble>(-m_ortho.extent) * aspectRatio,
                    static_cast<GLdouble>(m_ortho.extent) * aspectRatio,
                    static_cast<GLdouble>(-m_ortho.extent),
                    static_cast<GLdouble>(m_ortho.extent), m_ortho.nearPlane, m_ortho.farPlane);
        }
    } else {
        QMatrix4x4 projectionMat;
        projectionMat.setToIdentity();
        projectionMat.perspective(m_perspective.zoom, m_perspective.aspectRatio,
                                  m_perspective.nearPlane, m_perspective.farPlane);
        glLoadMatrixf(projectionMat.constData());
    }
}

void GLWidget::paintGL() {
    // clear all pixels
    glClearColor(0.0, 0.0, 0.0, 0.0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    setProjection();

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    QMatrix4x4 viewMat;
    viewMat.setToIdentity();
    viewMat.lookAt(m_lookAt.eye,
                   m_lookAt.eye+m_lookAt.center,
                   m_lookAt.up);
    glLoadMatrixf(viewMat.constData());

    if (m_showGrid) {
        drawGrid(magenta);
    }

    if (m_showWireFrame) {  // switch for wireframe
        glDisable(GL_LIGHTING); // disable lighting
        glColor4fv(red);        // set a colour for the model wireframe
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE); // switch to line mode
        glPushMatrix();
        drawObjects();
        glPopMatrix();
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL); // fill triangles for the rest of rendering
        glEnable(GL_LIGHTING);  // enable lighting for the rest of the rendering
    } else {
        glPushMatrix();
        drawObjects();
        glPopMatrix();
    }
}

void GLWidget::resizeGL(int w, int h) {

    viewWidth = w;
    viewHeight = h;

    m_perspective.zoom = 45.0;
    m_perspective.aspectRatio = static_cast<double>(viewWidth) / static_cast<float>(viewHeight ? viewHeight : 1.0f);
    m_perspective.nearPlane =  0.1f;
    m_perspective.farPlane = 100.0f;

    m_ortho.extent = 15.0f;
    m_ortho.nearPlane = 0.1f;
    m_ortho.farPlane = 100.0f;
    m_ortho.zoomFactor = 15.0f;

    glViewport(0, 0, viewWidth, viewHeight);
    this->update();
}

void GLWidget::keyPressEvent(QKeyEvent *event) {
    switch (event->key()) {

    case Qt::Key_1:
        m_showGrid = !m_showGrid;
        this->update();
        break;
    case Qt::Key_2:
        m_showWireFrame = !m_showWireFrame;
        this->update();
        break;
    case Qt::Key_3:
        m_runAnimation = !m_runAnimation;
        this->update();
        break;
    case Qt::Key_W:
        m_lookAt.eye += m_lookAt.center * 0.5f;
        this->update();
        break;
    case Qt::Key_S:
        m_lookAt.eye -= m_lookAt.center * 0.5f;
        this->update();
        break;
    case Qt::Key_A:
        m_lookAt.eye -= QVector3D::crossProduct(m_lookAt.center, QVector3D(0,1,0)) * 0.5f;
        this->update();
        break;
    case Qt::Key_D:
        m_lookAt.eye += QVector3D::crossProduct(m_lookAt.center, QVector3D(0,1,0)) * 0.5f;
        this->update();
        break;
    case Qt::Key_P:
        m_orthoProjection = !m_orthoProjection;
        this->update();
        break;
    case Qt::Key_Q:
        qApp->exit();
        break;
    default:
        event->ignore();
        break;
    }
}

void GLWidget::wheelEvent(QWheelEvent *event) {

    float delta = event->angleDelta().y() > 0 ? -5.0f : +5.0f;
    m_perspective.zoom += delta;
    if(m_perspective.zoom < 10.0f) {
        m_perspective.zoom = 10.0f;
    } else if(m_perspective.zoom > 120.0f) {
        m_perspective.zoom = 120.0f;
    }

    float numDegrees = (static_cast<float>(event->angleDelta().y()) / 8.0f);
    float numSteps = numDegrees / (180.0f * (1.0f / m_ortho.extent));
    m_ortho.extent -= numSteps;
    if (m_ortho.extent <= 0.0f) {
        m_ortho.extent = 0.0001f;
    }
    viewWidth = this->width();
    viewHeight = this->height();

    this->update();
}

void GLWidget::mousePressEvent(QMouseEvent *event) {
    if (event->button() == Qt::LeftButton) {
        m_mousePressPosition = QVector2D(event->localPos());
    }
}

void GLWidget::mouseMoveEvent(QMouseEvent *event) {
    if (event->buttons() == Qt::LeftButton) {
        // Update rotation
        auto diff = QVector2D(event->localPos()) - m_mousePressPosition;
        auto n = QVector3D(diff.y(), diff.x(), 0.0).normalized();
        float mSpeed = diff.length() * 0.1f;
        mRotationAxis = (mRotationAxis + n * mSpeed).normalized();
        mRotation = QQuaternion::fromAxisAndAngle(mRotationAxis, mSpeed) * mRotation;

        // Update center vector based on rotation
        QMatrix4x4 mat;
        mat.setToIdentity();
        mat.rotate(mRotation);
        m_lookAt.center = {+0.0f, +0.0f, -1.0f};
        m_lookAt.center = m_lookAt.center * mat;

        m_mousePressPosition = QVector2D(event->localPos());


        std::cout << "LookAt Center: "<< m_lookAt.center.x() << " , " <<  m_lookAt.center.y() << " , " <<  m_lookAt.center.z() << std::endl;
        std::cout << "EYE: " << m_lookAt.eye.x() << "  , " <<  m_lookAt.eye.y() << " ," << m_lookAt.eye.z() <<std::endl;
        std::cout << "Rotation: " << mRotation.x() << " , " <<  mRotation.y() << ", "<< mRotation.z() << std::endl;
    }
}

void GLWidget::mouseReleaseEvent(QMouseEvent *event) {
}

void GLWidget::resetCamera() {

    m_lookAt.eye = {+0.0f, +5.0f, 15.0f};
    m_lookAt.center = {+0.0f, +0.0f, -1.0f};
    m_lookAt.up = {+0.0f, +1.0f, +0.0f};

    mRotationAxis = QVector3D(0,0,0);
    mRotation = QQuaternion();

    m_ortho.extent = m_ortho.zoomFactor;
    m_perspective.zoom = 45.0;

    this->update();
}

void GLWidget::resetAnimation() {

    resetCamera();
    m_time = 0;
    animate(0);


}
// function to draw grid on z = 0 plane
void GLWidget::drawGrid(GLfloat *colour) {
    int x, z;
    int nGridlines = 3;
    // edges don't reflect
    glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, black);
    glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, colour); // but they do emit
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    for (x = -nGridlines; x <= nGridlines; x++) { // for each x
        if (x % 12)
            glLineWidth(1.0);
        else
            glLineWidth(2.0);
        glBegin(GL_LINES);
        glVertex3i(x, 0, -nGridlines);
        glVertex3i(x, 0, +nGridlines);
        glEnd();
    }                                             // for each x
    for (z = -nGridlines; z <= nGridlines; z++) { // for each y
        if (z % 12)
            glLineWidth(1.0);
        else
            glLineWidth(2.0);
        glBegin(GL_LINES);
        glVertex3i(-nGridlines, 0, z);
        glVertex3i(+nGridlines, 0, z);
        glEnd();
    } // for each y
    glLineWidth(1.0);
    glPopMatrix();
    // stop emitting, otherwise other objects will emit the same colour
    glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, black);
}


//  TO DO: draw an articultated character, you can use these
//  member functions or model the character using a separate class.

//  Criteria:

//  The character you create must have :

//  1. two arms with elbow joints
//  2. two legs with knee joints
//  3. a body
//  4. a head

//  You may also create as many other characters/objects for your scene as you wish

void GLWidget::setupObjects() {

    /* Example code to get you started */

    m_objects["body"] = new Cylinder(0.7f, 2.f, 32);
    m_objects["body"]->m_color = blue;

    m_objects["head"] = new Sphere(0.8f, 32, 32);
    m_objects["head"]->m_color = black;

    m_objects["shoulder"] = new Sphere(0.3f, 32, 32);
    m_objects["shoulder"]->m_color = orange;

    m_objects["arm"] = new Cylinder(0.25f, 1.8f, 32);
    m_objects["arm"]->m_color = orange;

    m_objects["leg"] = new Cylinder(0.3f, 2.f, 32);
    m_objects["leg"]->m_color = orange;

    m_objects["earth"] = new Sphere(3.0f, 64, 64);
    m_objects["earth"]->loadTexture(QDir::homePath() + "/Desktop/earth.jpg", m_textures[0]);

    m_objects["skybox"] = new Sphere(60.0f, 32, 32);
    m_objects["skybox"]->loadTexture(QDir::homePath() + "/Desktop/skybox.jpg", m_textures[2]);

    m_objects["cube"] = new Sphere(1.0f,64,64);
    m_objects["cube"]->m_color = grey;
    m_objects["cube"]->loadTexture(QDir::homePath() + "/Desktop/astroid.jpg",m_textures[1]);


}

void GLWidget::drawObjects() {

    /* Example code to get you started
     * Doesn't include any rotation/translation/scaling based on animation */


    // Skybox
    glPushMatrix();
    glTranslatef(0.f, 0.f, 0.f);
    glRotatef(-90.f, 1.f, 0.f, 0.f);
    m_objects["skybox"]->draw();
    glPopMatrix();

    // Earth
    glPushMatrix();
    glTranslatef(0.f, 0.f, 0.f);
    glRotatef(-90.f, 1.f, 0.f, 0.f);
    glRotatef(m_animation.earthTheta * 0.5f, 0.f, 0.f, 1.f);
    m_objects["earth"]->draw();
    glPopMatrix();



    // add cube
    glPushMatrix();

    glTranslatef(5.0f, 6.0f, -10.0f);
    glTranslatef(m_animation.cube_posnx, m_animation.cube_posny, m_animation.cube_posnz);
    glRotatef(m_animation.earthTheta * 2, 0.f, 0.f, 1.f);
    m_objects["cube"]->draw();
    glPopMatrix();

    // Astronaut - transformations to apply to the full character
    glPushMatrix();
    glTranslatef(-1.0f, -1.f, -3.f);
//    glRotatef(90.f, 1, 0.5f ,0 );
//    glRotatef(-35.f, 0, 0, 1);
    glScalef(0.16f, 0.16f, 0.16f);
    //    glTranslatef(15.f, 0.f, 0.f);

    // Now add body parts / bones in a hierarchy

    // body
    glPushMatrix();
//    glTranslatef(-1.0f, 1.5f, 0.0f);
    glTranslatef(m_animation.posn_x,m_animation.posn_y , m_animation.posn_z);

    glPushMatrix();
    glRotatef(90.f, 1, 0, 0);
    glRotatef(m_animation.bodyTheta, 0,0,1);
    m_objects["body"]->draw();
    glPopMatrix();

    // head
    glPushMatrix();
    glTranslatef(0.0f, 0.6f, 0.0f);
    m_objects["head"]->draw();
    glPopMatrix();

    // right shoulder
    glPushMatrix();
    glTranslatef(0.8f, -0.1f, 0.0f);
    m_objects["shoulder"]->draw();

    // right arm
    glPushMatrix();
    glTranslatef(0.0f, -0.1f, 0.0f);

    glPushMatrix();
    glRotatef(90.f, 1, 0, 0);
    m_objects["arm"]->draw();
    glPopMatrix();

    glPopMatrix();

    glPopMatrix();

    // left shoulder
    glPushMatrix();
    glTranslatef(-0.8f, -0.1f, 0.0f);
    m_objects["shoulder"]->draw();

    // left arm
    glPushMatrix();
    glTranslatef(0.0f, -0.1f, 0.0f);

    glPushMatrix();
    glRotatef(90.f, 1, 0, 0);
    glRotatef(m_animation.right_arm, 0.0f, 0.0f,0.0f);
    m_objects["arm"]->draw();

    glPopMatrix();

    glPopMatrix();

    glPopMatrix();

    // right leg
    glPushMatrix();
    glTranslatef(0.3f, -1.9f, 0.0f);

    glPushMatrix();
    glRotatef(m_animation.right_leg, 1, 0, 0);
    m_objects["leg"]->draw();
    glPopMatrix();

    glPopMatrix();

    // left leg
    glPushMatrix();
    glTranslatef(-0.3f, -1.9f, 0.0f);

    glPushMatrix();
    glRotatef(90.f, 1, 0, 0);
    m_objects["leg"]->draw();
    glPopMatrix();

    glPopMatrix();

    glPopMatrix();

    glPopMatrix();



}

// Automatically called by the timer
void GLWidget::idle() {
    if (m_runAnimation == true) {
        if (m_time < m_nframes)
            m_time++;
        else
            m_time = 0;
        animate(m_time);
    }
    this->update();
}


void GLWidget::animate(int time) {
    /* TODO
     * This is where you should update the rotation/position/scale values for
     * different objects, such as those in the m_animation struct (see header)
     * The provided fields (e.g. left_arm, right_arm) are just examples, you can
     * add/subtract any fields that are useful for the animations you want to create. */

    std::cout << "animation at frame " << time << " is playing. " << std::endl;
    computePosition(time, "camera");
    computePosition(time, "earth");
    computePosition(time, "cube");


    computePosition(time, "body");
    computePosition(time, "right_arm");
    computePosition(time, "right_leg");













    //    GLfloat theta = 0.0;
    //    theta = (time/600.0) * 360.0;



    //    std::cout << "theta = " << theta << std::endl;

    //    m_animation.posn_x = cos(theta);

    //    glRotatef(theta, 0,0,1);



    //    if(time > 200){
    //        m_objects["cube"]->m_color = green;

    //    }
    //    else
    //        m_objects["cube"]->m_color = red;


    ////    m_objects["cube"]->draw();


}

void GLWidget::computePosition(int time, string forObj){
    GLfloat theta = 0.0;





    if(forObj == "earth"){
        m_animation.earthTheta = ((time * 1.0)/m_nframes) * 360.0 * 0.1;
    }

    if(forObj == "body"){

        if(time <= 10){

            m_animation.bodyTheta = 0;
            m_animation.posn_x = -2;
            m_animation.posn_y = - 2;
            m_animation.posn_z = 0;

        }

        if(time >= 300 && time < 400){

//            m_animation.bodyTheta = 0;
//            m_animation.posn_x = (1 - (time/500.0))*-1+ (time/500.0) * (m_animation.cube_posnx);
//            m_animation.posn_y = (1 - (time/500.0))*-1+ (time/500.0) * (m_animation.cube_posny);
//            m_animation.posn_z = (1 - (time/500.0))*-3 + (time/500.0) * (m_animation.cube_posnz);

//            m_animation.posn_x -= 0.05f;
//            m_animation.posn_y += 0.15f;
//            m_animation.posn_z += 0.015f;
//            m_animation.posn_x  = -1.0f + (time/600.0)*(0 - 5.0f);
//            m_animation.posn_y = -1.0f + (time/600.0)*(0 - 6.0f);
//            m_animation.posn_z = -3.0f + (time/600.0)*(0 - -10.0f);
            m_animation.posn_x += m_lookAt.center.x();
            m_animation.posn_y += 1.6f*m_lookAt.center.y();
            m_animation.posn_z += 0.98*m_lookAt.center.z();

        }

        if(time >= 400 && time < 430){

//            m_animation.bodyTheta = 0;
//            m_animation.posn_x = (1 - (time/500.0))*-1+ (time/500.0) * (m_animation.cube_posnx);
//            m_animation.posn_y = (1 - (time/500.0))*-1+ (time/500.0) * (m_animation.cube_posny);
//            m_animation.posn_z = (1 - (time/500.0))*-3 + (time/500.0) * (m_animation.cube_posnz);

//            m_animation.posn_x -= 0.05f;
//            m_animation.posn_y += 0.15f;
//            m_animation.posn_z += 0.015f;
//            m_animation.posn_x  = -1.0f + (time/600.0)*(0 - 5.0f);
//            m_animation.posn_y = -1.0f + (time/600.0)*(0 - 6.0f);
//            m_animation.posn_z = -3.0f + (time/600.0)*(0 - -10.0f);
//            m_animation.posn_x += m_lookAt.center.x();
//            m_animation.posn_y += 1.9f*m_lookAt.center.y();
            m_animation.posn_z -= 0.5*m_lookAt.center.z();

        }




//        if(time >= 430 && time <= 620){

//            m_animation.bodyTheta = -90;

//m_animation.posn_x = 0;
//m_animation.posn_y = 0;
//m_animation.posn_z = 0;


////            m_animation.posn_x += m_lookAt.center.x();
////            m_animation.posn_y += 1.2*m_lookAt.center.y();
////            m_animation.posn_z += 0.8*m_lookAt.center.z();

////            m_animation.posn_x  = -1.0f + (time/600.0)*(0 - 5.0f);
////                        m_animation.posn_y = -1.0f + (time/650.0)*(0 - 6.0f);
////                        m_animation.posn_z = -3.0f + (time/650.0)*(0 - -10.0f);




//        }


    }

    if(forObj == "right_arm"){



        if(time >= 430 && time <= 450){
            m_animation.right_arm = 15.0*((time)/450.0);
        }
        else{

            m_animation.right_arm = 90;

        }

    }
    if(forObj == "right_leg"){



        if(time >= 430 && time <= 500){
            m_animation.right_leg = 180.0*((time)/500.0);
        }

        if(time >= 600 && time <= 700){
            m_animation.right_leg = 35.0*((time)/650.0);
        }
        else{

            m_animation.right_leg = 0;

        }

    }

    if(forObj == "camera"){

        QVector3D scene1(10.0777  , 12.9622 ,-21.772);

        if(time <= 100){
            resetCamera();
            m_lookAt.eye = scene1;
            m_lookAt.center = {+0.0f, +0.0f, -1.0f};
            m_lookAt.up = {+0.0f, +1.0f, +0.0f};


            // Update center vector based on rotation
            mRotation = QQuaternion(1,0.459524 , -1.04421, -0.453582);
            QMatrix4x4 mat;
            mat.setToIdentity();
            mat.rotate(mRotation);
            m_lookAt.center = {+0.0f, +0.0f, -1.0f};
            m_lookAt.center = m_lookAt.center * mat;

            //             m_lookAt.eye += m_lookAt.center * 0.85f;

            this->update();


        }

        if(time == 199){

            resetCamera();
            QVector3D scene2( 2.60386  , -1.77042 ,3.30559);
            m_lookAt.eye = scene2;
            m_lookAt.center = {+0.0f, +0.0f, -1.0f};
            m_lookAt.up = {+0.0f, +1.0f, +0.0f};

            // Update center vector based on rotation
            mRotation = QQuaternion(1, -0.157485 , 0.111672, -0.0582336);
            QMatrix4x4 mat;
            mat.setToIdentity();
            mat.rotate(mRotation);
            m_lookAt.center = {+0.0f, +0.0f, -1.0f};
            m_lookAt.center = m_lookAt.center * mat;
            this->update();


        }

        if(time >= 200 && time <= 350){ // Camera Rotation
            // Update center vector based on rotation
            auto diff = m_lookAt.center + (time/600.0)*(QVector2D( -0.120077 , 0.30348) - m_lookAt.center);
            auto n = QVector3D(diff.y(), diff.x(), 0.0).normalized();
            float mSpeed = diff.length() * 0.1f;
            mRotationAxis = (mRotationAxis + n * mSpeed).normalized();
            mRotation = QQuaternion::fromAxisAndAngle(mRotationAxis, mSpeed) * mRotation;

            // Update center vector based on rotation
            QMatrix4x4 mat;
            mat.setToIdentity();
            mat.rotate(mRotation);
            m_lookAt.center = {+0.0f, +0.0f, -1.0f};
            m_lookAt.center = m_lookAt.center * mat;

        }



        // superman
        if(time == 350){
            resetCamera();
            QVector3D scene2(-3.44906  , -1.89644 ,-1.50923);
            m_lookAt.eye = scene2;
            m_lookAt.center = {+0.0f, +0.0f, -1.0f};
            m_lookAt.up = {+0.0f, +1.0f, +0.0f};

            // Update center vector based on rotation
            mRotation = QQuaternion(1, -0.146451 , 0.355866, -0.219709);
            QMatrix4x4 mat;
            mat.setToIdentity();
            mat.rotate(mRotation);
            m_lookAt.center = {+0.0f, +0.0f, -1.0f};
            m_lookAt.center = m_lookAt.center * mat;
            this->update();

        }

        // kick
        if(time == 450){
            resetCamera();
            QVector3D scene3(1.99496  , 5 ,15.0708);
            m_lookAt.eye = scene3;
            m_lookAt.center = {+0.0f, +0.0f, -1.0f};
            m_lookAt.up = {+0.0f, +1.0f, +0.0f};

            // Update center vector based on rotation
            mRotation = QQuaternion(1, -0.0215988 , 0.0549024, -0.0048815);
            QMatrix4x4 mat;
            mat.setToIdentity();
            mat.rotate(mRotation);
            m_lookAt.center = {+0.0f, +0.0f, -1.0f};
            m_lookAt.center = m_lookAt.center * mat;

            m_perspective.zoom = 20;
            this->update();

        }

    }

    if(forObj == "cube"){
        if(time == 0){
            m_animation.cube_posnx = 5.0f;
            m_animation.cube_posny = 6.0f;
            m_animation.cube_posnz = -10.0f;
        }
        if(time > 5 && time < 500){

            m_animation.cube_posnx = 5.0f + (time/600.0)*(0 - 5.0f);
            m_animation.cube_posny = 6.0f + (time/600.0)*(0 - 6.0f);
            m_animation.cube_posnz = -10.0f + (time/600.0)*(0 - -10.0f);
        }

        if(time == 500){


            m_animation.cube_posnx = 0.f;
            m_animation.cube_posny = 0.f;
            m_animation.cube_posnz = 0.0f;


        }

        if(time >= 510){


            m_animation.cube_posnx += 0.1f;
            m_animation.cube_posny += 0.1f;
            m_animation.cube_posnz -= 0.10f;


        }
    }


}
GLWidget::~GLWidget() {
}
