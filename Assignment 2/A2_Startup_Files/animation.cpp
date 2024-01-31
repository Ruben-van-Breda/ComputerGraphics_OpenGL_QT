#include "animation.h"
const float PI = 3.1415926f;
 Cube *cube = new Cube();
animation::animation()
{
    glPushMatrix();

    cube->draw();
    glPopMatrix();
}

void computePosition(int time){
    float theta = (time/600) * 2 * PI;
    float x = cos(theta);
    float y = sin(theta);

    glTranslatef(x,y,0.0);
    cube->draw();
}




