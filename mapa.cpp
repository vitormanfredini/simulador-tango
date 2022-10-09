#include "mapa.h"
#include <math.h>
#include "via.h"
#include <stdio.h>
#include <string>
#include <stdlib.h>
#include <iomanip>
#include <pthread.h>
#include <cstring>

Mapa::Mapa() {
    numExecucao = 0;
    
    precisaoFloat = 0.00005;
    distanciaAceitavelCarroDaFrente = 1.0f;

    pthread_mutexattr_t mutexattr;
    pthread_mutexattr_init(&mutexattr);
    pthread_mutex_init(&mutex,&mutexattr);
}

Mapa::~Mapa() {
    delete[] arrVias;
    delete[] arrSemaforos;
    delete[] arrCarros;
    delete[] arrInterseccoes;
    pthread_mutex_destroy(&mutex);
}

void Mapa::init(int miliSegundos,const char *arquivoMapa,const char *arquivoSemaforos, int flagModoExecucao){

    lock();

    int c=0;
    //s� nao libera da mem�ria os objetos quando for a primeira execucao (pois ainda nao estao na mem�ria)
    if(numExecucao > 0){
        delete[] arrVias;
        delete[] arrSemaforos;
        delete[] arrCarros;
        delete[] arrInterseccoes;
    }

    //instancia objetos
    arrVias = new Via[200];
    arrConversoes = new Conversao[2500];
    arrSemaforos = new Semaforo[100];
    arrCarros = new Carro[200];

    //numero de objetos carregados
    numViasCarregadas = 0;
    numInterseccoesCarregadas = 0;
    numSemaforosCarregados = 0;
    numConversoesCarregadas = 0;
    numCarrosCarregados = 0;

    //carrega o mapa especificado
    loadfile(arquivoMapa);
    //carrega a configuracao de semaforos especificada
    loadfile(arquivoSemaforos);

    //guarda a quantos milisegundos � feita a atualizacao dos objetos
    atualizacaoMs = miliSegundos;
    //divide vias concorrentes
    while(dividirViasConcorrentes());
    //salvamos entre quais vias e qual foi o ponto de interseccao entre elas.
    salvaInterseccoes();
    //verifica se todos as conversoes informadas existem na pratica
    validaConversoes();
    //calcula orientacao dos semaforos baseado nas vias que ele controla
    calcularOrientacaoSemaforos();
    //calcula orientacao dos carros baseado na via que ele est�
    for(c=0;c<numCarrosCarregados;c++){
        posicionaCarro(arrCarros[c],0);
    }
    //monta as ruas baseando-se nos pontos de inicio e fim das vias.
    for(c=0; c < numViasCarregadas; c++){
        arrVias[c].calcularCoordenadas();
        arrVias[c].gen3dObject();
    }
    //gera os objetos 3d dos semaforos
    for(c=0; c < numSemaforosCarregados; c++){
        arrSemaforos[c].calculaCoordenadas();
        arrSemaforos[c].montaObjeto3d(arrSemaforos[c].getEstado());
    }

    //libera da memoria o array de conversoes (ja passamos todas as conversoes para os objetos dos carros)
    delete[] arrConversoes;

    numExecucao++;

    unlock();

}

//tipos de dados nos arquivos de mapa
enum tipoInfo {
    MAPA_VAZIO = 0,
    MAPA_VIA = 1,
    MAPA_CONVERSAO = 2,
    MAPA_SEMAFORO = 3,
    MAPA_CARRO = 4
};

void Mapa::loadfile(const char *filename){

    //ponteiro do arquivo de entrada
    FILE *arquivoEntrada;
    int tipoInformacao,c,c2;
    bool flagTratarValor;
    char line[256];
    
    //abrimos o arquivo de entrada
    arquivoEntrada = fopen(filename, "r");
    if (arquivoEntrada == NULL){
        perror("Erro ao abrir arquivo do mapa");
        exit(1);
    }

    //variaveis de manuseio das informacoes do mapa
    //para vias:
    float handleInicioFim[6];
    int handleNumdeFaixas;
    char handleNomedavia[150];
    //para conversoes:
    int handleVia1,handleVia2,handleIdCarro,handleLado;
    //para semaforos:
    int handleTempoVermelho,handleTempoVerde,handleEstadoInicial;
    //para carros:
    float handleVelocidade;

    int linhaLeitura = 0;
    tipoInformacao = MAPA_VAZIO;
    //le linha por linha do arquivo
    while (fgets(line, sizeof line, arquivoEntrada) != NULL){
        //busca strings dos tipos de informacoes
        if (strcmp(line, "[vias]\n") == 0 || strcmp(line, "[vias]\r\n") == 0){
            tipoInformacao = MAPA_VIA;
            flagTratarValor = false;
        } else if (strcmp(line, "[conversoes]\n") == 0 || strcmp(line, "[conversoes]\r\n") == 0) {
            tipoInformacao = MAPA_CONVERSAO;
            flagTratarValor = false;
        } else if (strcmp(line, "[semaforos]\n") == 0 || strcmp(line, "[semaforos]\r\n") == 0) {
            tipoInformacao = MAPA_SEMAFORO;
            flagTratarValor = false;
        } else if (strcmp(line, "[carros]\n") == 0 || strcmp(line, "[carros]\r\n") == 0) {
            tipoInformacao = MAPA_CARRO;
            flagTratarValor = false;
        } else if (strcmp(line, "\n") == 0 || strcmp(line, "\r\n") == 0) {
            flagTratarValor = false;
        } else {
            if(tipoInformacao == MAPA_VAZIO){
                printf("Informacao fora de secao na linha %d do arquivo %s\n",linhaLeitura,filename);
                exit(1);
            }
            flagTratarValor = true;
        }

        //se � um valor a ser tratado
        if (flagTratarValor == true) {
            //trata informacao de acordo com o que ela �
            switch (tipoInformacao) {
                case MAPA_VIA:
                    if(sscanf(line,"%f,%f,%f,%f,%f,%f,%d,%[^\t\n]",&handleInicioFim[0],&handleInicioFim[1],&handleInicioFim[2],&handleInicioFim[3],&handleInicioFim[4],&handleInicioFim[5],&handleNumdeFaixas,handleNomedavia) != 8){
                        printf("Erro ao carregar via %d\n",numViasCarregadas);
                        exit(1);
                    }
                    arrVias[numViasCarregadas].setId(numViasCarregadas);
                    arrVias[numViasCarregadas].setIdPai(-1);
                    arrVias[numViasCarregadas].setCoordInicio(handleInicioFim[0],handleInicioFim[1],handleInicioFim[2]);
                    arrVias[numViasCarregadas].setCoordFim(handleInicioFim[3],handleInicioFim[4],handleInicioFim[5]);
                    arrVias[numViasCarregadas].setNumDeVias(handleNumdeFaixas);
                    arrVias[numViasCarregadas].setNomeDaVia(handleNomedavia);
                    numViasCarregadas++;
                    break;
                case MAPA_CONVERSAO:
                    if(sscanf(line,"%d,%d,%d,%d",&handleIdCarro,&handleVia1,&handleVia2,&handleLado) != 4){
                        printf("Erro ao carregar conversao %d\n",numConversoesCarregadas);
                        exit(1);
                    }
                    arrConversoes[numConversoesCarregadas].setIdCarro(handleIdCarro);
                    arrConversoes[numConversoesCarregadas].setIdViaOrigem(handleVia1);
                    arrConversoes[numConversoesCarregadas].setIdViaDestino(handleVia2);
                    arrConversoes[numConversoesCarregadas].setLado(handleLado);
                    numConversoesCarregadas++;
                    break;
                case MAPA_SEMAFORO:
                    if(sscanf(line,"%d,%d,%d,%d,%d",&handleVia1,&handleVia2,&handleTempoVerde,&handleTempoVermelho,&handleEstadoInicial) != 5){
                        printf("Erro ao carregar semaforo %d\n",numSemaforosCarregados);
                        exit(1);
                    }
                    //printf("%d,%d,%d,%d,%d\n",handleVia1,handleVia2,handleTempoVerde,handleTempoVermelho,handleEstadoInicial);
                    arrSemaforos[numSemaforosCarregados].setId(numSemaforosCarregados);
                    arrSemaforos[numSemaforosCarregados].setIdVia1(handleVia1);
                    arrSemaforos[numSemaforosCarregados].setIdVia2(handleVia2);
                    arrSemaforos[numSemaforosCarregados].setTempoVerde(handleTempoVerde);
                    arrSemaforos[numSemaforosCarregados].setTempoVermelho(handleTempoVermelho);
                    arrSemaforos[numSemaforosCarregados].setEstado(handleEstadoInicial);
                    arrSemaforos[numSemaforosCarregados].setEstadoInicial(handleEstadoInicial);
                    numSemaforosCarregados++;
                    break;
                case MAPA_CARRO:
                    if(sscanf(line,"%f",&handleVelocidade) != 1){
                        printf("Erro ao carregar carro %d\n",numCarrosCarregados);
                        exit(1);
                    }
                    arrCarros[numCarrosCarregados].setId(numCarrosCarregados);
                    arrCarros[numCarrosCarregados].setVelocidade(handleVelocidade);
                    numCarrosCarregados++;
                    break;
                default:
                    //nada
                    break;
            }
        }
        linhaLeitura++;
    }

    // agora passamos todas as conversoes carregadas para os carros correspondentes
    // fazemos isso agora pois as conversoes podem estar sendo carregadas antes dos carros
    for(c=0;c<numConversoesCarregadas;c++){
        for(c2=0;c2<numCarrosCarregados;c2++){
            if(arrCarros[c2].getId() == arrConversoes[c].getIdCarro()){
                arrCarros[c2].addConversao(arrConversoes[c]);
            }
        }
    }
    fclose(arquivoEntrada);
}

