#include "interseccao.h"

Interseccao::Interseccao(){
}

Interseccao::~Interseccao(){
}
void Interseccao::setIdVia1(int id){
    idVia1 = id;
}
void Interseccao::setIdVia2(int id){
    idVia2 = id;
}
void Interseccao::setX(float novoX){
    x = novoX;
}
void Interseccao::setZ(float novoZ){
    z = novoZ;
}
int Interseccao::getIdVia1(){
    return idVia1;
}
int Interseccao::getIdVia2(){
    return idVia2;
}
float Interseccao::getX(){
    return x;
}
float Interseccao::getZ(){
    return z;
}