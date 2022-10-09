#include <math.h>

#include "semaforo.h"
#include <stdio.h>

Semaforo::Semaforo(){
    largura = 0.30f;
    altura = 0.50f;
    alturaDoChao = 4.0f;
    contadorMs = 0;
    pontos = 0;
}

Semaforo::~Semaforo(){
}

void Semaforo::setId(int idNovo){
    id = idNovo;
}
void Semaforo::setIdVia1(int id){
    idVia1 = id;
}
void Semaforo::setIdVia2(int id){
    idVia2 = id;
}
void Semaforo::setX(float novoX){
    x = novoX;
}
void Semaforo::setZ(float novoZ){
    z = novoZ;
}
void Semaforo::setXOrientacao(float novoXOrientacao){
    xOrientacao = novoXOrientacao;
}
void Semaforo::setZOrientacao(float novoZOrientacao){
    zOrientacao = novoZOrientacao;
}
void Semaforo::setTempoVermelho(int tempoMs){
    tempoVermelho = tempoMs;
}
void Semaforo::setTempoVerde(int tempoMs){
    tempoVerde = tempoMs;
}
void Semaforo::setEstado(int novoEstado){
    estado = novoEstado;
}
void Semaforo::setEstadoInicial(int novoEstado){
    estadoInicial = novoEstado;
}
int Semaforo::getId(){
    return id;
}
int Semaforo::getIdVia1(){
    return idVia1;
}
int Semaforo::getIdVia2(){
    return idVia2;
}
float Semaforo::getX(){
    return x;
}
float Semaforo::getZ(){
    return z;
}
float Semaforo::getXOrientacao(){
    return xOrientacao;
}
float Semaforo::getZOrientacao(){
    return zOrientacao;
}
int Semaforo::getTempoVermelho(){
    return tempoVermelho;
}
int Semaforo::getTempoVerde(){
    return tempoVerde;
}
int Semaforo::getEstado(){
    return estado;
}
int Semaforo::getEstadoInicial(){
    return estadoInicial;
}
void Semaforo::calculaCoordenadas(){
    //para calcular os poligonos do semaforo,
    //primeiro reduzimos a reta formada pela coordenada do semaforo e a coordenada de orientacao dele para a origem
    float xreduzido = xOrientacao-x;
    float zreduzido = zOrientacao-z;
    //rotacionamos a reta em 45 e 315 graus para achar as pontas da face frontal
    float xrotacionado45 = xreduzido*cosf(45) - zreduzido*sin(45);
    float zrotacionado45 = xreduzido*sinf(45) + zreduzido*cosf(45);
    float xrotacionado315 = xreduzido*cosf(-45) - zreduzido*sin(-45);
    float zrotacionado315 = xreduzido*sinf(-45) + zreduzido*cosf(-45);
    //agora pegamos a distancia do centro do semaforo at� os pontos da face frontal (igual para os dois)
    float distCentroAteVerticeFrontal = (getLargura()/2)*sqrtf(2);
    
    //coordenadas da face de cima
    int aumentarTopo = 1;
    //as coordenadas frontais sao a reta rotacionada divida pela sua distancia multiplicado pela "hipotenusa" do triangulo formado para resolver o problema
    coordsCalculadas[0][0] = (xrotacionado45/ (getLargura()/2)) * (distCentroAteVerticeFrontal) * aumentarTopo + x;
    coordsCalculadas[0][1] = getAlturaDoChao()+getAltura();
    coordsCalculadas[0][2] = (zrotacionado45/ (getLargura()/2)) * (distCentroAteVerticeFrontal) * aumentarTopo + z;
    coordsCalculadas[1][0] = (xrotacionado315/(getLargura()/2)) * (distCentroAteVerticeFrontal) * aumentarTopo + x;
    coordsCalculadas[1][1] = getAlturaDoChao()+getAltura();
    coordsCalculadas[1][2] = (zrotacionado315/(getLargura()/2)) * (distCentroAteVerticeFrontal) * aumentarTopo + z;
    //coordenadas traseiras
    coordsCalculadas[2][0] = coordsCalculadas[0][0] - xreduzido * 2 * aumentarTopo ;
    coordsCalculadas[2][1] = getAlturaDoChao()+getAltura();
    coordsCalculadas[2][2] = coordsCalculadas[0][2] - zreduzido * 2 * aumentarTopo ;
    coordsCalculadas[3][0] = coordsCalculadas[1][0] - xreduzido * 2 * aumentarTopo ;
    coordsCalculadas[3][1] = getAlturaDoChao()+getAltura();
    coordsCalculadas[3][2] = coordsCalculadas[1][2] - zreduzido * 2 * aumentarTopo ;
    
    //coordenadas da face debaixo
    int aumentarBase = 1;
    //coordenadas frontais
    coordsCalculadas[4][0] = (xrotacionado45/ (getLargura()/2)) * (distCentroAteVerticeFrontal) * aumentarBase + x ;
    coordsCalculadas[4][1] = getAlturaDoChao();
    coordsCalculadas[4][2] = (zrotacionado45/ (getLargura()/2)) * (distCentroAteVerticeFrontal) * aumentarBase + z;
    coordsCalculadas[5][0] = (xrotacionado315/(getLargura()/2)) * (distCentroAteVerticeFrontal) * aumentarBase + x;
    coordsCalculadas[5][1] = getAlturaDoChao();
    coordsCalculadas[5][2] = (zrotacionado315/(getLargura()/2)) * (distCentroAteVerticeFrontal) * aumentarBase + z;
    //coordenadas traseiras
    coordsCalculadas[6][0] = coordsCalculadas[4][0] - xreduzido * 2 * aumentarBase;
    coordsCalculadas[6][1] = getAlturaDoChao();
    coordsCalculadas[6][2] = coordsCalculadas[4][2] - zreduzido * 2 * aumentarBase;
    coordsCalculadas[7][0] = coordsCalculadas[5][0] - xreduzido * 2 * aumentarBase;
    coordsCalculadas[7][1] = getAlturaDoChao();
    coordsCalculadas[7][2] = coordsCalculadas[5][2] - zreduzido * 2 * aumentarBase;
}

