#include "mouseecamera.h"

#include <stdio.h>
#include <math.h>
#include <stdlib.h>

//framework que s— Ž usado em mac (para centralizar o ponteiro do mouse)
#ifdef __APPLE__
#include "ApplicationServices/ApplicationServices.h"
#endif

MouseECamera::MouseECamera(){
}

MouseECamera::~MouseECamera() {
}

void MouseECamera::Init(int width, int height){
    rotateHorizontal = 0;
    rotateVertical = 0;
    totalWidth = 0;
    totalHeight = 0;
    MouseECamera::setCameraLimits(width, height);
    xAnterior = centerWidth;
    yAnterior = centerHeight - 100;//iniciar camera um pouco abaixada
    #ifdef __APPLE__
        CGSetLocalEventsSuppressionInterval(0);
    #endif
}

void MouseECamera::setCameraLimits(int width, int height) {
    totalWidth = width;
    totalHeight = height;
    centerWidth = floor((float)width/2.0f);
    centerHeight = floor((float)height/2.0f);
}

void MouseECamera::amoveTo(int x, int y){
    //a partir de quanto mexeu o mouse a partir da ultima posicao dele, roda a camera
    rotateHorizontal += (float)(-0.05f)*(xAnterior - x);
    if(rotateHorizontal < 0) rotateHorizontal += 360;
    if(rotateHorizontal > 360) rotateHorizontal -= 360;
    rotateVertical += (float)(-0.05f)*(yAnterior - y);
    if(rotateVertical < 0) rotateVertical += 360;
    if(rotateVertical > 360) rotateVertical -= 360;
    //se esta chegando perto da borda, muda ele pro meio.
    if( x <= 20 || y <= 20 || x >= totalWidth-20 || y >= totalHeight-20) {
        //centraliza o mouse
        poeMouseCentro();
    }else{
        xAnterior = x;
        yAnterior = y;
    }
}

void MouseECamera::poeMouseCentro(){
    //se esta chegando perto da borda, muda ele pro meio.
    glutWarpPointer(centerWidth,centerHeight);
    xAnterior = centerWidth;
    yAnterior = centerHeight;
}

float MouseECamera::getAnguloRotateHorizontal(){
    return rotateHorizontal;
}

float MouseECamera::getAnguloRotateVertical(){
    return rotateVertical;
}

int MouseECamera::getCenterWidth(){
    return centerWidth;
}

int MouseECamera::getCenterHeight(){
    return centerHeight;
}
