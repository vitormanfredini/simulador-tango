#include "via.h"
#include <cstring>
#include <math.h>
#include <stdio.h>

Via::Via(){
}

Via::~Via(){
}

void Via::setId(int idNovo){
    id = idNovo;
}

int Via::getId(){
    return id;
}

void Via::setIdPai(int idPaiNovo){
    idPai = idPaiNovo;
}

int Via::getIdPai(){
    return idPai;
}

void Via::setCoordInicio(float x, float y, float z){
    coordInicio[0] = x;
    coordInicio[1] = y;
    coordInicio[2] = z;
}

void Via::setCoordFim(float x, float y, float z){
    coordFim[0] = x;
    coordFim[1] = y;
    coordFim[2] = z;
}

void Via::setNumDeVias(int num){
    numDeVias = num;
}

void Via::setNomeDaVia(char* nome){
    strcpy(nomeDaVia,nome);
}



float* Via::getCoordInicio(){
    return coordInicio;
}

float* Via::getCoordFim(){
    return coordFim;
}

int Via::getNumDeVias(){
    return numDeVias;
}

char* Via::getNomeDaVia(){
    return nomeDaVia;
}

void Via::gen3dObject(){
    int idPoligono;
    int c;
    float cor;

    idPoligono = objeto3d.newPoligono();
    for(c=0; c<4; c++){
        if(c < 2){
            cor = 0.56f;
        }else{
            //cor = 0.6f;
            cor = 0.56f;
        }
        objeto3d.newVertice(coords[c][0],coords[c][1],coords[c][2], cor, cor, cor, idPoligono);
    }
}

void Via::calcularCoordenadas(){

    //printf("id=%d idpai=%d  - coords %f %f - %f %f\n", id, idPai, coordInicio[0],coordInicio[2],coordFim[0],coordFim[2]);
    
    //largura das faixas
    float faixa = numDeVias * 2.0f;
    //aqui calculamos a distancia da guia da via, usando o teorema de pitagoras.
    float distanciadavia;
    distanciadavia = sqrtf(powf(coordInicio[0]-coordFim[0],2) + powf(coordInicio[2]-coordFim[2],2));
    //Agora calcularemos o terceiro ponto da via para os dois lado, que formam triangulos retangulos. (este terceiro ponto � metade da largura da via e equivale ao ponto que encosta no meio-fio)
    //Para isso, reduzimos a linha da reta para a origem do plano
    float xreduzido = coordFim[0]-coordInicio[0];
    float zreduzido = coordFim[2]-coordInicio[2];
    //formulas de rota��o
    //float xrotacionado = xreduzido*cosf(90) - zreduzido*sin(90);
    //float yrotacionado = xreduzido*sinf(90) + zreduzido*cosf(90);
    //rodamos ela em 90 graus e 270 graus
    //simplificando as equa��es acima, uma vez que cosseno de 90 e de 270 � zero, seno de 90 � 1 e seno de 270 � -1, temos:
    float x1rotacionado = -zreduzido;
    float z1rotacionado = xreduzido;
    float x2rotacionado = zreduzido;
    float z2rotacionado = -xreduzido;
    //multiplicando pela largura da faixa/2, teremos as coordenadas, reduzidas � origem.
    //e dividimos pela distancia para saber quanto � a Unidade
    //a� somamos com a coordenada de inicio (que subtraimos para reduzir � origem) e teremos as coordenadas certas.
    float x1Final = ((x1rotacionado/distanciadavia)*(faixa/2))*2 + coordInicio[0];
    float z1Final = ((z1rotacionado/distanciadavia)*(faixa/2))*2 + coordInicio[2];
    float x2Final = ((x2rotacionado/distanciadavia)*(faixa/2))*2 + coordInicio[0];
    float z2Final = ((z2rotacionado/distanciadavia)*(faixa/2))*2 + coordInicio[2];
    //agora, pra saber os outros dois pontos que formam o retangulo da via, basta somar a coordfim (com a reta da via reduzida a origem) aos pontos que ja achamos.
    float x3Final = x1Final+xreduzido;
    float z3Final = z1Final+zreduzido;
    float x4Final = x2Final+xreduzido;
    float z4Final = z2Final+zreduzido;

    //finalmente, os vertices do retangulo da via
    coords[0][0] = x2Final;
    coords[0][1] = coordInicio[1];
    coords[0][2] = z2Final;

    coords[1][0] = x1Final;
    coords[1][1] = coordInicio[1];
    coords[1][2] = z1Final;

    coords[2][0] = x3Final;
    coords[2][1] = coordFim[1];
    coords[2][2] = z3Final;
    
    coords[3][0] = x4Final;
    coords[3][1] = coordFim[1];
    coords[3][2] = z4Final;
}

void Via::render(){
    objeto3d.render();
}

bool Via::carroEstaDentroDaVia(float xDestino,float zDestino){
    float xMaior,zMaior,xMenor,zMenor;
    //verifica quais sao as coordenadas maiores e menores para compararmos do jeito certo.
    if(coordInicio[0] > coordFim[0]){
        xMaior = coordInicio[0];
        xMenor = coordFim[0];
    }else{
        xMenor = coordInicio[0];
        xMaior = coordFim[0];
    }
    if(coordInicio[2] > coordFim[2]){
        zMaior = coordInicio[2];
        zMenor = coordFim[2];
    }else{
        zMenor = coordInicio[2];
        zMaior = coordFim[2];
    }

    //folga dada devido a nao precisao dos floats
    //float folga = 0.2f + ((numDeVias-1) * 2.0f);
    float folga = 0.2f;
    if(
        (xDestino >= xMenor - folga) &&
        (xDestino <= xMaior + folga) &&
        (zDestino <= zMaior + folga) &&
        (zDestino >= zMenor - folga)
      ){
        return true;
    }else{
        return false;
    }
}

void Via::debug(){
    printf("-----\n");
    printf("id = %d\n",id);
    printf("id = %d\n",idPai);
    printf("coordInicio[0] = %f\n",coordInicio[0]);
    printf("coordInicio[2] = %f\n",coordInicio[2]);
    printf("coordFim[0] = %f\n",coordFim[0]);
    printf("coordFim[2] = %f\n",coordFim[2]);
    printf("-----\n");
}