void Mapa::render(int modoExecucao, bool boolMarcaPontosSemaforos){

    lock();

    int c;
    //renderiza vias
    if(modoExecucao == 1) // so renderiza no modo simulacao
    for(c=0; c < numViasCarregadas; c++){
        arrVias[c].render();
    }
    //renderiza semaforos
    for(c=0; c < numSemaforosCarregados; c++){
        arrSemaforos[c].incrementaTempo(atualizacaoMs);
        if(modoExecucao == 1) // so renderiza no modo simulacao
            arrSemaforos[c].render();
    }

    //atualiza posicao dos carros
    atualizaCarros(boolMarcaPontosSemaforos);

    //renderiza carros
    for(c=0; c < numCarrosCarregados; c++){
        if(arrCarros[c].getEstado() != 0){
            //atualiza os poligonos
            arrCarros[c].gen3dObject();
            //renderiza
            if(modoExecucao == 1) // so renderiza no modo simulacao
                arrCarros[c].render();
        }
    }

    unlock();
}

bool Mapa::dividirViasConcorrentes(){
    int c,c2;
    float *coordsInicio1,*coordsFim1,*coordsInicio2,*coordsFim2;
    float delta,x,z,A1,A2,B1,B2,C1,C2;
    float valorMenorX1,valorMaiorX1,valorMenorZ1,valorMaiorZ1,valorMenorX2,valorMaiorX2,valorMenorZ2,valorMaiorZ2;
    bool flagDividiu = false,cortaPrimeira,cortaSegunda;

    //para cada via, verifica se ela cruza com alguma outra.
    for(c=0; c < numViasCarregadas; c++){
        coordsInicio1 = arrVias[c].getCoordInicio();
        coordsFim1 = arrVias[c].getCoordFim();
        
        A1 = coordsFim1[2]-coordsInicio1[2];
        B1 = coordsInicio1[0]-coordsFim1[0];
        C1 = A1*coordsInicio1[0]+B1*coordsInicio1[2];

        for(c2=c; c2 < numViasCarregadas; c2++){
            //nao verifica se for a mesma reta
            if(c != c2){

                coordsInicio2 = arrVias[c2].getCoordInicio();
                coordsFim2 = arrVias[c2].getCoordFim();

                A2 = coordsFim2[2]-coordsInicio2[2];
                B2 = coordsInicio2[0]-coordsFim2[0];
                C2 = A2*coordsInicio2[0]+B2*coordsInicio2[2];
                //se o delta das forem iguais, entao as retas sao paralelas, portanto nao se cruzam.
                // so faremos o calculo se nao forem paralelas
                delta = A1*B2 - A2*B1;
                if(delta != 0){
                    x = (B2*C1 - B1*C2)/delta;
                    z = (A1*C2 - A2*C1)/delta;
                    // o x e z encontrados dizem respeito a retas, mas queremos saber apenas nos segmentos de retas.
                    // entao verificamos se os pontos encontrados estao dentro dos nossos segmentos
                    // primeiro verificamos quanl dos pontos (inicio e fim) � o menor e o maior
                    if(coordsInicio1[0] > coordsFim1[0]){
                        valorMenorX1 = coordsFim1[0];
                        valorMaiorX1 = coordsInicio1[0];
                    }else{
                        valorMaiorX1 = coordsFim1[0];
                        valorMenorX1 = coordsInicio1[0];
                    }
                    if(coordsInicio2[0] > coordsFim2[0]){
                        valorMenorX2 = coordsFim2[0];
                        valorMaiorX2 = coordsInicio2[0];
                    }else{
                        valorMaiorX2 = coordsFim2[0];
                        valorMenorX2 = coordsInicio2[0];
                    }
                    if(coordsInicio1[2] > coordsFim1[2]){
                        valorMenorZ1 = coordsFim1[2];
                        valorMaiorZ1 = coordsInicio1[2];
                    }else{
                        valorMaiorZ1 = coordsFim1[2];
                        valorMenorZ1 = coordsInicio1[2];
                    }
                    if(coordsInicio2[2] > coordsFim2[2]){
                        valorMenorZ2 = coordsFim2[2];
                        valorMaiorZ2 = coordsInicio2[2];
                    }else{
                        valorMaiorZ2 = coordsFim2[2];
                        valorMenorZ2 = coordsInicio2[2];
                    }
                    // verificamos se os segmentos se encontram ("alargamos" um pouco a reta considerando a possivel diferen�a gerada pelo Float)
                    if(
                            x >= (valorMenorX1 - precisaoFloat) &&
                            x <= (valorMaiorX1 + precisaoFloat) &&
                            x >= (valorMenorX2 - precisaoFloat) &&
                            x <= (valorMaiorX2 + precisaoFloat) &&
                            z >= (valorMenorZ1 - precisaoFloat) &&
                            z <= (valorMaiorZ1 + precisaoFloat) &&
                            z >= (valorMenorZ2 - precisaoFloat) &&
                            z <= (valorMaiorZ2 + precisaoFloat)
                      ){

                        //e s� dividimos a reta caso o encontro dela com a outra nao seja em sua extremidade
                        //verificamos entao a primeira reta
                        cortaPrimeira = false;
                        if(eIgualf(x,coordsInicio1[0]) == false || eIgualf(z,coordsInicio1[2]) == false){
                            if(eIgualf(x,coordsFim1[0]) == false || eIgualf(z,coordsFim1[2]) == false){
                                cortaPrimeira = true;
                            }
                        }
                        //segunda reta
                        cortaSegunda = false;
                        if(eIgualf(x,coordsInicio2[0]) == false || eIgualf(z,coordsInicio2[2]) == false){
                            if(eIgualf(x,coordsFim2[0]) == false || eIgualf(z,coordsFim2[2]) == false){
                                cortaSegunda = true;
                            }
                        }
                        //agora fazemos os cortes
                        if(cortaPrimeira == true){
                            //seta a coordenada de inicio da nova via para o lugar que encontra e o fim para o fim do segmento anterior
                            arrVias[numViasCarregadas].setId(numViasCarregadas);
                            arrVias[numViasCarregadas].setCoordInicio(x,coordsFim1[1],z);
                            arrVias[numViasCarregadas].setCoordFim(coordsFim1[0],coordsFim1[1],coordsFim1[2]);
                            arrVias[numViasCarregadas].setNumDeVias(arrVias[c].getNumDeVias());
                            arrVias[numViasCarregadas].setNomeDaVia(arrVias[c].getNomeDaVia());
                            if(arrVias[c].getIdPai() != -1){
                                arrVias[numViasCarregadas].setIdPai(arrVias[c].getIdPai());
                            }else{
                                arrVias[numViasCarregadas].setIdPai(arrVias[c].getId());
                            }
                            numViasCarregadas++;
                            //e na via que foi cortada, setamos o fim dela para o ponto que cortou.
                            arrVias[c].setCoordFim(x,coordsInicio1[1],z);
                            flagDividiu = true;
                        }
                        if(cortaSegunda == true){
                            //seta a coordenada de inicio da nova via para o lugar que encontra e o fim da via para o fim da via anterior
                            arrVias[numViasCarregadas].setId(numViasCarregadas);
                            arrVias[numViasCarregadas].setCoordInicio(x,coordsFim2[1],z);
                            arrVias[numViasCarregadas].setCoordFim(coordsFim2[0],coordsFim2[1],coordsFim2[2]);
                            arrVias[numViasCarregadas].setNumDeVias(arrVias[c2].getNumDeVias());
                            arrVias[numViasCarregadas].setNomeDaVia(arrVias[c2].getNomeDaVia());
                            arrVias[numViasCarregadas].setIdPai(arrVias[c2].getId());
                            if(arrVias[c2].getIdPai() != -1){
                                arrVias[numViasCarregadas].setIdPai(arrVias[c2].getIdPai());
                            }else{
                                arrVias[numViasCarregadas].setIdPai(arrVias[c2].getId());
                            }
                            numViasCarregadas++;
                            //seta a coordenada de fim da via para o lugar onde as retas se encontram
                            arrVias[c2].setCoordFim(x,coordsInicio2[1],z);
                            flagDividiu = true;
                        }

                        //se dividiu 1 ou 2 retas, retorna verdadeiro.
                        if(flagDividiu == true){
                            return true;
                        }
                    }
                }
            }
        }
    }
    return false;
}

