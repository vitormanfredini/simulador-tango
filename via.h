#ifndef VIA_H
#define VIA_H

#include "objeto3d.h"

class Via {
    
private:
    int id;
    int idPai;
    float coordInicio[3];
    float coordFim[3];
    int numDeVias;
    char nomeDaVia[200];
    Objeto3d objeto3d;
    float coords[4][3];// 4 coordenadas da via com a largura aplicada.

public:
    void setId(int idNovo);
    void setIdPai(int idPaiNovo);
    void setCoordInicio(float x, float y, float z);
    void setCoordFim(float x, float y, float z);
    void setNumDeVias(int num);
    void setNomeDaVia(char* nome);

    int getId();
    int getIdPai();
    float* getCoordInicio();
    float* getCoordFim();
    int getNumDeVias();
    char* getNomeDaVia();
    void gen3dObject();
    void calcularCoordenadas();
    void render();
    bool carroEstaDentroDaVia(float xDestino,float zDestino);
    void debug();

    Via();
    virtual ~Via();
};

#endif
