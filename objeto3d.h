#ifndef OBJETO3D_H
#define OBJETO3D_H

#ifdef __APPLE__
#include <OpenGL/OpenGL.h>
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

class Objeto3d {

    struct Vertice {
        //pontos geometricos
        float x;
        float y;
        float z;
        //componentes de cor (red, green e blue)
        float r;
        float g;
        float b;
    };

    struct Poligono {
        //cada poligono tem seus vertices
        Vertice arrVertices[4];
        int qtdVertices;
    };

private:
    Poligono arrPoligonos[20];
    int idTextura;
    int idMaterial;
    int qtdPoligonos;
public:
    int newPoligono();
    void newVertice(float x, float y, float z, float r, float g, float b, int idPoligono);
    void setTextura(int idTextura);
    void setMaterial(int idMaterial);
    void render();
    void limpa();

    Objeto3d();
    virtual ~Objeto3d();
};

#endif