//workaround para comparar floats
bool Mapa::eIgualf(float num1, float num2){
    float aux;
    if(num1 > num2){
        aux = num1 - num2;
    }else{
        aux = num2 - num1;
    }
    if(aux < precisaoFloat && aux > -precisaoFloat){
        return true;
    }else{
        return false;
    }
}

bool Mapa::eIgualf(float num1, float num2, float tolerancia){
    float aux;
    if(num1 > num2){
        aux = num1 - num2;
    }else{
        aux = num2 - num1;
    }
    if(aux < tolerancia && aux > -tolerancia){
        return true;
    }else{
        return false;
    }
}

void Mapa::salvaInterseccoes(){
    int c,c2,c3;
    arrInterseccoes = new Interseccao[100];
    float *coordsInicio1,*coordsFim1,*coordsInicio2,*coordsFim2,xIntersec,zIntersec;
    bool guardaIntersec;
    int handleId1,handleId2;
    
    for(c=0;c<numViasCarregadas;c++){
        //pegamos as coordenadas da via para testarmos se elas se interceptam
        coordsInicio1 = arrVias[c].getCoordInicio();
        coordsFim1 = arrVias[c].getCoordFim();
        //pega Id da primeira via (pega idPai prioritariamente, se nao tiver, pega o id)
        if(arrVias[c].getIdPai() != -1){
            handleId1 = arrVias[c].getIdPai();
        }else{
            handleId1 = arrVias[c].getId();
        }
        for(c2=0;c2<numViasCarregadas;c2++){
            //nao compara uma via com ela mesma
            if(c!=c2){
                //pega Id da segunda via (pega idPai prioritariamente, se nao tiver, pega o id)
                if(arrVias[c2].getIdPai() != -1){
                    handleId2 = arrVias[c2].getIdPai();
                }else{
                    handleId2 = arrVias[c2].getId();
                }
                //nao compara duas vias com o mesmo id/idpai, porque sao continuacoes, portanto, nao sao interseccoes
                if(handleId2 != handleId1){
                    //aqui verificamos se elas realmente se interceptam
                    guardaIntersec = false;
                    //pegamos as coordenadas da via2
                    coordsInicio2 = arrVias[c2].getCoordInicio();
                    coordsFim2 = arrVias[c2].getCoordFim();
                    //se elas se interceptam, entao marca flag para salvar a interseccao
                    if( (eIgualf(coordsInicio1[0],coordsInicio2[0])) && (eIgualf(coordsInicio1[2],coordsInicio2[2])) ){
                        xIntersec = coordsInicio1[0];
                        zIntersec = coordsInicio1[2];
                        guardaIntersec = true;
                    }
                    if( (eIgualf(coordsInicio1[0],coordsFim2[0])) && (eIgualf(coordsInicio1[2],coordsFim2[2])) ){
                        xIntersec = coordsInicio1[0];
                        zIntersec = coordsInicio1[2];
                        guardaIntersec = true;
                    }
                    if( (eIgualf(coordsFim1[0],coordsInicio2[0])) && (eIgualf(coordsFim1[2],coordsInicio2[2])) ){
                        xIntersec = coordsFim1[0];
                        zIntersec = coordsFim1[2];
                        guardaIntersec = true;
                    }
                    if( (eIgualf(coordsFim1[0],coordsFim2[0])) && (eIgualf(coordsFim1[2],coordsFim2[2])) ){
                        xIntersec = coordsFim1[0];
                        zIntersec = coordsFim1[2];
                        guardaIntersec = true;
                    }
                    //verificamos antes se n�o h� esta interseccao no array para nao repetir
                    for(c3=0;c3 < numInterseccoesCarregadas;c3++){
                        if(
                            ((arrInterseccoes[c3].getIdVia1() == handleId1) && ((arrInterseccoes[c3].getIdVia2() == handleId2))) ||
                            ((arrInterseccoes[c3].getIdVia1() == handleId2) && ((arrInterseccoes[c3].getIdVia2() == handleId1)))
                        ){
                            //se a interseccao ja foi salva, marca para nao salvar 
                            guardaIntersec = false;
                        }
                    }
                    //aqui salvamos a interseccao, se for necessario
                    if(guardaIntersec == true){
                        arrInterseccoes[numInterseccoesCarregadas].setIdVia1(handleId1);
                        arrInterseccoes[numInterseccoesCarregadas].setIdVia2(handleId2);
                        arrInterseccoes[numInterseccoesCarregadas].setX(xIntersec);
                        arrInterseccoes[numInterseccoesCarregadas].setZ(zIntersec);
                        numInterseccoesCarregadas++;
                    }
                }
            }
        }
    }
}

void Mapa::validaConversoes(){
    int c;
    for(c=0;c<numConversoesCarregadas;c++){
        if(existeInterseccao(arrConversoes[c].getIdViaOrigem(),arrConversoes[c].getIdViaDestino()) == false){
            printf("A conversao entre as vias (%d e %d) e invalida, pois elas nao se cruzam\n",arrConversoes[c].getIdViaOrigem(),arrConversoes[c].getIdViaDestino());
            exit(1);
        }
    }
}

void Mapa::calcularOrientacaoSemaforos(){
    int c;
    float *handleCoordsInicio, *handleCoordsFim, handleCoordsReduzidas[3], distanciadavia;
    Via viaHandle;
    //para cada semaforo
    for(c=0;c<numSemaforosCarregados;c++){
        //verifica se existe interseccao para poder existir semaforo
        if(existeInterseccao(arrSemaforos[c].getIdVia1(),arrSemaforos[c].getIdVia2())){
            //pega o x e y da interseccao
            arrSemaforos[c].setX(getCoordInterseccao(arrSemaforos[c].getIdVia1(),arrSemaforos[c].getIdVia2(),'x'));
            arrSemaforos[c].setZ(getCoordInterseccao(arrSemaforos[c].getIdVia1(),arrSemaforos[c].getIdVia2(),'z'));
        }else{
            printf("Este semaforo e invalido, pois nao ha intereseccao entre as vias (%d e %d)\n",arrSemaforos[c].getIdVia1(),arrSemaforos[c].getIdVia2());
            exit(1);
        }
        //calcula posicao do semaforo baseado na posicao da via principal dele
        //pra isso, primeiro pegamos as coordenadas da via
        viaHandle = getVia(arrSemaforos[c].getIdVia1());
        handleCoordsInicio = viaHandle.getCoordInicio();
        handleCoordsFim = viaHandle.getCoordFim();

        //aqui fazemos o calculo para que o semaforo fique virado de frente para a via
        //mas antes vemos se ele esta no inicio ou no fim da via
        if(eIgualf(handleCoordsInicio[0],arrSemaforos[c].getX()) && eIgualf(handleCoordsInicio[2],arrSemaforos[c].getZ()) ){
            //se o semaforo esta no inicio da via
            //reduzimos a via a origem com o ponto de inicio na origem
            handleCoordsReduzidas[0] = handleCoordsFim[0] - handleCoordsInicio[0];
            handleCoordsReduzidas[2] = handleCoordsFim[2] - handleCoordsInicio[2];
        }else{
            //se o semaforo esta no fim da via
            //reduzimos a via a origem com o ponto de inicio na origem e giramos a reta 180 graus (multiplicando por -1)
            handleCoordsReduzidas[0] = (handleCoordsFim[0] - handleCoordsInicio[0]) * (-1);
            handleCoordsReduzidas[2] = (handleCoordsFim[2] - handleCoordsInicio[2]) * (-1);
        }

        //aqui calculamos a distancia da via, para podermos calcular o ponto de orientacao do semaforo
        distanciadavia = sqrtf(powf(handleCoordsFim[0]-handleCoordsInicio[0],2) + powf(handleCoordsFim[2]-handleCoordsInicio[2],2));

        //a coordenada de orientacao do semaforo � entao
        //coordenada reduzida dividida pela distancia dela e multiplicada pela largura do semaforo/2
        //somada a coordenada do semaforo
        arrSemaforos[c].setXOrientacao(arrSemaforos[c].getX()+((handleCoordsReduzidas[0]/distanciadavia)*(arrSemaforos[c].getLargura()/2)));
        arrSemaforos[c].setZOrientacao(arrSemaforos[c].getZ()+((handleCoordsReduzidas[2]/distanciadavia)*(arrSemaforos[c].getLargura()/2)));
    }
}

