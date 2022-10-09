#include "ga.h"
#include "semaforo.h"

#include <stdio.h>
#include <cstring>
#include <sys/types.h>
#include <sys/stat.h>

Ga::Ga(){
    geracao = 0;
    numCromossomos = 0;
}

Ga::~Ga(){
}
int Ga::novaGeracao(){
    geracao = geracao+1;
    return geracao;
}
int Ga::getGeracao(){
    return geracao;
}
void Ga::setDiretorioDeSaida(const char* nome){
    strcpy(diretorioSaida,nome);
}
char* Ga::getDiretorioDeSaida(int geracao){
    //proximo diretorio
    char *handleDir,handle[10];
    handleDir = handle;
    sprintf(handleDir,"%s/%d",diretorioSaida,getGeracao());
    //cria diretorio de saida dessa geracao
    if (mkdir(handle, 0777) == -1) {  // Create the directory
        printf("Erro ao criar diretorio de saida: %s\n",handleDir);
        exit(1);
    }
    strcpy(diretorioSaidaAtual,handle);
    return diretorioSaidaAtual;
}

void Ga::ranking(int numConfs,int* arrPerformance){
    int c,handle;
    numArrRanking = 0;
    //passamos por todos as configuracoes testadas e organizamos um novo array com os ids delas em ordem de menor performance para a maior performance
    for(c=0;c<numConfs;c++){
        handle = getMenorDoArray(arrPerformance,numConfs,arrRankingAtual,numArrRanking);
        arrRankingAtual[numArrRanking] = handle;
        numArrRanking++;
    }
}

int Ga::getMenorDoArray(int* arrayPegar,int numElementos,int* arrNegados,int numElementosNegados){
    int c,c2,indiceMenorValor;
    int menorAteAgora = -1;
    bool guardar;

    //pega o menor numero
    for(c=0;c<numElementos;c++){
        //se este numero e menor que o menor ate agora
        if(arrayPegar[c] < menorAteAgora || menorAteAgora == -1){
            guardar = true;
            //verifica se o numero nao esta no array de negados
            for(c2=0;c2<numElementosNegados;c2++){
                if(arrNegados[c2] == c){
                    guardar = false;
                }
            }
            //se nao esta no array de negados
            if(guardar == true){
                //guarda o menor valor ate agora
                menorAteAgora = arrayPegar[c];
                //e guarda o menor indice
                indiceMenorValor = c;
            }
        }
    }
    return indiceMenorValor;
}

