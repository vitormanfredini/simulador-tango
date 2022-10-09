#ifndef SEMAFORO_H
#define SEMAFORO_H

#include "objeto3d.h"

class Semaforo {
    
private:
    int id;
    int idVia1;
    int idVia2;
    float x;
    float z;
    float xOrientacao;
    float zOrientacao;
    int tempoVermelho;
    int tempoVerde;
    int estadoInicial;
    int estado; //1 para "verde para a via1 e vermelho para a 2" e 2 para "vermelho para a via 1 e verde para a via 2"
    Objeto3d objeto3d;
    float largura;
    float altura;
    float alturaDoChao;
    int contadorMs;
    float coordsCalculadas[8][3];
    int pontos;

public:
    void setId(int idNovo);
    void setIdVia1(int id);
    void setIdVia2(int id);
    void setX(float novoX);
    void setZ(float novoZ);
    void setXOrientacao(float novoXOrientacao);
    void setZOrientacao(float novoZOrientacao);
    void setTempoVermelho(int tempoMs);
    void setTempoVerde(int tempoMs);
    void setEstadoInicial(int novoEstado);
    void setEstado(int novoEstado);
    int getId();
    int getIdVia1();
    int getIdVia2();
    float getX();
    float getZ();
    float getXOrientacao();
    float getZOrientacao();
    int getTempoVermelho();
    int getTempoVerde();
    int getEstadoInicial();
    int getEstado();
    void calculaCoordenadas();
    void montaObjeto3d(int estado);
    void monta();
    void render();
    float getLargura();
    float getAltura();
    float getAlturaDoChao();
    void incrementaTempo(int milisegundos);
    void marcaPonto();
    int getPontos();

    Semaforo();
    virtual ~Semaforo();
};

#endif