bool Mapa::posicionaCarro(Carro & carro,int flagPosicionamento){
    float *handleCoordsInicio, *handleCoordsFim, *handleCoordsInicio2, *handleCoordsFim2, handleCoordsReduzidas[3], distanciadavia;
    Via viaHandle;
    //se o carro esta fora do mapa, pega via de origem da primeira conversao.
    if(carro.getIdViaAtual() == -1){
        carro.setIdViaAtual(carro.getIdViaInicial());
        //pega via atual
        viaHandle = getVia(carro.getIdViaAtual());
        //pegamos as coordenadas da via
        handleCoordsInicio = viaHandle.getCoordInicio();
        handleCoordsFim = viaHandle.getCoordFim();
        bool encostouInicio = false;
        //ve qual � o lado que n�o est� encostando em nenhuma outra via para colocar o carro
        //pegamos as coordenadas da via
        handleCoordsInicio2 = viaHandle.getCoordInicio();
        handleCoordsFim2 = viaHandle.getCoordFim();
        //vemos se a via atual da itera��o encosta no inicio da via inicial
        if(
            ( eIgualf(handleCoordsInicio[0],handleCoordsInicio2[0]) &&
            eIgualf(handleCoordsInicio[2],handleCoordsInicio2[2]) )
            ||
            ( eIgualf(handleCoordsInicio[0],handleCoordsFim2[0]) &&
            eIgualf(handleCoordsInicio[2],handleCoordsFim2[2]) )
          ){
            encostouInicio = true;
        }
        //nao verificamos para o fim da via, porque se nao encostou no inicio, com certeza encostou no fim
        //entao, se encostou no ininio
        if(encostouInicio == true){
            //seta a flag para o inicio da via
            flagPosicionamento = 1;
        }else{
            //senao seta a flag para o fim da via
            flagPosicionamento = 2;
        }
        //se nao tem espaco para colocar o carro, nao poe e retorna.
        if(temEspacoNaVia(carro,viaHandle,flagPosicionamento)==false){
            carro.setIdViaAtual(-1);
            return false;
        }
    }else{
        //pega via atual
        viaHandle = getVia(carro.getIdViaAtual());
        //pegamos as coordenadas da via
        handleCoordsInicio = viaHandle.getCoordInicio();
        handleCoordsFim = viaHandle.getCoordFim();
    }

    //verifica se existe a via
    if(existeVia(carro.getIdViaAtual()) == false){
        printf("O carro esta posicionado em uma via (%d) que nao existe\n",carro.getIdViaAtual());
        exit(1);
    }
    //calcula coordenadas de inicio e fim do carro, baseado na nas coordenadas da via em que ele est�.
    //aqui calculamos a distancia da via, para podermos calcular o ponto de orientacao do carro
    distanciadavia = sqrtf(powf(handleCoordsFim[0]-handleCoordsInicio[0],2) + powf(handleCoordsFim[2]-handleCoordsInicio[2],2));

    //reduzimos a via a origem com o ponto de inicio na origem (mantendo o carro na mesma orientacao dela)
    handleCoordsReduzidas[0] = handleCoordsFim[0] - handleCoordsInicio[0];
    handleCoordsReduzidas[2] = handleCoordsFim[2] - handleCoordsInicio[2];

    //verifica se � pra colocar o carro no inicio ou no fim da via
    switch(flagPosicionamento){
        case 1: //inicio da via
            carro.setX(handleCoordsInicio[0]);
            carro.setZ(handleCoordsInicio[2]);
            //coordenada de orientacao: coordenada reduzida dividida pela distancia dela e multiplicada pelo comprimento do carro, somada � coordenada do carro
            carro.setXOrientacao(carro.getX()+((handleCoordsReduzidas[0]/distanciadavia)*carro.getComprimento()));
            carro.setZOrientacao(carro.getZ()+((handleCoordsReduzidas[2]/distanciadavia)*carro.getComprimento()));
            break;
        case 2: //fim da via
            carro.setX(handleCoordsFim[0]);
            carro.setZ(handleCoordsFim[2]);
            //coordenada de orientacao: coordenada do carro menos a coordenada reduzida dividida pela distancia dela e multiplicada pelo comprimento do carro
            carro.setXOrientacao(carro.getX()-((handleCoordsReduzidas[0]/distanciadavia)*carro.getComprimento()));
            carro.setZOrientacao(carro.getZ()-((handleCoordsReduzidas[2]/distanciadavia)*carro.getComprimento()));
            break;
    }
    //e seta o sentido que ele foi posicionado
    carro.setSentido(flagPosicionamento);
    //marca que nao esta mais parado em um sinal vermelho
    carro.andouDoSinalVermelho();
    return true;
}

bool Mapa::existeInterseccao(int idVia1,int idVia2){
    //se os idVia2 for -1 retorna positivo, pois significa que � a sa�da do carro do mapa.
    if(idVia2 == -1) return true;
    int c;
    for(c=0;c<numInterseccoesCarregadas;c++){
        if(arrInterseccoes[c].getIdVia1() == idVia1 && arrInterseccoes[c].getIdVia2() == idVia2) return true;
        if(arrInterseccoes[c].getIdVia1() == idVia2 && arrInterseccoes[c].getIdVia2() == idVia1) return true;
    }
    return false;
}

bool Mapa::existeVia(int idVia){
    int c;
    for(c=0;c<numViasCarregadas;c++){
        if(arrVias[c].getId() == idVia) return true;
    }
    //printf("%d\n",c);
    return false;
}

float Mapa::getCoordInterseccao(int idVia1,int idVia2,char coord){
    int c;
    for(c=0;c<numInterseccoesCarregadas;c++){
        if( (arrInterseccoes[c].getIdVia1() == idVia1 && arrInterseccoes[c].getIdVia2() == idVia2) || (arrInterseccoes[c].getIdVia1() == idVia2 && arrInterseccoes[c].getIdVia2() == idVia1) ){
            switch(coord){
                case 'x':
                    return arrInterseccoes[c].getX();
                    break;
                case 'z':
                    return arrInterseccoes[c].getZ();
                    break;
                default:
                    printf("Coordenada %c n�o existe.\n",coord);
                    exit(1);
                    break;
            }
        }
    }
    printf("Nao existe interseccao entre as vias %d e %d para pegar coordenadas.\n",idVia1,idVia2);
    exit(1);
}

