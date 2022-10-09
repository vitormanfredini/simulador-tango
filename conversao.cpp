#include "conversao.h"

Conversao::Conversao(){
}

Conversao::~Conversao(){
}

void Conversao::setId(int idNovo){
    id = idNovo;
}
void Conversao::setIdCarro(int id){
    idCarro = id;
}
void Conversao::setIdViaOrigem(int id){
    idViaOrigem = id;
}
void Conversao::setIdViaDestino(int id){
    idViaDestino = id;
}
void Conversao::setLado(int novoLado){
    lado = novoLado;
}
int Conversao::getId(){
    return id;
}
int Conversao::getIdCarro(){
    return idCarro;
}
int Conversao::getIdViaOrigem(){
    return idViaOrigem;
}
int Conversao::getIdViaDestino(){
    return idViaDestino;
}
int Conversao::getLado(){
    return lado;
}