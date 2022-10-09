#include "tecladoemovimentacao.h"

#include <stdio.h>
#include <math.h>
#include <stdlib.h>

TecladoEMovimentacao::TecladoEMovimentacao(){
    somador = 0.04f;
    flagCima = false;
    flagBaixo = false;
    flagEsquerda = false;
    flagDireita = false;
    MovEsq = 0.0f;
    MovDir = 0.0f;
    MovCim = 0.0f;
    MovBai = 0.0f;

    teste = false;
}

TecladoEMovimentacao::~TecladoEMovimentacao() {
}

//trata eventos de teclado
//1 para keypress e 2 para keyup
void TecladoEMovimentacao::handleKeyboardEvent(unsigned char key, int x, int y, int tipo){
    switch (key){
        case 27: //ESC
            exit(0);
        case 'r':
            if(tipo == 1){
                teste = true;
            }
        break;
        case 119: //cima
            if(tipo == 1){
                flagCima = true;
                flagBaixo = false;
            }
            if(tipo == 2){
                flagCima = false;
            }
        break;
        case 115: //baixo
            if(tipo == 1){
                flagCima = false;
                flagBaixo = true;
            }
            if(tipo == 2){
                flagBaixo = false;
            }
        break;
        case 97: //esquerda
            if(tipo == 1){
                flagDireita = false;
                flagEsquerda = true;
            }
            if(tipo == 2){
                flagEsquerda = false;
            }
        break;
        case 100: //direita
            if(tipo == 1){
                flagDireita = true;
                flagEsquerda = false;
            }
            if(tipo == 2){
                flagDireita = false;
            }
        break;
        case 111: //subir
            if(tipo == 1){
                flagDescer = true;
                flagSubir = false;
            }
            if(tipo == 2){
                flagDescer = false;
            }
        break;
        case 112: //descer
            if(tipo == 1){
                flagSubir = true;
                flagDescer = false;
            }
            if(tipo == 2){
                flagSubir = false;
            }
        break;
    }

}

float TecladoEMovimentacao::getMovimentacaoCima(){
    if(flagCima == true){
        if(MovCim <= 2){
            MovCim = MovCim + somador;
        }
    }else{
        if(MovCim > 0){
            MovCim = MovCim - (somador * 3);
        }else{
            if(MovCim < 0){
                MovCim = 0;
            }
        }
    }
    return MovCim;
}

float TecladoEMovimentacao::getMovimentacaoBaixo(){
    if(flagBaixo == true){
        if(MovBai <= 2){
            MovBai = MovBai + somador;
        }
    }else{
        if(MovBai > 0){
            MovBai = MovBai - (somador * 3);
        }else{
            if(MovBai < 0){
                MovBai = 0;
            }
        }
    }
    return MovBai;
}

float TecladoEMovimentacao::getMovimentacaoEsquerda(){
    if(flagEsquerda == true){
        if(MovEsq <= 2){
            MovEsq = MovEsq + somador;
        }
    }else{
        if(MovEsq > 0){
            MovEsq = MovEsq - (somador * 3);
        }else{
            if(MovEsq < 0){
                MovEsq = 0;
            }
        }
    }
    return MovEsq;
}

float TecladoEMovimentacao::getMovimentacaoDireita(){
    if(flagDireita == true){
        if(MovDir <= 2){
            MovDir = MovDir + somador;
        }
    }else{
        if(MovDir > 0){
            MovDir = MovDir - (somador * 3);
        }else{
            if(MovDir < 0){
                MovDir = 0;
            }
        }
    }
    return MovDir;
}

float TecladoEMovimentacao::getMovimentacaoSubir(){
    if(flagSubir == true){
        if(MovSubir <= 2){
            MovSubir = MovSubir + somador;
        }
    }else{
        if(MovSubir > 0){
            MovSubir = MovSubir - (somador * 3);
        }else{
            if(MovSubir < 0){
                MovSubir = 0;
            }
        }
    }
    return MovSubir;
}

float TecladoEMovimentacao::getMovimentacaoDescer(){
    if(flagDescer == true){
        if(MovDescer <= 2){
            MovDescer = MovDescer + somador;
        }
    }else{
        if(MovDescer > 0){
            MovDescer = MovDescer - (somador * 3);
        }else{
            if(MovDescer < 0){
                MovDescer = 0;
            }
        }
    }
    return MovDescer;
}
