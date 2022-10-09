#ifndef MAPA_H
#define MAPA_H

#include "via.h"
#include "conversao.h"
#include "semaforo.h"
#include "interseccao.h"
#include "carro.h"
#include <pthread.h>

class Mapa {

private:
    Via *arrVias;
    Conversao *arrConversoes;
    Semaforo *arrSemaforos;
    Interseccao *arrInterseccoes;
    Carro *arrCarros;
    int numViasCarregadas;
    int numConversoesCarregadas;
    int numSemaforosCarregados;
    int numInterseccoesCarregadas;
    int numCarrosCarregados;
    int atualizacaoMs; //tempo em milisegundos entre cada atualizacao
    float precisaoFloat; //diferen�a aceitavel no calculo de floats
    int numExecucao;
    const char *mapaFilename;
    pthread_mutex_t mutex;

    //atributos de configura��o
    float distanciaAceitavelCarroDaFrente;
public:
    void init(int miliSegundos,const char *arquivoMapa,const char *arquivoSemaforos, int flagModoExecucao);
    void loadfile(const char *filename);
    void gen3dObjectDasVias();
    bool dividirViasConcorrentes();
    void render(int modoExecucao, bool boolMarcaPontosSemaforos);
    bool eIgualf(float num1, float num2);
    bool eIgualf(float num1, float num2, float tolerancia);
    void salvaInterseccoes();
    void calcularOrientacaoSemaforos();
    void calcularOrientacaoCarro(Carro & carro, int flagPosicionamento);//0 para procurar a posicao inicial na via - 1 para inicio e 2 para fim
    bool existeInterseccao(int idVia1,int idVia2);
    float getCoordInterseccao(int idVia1,int idVia2,char coord);
    void validaConversoes();
    bool existeVia(int idVia);
    void atualizaCarros(bool boolMarcaPontoSemaforos);
    Via getVia(int id);
    Semaforo getSemaforo(int id);
    float modulo(float entrada);
    int getIdPai(int idFilha);
    void posicionaCarroContinuacaoDaVia(Carro & carro);
    int* getArrayViasDireitaEsquerda(float *handleCoordsDestinoInicio, float *handleCoordsDestinoFim, float *handleCoordsDestinoInicio2, float *handleCoordsDestinoFim2, float *handleCoordsOrigemInicio, float *handleCoordsOrigemFim, int idVia1, int idVia2);
    bool temEspacoParaAndar(Carro & carro);
    int temSemaforoVermelho(int idViaPrincipal, int idViaSecundaria);
    bool temEspacoNaVia(Carro & carro,Via via,int InicioOuFim);
    const char* getMapaFilename();
    void lock();
    void unlock();
    bool posicionaCarro(Carro & carro,int flagPosicionamento);
    void marcaPontoSemaforo(int idSemaforo);
    int qualSemaforoCausandoTrafego(int idVia, int sentido);
    void mostrarResultados();
    int getPerformance();
    int* getInfoGene(int id);
    int getNumSemaforos();
    int qualViaEstaCruzando(Carro & carro);
    int* getMelhorPerformanceSemaforos();
    
    Mapa();
    virtual ~Mapa();
};

#endif