void Mapa::atualizaCarros(bool boolMarcaPontoSemaforos){
    float comprimentoDoCarro,x,z,xOrientacao,zOrientacao,ratio,somarX,somarZ,x3,z3,*handleCoordsOrigemInicio, *handleCoordsOrigemFim,*handleCoordsDestinoInicio, *handleCoordsDestinoFim, *handleCoordsDestinoInicio2, *handleCoordsDestinoFim2;
    int c,c2,numViasPossiveis,numViasPossiveis2,idDestino,ladoConversao,idEsq,idDir;
    Via viaOrigem,*arrViasPossiveis,*arrViasPossiveis2,viaDestino;
    for(c=0;c<numCarrosCarregados;c++){
        //aqui calculamos quanto somaremos no x,z,xOrientacao e zOrientacao do carro mas ainda n�o somamos.
        x = arrCarros[c].getX();
        z = arrCarros[c].getZ();
        xOrientacao = arrCarros[c].getXOrientacao();
        zOrientacao = arrCarros[c].getZOrientacao();
        comprimentoDoCarro = sqrt(powf((xOrientacao-x),2) + powf((zOrientacao - z),2));
        ratio = arrCarros[c].getVelocidade() / comprimentoDoCarro;
        x3 = ratio * xOrientacao + (1 - ratio) * x;
        z3 = ratio * zOrientacao + (1 - ratio) * z;
        somarX = x3-x;
        somarZ = z3-z;

        //se o carro estiver fora do mapa (id da via é -1)
        if(arrCarros[c].getIdViaAtual() == -1){
            //tenta colocar ele no mapa
            if(posicionaCarro(arrCarros[c],0)==true){
                arrCarros[c].setEstado(1);
            }else{
                arrCarros[c].setEstado(0);
            }
            //interrompe o loop para este carro
            continue;
        }

        //se o carro esta em uma via valida no mapa
        arrCarros[c].setEstado(1);

        //carregamos a via atual do carro
        viaOrigem = getVia(arrCarros[c].getIdViaAtual());

        //se o carro acabou de percorrer a via (sua frente ja esta fora da via atual)
        if(viaOrigem.carroEstaDentroDaVia(xOrientacao+somarX,zOrientacao+somarZ) == false){
            //entao pega o id da proxima via
            idDestino = arrCarros[c].getIdProximaVia();
            //se a proxima via for -1, isto significa que o carro acabou seu trajeto
            if(idDestino == -1){
                //marca que esta fora da via
                arrCarros[c].setIdViaAtual(-1);
                //tenta reposicionar o carro no inicio
                if(posicionaCarro(arrCarros[c],0)){
                    //marca que o carro esta no mapa
                    arrCarros[c].setEstado(1);
                }else{
                    //marca estado para "fora do mapa"
                    arrCarros[c].setEstado(0);
                }
                //interrompe o loop para este carro
                continue;
            }else{
                arrCarros[c].setEstado(1);
            }
            // pegamos as possiveis vias correspondentes (filhas e pais)
            // fazemos isso pelo fato de que cortamos as vias no lugar que elas fazem interseccoes com outras
            numViasPossiveis = 0;
            arrViasPossiveis = new Via[100];
            for(c2=0;c2<numViasCarregadas;c2++){
                if(arrVias[c2].getId() == idDestino || arrVias[c2].getIdPai() == idDestino){
                    arrViasPossiveis[numViasPossiveis] = arrVias[c2];
                    numViasPossiveis++;
                }
            }
            //percorre as vias possiveis e guarda somente as que encostam na via de origem comparando suas coordenadas
            handleCoordsOrigemInicio = viaOrigem.getCoordInicio();
            handleCoordsOrigemFim = viaOrigem.getCoordFim();
            numViasPossiveis2 = 0;
            arrViasPossiveis2 = new Via[10];
            for(c2=0;c2<numViasPossiveis;c2++){
                handleCoordsDestinoInicio = arrViasPossiveis[c2].getCoordInicio();
                handleCoordsDestinoFim = arrViasPossiveis[c2].getCoordFim();
                //se alguma das coordenadas de origem for igual a alguma das da via de destino, guarda a via.
                if(
                        ((eIgualf(handleCoordsOrigemInicio[0],handleCoordsDestinoInicio[0])) && (eIgualf(handleCoordsOrigemInicio[2],handleCoordsDestinoInicio[2]))) ||
                        ((eIgualf(handleCoordsOrigemInicio[0],handleCoordsDestinoFim[0])) && (eIgualf(handleCoordsOrigemInicio[2],handleCoordsDestinoFim[2]))) ||
                        ((eIgualf(handleCoordsOrigemFim[0],handleCoordsDestinoInicio[0])) && (eIgualf(handleCoordsOrigemFim[2],handleCoordsDestinoInicio[2]))) ||
                        ((eIgualf(handleCoordsOrigemFim[0],handleCoordsDestinoFim[0])) && (eIgualf(handleCoordsOrigemFim[2],handleCoordsDestinoFim[2])))
                  ){
                    arrViasPossiveis2[numViasPossiveis2] = arrViasPossiveis[c2];
                    numViasPossiveis2++;
                }
            }
            //liberamos o arrViasPossiveis da memoria, pois ja terminamos de usa-lo.
            delete[] arrViasPossiveis;
            //neste ponto so temos as vias que realmente encostam na via de origem (maximo 2)
            //se for apenas 1 via, ja sabemos qual �.
            bool ContinuarNaMesmaVia = false;
            if(numViasPossiveis2 == 1){
                idDir = arrViasPossiveis2[0].getId();
                idEsq = arrViasPossiveis2[0].getId();
            }else if(numViasPossiveis2 == 2){
                //se forem 2 vias
                //pegamos as coordenadas da primeira via para calcularmos o coeficiente angular das vias (elas tem o mesmo coef. angular pois fazem parte da mesma via pai)
                handleCoordsDestinoInicio = arrViasPossiveis2[0].getCoordInicio();
                handleCoordsDestinoFim = arrViasPossiveis2[0].getCoordFim();
                //e tambem pegamos da segunda via pois usaremos nos calculos
                handleCoordsDestinoInicio2 = arrViasPossiveis2[1].getCoordInicio();
                handleCoordsDestinoFim2 = arrViasPossiveis2[1].getCoordFim();
                //aqui pegamos as vias que viram pra direita e pra esquerda, respectivamente em um array
                int *arrDireitaEsquerda = getArrayViasDireitaEsquerda(handleCoordsDestinoInicio,handleCoordsDestinoFim,handleCoordsDestinoInicio2,handleCoordsDestinoFim2,handleCoordsOrigemInicio,handleCoordsOrigemFim, arrViasPossiveis2[0].getId(),arrViasPossiveis2[1].getId());
                idDir = arrDireitaEsquerda[0];
                idEsq = arrDireitaEsquerda[1];
            }else if(numViasPossiveis2 == 0){
                //aqui o carro deve continuar na mesma via (se nao existir continuacao, a rota foi tracada errada)
                ContinuarNaMesmaVia = true;
            }else{
                printf("Mais de duas vias encostando na reta de origem\n");
                exit(1);
            }

            //liberamos o arrViasPossiveis2 da memoria, pois ja terminamos de usa-lo.
            delete[] arrViasPossiveis2;

            int InicioOuFim;
            int idViaDestino;
            int idSemaforo;

            //se � pro carro continuar na mesma via
            if(ContinuarNaMesmaVia){

                float *coordsViaOrigem,distanciaDoInicio,distanciaDoFim,*handleCoordsInicio,*handleCoordsFim;

                //ve distancia do inicio
                distanciaDoInicio = sqrtf( powf(handleCoordsOrigemInicio[0]-xOrientacao,2) + powf(handleCoordsOrigemInicio[2]-zOrientacao,2) );
                //ve distancia do fim
                distanciaDoFim = sqrtf( powf(handleCoordsOrigemFim[0]-xOrientacao,2) + powf(handleCoordsOrigemFim[2]-zOrientacao,2) );

                if(distanciaDoInicio < distanciaDoFim){
                    coordsViaOrigem = handleCoordsOrigemInicio;
                }else{
                    coordsViaOrigem = handleCoordsOrigemFim;
                }

                //pega id pai da via atual
                int idPai = viaOrigem.getIdPai();
                if(idPai == -1){
                    idPai = viaOrigem.getId();
                }

                int idContinuacao = -1;
                //procura, entre todas as vias que fazem parte da via que ele esta, qual e a continuacao.
                int c2;
                for(c2=0;c2<numViasCarregadas;c2++){
                    if(viaOrigem.getId() != arrVias[c2].getId()){
                        if(idPai == arrVias[c2].getId() || idPai == arrVias[c2].getIdPai()){
                            //pega as coordenadas de inicio e fim, da via da itera��o
                            handleCoordsInicio = arrVias[c2].getCoordInicio();
                            handleCoordsFim = arrVias[c2].getCoordFim();
                            //verifica se a via encosta no inicio ou no fim
                            //se sim, guarda o id da via e se vai posicionar no comeco ou fim
                            if( eIgualf(coordsViaOrigem[0],handleCoordsInicio[0]) && eIgualf(coordsViaOrigem[2],handleCoordsInicio[2]) ){
                                idContinuacao = arrVias[c2].getId();
                                InicioOuFim = 1;
                                break;
                            }
                            if( eIgualf(coordsViaOrigem[0],handleCoordsFim[0]) && eIgualf(coordsViaOrigem[2],handleCoordsFim[2]) ){
                                idContinuacao = arrVias[c2].getId();
                                InicioOuFim = 2;
                                break;
                            }
                        }
                    }
                }

                if(idContinuacao != -1){
                    idViaDestino = idContinuacao;
                    viaDestino = getVia(idViaDestino);
                    //guarda se tem semaforo vermelho
                    idSemaforo = temSemaforoVermelho(arrCarros[c].getIdViaAtual(),qualViaEstaCruzando(arrCarros[c]));
                }else{
                    //se achou a continuacao
                    printf("Nao ha continuacao para a via atual para o carro %d na via %d\n",arrCarros[c].getId(),getIdPai(arrCarros[c].getIdViaAtual()));
                    exit(1);
                }

            }else{

                //se � pro carro fazer alguma convers�o
                //mudamos o carro para a via correta.
                //pra isso, pegamos o lado da conversao que o carro est� prestes a fazer
                ladoConversao = arrCarros[c].getLadoConversaoParaProximaVia();
                if(ladoConversao == 1){
                    //se for direita
                    idViaDestino = idDir;
                }else{
                    //se for esquerda
                    idViaDestino = idEsq;
                }
                //agora que sabemos o id da via, vemos qual � o ponto dela que encosta com a via de origem, para saber onde posicionamos o carro
                //pegamos a via de destino
                viaDestino = getVia(idViaDestino);
                handleCoordsDestinoInicio = viaDestino.getCoordInicio();
                handleCoordsDestinoFim = viaDestino.getCoordFim();
                //vemos qual � o lado da via de destino que encosta na via de origem.
                InicioOuFim = 2;
                if(
                        ( eIgualf(handleCoordsOrigemInicio[0],handleCoordsDestinoInicio[0]) &&
                        eIgualf(handleCoordsOrigemInicio[2],handleCoordsDestinoInicio[2]) )
                        ||
                        ( eIgualf(handleCoordsOrigemFim[0],handleCoordsDestinoInicio[0]) &&
                        eIgualf(handleCoordsOrigemFim[2],handleCoordsDestinoInicio[2]) )
                  ){
                    InicioOuFim = 1;
                }
                //guarda se tem semaforo vermelho
                idSemaforo = temSemaforoVermelho(arrCarros[c].getIdViaAtual(),idViaDestino);
            }

            //se o carro tem espa�o na proxima via e n�o tem nenhum sem�foro vermelho para ele
            bool temEspacoNaProximaVia = temEspacoNaVia(arrCarros[c],viaDestino,InicioOuFim);
            if(temEspacoNaProximaVia == true && idSemaforo == -1){
                arrCarros[c].setIdViaAtual(idViaDestino);
                if(ContinuarNaMesmaVia == false){
                    arrCarros[c].setConversaoAtual(arrCarros[c].getConversaoAtual() + 1);
                }
                posicionaCarro(arrCarros[c],InicioOuFim);
            }else{
                //se o carro parou por sinal vermelho
                if(idSemaforo != -1){
                    //se o carro ainda nao esta parado no sinal vermelho
                    //if(arrCarros[c].estaParadoNoSinalVermelho() == false){
                        //marca que o carro esta parado por sinal vermelho
                        arrCarros[c].parouNoSinalVermelho(idSemaforo);
                        //marca um ponto para o semaforo
                        if(boolMarcaPontoSemaforos) marcaPontoSemaforo(idSemaforo);
                    //}
                }
            }

            

        }else{
            //se houver espa�o na sua frente, movimentamos o carro 1 unidade de velocidade pra frente.
            if(temEspacoParaAndar(arrCarros[c])){
                arrCarros[c].setX(x+somarX);
                arrCarros[c].setZ(z+somarZ);
                arrCarros[c].setXOrientacao(xOrientacao+somarX);
                arrCarros[c].setZOrientacao(zOrientacao+somarZ);
            }else{
                //se nao tem espaco pra andar e tem pelo menos um outro carro na via parado em semaforo, considera que esta parando por causa do semaforo tambem
                int idSemaforo = qualSemaforoCausandoTrafego(arrCarros[c].getIdViaAtual(),arrCarros[c].getSentido());
                //se realmente estiver parando atras de outros carros que estao parados em samaforo
                if(idSemaforo != -1){
                    //se o carro ainda nao esta parado no sinal vermelho
                    //if(arrCarros[c].estaParadoNoSinalVermelho() == false){
                        //marca que o carro esta parado por sinal vermelho
                        arrCarros[c].parouNoSinalVermelho(idSemaforo);
                        //marca um ponto para o semaforo
                        if(boolMarcaPontoSemaforos) marcaPontoSemaforo(idSemaforo);
                    //}
                }
            }
        }

        
    }
}

