#ifndef TECLADOEMOVIMENTACAO_H
#define TECLADOEMOVIMENTACAO_H

#ifdef __APPLE__
#include <OpenGL/OpenGL.h>
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

class TecladoEMovimentacao {
private:
    float somador;
    //flags de movimentação
    bool flagCima;
    bool flagBaixo;
    bool flagEsquerda;
    bool flagDireita;
    bool flagSubir;
    bool flagDescer;
    //Quocientes de movimentacao
    float MovEsq;
    float MovDir;
    float MovCim;
    float MovBai;
    float MovSubir;
    float MovDescer;


    bool teste;
public:
        void handleKeyboardEvent(unsigned char key, int x, int y, int tipo);

        float getMovimentacaoCima();
        float getMovimentacaoBaixo();
        float getMovimentacaoEsquerda();
        float getMovimentacaoDireita();
        float getMovimentacaoSubir();
        float getMovimentacaoDescer();

	TecladoEMovimentacao();
	virtual ~TecladoEMovimentacao();
};

#endif
