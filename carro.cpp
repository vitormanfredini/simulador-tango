#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctime>

#include "carro.h"
#include "conversao.h"

Carro::Carro(){
    x = 0;
    z = 0;
    xOrientacao = 0;
    zOrientacao = 0;
    conversaoAtual = 0;
    aceleracao = 0.0f;
    largura = 1.6f;
    altura = 1.2f;
    comprimento = 2.9f; //minimo 1.5f
    estado = 0; //todo carro comeca fora do mapa, at� ter espa�o para entrar.
    numConversoes = 0;
    arrConversoes = new Conversao[100];
    idViaAtual = -1;
    alturaDoChao = 0.0f;
    paradoSinalVermelhoId = -1;

    //cores randomicas para os carros
    int c=0;
    float corRandom;
    for(c=0; c<3; c++){
        corRandom = (rand() % 100 + 1);
        while(corRandom > 1){
            corRandom = corRandom/3;
        }
        cor[c] = corRandom;
    }
}

Carro::~Carro(){
    delete[] arrConversoes;
}
void Carro::setId(int idNovo){
    id = idNovo;
}
void Carro::setIdViaAtual(int id){
    idViaAtual = id;
}
void Carro::setX(float novoX){
    x = novoX;
}
void Carro::setZ(float novoZ){
    z = novoZ;
}
void Carro::setXOrientacao(float novoXOrientacao){
    xOrientacao = novoXOrientacao;
}
void Carro::setZOrientacao(float novoZOrientacao){
    zOrientacao = novoZOrientacao;
}
void Carro::setAceleracao(float novaAceleracao){
    aceleracao = novaAceleracao;
}
void Carro::setVelocidade(float novaVelocidade){
    velocidade = novaVelocidade;
}
void Carro::setSentido(int sentidoNovo){
    sentido = sentidoNovo;
}
void Carro::setEstado(int estadoNovo){
    estado = estadoNovo;
}
void Carro::setConversaoAtual(int id){
    conversaoAtual = id;
}
int Carro::getId(){
    return id;
}
int Carro::getIdViaAtual(){
    return idViaAtual;
}
float Carro::getX(){
    return x;
}
float Carro::getZ(){
    return z;
}
float Carro::getXOrientacao(){
    return xOrientacao;
}
float Carro::getZOrientacao(){
    return zOrientacao;
}
float Carro::getLargura(){
    return largura;
}
float Carro::getAltura(){
    return altura;
}
float Carro::getComprimento(){
    return comprimento;
}
float Carro::getAlturaDoChao(){
    return alturaDoChao;
}
float Carro::getAceleracao(){
    return aceleracao;
}
float Carro::getVelocidade(){
    return velocidade;
}
int Carro::getSentido(){
    return sentido;
}
int Carro::getEstado(){
    return estado;
}
int Carro::getConversaoAtual(){
    return conversaoAtual;
}