Via Mapa::getVia(int id){
    int c;
    for(c=0;c<numViasCarregadas;c++){
        if(arrVias[c].getId() == id){
            return arrVias[c];
        }
    }
    printf("Via %d nao existe\n",id);
    exit(1);
}

Semaforo Mapa::getSemaforo(int id){
    int c;
    for(c=0;c<numSemaforosCarregados;c++){
        if(arrSemaforos[c].getId() == id){
            return arrSemaforos[c];
        }
    }
    printf("Semaforo %d nao existe\n",id);
    exit(1);
}

float Mapa::modulo(float entrada){
    if(entrada < 0) entrada = entrada*(-1);
    return entrada;
}

int Mapa::getIdPai(int idFilha){
    int c;
    for(c=0;c<numViasCarregadas;c++){
        if(idFilha == arrVias[c].getId()){
            if(arrVias[c].getIdPai() != -1){
                return arrVias[c].getIdPai();
            }else{
                return idFilha;
            }
        }
    }
    printf("Via %d nao existe ao tentar pegar idPai\n",idFilha);
    exit(1);
}

void Mapa::posicionaCarroContinuacaoDaVia(Carro & carro){
    
}

int* Mapa::getArrayViasDireitaEsquerda(float *handleCoordsDestinoInicio,float *handleCoordsDestinoFim,float *handleCoordsDestinoInicio2,float *handleCoordsDestinoFim2, float *handleCoordsOrigemInicio, float *handleCoordsOrigemFim, int idVia1, int idVia2){
    float pontosOrigem[3],pontosOrigemNaoEncosta[3],pontosDestino1[3],pontosDestino2[3];
    int idEsq,idDir,idAux;
    //pegamos as coordenadas do ponto da reta de origem que encosta nas duas de destino
    if(
        ( eIgualf(handleCoordsDestinoInicio[0],handleCoordsOrigemInicio[0]) && eIgualf(handleCoordsDestinoInicio[2],handleCoordsOrigemInicio[2]) ) ||
        ( eIgualf(handleCoordsDestinoFim[0],handleCoordsOrigemInicio[0]) && eIgualf(handleCoordsDestinoFim[2],handleCoordsOrigemInicio[2]) )
      ){
        pontosOrigem[0] = handleCoordsOrigemInicio[0];
        pontosOrigem[2] = handleCoordsOrigemInicio[2];
        pontosOrigemNaoEncosta[0] = handleCoordsOrigemFim[0];
        pontosOrigemNaoEncosta[2] = handleCoordsOrigemFim[2];
    }else{
        pontosOrigem[0] = handleCoordsOrigemFim[0];
        pontosOrigem[2] = handleCoordsOrigemFim[2];
        pontosOrigemNaoEncosta[0] = handleCoordsOrigemInicio[0];
        pontosOrigemNaoEncosta[2] = handleCoordsOrigemInicio[2];
    }
    //pegamos as coordenadas dos pontos das outras retas que nao encostam na via de origem
    //primeira reta
    if( eIgualf(pontosOrigem[0],handleCoordsDestinoInicio[0]) && eIgualf(pontosOrigem[2],handleCoordsDestinoInicio[2]) ){
        pontosDestino1[0] = handleCoordsDestinoFim[0];
        pontosDestino1[2] = handleCoordsDestinoFim[2];
    }else{
        pontosDestino1[0] = handleCoordsDestinoInicio[0];
        pontosDestino1[2] = handleCoordsDestinoInicio[2];
    }
    //segunda reta
    if( eIgualf(pontosOrigem[0],handleCoordsDestinoInicio2[0]) && eIgualf(pontosOrigem[2],handleCoordsDestinoInicio2[2]) ){
        pontosDestino2[0] = handleCoordsDestinoFim2[0];
        pontosDestino2[2] = handleCoordsDestinoFim2[2];
    }else{
        pontosDestino2[0] = handleCoordsDestinoInicio2[0];
        pontosDestino2[2] = handleCoordsDestinoInicio2[2];
    }

    //antes de calcular os coeficientes, verificamos se as retas s�o paralelas ao X ou ao Z
    //nestes casos trataremos separadamente
    bool existeCoefAngOrigem = true;
    bool existeCoefAngDestino = true;
    //printf("%f %f %f %f\n",handleCoordsOrigemFim[0],handleCoordsOrigemInicio[0],handleCoordsOrigemFim[2],handleCoordsOrigemInicio[2]);
    if( eIgualf(handleCoordsOrigemFim[0],handleCoordsOrigemInicio[0]) || eIgualf(handleCoordsOrigemFim[2],handleCoordsOrigemInicio[2]) ){
        existeCoefAngOrigem = false;
    }
    //printf("%f %f %f %f\n",handleCoordsDestinoFim[0],handleCoordsDestinoInicio[0],handleCoordsDestinoFim[2],handleCoordsDestinoInicio[2]);
    if( eIgualf(handleCoordsDestinoFim[0],handleCoordsDestinoInicio[0]) || eIgualf(handleCoordsDestinoFim[2],handleCoordsDestinoInicio[2]) ){
        existeCoefAngDestino = false;
    }

    //se existe coeficiente angular para as duas retas e ele nao � zero
    if(existeCoefAngOrigem == true && existeCoefAngDestino == true){
        //coeficiente angular da via de origem
        float coefAngularOrigem = (handleCoordsOrigemFim[2]-handleCoordsOrigemInicio[2])/(handleCoordsOrigemFim[0]-handleCoordsOrigemInicio[0]);
        //e calculamos tambem o coeficiente angular das vias de destino
        float coefAngularDestino = (handleCoordsDestinoFim[2]-handleCoordsDestinoInicio[2])/(handleCoordsDestinoFim[0]-handleCoordsDestinoInicio[0]);
        //aqui comparamos os valores que temos para saber qual � direita e qual � esquerda
        //se o coeficiente angular das duas retas forem positivos
        if(coefAngularOrigem > 0 && coefAngularDestino > 0){
            if(modulo(coefAngularOrigem) < modulo(coefAngularDestino)){
                if(pontosDestino1[2] < pontosDestino2[2]){
                    idDir = idVia1;
                    idEsq = idVia2;
                }else{
                    idDir = idVia2;
                    idEsq = idVia1;
                }
            }else{
                if(pontosDestino1[2] < pontosDestino2[2]){
                    idDir = idVia2;
                    idEsq = idVia1;
                }else{
                    idDir = idVia1;
                    idEsq = idVia2;
                }
            }
        //se o coeficiente das duas retas forem negativos
        }else if(coefAngularOrigem < 0 && coefAngularDestino < 0){
            //CALCULADO
            if(modulo(coefAngularOrigem) < modulo(coefAngularDestino)){
                if(pontosDestino1[2] < pontosDestino2[2]){
                    idDir = idVia2;
                    idEsq = idVia1;
                }else{
                    idDir = idVia1;
                    idEsq = idVia2;
                }
            }else{

                if(pontosDestino1[2] < pontosDestino2[2]){
                    idDir = idVia1;
                    idEsq = idVia2;
                }else{
                    idDir = idVia2;
                    idEsq = idVia1;
                }
            }
        //se o coef da primeira for positiva e o da segunda for negativa
        }else if(coefAngularOrigem > 0 && coefAngularDestino < 0){
            if(pontosDestino1[2] < pontosDestino2[2]){
                idDir = idVia2;
                idEsq = idVia1;
            }else{
                idDir = idVia1;
                idEsq = idVia2;
            }
        //se o coef da primeira for negativa e o da segunda for positiva
        }else if(coefAngularOrigem < 0 && coefAngularDestino > 0){
            if(pontosDestino1[2] < pontosDestino2[2]){
                idDir = idVia2;
                idEsq = idVia1;
            }else{
                idDir = idVia1;
                idEsq = idVia2;
            }
        }
        //se os valores de X v�o da esquerda pra direita, o que fizemos acima est� certo.
        //mas se for ao contr�rio, temos que inverter.
        if(pontosOrigemNaoEncosta[0] > pontosOrigem[0]){
            idAux = idDir;
            idDir = idEsq;
            idEsq = idAux;
        }
    }else{
        //se algum dos coeficientes nao existem ou s�o zero
        //se nao existe coeficiente na reta de origem ou ele � zero
        if(existeCoefAngOrigem == false){
            //se os dois Z da reta de origem s�o iguais (paralela ao X)
            if( eIgualf(pontosOrigem[2],pontosOrigemNaoEncosta[2]) ){
                //se a coordenada X de inicio for menor que a de fim
                if(pontosOrigemNaoEncosta[0] < pontosOrigem[0]){
                    //a via com maior Z � direita e a com menor Z � esquerda
                    if(pontosDestino1[2] > pontosDestino2[2]){
                        idEsq = idVia2;
                        idDir = idVia1;
                    }else{
                        idEsq = idVia1;
                        idDir = idVia2;
                    }
                }else
                //se a coordenada de inicio for maior que a de fim
                if(pontosOrigemNaoEncosta[0] > pontosOrigem[0]){
                    //a via com maior Z � esquerda e a com menor Z � direita
                    if(pontosDestino1[2] > pontosDestino2[2]){
                        idEsq = idVia1;
                        idDir = idVia2;
                    }else{
                        idEsq = idVia2;
                        idDir = idVia1;
                    }
                }
            //se os dois X da reta de origem forem iguais (paralela ao Z)
            }else{
                //se a coordenada Z de inicio for menor que a de fim
                if(pontosOrigemNaoEncosta[2] < pontosOrigem[2]){
                    //a via com maior X � esquerda e a com menor X � direita
                    if(pontosDestino1[0] > pontosDestino2[0]){
                        idEsq = idVia1;
                        idDir = idVia2;
                    }else{
                        idEsq = idVia2;
                        idDir = idVia1;
                    }
                }else
                //se a coordenada Z de inicio for maior que a de fim
                if(pontosOrigemNaoEncosta[2] > pontosOrigem[2]){
                    //a via com maior X � direita e a com menor X � esquerda
                    if(pontosDestino1[0] > pontosDestino2[0]){
                        idEsq = idVia2;
                        idDir = idVia1;
                    }else{
                        idEsq = idVia1;
                        idDir = idVia2;
                    }
                }
            }
        }else
        //se nao existe coeficiente na reta de destino ou ele � zero
        if(existeCoefAngDestino == false){
            //se os dois Z da reta de destino s�o iguais (paralela ao X)
            if( eIgualf(handleCoordsDestinoFim[2],handleCoordsDestinoInicio[2]) ){
                //se a coordenada Z de origem � menor do que a de fim
                if(pontosOrigemNaoEncosta[2] < pontosOrigem[2]){
                    //a via com menor Z � esquerda e a com maior Z � direita
                    if(pontosDestino1[0] < pontosDestino2[0]){
                        idEsq = idVia2;
                        idDir = idVia1;
                    }else{
                        idEsq = idVia1;
                        idDir = idVia2;
                    }
                }else
                //se a coordenada Z de inicio for maior que a de fim
                if(pontosOrigemNaoEncosta[2] > pontosOrigem[2]){
                    //a via com menor X � esquerda e a com maior X � direita
                    if(pontosDestino1[0] < pontosDestino2[0]){
                        idEsq = idVia1;
                        idDir = idVia2;
                    }else{
                        idEsq = idVia2;
                        idDir = idVia1;
                    }
                }
            //se os dois X da reta de destino forem iguais (paralela ao Z)
            }else{
                //se a coordenada origem X de inicio for menor que a de fim
                if(pontosOrigemNaoEncosta[0] < pontosOrigem[0]){
                    //a via com menor Z � esquerda e a com maior Z � direita
                    if(pontosDestino1[2] < pontosDestino2[2]){
                        idEsq = idVia1;
                        idDir = idVia2;
                    }else{
                        idEsq = idVia2;
                        idDir = idVia1;
                    }
                }else
                //se a coordenada origem X de inicio for maior que a de fim
                if(pontosOrigemNaoEncosta[0] > pontosOrigem[0]){
                    //a via com menor Z � direita e a com maior Z � esquerda
                    if(pontosDestino1[2] < pontosDestino2[2]){
                        idEsq = idVia2;
                        idDir = idVia1;
                    }else{
                        idEsq = idVia1;
                        idDir = idVia2;
                    }
                }
            }
        }
    }

    int arrResposta[2];
    int *pArrResposta = arrResposta;
    arrResposta[0] = idDir;
    arrResposta[1] = idEsq;
    return pArrResposta;
}