void Ga::crossoverEMutacao(){
    int c,c2,infoGravar[3],mutacao;
    int numArquivoCriado = 1;
    char arquivoCromossomo[100];
    char linhaHandle[100];
    int indiceCromossomo,indiceMelhorCromossomo,randominfo;
    int probMutacao = 7000;//milisegundos segundos pra mais ou pra menos
    int probabilidadeQualCromossomo = 2;//probabilidade de cruzamento da informacao dos genes (se pega info do primeiro ou do segundo)
    int probabilidadeMutacao = 2;//probabilidade de haver mutacao ou nao. 1 a cada X.
    int tempoMaximoSemaforo = 60000;
    int tempoMinimoSemaforo = 750;
    //int indiceProb;
    
    //O 3 cromossomos melhor rankeados cruzam com os outros 50 primeiros depois dele
    for(c=1;c<numArrRanking && c<51;c++){
        //pega o indice do cromossomo para cruzar (randomiza entre os 3 primeiros)
        indiceMelhorCromossomo = arrRankingAtual[rand()%3];
        //pega o indice correspondente do cromossomo
        indiceCromossomo = arrRankingAtual[c];
        //abre arquivo do novo cromossomo
        FILE * pFile;
        sprintf(arquivoCromossomo,"%s/conf%05d.txt",diretorioSaidaAtual,numArquivoCriado);
        pFile = fopen (arquivoCromossomo,"w");
        if (pFile==NULL){
            printf("Nao foi possivel criar arquivo do novo cromossomo %s\n",arquivoCromossomo);
            exit(1);
        }
        fputs ("[semaforos]\n",pFile);
        //para cada gene (semaforo)
        for(c2=0;c2<arrCromossomos[c].numGenes;c2++){
            //randomiza quais informacoes vao ser cruzadas
            //tempo verde
            if(rand()%probabilidadeQualCromossomo == 0){infoGravar[0]=arrCromossomos[indiceMelhorCromossomo].arrGenes[c2].tempoVerde;}else{infoGravar[0]=arrCromossomos[indiceCromossomo].arrGenes[c2].tempoVerde;}
            //tempo vermelho
            if(rand()%probabilidadeQualCromossomo == 0){infoGravar[1]=arrCromossomos[indiceMelhorCromossomo].arrGenes[c2].tempoVermelho;}else{infoGravar[1]=arrCromossomos[indiceCromossomo].arrGenes[c2].tempoVermelho;}
            //estado inicial
            if(rand()%probabilidadeQualCromossomo == 0){infoGravar[2]=arrCromossomos[indiceMelhorCromossomo].arrGenes[c2].estadoInicial;}else{infoGravar[2]=arrCromossomos[indiceCromossomo].arrGenes[c2].estadoInicial;}
            
            //vai mutar esse cromossomo?
            if(rand()%probabilidadeMutacao == 0){
                //randomiza a informacao do gene que vai alterar
                randominfo = rand()%3;
                //calcula mutacao
                mutacao = rand()%probMutacao;
                //randomiza se e positivo ou negativo
                if(rand()%2==0){
                    mutacao = mutacao*(-1);
                }
                //se for estado inicial
                if(randominfo == 2){
                    //inverte o valor
                    if(infoGravar[randominfo] == 1){
                        infoGravar[randominfo] = 2;
                    }else{
                        infoGravar[randominfo] = 1;
                    }
                }else{
                    //se for tempo vermelho ou verde, aplica mutacao
                    infoGravar[randominfo] += mutacao;
                }
            }
            //mantem dentro do valor minimo/maximo de tempo
            if(infoGravar[0] > tempoMaximoSemaforo){
                infoGravar[0] = tempoMaximoSemaforo;
            }else if(infoGravar[0] < tempoMinimoSemaforo){
                infoGravar[0] = tempoMinimoSemaforo;
            }
            if(infoGravar[1] > tempoMaximoSemaforo){
                infoGravar[1] = tempoMaximoSemaforo;
            }else if(infoGravar[1] < tempoMinimoSemaforo){
                infoGravar[1] = tempoMinimoSemaforo;
            }
            
            if(infoGravar[2] == 0){
                infoGravar[2] = 1;
            }
            
            //prepara a linha a ser gravada no arquivo
            sprintf(linhaHandle,"%d,%d,%d,%d,%d\n",arrCromossomos[indiceCromossomo].arrGenes[c2].idVia1,arrCromossomos[indiceCromossomo].arrGenes[c2].idVia2,infoGravar[0],infoGravar[1],infoGravar[2]);
            //grava no arquivo este gene
            fputs (linhaHandle,pFile);
        }
        fclose (pFile);
        //fecha arquivo
        numArquivoCriado++;
    }
    
    int indiceCruzar = 40;
    //e os cromossomos tambem se cruzam entre si no padrao 1 e 2, 3 e 4, 5 e 6, 7 e 8, e assim por diante...
    //enquanto ainda tiverem pares de cromossomos a serem cruzados
    while(numArquivoCriado <= 75){
        //abre arquivo do novo cromossomo
        FILE * pFile;
        sprintf(arquivoCromossomo,"%s/conf%05d.txt",diretorioSaidaAtual,numArquivoCriado);
        pFile = fopen (arquivoCromossomo,"w");
        if (pFile==NULL){
            printf("Nao foi possivel criar arquivo do novo cromossomo %s\n",arquivoCromossomo);
            exit(1);
        }
        fputs ("[semaforos]\n",pFile);
        //para cada gene (semaforo)
        for(c2=0;c2<arrCromossomos[c].numGenes;c2++){
            //randomiza quais informacoes vao ser cruzadas
            //tempo verde
            if(rand()%probabilidadeQualCromossomo == 0){infoGravar[0]=arrCromossomos[arrRankingAtual[indiceCruzar]].arrGenes[c2].tempoVerde;}else{infoGravar[0]=arrCromossomos[arrRankingAtual[indiceCruzar]+1].arrGenes[c2].tempoVerde;}
            //tempo vermelho
            if(rand()%probabilidadeQualCromossomo == 0){infoGravar[1]=arrCromossomos[arrRankingAtual[indiceCruzar]].arrGenes[c2].tempoVermelho;}else{infoGravar[1]=arrCromossomos[arrRankingAtual[indiceCruzar]+1].arrGenes[c2].tempoVermelho;}
            //estado inicial
            if(rand()%probabilidadeQualCromossomo == 0){infoGravar[2]=arrCromossomos[arrRankingAtual[indiceCruzar]].arrGenes[c2].estadoInicial;}else{infoGravar[2]=arrCromossomos[arrRankingAtual[indiceCruzar]+1].arrGenes[c2].estadoInicial;}
            
            //vai mutar esse cromossomo?
            if(rand()%probabilidadeMutacao == 0){
                //randomiza a informacao do gene que vai alterar
                randominfo = rand()%3;
                //calcula mutacao
                mutacao = rand()%probMutacao;
                //randomiza se e positivo ou negativo
                if(rand()%2==0){
                    mutacao = mutacao*(-1);
                }
                //se for estado inicial
                if(randominfo == 2){
                    //inverte o valor
                    if(infoGravar[randominfo] == 1){
                        infoGravar[randominfo] = 2;
                    }else{
                        infoGravar[randominfo] = 1;
                    }
                }else{
                    //se for tempo vermelho ou verde, aplica mutacao
                    infoGravar[randominfo] += mutacao;
                }
            }
            //mantem dentro do valor minimo/maximo de tempo
            if(infoGravar[0] > tempoMaximoSemaforo){
                infoGravar[0] = tempoMaximoSemaforo;
            }else if(infoGravar[0] < tempoMinimoSemaforo){
                infoGravar[0] = tempoMinimoSemaforo;
            }
            if(infoGravar[1] > tempoMaximoSemaforo){
                infoGravar[1] = tempoMaximoSemaforo;
            }else if(infoGravar[1] < tempoMinimoSemaforo){
                infoGravar[1] = tempoMinimoSemaforo;
            }
            
            if(infoGravar[2] == 0){
                infoGravar[2] = 2;
            }
            
            //prepara a linha a ser gravada no arquivo
            sprintf(linhaHandle,"%d,%d,%d,%d,%d\n",arrCromossomos[indiceCruzar].arrGenes[c2].idVia1,arrCromossomos[indiceCruzar].arrGenes[c2].idVia2,infoGravar[0],infoGravar[1],infoGravar[2]);
            //grava no arquivo este gene
            fputs (linhaHandle,pFile);
        }
        fclose (pFile);
        //fecha arquivo
        numArquivoCriado++;
        //incrementa 2 no numero de cromossomos que ja cruzaram
        indiceCruzar = indiceCruzar + 2;
    }
    
    indiceCruzar = 0;
    //e os cromossomos tambem se cruzam entre si no padrão 1 e 3, 4 e 6, 7 e 9, 10 e 12, e assim por diante...
    while(numArquivoCriado <= 100){
        //abre arquivo do novo cromossomo
        FILE * pFile;
        sprintf(arquivoCromossomo,"%s/conf%05d.txt",diretorioSaidaAtual,numArquivoCriado);
        pFile = fopen (arquivoCromossomo,"w");
        if (pFile==NULL){
            printf("Nao foi possivel criar arquivo do novo cromossomo %s\n",arquivoCromossomo);
            exit(1);
        }
        fputs ("[semaforos]\n",pFile);
        //para cada gene (semaforo)
        for(c2=0;c2<arrCromossomos[c].numGenes;c2++){
            //randomiza quais informacoes vao ser cruzadas
            //tempo verde
            if(rand()%probabilidadeQualCromossomo == 0){infoGravar[0]=arrCromossomos[arrRankingAtual[indiceCruzar]].arrGenes[c2].tempoVerde;}else{infoGravar[0]=arrCromossomos[arrRankingAtual[indiceCruzar]+2].arrGenes[c2].tempoVerde;}
            //tempo vermelho
            if(rand()%probabilidadeQualCromossomo == 0){infoGravar[1]=arrCromossomos[arrRankingAtual[indiceCruzar]].arrGenes[c2].tempoVermelho;}else{infoGravar[1]=arrCromossomos[arrRankingAtual[indiceCruzar]+2].arrGenes[c2].tempoVermelho;}
            //estado inicial
            if(rand()%probabilidadeQualCromossomo == 0){infoGravar[2]=arrCromossomos[arrRankingAtual[indiceCruzar]].arrGenes[c2].estadoInicial;}else{infoGravar[2]=arrCromossomos[arrRankingAtual[indiceCruzar]+2].arrGenes[c2].estadoInicial;}
            
            //vai mutar esse cromossomo?
            if(rand()%probabilidadeMutacao == 0){
                //randomiza a informacao do gene que vai alterar
                randominfo = rand()%3;
                //calcula mutacao
                mutacao = rand()%probMutacao;
                //randomiza se é positivo ou negativo
                if(rand()%2==0){
                    mutacao = mutacao*(-1);
                }
                //se for estado inicial
                if(randominfo == 2){
                    //inverte o valor
                    if(infoGravar[randominfo] == 1){
                        infoGravar[randominfo] = 2;
                    }else{
                        infoGravar[randominfo] = 1;
                    }
                }else{
                    //se for tempo vermelho ou verde, aplica mutacao
                    infoGravar[randominfo] += mutacao;
                }
            }
            //mantem dentro do valor minimo/maximo de tempo
            if(infoGravar[0] > tempoMaximoSemaforo){
                infoGravar[0] = tempoMaximoSemaforo;
            }else if(infoGravar[0] < tempoMinimoSemaforo){
                infoGravar[0] = tempoMinimoSemaforo;
            }
            if(infoGravar[1] > tempoMaximoSemaforo){
                infoGravar[1] = tempoMaximoSemaforo;
            }else if(infoGravar[1] < tempoMinimoSemaforo){
                infoGravar[1] = tempoMinimoSemaforo;
            }
            
            if(infoGravar[2] == 0){
                infoGravar[2] = 1;
            }
                
            //prepara a linha a ser gravada no arquivo
            sprintf(linhaHandle,"%d,%d,%d,%d,%d\n",arrCromossomos[indiceCruzar].arrGenes[c2].idVia1,arrCromossomos[indiceCruzar].arrGenes[c2].idVia2,infoGravar[0],infoGravar[1],infoGravar[2]);
            //grava no arquivo este gene
            fputs (linhaHandle,pFile);
        }
        fclose (pFile);
        //fecha arquivo
        numArquivoCriado++;
        //incrementa 2 no numero de cromossomos que ja cruzaram
        indiceCruzar = indiceCruzar + 3;
    }
    
}

void Ga::criaArquivoConfSemaforo(char *diretorio){
    
}

void Ga::newGene(int idVia1, int idVia2, int tempoVerde, int tempoVermelho, int estadoInicial, int idCromossomo){
    arrCromossomos[idCromossomo].arrGenes[arrCromossomos[idCromossomo].numGenes].idVia1 = idVia1;
    arrCromossomos[idCromossomo].arrGenes[arrCromossomos[idCromossomo].numGenes].idVia2 = idVia2;
    arrCromossomos[idCromossomo].arrGenes[arrCromossomos[idCromossomo].numGenes].tempoVerde = tempoVerde;
    arrCromossomos[idCromossomo].arrGenes[arrCromossomos[idCromossomo].numGenes].tempoVermelho = tempoVermelho;
    arrCromossomos[idCromossomo].arrGenes[arrCromossomos[idCromossomo].numGenes].estadoInicial = estadoInicial;
    arrCromossomos[idCromossomo].numGenes++;
}

int Ga::newCromossomo(){
    arrCromossomos[numCromossomos].numGenes = 0;
    return numCromossomos++;
}

void Ga::limpaCromossomos(){
    numCromossomos = 0;
}

int Ga::getNumCromossomos(){
    return numCromossomos;
}