void Carro::gen3dObject(){
    //sempre antes de gerar os poligonos, limpa os ja existentes.
    objeto3d.limpa();

    int idPoligono;
    //para calcular os poligonos do carro,
    //primeiro reduzimos a reta formada pela coordenada do carro e a coordenada de orientacao dele para a origem
    float xreduzido = xOrientacao-x;
    float zreduzido = zOrientacao-z;
    //rotacionamos a reta em 90 e 270 graus para achar as pontas da face frontal
    float xrotacionado90 = xreduzido*cosf(90) - zreduzido*sin(90);
    float zrotacionado90 = xreduzido*sinf(90) + zreduzido*cosf(90);
    float xrotacionado270 = xreduzido*cosf(-90) - zreduzido*sin(-90);
    float zrotacionado270 = xreduzido*sinf(-90) + zreduzido*cosf(-90);
    //agora pegamos o comprimento do carro
    float distInicioFimCarro = getComprimento();

    //as coordenadas traseiras sao a reta rotacionada divida pela sua distancia multiplicado pela "hipotenusa" do triangulo formado
    float x1 = (xrotacionado90 /distInicioFimCarro) * (getLargura()/2) + x;
    float z1 = (zrotacionado90 /distInicioFimCarro) * (getLargura()/2) + z;
    float x2 = (xrotacionado270/distInicioFimCarro) * (getLargura()/2) + x;
    float z2 = (zrotacionado270/distInicioFimCarro) * (getLargura()/2) + z;

    //colocamos o carro na faixa da direita
    float x1rotacionado90 = (xrotacionado90 /distInicioFimCarro) * (getLargura()/2)*cosf(90) - (zrotacionado90 /distInicioFimCarro) * (getLargura()/2)*sin(90);
    float z1rotacionado90 = (xrotacionado90 /distInicioFimCarro) * (getLargura()/2)*sinf(90) + (zrotacionado90 /distInicioFimCarro) * (getLargura()/2)*cosf(90);
    x1 = x1 - (x1rotacionado90 / distInicioFimCarro) * (getLargura()*3);
    z1 = z1 - (z1rotacionado90 / distInicioFimCarro) * (getLargura()*3);
    x2 = x2 - (x1rotacionado90 / distInicioFimCarro) * (getLargura()*3);
    z2 = z2 - (z1rotacionado90 / distInicioFimCarro) * (getLargura()*3);
    
    //coordenadas frontais
    float x3 = x1 + xreduzido;
    float z3 = z1 + zreduzido;
    float x4 = x2 + xreduzido;
    float z4 = z2 + zreduzido;
    //coordenadas do teto (parte que encosta na parte dianteira)
    float xTeto1 = x1 + xreduzido/1.7f;
    float zTeto1 = z1 + zreduzido/1.7f;
    float xTeto2 = x2 + xreduzido/1.7f;
    float zTeto2 = z2 + zreduzido/1.7f;
    //coordenadas do teto (parte que encosta na parte traseira)
    float xTeto3 = x1 + xreduzido/8.0f;
    float zTeto3 = z1 + zreduzido/8.0f;
    float xTeto4 = x2 + xreduzido/8.0f;
    float zTeto4 = z2 + zreduzido/8.0f;

    float corBaixo = 0.2f;

    //face frontal
    idPoligono = objeto3d.newPoligono();
    objeto3d.newVertice(x4,getAlturaDoChao(),z4            , corBaixo, corBaixo, corBaixo, idPoligono);
    objeto3d.newVertice(x3,getAlturaDoChao(),z3            , corBaixo, corBaixo, corBaixo, idPoligono);
    objeto3d.newVertice(x3,getAlturaDoChao()+getAltura()/2,z3, cor[0]  , cor[1]  ,   cor[2], idPoligono);
    objeto3d.newVertice(x4,getAlturaDoChao()+getAltura()/2,z4, cor[0]  , cor[1]  ,   cor[2], idPoligono);
    //capo frontal
    idPoligono = objeto3d.newPoligono();
    objeto3d.newVertice(x4,getAlturaDoChao()+getAltura()/2,z4      , cor[0]/2, cor[1]/2, cor[2]/2, idPoligono);
    objeto3d.newVertice(x3,getAlturaDoChao()+getAltura()/2,z3      , cor[0]/1.25, cor[1]/1.25, cor[2]/1.25, idPoligono);
    objeto3d.newVertice(xTeto1,getAlturaDoChao()+getAltura(),zTeto1, cor[0]/2, cor[1]/2, cor[2]/2, idPoligono);
    objeto3d.newVertice(xTeto2,getAlturaDoChao()+getAltura(),zTeto2, cor[0]/1.25, cor[1]/1.25, cor[2]/1.25, idPoligono);
    //teto
    idPoligono = objeto3d.newPoligono();
    objeto3d.newVertice(xTeto1,getAlturaDoChao()+getAltura(),zTeto1, cor[0]/1.2  , cor[1]/1.2  ,   cor[2]/1.2, idPoligono);
    objeto3d.newVertice(xTeto2,getAlturaDoChao()+getAltura(),zTeto2, cor[0]/1.2  , cor[1]/1.2  ,   cor[2]/1.2, idPoligono);
    objeto3d.newVertice(xTeto4,getAlturaDoChao()+getAltura(),zTeto4, cor[0]/1.2  , cor[1]/1.2  ,   cor[2]/1.2, idPoligono);
    objeto3d.newVertice(xTeto3,getAlturaDoChao()+getAltura(),zTeto3, cor[0]/1.2  , cor[1]/1.2  ,   cor[2]/1.2, idPoligono);
    //capo traseiro
    idPoligono = objeto3d.newPoligono();
    objeto3d.newVertice(x1,getAlturaDoChao()+getAltura()/2,z1,       cor[0]/2  , cor[1]/2  ,   cor[2]/2, idPoligono);
    objeto3d.newVertice(x2,getAlturaDoChao()+getAltura()/2,z2,       cor[0]/1.25  , cor[1]/1.25  ,   cor[2]/1.25, idPoligono);
    objeto3d.newVertice(xTeto4,getAlturaDoChao()+getAltura(),zTeto4, cor[0]/2  , cor[1]/2,   cor[2]/2, idPoligono);
    objeto3d.newVertice(xTeto3,getAlturaDoChao()+getAltura(),zTeto3, cor[0]/1.25  , cor[1]/1.25  ,   cor[2]/1.25, idPoligono);
    //face traseira
    idPoligono = objeto3d.newPoligono();
    objeto3d.newVertice(x1,getAlturaDoChao()+getAltura()/2,z1, cor[0]  , cor[1]  ,   cor[2], idPoligono);
    objeto3d.newVertice(x2,getAlturaDoChao()+getAltura()/2,z2, cor[0]  , cor[1]  ,   cor[2], idPoligono);
    objeto3d.newVertice(x2,getAlturaDoChao(),z2            , corBaixo, corBaixo, corBaixo, idPoligono);
    objeto3d.newVertice(x1,getAlturaDoChao(),z1            , corBaixo, corBaixo, corBaixo, idPoligono);

    //face lateral esquerda baixo
    idPoligono = objeto3d.newPoligono();
    objeto3d.newVertice(x1,getAlturaDoChao(),z1            , corBaixo, corBaixo, corBaixo, idPoligono);
    objeto3d.newVertice(x3,getAlturaDoChao(),z3            , corBaixo, corBaixo, corBaixo, idPoligono);
    objeto3d.newVertice(x3,getAlturaDoChao()+getAltura()/2,z3, cor[0]  , cor[1]  ,   cor[2], idPoligono);
    objeto3d.newVertice(x1,getAlturaDoChao()+getAltura()/2,z1, cor[0]  , cor[1]  ,   cor[2], idPoligono);
    //face lateral esquerda cima
    idPoligono = objeto3d.newPoligono();
    objeto3d.newVertice(x3,getAlturaDoChao()+getAltura()/2,z3, cor[0]  , cor[1]  ,   cor[2], idPoligono);
    objeto3d.newVertice(x1,getAlturaDoChao()+getAltura()/2,z1, cor[0]  , cor[1]  ,   cor[2], idPoligono);
    objeto3d.newVertice(xTeto3,getAlturaDoChao()+getAltura(),zTeto3, cor[0]/1.2  , cor[1]/1.2  ,   cor[2]/1.2, idPoligono);
    objeto3d.newVertice(xTeto1,getAlturaDoChao()+getAltura(),zTeto1, cor[0]/1.2  , cor[1]/1.2  ,   cor[2]/1.2, idPoligono);
    


    //face lateral direita cima
    idPoligono = objeto3d.newPoligono();
    objeto3d.newVertice(x2,getAlturaDoChao(),z2, corBaixo, corBaixo, corBaixo, idPoligono);
    objeto3d.newVertice(x4,getAlturaDoChao(),z4, corBaixo, corBaixo, corBaixo, idPoligono);
    objeto3d.newVertice(x4,getAlturaDoChao()+getAltura()/2,z4, cor[0]  , cor[1]  ,   cor[2], idPoligono);
    objeto3d.newVertice(x2,getAlturaDoChao()+getAltura()/2,z2, cor[0]  , cor[1]  ,   cor[2], idPoligono);
    //face lateral direita baixo
    idPoligono = objeto3d.newPoligono();
    objeto3d.newVertice(x4,getAlturaDoChao()+getAltura()/2,z4, cor[0]  , cor[1]  ,   cor[2], idPoligono);
    objeto3d.newVertice(x2,getAlturaDoChao()+getAltura()/2,z2, cor[0]  , cor[1]  ,   cor[2], idPoligono);
    objeto3d.newVertice(xTeto4,getAlturaDoChao()+getAltura(),zTeto4, cor[0]/1.2  , cor[1]/1.2  ,   cor[2]/1.2, idPoligono);
    objeto3d.newVertice(xTeto2,getAlturaDoChao()+getAltura(),zTeto2, cor[0]/1.2  , cor[1]/1.2  ,   cor[2]/1.2, idPoligono);


    //face inferior
    idPoligono = objeto3d.newPoligono();
    objeto3d.newVertice(x1,getAlturaDoChao(),z1, corBaixo, corBaixo, corBaixo, idPoligono);
    objeto3d.newVertice(x2,getAlturaDoChao(),z2, corBaixo, corBaixo, corBaixo, idPoligono);
    objeto3d.newVertice(x4,getAlturaDoChao(),z4, corBaixo, corBaixo, corBaixo, idPoligono);
    objeto3d.newVertice(x3,getAlturaDoChao(),z3, corBaixo, corBaixo, corBaixo, idPoligono);
    

}
void Carro::render(){
    objeto3d.render();
}
void Carro::addConversao(Conversao conversao){
    conversao.setId(numConversoes);
    arrConversoes[numConversoes] = conversao;
    numConversoes++;
}