bool Mapa::temEspacoParaAndar(Carro & carro){
    int c,idViaAtual;
    Via viaAtual;
    //float *coordsInicio, *coordsFim;
    float coordsCarro1[3],coordsCarro2[3],distanciaEntreOsCarros;
    //se o carro n�o est� no mapa (id da via � -1) retorna falso.
    idViaAtual = carro.getIdViaAtual();
    if(idViaAtual == -1) return false;
    //pega a via atual do carro
    viaAtual = getVia(idViaAtual);
    //coordsInicio = viaAtual.getCoordInicio();
    //coordsFim = viaAtual.getCoordInicio();
    //coordenadas da frente do carro que quer andar
    coordsCarro1[0] = carro.getXOrientacao();
    coordsCarro1[2] = carro.getZOrientacao();
    //verificamos, nos carros que estao na mesma via que ele, se h� algum carro a uma distancia menor do que a aceitavel.
    for(c=0;c<numCarrosCarregados;c++){
        //se os carros estao na mesma via
        if(arrCarros[c].getIdViaAtual() == carro.getIdViaAtual()){
            //se os carros estao no mesmo sentido
            if(arrCarros[c].getSentido() == carro.getSentido()){
                coordsCarro2[0] = arrCarros[c].getX();
                coordsCarro2[2] = arrCarros[c].getZ();
                distanciaEntreOsCarros = sqrtf(powf(coordsCarro2[0]-coordsCarro1[0],2) + powf(coordsCarro2[2]-coordsCarro1[2],2));
                //se a distancia nao for aceitavel
                if(distanciaEntreOsCarros < distanciaAceitavelCarroDaFrente){
                    //retorna false
                    return false;
                }
            }
        }
    }
    //se verificou todos os carros e todos estao aceitavelmente longe, entao retorna true.
    return true;
}

