#include "objeto3d.h"

#include <stdio.h>
#include <math.h>
#include <stdlib.h>

#ifdef __APPLE__
#include <OpenGL/OpenGL.h>
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

Objeto3d::Objeto3d() {
    qtdPoligonos = 0;
}

Objeto3d::~Objeto3d() {
}

void Objeto3d::newVertice(float x, float y, float z, float r, float g, float b, int idPoligono){
    arrPoligonos[idPoligono].arrVertices[arrPoligonos[idPoligono].qtdVertices].x = x;
    arrPoligonos[idPoligono].arrVertices[arrPoligonos[idPoligono].qtdVertices].y = y;
    arrPoligonos[idPoligono].arrVertices[arrPoligonos[idPoligono].qtdVertices].z = z;
    arrPoligonos[idPoligono].arrVertices[arrPoligonos[idPoligono].qtdVertices].r = r;
    arrPoligonos[idPoligono].arrVertices[arrPoligonos[idPoligono].qtdVertices].g = g;
    arrPoligonos[idPoligono].arrVertices[arrPoligonos[idPoligono].qtdVertices].b = b;
    arrPoligonos[idPoligono].qtdVertices++;
}

int Objeto3d::newPoligono(){
    arrPoligonos[qtdPoligonos].qtdVertices = 0;
    return qtdPoligonos++;
}

void Objeto3d::setTextura(int idTextura) {

}

void Objeto3d::setMaterial(int idMaterial) {

}

//renderiza os poligonos
void Objeto3d::render(){
    int cp;
    int cv;

    for(cp=0; cp < qtdPoligonos; cp++){
        glPushMatrix();
        //glEnable(GL_TEXTURE_2D);
        //glBindTexture(GL_TEXTURE_2D, textureid);
        switch (arrPoligonos[cp].qtdVertices){
            case 4: //se tiver 4 vertices, renderiza quadrado
                glBegin(GL_QUADS);
                break;
            case 3: //se tiver 3 vertices, renderiza triangulo
                glBegin(GL_TRIANGLES);
                break;
            case 2: //se tiver 2 vertices, renderiza linha
                glBegin(GL_LINES);
                break;
            default: //se tiver 1 vertices ou menos, n‹o faz nada.
                return;
        }
        for(cv=0; cv < arrPoligonos[cp].qtdVertices; cv++){
            glColor3f(arrPoligonos[cp].arrVertices[cv].r, arrPoligonos[cp].arrVertices[cv].g, arrPoligonos[cp].arrVertices[cv].b);
            glVertex3f(arrPoligonos[cp].arrVertices[cv].x, arrPoligonos[cp].arrVertices[cv].y, arrPoligonos[cp].arrVertices[cv].z);
        }
        glEnd();
        //glDisable(GL_TEXTURE_2D);
        glPopMatrix();
    }
}

void Objeto3d::limpa(){
    qtdPoligonos=0;
}