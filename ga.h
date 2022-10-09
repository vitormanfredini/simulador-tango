

#ifndef GA_H
#define GA_H

class Ga {

private:

    struct Gene {
        int idVia1;
        int idVia2;
        int tempoVerde;
        int tempoVermelho;
        int estadoInicial;
    };

    struct Cromossomo {
        //cada cromossomo tem seus genes
        Gene arrGenes[100];
        int numGenes;
    };

    int geracao;
    char diretorioSaida[100];
    char diretorioSaidaAtual[100];
    int arrRankingAtual[200];
    int numArrRanking;
    Cromossomo arrCromossomos[100];
    Cromossomo arrCromossomosHandle[100];
    int numCromossomos;

public:
    int novaGeracao();
    int getGeracao();
    void setDiretorioDeSaida(const char* nome);
    char* getDiretorioDeSaida(int geracao);
    void ranking(int numConfs,int* arrPerformance);
    int getMenorDoArray(int* arrayPegar,int numElementos,int* arrNegados,int numElementosNegados);
    void crossoverEMutacao();
    void newGene(int idVia1, int idVia2, int tempoVerde, int tempoVermelho, int estadoInicial, int idCromossomo);
    int newCromossomo();
    void limpaCromossomos();
    int getNumCromossomos();
    void criaArquivoConfSemaforo(char *diretorioSaidaAtual);

    Ga();
    virtual ~Ga();
};

#endif