int Mapa::temSemaforoVermelho(int idViaPrincipal, int idViaSecundaria){
    int c = 1;
    
    //garante que os dois IDs sejam os IdPais, se ja nao forem.
    idViaPrincipal = getIdPai(idViaPrincipal);
    idViaSecundaria = getIdPai(idViaSecundaria);
    
    //verificamos se tem algum sem�foro na troca de vias que ele est� tentando fazer
    for(c=0;c<numSemaforosCarregados;c++){
        
        //printf("1 --- %d %d %d %d %d\n",arrSemaforos[c].getIdVia1(),idViaPrincipal,arrSemaforos[c].getIdVia2(),idViaSecundaria,arrSemaforos[c].getEstado());
        
        //se tem um sem�foto vermelho para ele da via 1 para a via 2
        if(arrSemaforos[c].getIdVia1() == idViaPrincipal && arrSemaforos[c].getIdVia2() == idViaSecundaria && arrSemaforos[c].getEstado() == 2){
            return arrSemaforos[c].getId();
        }
        //se tem um sem�foto vermelho para ele da via 2 para a via 1
        if(arrSemaforos[c].getIdVia1() == idViaSecundaria && arrSemaforos[c].getIdVia2() == idViaPrincipal && arrSemaforos[c].getEstado() == 1){    
            return arrSemaforos[c].getId();
        }
    }
    return -1;
}

bool Mapa::temEspacoNaVia(Carro & carro,Via via,int InicioOuFim){
    int c;
    float *coordsVia;
    float coordsCarro[3],distanciaLivre;

    if(InicioOuFim == 1){
        coordsVia = via.getCoordInicio();
    }else{
        coordsVia = via.getCoordFim();
    }
    //verificamos, nos carros que estao na mesma via que ele, se h� algum carro a uma distancia menor do que a aceitavel.
    for(c=0;c<numCarrosCarregados;c++){
        //se os carros estao na mesma via
        if(arrCarros[c].getIdViaAtual() == via.getId()){
            //se os carros estao no mesmo sentido
            if(arrCarros[c].getSentido() == InicioOuFim){
                if(InicioOuFim == 1){
                    coordsCarro[0] = arrCarros[c].getX();
                    coordsCarro[2] = arrCarros[c].getZ();
                }else{
                    coordsCarro[0] = arrCarros[c].getX();
                    coordsCarro[2] = arrCarros[c].getZ();
                }
                //calcula distancia livre entre o carro e as coordenadas da via
                //que � a distancia entre o carro e as coordenadas da via menos a distancia aceitavel entre os carros
                distanciaLivre = sqrtf(powf(coordsCarro[0]-coordsVia[0],2) + powf(coordsCarro[2]-coordsVia[2],2)) - distanciaAceitavelCarroDaFrente;
                //se a distancia nao for aceitavel
                if(distanciaLivre < carro.getComprimento()){
                    //retorna false
                    return false;
                }
            }
        }
    }
    return true;
}

const char* Mapa::getMapaFilename(){
    return mapaFilename;
}

void Mapa::lock(){
    pthread_mutex_lock(&mutex);
}

void Mapa::unlock(){
    pthread_mutex_unlock(&mutex);
}

void Mapa::marcaPontoSemaforo(int idSemaforo){
    int c;
    //marca um ponto no semaforo
    for(c=0;c<numSemaforosCarregados;c++){
        //se tem um sem�foto vermelho para ele da via 1 para a via 2
        if(arrSemaforos[c].getId() == idSemaforo){
            arrSemaforos[c].marcaPonto();
            return;
        }
    }
}

int Mapa::qualSemaforoCausandoTrafego(int idVia, int sentido){
    int c;
    Semaforo handleSemaforo;
    for(c=0;c<numCarrosCarregados;c++){
        //se o carro esta na mesma via
        if(arrCarros[c].getIdViaAtual() == idVia){
            if(arrCarros[c].estaParadoNoSinalVermelho() == true){
                handleSemaforo = getSemaforo(arrCarros[c].getIdSemaforoVermelho());
                //se o sem�foro ainda est� vermelho
                if(handleSemaforo.getEstado() == 2){
                    return handleSemaforo.getId();
                }
            }
        }
    }
    return -1;
}

void Mapa::mostrarResultados(){
    int c;
    for(c=0;c<numSemaforosCarregados;c++){
        printf("%d %d\n",arrSemaforos[c].getId(),arrSemaforos[c].getPontos());
    }
}

int Mapa::getPerformance(){
    int c;
    int performance = 0;
    for(c=0;c<numSemaforosCarregados;c++){
        performance += arrSemaforos[c].getPontos();
    }
    return performance;
}

int* Mapa::getMelhorPerformanceSemaforos(){
    int *arrPerformances = new int[numSemaforosCarregados];
    for(int c=0;c<numSemaforosCarregados;c++){
         arrPerformances[c] = arrSemaforos[c].getPontos();
    }
    return arrPerformances;
}

int* Mapa::getInfoGene(int id){
    int *arrGene = new int[5];
    Semaforo objSemaforo;
    objSemaforo = getSemaforo(id);
    arrGene[0] = objSemaforo.getIdVia1();
    arrGene[1] = objSemaforo.getIdVia2();
    arrGene[2] = objSemaforo.getTempoVerde();
    arrGene[3] = objSemaforo.getTempoVermelho();
    arrGene[4] = objSemaforo.getEstadoInicial();
    return arrGene;
}

int Mapa::getNumSemaforos(){
    return numSemaforosCarregados;
}

int Mapa::qualViaEstaCruzando(Carro & carro){
    int c;
    float *handleCoordsDestinoInicio,*handleCoordsDestinoFim,tolerancia;
    for(c=0;c<numViasCarregadas;c++){
        //nao compara com a mesma via
        if(getIdPai(arrVias[c].getId()) == getIdPai(carro.getIdViaAtual())) continue;
        handleCoordsDestinoInicio = arrVias[c].getCoordInicio();
        handleCoordsDestinoFim = arrVias[c].getCoordFim();
        //tolerancia (precisamos de tolerancia porque o carro nao esta exatamente no mesmo ponto da via
        //esta parte precisar� ser reescrita quando tivermos mais que 1 faixa nas vias
        tolerancia = carro.getVelocidade()*3;
        //verifica se a parte da frente est� encostada na via
        if(
                ( (eIgualf(carro.getXOrientacao(),handleCoordsDestinoInicio[0],tolerancia)) && (eIgualf(carro.getZOrientacao(),handleCoordsDestinoInicio[2],tolerancia)) ) ||
                ( (eIgualf(carro.getXOrientacao(),handleCoordsDestinoFim[0],tolerancia)) && (eIgualf(carro.getZOrientacao(),handleCoordsDestinoFim[2],tolerancia)) )
          ){
            return arrVias[c].getId();
        }
    }
    printf("nao foi possivel encontrar via que esta cruzando\n");
    exit(1);
}