void Semaforo::montaObjeto3d(int estado){
    int idPoligono;
    float Via1Verde,Via1Vermelho,Via2Verde,Via2Vermelho;
    //limpamos o objeto
    objeto3d.limpa();
    //seta cores que sinalizarao para as vias, dependendo do tempo semaforo. Via1 � a via principal do sem�foro e Via2 � a secundaria.
    if(estado == 1){
        Via1Verde = 1.0f;
        Via1Vermelho = 0.0f;
        Via2Verde = 0.0f;
        Via2Vermelho = 1.0f;
    }else{
        Via1Verde = 0.0f;
        Via1Vermelho = 1.0f;
        Via2Verde = 1.0f;
        Via2Vermelho = 0.0f;
    }
    float cinzaEscuro = 0.4f;

    //face principal
    idPoligono = objeto3d.newPoligono();
    objeto3d.newVertice(coordsCalculadas[0][0],getAlturaDoChao()+getAltura(),coordsCalculadas[0][2], Via1Vermelho, Via1Verde, 0.0f, idPoligono);
    objeto3d.newVertice(coordsCalculadas[1][0],getAlturaDoChao()+getAltura(),coordsCalculadas[1][2], Via1Vermelho, Via1Verde, 0.0f, idPoligono);
    objeto3d.newVertice(coordsCalculadas[5][0],getAlturaDoChao()            ,coordsCalculadas[5][2], Via1Vermelho, Via1Verde, 0.0f, idPoligono);
    objeto3d.newVertice(coordsCalculadas[4][0],getAlturaDoChao()            ,coordsCalculadas[4][2], Via1Vermelho, Via1Verde, 0.0f, idPoligono);
    //face principal
    idPoligono = objeto3d.newPoligono();
    objeto3d.newVertice(coordsCalculadas[7][0],getAlturaDoChao()            ,coordsCalculadas[7][2], Via1Vermelho, Via1Verde, 0.0f, idPoligono);
    objeto3d.newVertice(coordsCalculadas[6][0],getAlturaDoChao()            ,coordsCalculadas[6][2], Via1Vermelho, Via1Verde, 0.0f, idPoligono);
    objeto3d.newVertice(coordsCalculadas[2][0],getAlturaDoChao()+getAltura(),coordsCalculadas[2][2], Via1Vermelho, Via1Verde, 0.0f, idPoligono);
    objeto3d.newVertice(coordsCalculadas[3][0],getAlturaDoChao()+getAltura(),coordsCalculadas[3][2], Via1Vermelho, Via1Verde, 0.0f, idPoligono);
    
    //face lateral
    idPoligono = objeto3d.newPoligono();
    objeto3d.newVertice(coordsCalculadas[4][0],getAlturaDoChao()            ,coordsCalculadas[4][2], Via2Vermelho, Via2Verde, 0.0f, idPoligono);
    objeto3d.newVertice(coordsCalculadas[6][0],getAlturaDoChao()            ,coordsCalculadas[6][2], Via2Vermelho, Via2Verde, 0.0f, idPoligono);
    objeto3d.newVertice(coordsCalculadas[2][0],getAlturaDoChao()+getAltura(),coordsCalculadas[2][2], Via2Vermelho, Via2Verde, 0.0f, idPoligono);
    objeto3d.newVertice(coordsCalculadas[0][0],getAlturaDoChao()+getAltura(),coordsCalculadas[0][2], Via2Vermelho, Via2Verde, 0.0f, idPoligono);
    //face lateral
    idPoligono = objeto3d.newPoligono();
    objeto3d.newVertice(coordsCalculadas[5][0],getAlturaDoChao()            ,coordsCalculadas[5][2], Via2Vermelho, Via2Verde, 0.0f, idPoligono);
    objeto3d.newVertice(coordsCalculadas[7][0],getAlturaDoChao()            ,coordsCalculadas[7][2], Via2Vermelho, Via2Verde, 0.0f, idPoligono);
    objeto3d.newVertice(coordsCalculadas[3][0],getAlturaDoChao()+getAltura(),coordsCalculadas[3][2], Via2Vermelho, Via2Verde, 0.0f, idPoligono);
    objeto3d.newVertice(coordsCalculadas[1][0],getAlturaDoChao()+getAltura(),coordsCalculadas[1][2], Via2Vermelho, Via2Verde, 0.0f, idPoligono);

    //face inferior
    idPoligono = objeto3d.newPoligono();
    objeto3d.newVertice(coordsCalculadas[4][0],getAlturaDoChao()            ,coordsCalculadas[4][2], cinzaEscuro, cinzaEscuro, cinzaEscuro, idPoligono);
    objeto3d.newVertice(coordsCalculadas[5][0],getAlturaDoChao()            ,coordsCalculadas[5][2], cinzaEscuro, cinzaEscuro, cinzaEscuro, idPoligono);
    objeto3d.newVertice(coordsCalculadas[7][0],getAlturaDoChao()            ,coordsCalculadas[7][2], cinzaEscuro, cinzaEscuro, cinzaEscuro, idPoligono);
    objeto3d.newVertice(coordsCalculadas[6][0],getAlturaDoChao()            ,coordsCalculadas[6][2], cinzaEscuro, cinzaEscuro, cinzaEscuro, idPoligono);
    //face superior
    idPoligono = objeto3d.newPoligono();
    objeto3d.newVertice(coordsCalculadas[0][0],getAlturaDoChao()+getAltura(),coordsCalculadas[0][2], cinzaEscuro, cinzaEscuro, cinzaEscuro, idPoligono);
    objeto3d.newVertice(coordsCalculadas[1][0],getAlturaDoChao()+getAltura(),coordsCalculadas[1][2], cinzaEscuro, cinzaEscuro, cinzaEscuro, idPoligono);
    objeto3d.newVertice(coordsCalculadas[3][0],getAlturaDoChao()+getAltura(),coordsCalculadas[3][2], cinzaEscuro, cinzaEscuro, cinzaEscuro, idPoligono);
    objeto3d.newVertice(coordsCalculadas[2][0],getAlturaDoChao()+getAltura(),coordsCalculadas[2][2], cinzaEscuro, cinzaEscuro, cinzaEscuro, idPoligono);

}
void Semaforo::render(){
    objeto3d.render();
}
void Semaforo::incrementaTempo(int milisegundos){
    contadorMs += milisegundos;
    switch (estado){
        case 1:
            if(contadorMs > tempoVerde){
                contadorMs = 0;
                estado = 2;
                montaObjeto3d(estado);
            }
            break;
        case 2:
            if(contadorMs > tempoVermelho){
                contadorMs = 0;
                estado = 1;
                montaObjeto3d(estado);
            }
            break;
    }
}
float Semaforo::getLargura(){
    return largura;
}
float Semaforo::getAltura(){
    return altura;
}
float Semaforo::getAlturaDoChao(){
    return alturaDoChao;
}
void Semaforo::marcaPonto(){
    pontos += 1;
}
int Semaforo::getPontos(){
    return pontos;
}