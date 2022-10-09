#ifndef CARRO_H
#define CARRO_H

#include "objeto3d.h"
#include "conversao.h"

class Carro {
    
private:
    int id;
    float x;
    float z;
    float xOrientacao;
    float zOrientacao;
    float largura;
    float altura;
    float comprimento;
    float alturaDoChao;
    int idViaAtual;
    int sentido; //1 para inicio-fim e 2 para fim-inicio.
    float velocidade;
    float aceleracao;
    Objeto3d objeto3d;
    int estado; //0 para fora do mapa, 1 para parado e 2 para andando
    Conversao *arrConversoes;
    int conversaoAtual;
    int numConversoes;
    float cor[3];
    int paradoSinalVermelhoId;

public:
    void setId(int idNovo);
    void setIdViaAtual(int id);
    void setX(float novoX);
    void setZ(float novoZ);
    void setXOrientacao(float novoXOrientacao);
    void setZOrientacao(float novoZOrientacao);
    void setAceleracao(float novaAceleracao);
    void setVelocidade(float novaVelocidade);
    void setSentido(int sentidoNovo);
    void setEstado(int estadoNovo);
    void setConversaoAtual(int id);
    int getId();
    int getIdViaAtual();
    float getX();
    float getZ();
    float getXOrientacao();
    float getZOrientacao();
    void gen3dObject();
    float getLargura();
    float getAltura();
    float getAlturaDoChao();
    float getComprimento();
    float getAceleracao();
    float getVelocidade();
    int getSentido();
    int getEstado();
    void render();
    void addConversao(Conversao conversao);
    int getIdProximaVia();
    int getLadoConversaoParaProximaVia();
    int getNumConversoes();
    int getIdViaInicial();
    int getConversaoAtual();
    void parouNoSinalVermelho(int idSemaforo);
    void andouDoSinalVermelho();
    bool estaParadoNoSinalVermelho();
    int getIdSemaforoVermelho();


    Carro();
    virtual ~Carro();
};

#endif