int Carro::getIdProximaVia(){
    int c;
    for(c=0;c<numConversoes;c++){
        if(conversaoAtual == arrConversoes[c].getId()){
            if(arrConversoes[c].getIdViaDestino() == -1){
                conversaoAtual = 0;
            }
            return arrConversoes[c].getIdViaDestino();
        }
    }

    printf("A via de origem %d nao esta prevista nas conversoes do carro %d ao tentar pegar id da proxima via\n",getIdViaAtual(),getId());
    exit(1);
}

int Carro::getLadoConversaoParaProximaVia(){

    int c;
    for(c=0;c<numConversoes;c++){
        if(conversaoAtual == arrConversoes[c].getId()){
            if(arrConversoes[c].getIdViaDestino() == -1){
                conversaoAtual = 0;
            }
            return arrConversoes[c].getLado();
        }
    }
    printf("A via de origem %d nao esta prevista nas conversoes do carro %d ao tentar pegar o lado da conversao para a proxima via\n",getIdViaAtual(),getId());
    exit(1);
}
int Carro::getNumConversoes(){
    return numConversoes;
}
int Carro::getIdViaInicial(){
    if(numConversoes > 0){
        return arrConversoes[0].getIdViaOrigem();
    }else{
        printf("A carro %d nao tem conversoes para pegar id da via inicial\n",getId());
        exit(1);
    }
}

void Carro::parouNoSinalVermelho(int idSemaforo){
    paradoSinalVermelhoId = idSemaforo;
}
void Carro::andouDoSinalVermelho(){
    paradoSinalVermelhoId = -1;
}
bool Carro::estaParadoNoSinalVermelho(){
    if(paradoSinalVermelhoId == -1){
        return false;
    }
    return true;
}
int Carro::getIdSemaforoVermelho(){
    return paradoSinalVermelhoId;
}