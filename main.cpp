#include <stdlib.h>
#include <stdio.h>

#include <dirent.h>

#include <vector>

#include <string>
#include <string.h>

#include <fstream>
#include <iostream>
#include <ostream>

#include "mouseecamera.h"           // cabecalho da classe de mouse e camera
#include "tecladoemovimentacao.h"   // cabecalho da classe de teclado e movimentacao
#include "objeto3d.h"               // cabecalho da classe de objeto 3d
#include "mapa.h"                   // cabecalho da classe de mapa
#include "via.h"                    // cabecalho da classe de via
#include "ga.h"                     // cabecalho da classe de algoritmo genetico

#ifdef __APPLE__
#include <OpenGL/OpenGL.h>
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

using namespace std;

MouseECamera objMouseECamera;
TecladoEMovimentacao objTecladoEMovimentacao;
Mapa objMapa;

//sempre multiplo de 10
#define TEMPOATUALIZACAOMS 20

//centraliza o tratamento dos eventos de teclado de UP e PRESS para uma so funcao
void handleKeyPress(unsigned char key, int x, int y) {
    objTecladoEMovimentacao.handleKeyboardEvent(key, x, y, 1);
}

void handleKeyUp(unsigned char key, int x, int y) {
    objTecladoEMovimentacao.handleKeyboardEvent(key, x, y, 2);
}

//tratamento dos movimentos de mouse
void handleMouseClick(int a, int b, int c, int d) {
    //cout << a << " - " << b << " - " << c << " - " << d << "\n";
}

void handleMouseMovement(int x, int y) {
    //movimenta a camera
    objMouseECamera.amoveTo(x, y);
}

void handleResize(int w, int h) {
    glViewport(0, 0, w, h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(45.0, (float) w / (float) h, 0.5, 1350.0);
    objMouseECamera.setCameraLimits(w, h);
}

//flags de movimento
float movimentoZ = 0; //movimento em Z
float movimentoX = 0; //movimento em X
float movimentoY = 0; //movimento em Y

//funcao que atualiza os objetos
//chamada a cada 10 milisegundos
void update(int value) {

    movimentoX += (objTecladoEMovimentacao.getMovimentacaoEsquerda() - objTecladoEMovimentacao.getMovimentacaoDireita());
    movimentoY += (objTecladoEMovimentacao.getMovimentacaoSubir() - objTecladoEMovimentacao.getMovimentacaoDescer());
    movimentoZ += (objTecladoEMovimentacao.getMovimentacaoBaixo() - objTecladoEMovimentacao.getMovimentacaoCima());


    glutPostRedisplay();

    glutTimerFunc(TEMPOATUALIZACAOMS, update, 0);
}

void initRendering() {
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_LIGHTING);
    glEnable(GL_NORMALIZE);
    glEnable(GL_COLOR_MATERIAL);
    glEnable(GL_FOG);
    glEnable(GL_SMOOTH);

}

//loop que renderiza os objetos
void drawScene(){    

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    //rotacao da camera
    
    glRotatef(objMouseECamera.getAnguloRotateVertical(), 1.0f, 0.0f, 0.0f);
    glRotatef(objMouseECamera.getAnguloRotateHorizontal(), 0.0f, 1.0f, 0.0f);
    
    //glRotatef(82, 1.0f, 0.0f, 0.0f);
    //glRotatef(90, 0.0f, 1.0f, 0.0f);
    
    //printf("%f %f\n",objMouseECamera.getAnguloRotateVertical(),objMouseECamera.getAnguloRotateHorizontal());

    //luz ambiente
    GLfloat ambientLight[] = {1.0f, 1.0f, 1.0f, 0.0f};
    glLightModelfv(GL_LIGHT_MODEL_AMBIENT, ambientLight);
    
    //fundo
    glClearColor(0.35f, 0.52f, 0.76f, 1);
    //neblina
    GLfloat fogColor[] = {0.35f, 0.52f, 0.76f, 1};
    glFogfv(GL_FOG_COLOR, fogColor);
    glFogi(GL_FOG_MODE, GL_LINEAR);
    glFogf(GL_FOG_START, 500.0f);
    glFogf(GL_FOG_END, 750.0f);

    //posicao da cena inteira
    glTranslatef(-44.0f, -5.0f, 95.0f); //so pra levantar um pouco a visao
    
    glTranslatef(0.0f, 0.0f, -movimentoZ);
    glTranslatef(movimentoX, 0.0f, 0.0f);
    glTranslatef(0.0f, movimentoY, 0.0f);
    
    //glTranslatef(0.0f, 0.0f, 22.08);
    //glTranslatef(-13.59, 0.0f, 0.0f);
    //glTranslatef(0.0f, -191.07, 0.0f);
    
    //desenha o chao
    glBegin(GL_QUADS);
    glColor3f(0.07f,0.41f,0.14f);
    glVertex3f(-20000.0f,-3.0f,20000.0f);
    glVertex3f(20000.0f,-3.0f,20000.0f);
    glVertex3f(20000.0f,-3.0f,-20000.0f);
    glVertex3f(-20000.0f,-3.0f,-20000.0f);
    glEnd();

    //desenha objetos na tela
    objMapa.render(1,false);

    glFlush();
    glutSwapBuffers();

}


int main(int argc, char** argv){
    //primeiro de tudo, verificamos os parametros que recebemos do usuario para definir o modo de execucao
    int modoExecucao = 3; //1 simulacao, 2 otimizacao e 3 help

    //flags de parametros de simulacao
    bool pSimulacao = false;
    bool pSemaforos = false;

    //flags de parametros de otimizacao
    bool pOtimizacao = false;
    bool pPastaEntrada = false;
    bool pPastaSaida = false;

    //flags de parametros opcionais
    bool pSegundosOtimizacao = false;
    bool pGeracoes = false;
    
    string mapa,semaforos,direntrada,dirsaida;
    int segundosOtimizacao=300,numGeracoes=20;

    if(argc < 1){
        //mostra help
        modoExecucao = 3;
    }else if(argc == 2 && (strcmp(argv[1],"-h") == 0 || strcmp(argv[1],"--help") == 0) ){
        //mostra help
        modoExecucao = 3;
    }else if(argc > 2){
        int cont;
        for(cont=1;cont<argc;cont++){
            //se for parametro de simulacao
            if(strcmp(argv[cont],"-simulacao") == 0){
                //se ja carregou este parametro
                if(pSimulacao == true){
                    modoExecucao = 3;
                    printf("Parametro repetido: %s\n",argv[cont]);
                    break;
                }else if(pOtimizacao == true){
                    modoExecucao = 3;
                    printf("So um dos parametros e permitido por vez: --simulacao ou --otimizacao\n");
                    break;
                }else{
                    cont++;
                    if(cont < argc){
                        mapa.assign(argv[cont]);
                        pSimulacao = true;
                        modoExecucao = 1;
                        continue;
                    }else{
                        printf("Nenhum arquivo foi especificado para %s\n",argv[cont-1]);
                        modoExecucao = 3;
                        break;
                    }
                    
                }
            }else //se for parametro de simulacao
            if(strcmp(argv[cont],"-semaforos") == 0){
                //se ja carregou este parametro
                if(pSemaforos == true){
                    modoExecucao = 3;
                    printf("Parametro repetido: %s\n",argv[cont]);
                    break;
                }else{
                    cont++;
                    if(cont < argc){
                        semaforos.assign(argv[cont]);
                        pSemaforos = true;
                        continue;
                    }else{
                        printf("Nenhum arquivo foi especificado para %s\n",argv[cont-1]);
                        modoExecucao = 3;
                        break;
                    }
                }
            }else if(strcmp(argv[cont],"-otimizacao") == 0){
                //se ja carregou este parametro
                if(pOtimizacao == true){
                    modoExecucao = 3;
                    printf("Parametro repetido: %s\n",argv[cont]);
                    break;
                }else if(pSimulacao == true){
                    modoExecucao = 3;
                    printf("So um dos parametros e permitido por vez: --simulacao ou --otimizacao\n");
                    break;
                }else{
                    cont++;
                    if(cont < argc){
                        mapa.assign(argv[cont]);
                        pSimulacao = true;
                        modoExecucao = 2;
                    }else{
                        printf("Nenhum arquivo foi especificado para %s\n",argv[cont-1]);
                        modoExecucao = 3;
                        break;
                    }
                }

            }else if(strcmp(argv[cont],"-dir-entrada") == 0){
                //se ja carregou este parametro
                if(pPastaEntrada == true){
                    modoExecucao = 3;
                    printf("Parametro repetido: %s\n",argv[cont]);
                    continue;
                }else{
                    cont++;
                    if(cont < argc){
                        direntrada.assign(argv[cont]);
                        pPastaEntrada = true;
                    }else{
                        printf("Nenhuma pasta foi especificada para %s\n",argv[cont-1]);
                        modoExecucao = 3;
                        break;
                    }
                }
            }else if(strcmp(argv[cont],"-dir-saida") == 0){
                //se ja carregou este parametro
                if(pPastaSaida == true){
                    modoExecucao = 3;
                    printf("Parametro repetido: %s\n",argv[cont]);
                    continue;
                }else{
                    cont++;
                    if(cont < argc){
                        dirsaida.assign(argv[cont]);
                        pPastaSaida = true;
                    }else{
                        printf("Nenhuma pasta foi especificada para %s\n",argv[cont-1]);
                        modoExecucao = 3;
                        break;
                    }
                }
            }else if(strcmp(argv[cont],"-s") == 0){
                //se ja carregou este parametro
                if(pSegundosOtimizacao == true){
                    modoExecucao = 3;
                    printf("Parametro repetido: %s\n",argv[cont]);
                    continue;
                }else{
                    cont++;
                    if(cont < argc){
                        segundosOtimizacao = atoi(argv[cont]);
                        pPastaSaida = true;
                    }else{
                        printf("Nenhum valor foi especificado para %s\n",argv[cont-1]);
                        modoExecucao = 3;
                        break;
                    }
                }
            }else if(strcmp(argv[cont],"-g") == 0){
                //se ja carregou este parametro
                if(pGeracoes == true){
                    modoExecucao = 3;
                    printf("Parametro repetido: %s\n",argv[cont]);
                    continue;
                }else{
                    cont++;
                    if(cont < argc){
                        numGeracoes = atoi(argv[cont]);
                        pGeracoes = true;
                    }else{
                        printf("Nenhum valor foi especificado para %s\n",argv[cont-1]);
                        modoExecucao = 3;
                        break;
                    }
                }
            }else{
                modoExecucao = 3;
                printf("Parametro nao reconhecido: %s\n",argv[cont]);
                continue;
            }
        }
    }

    

    //agora verifica quais parametros foram passados dependendo do tipo de execucao
    //simulacao
    if(modoExecucao == 1){
        if(pSemaforos == false){
            modoExecucao = 3;
            printf("O arquivo de semaforos nao foi informado\n");
        }
    }else
    //otimizacao
    if(modoExecucao == 2){
        if(pPastaEntrada == false){
            modoExecucao = 3;
            printf("A pasta de entrada nao foi informada\n");
        }
        if(pPastaSaida == false){
            modoExecucao = 3;
            printf("A pasta de saida nao foi informada\n");
        }
    }

    //se e modo help, da explicacao sobre execucao
    if(modoExecucao == 3){
        printf("Uso:\n");
        printf("\tModo simulacao:\n");
        printf("\ttango -simulacao <arquivo do mapa> -semaforos <arquivo dos semaforos>\n");
        printf("\tModo otimizacao:\n");
        printf("\ttango -otimizacao <arquivo do mapa> -dir-entrada <diretorio com arquivos de entrada> -dir-saida <diretorio vazio para gerar saida> -s <segundos para simular cada configuracao> -g <quantas geracoes>\n");
        exit(1);
    }
    

    //alimentamos a funcao de random
    srand(time(NULL));

    //se for o modo de execucao de simulacao
    if(modoExecucao == 1){
        //le o mapa a as configuracoes de semaforos especificados por parametro e ja faz o processamento inicial
        objMapa.init(TEMPOATUALIZACAOMS, mapa.c_str(), semaforos.c_str(),1);
        //largura inicial da janela OpenGL
        int larguraInicial = 1250;
        int alturaInicial = 800;
        //inicializa classe de camera
        objMouseECamera.Init(larguraInicial, alturaInicial);

        glutInit(&argc, argv);
        glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
        glutInitWindowSize(larguraInicial, alturaInicial);
        char titulo[256];
        sprintf(titulo, "Simulador Tango - %s", mapa.c_str());
        glutCreateWindow(titulo);

        //prepara para comecar a renderizar
        initRendering();
        //poe mouse no centro da tela
        objMouseECamera.poeMouseCentro();

        //desenha objetos
        glutDisplayFunc(drawScene);
        //captura eventos de teclado
        glutKeyboardFunc(handleKeyPress);
        glutKeyboardUpFunc(handleKeyUp);
        glutIgnoreKeyRepeat(1);
        //redimensiona janela
        glutReshapeFunc(handleResize);
        //loop de atualizacao dos objetos
        glutTimerFunc(TEMPOATUALIZACAOMS, update, 0);
        //captura eventos de mouse
        glutMouseFunc(handleMouseClick);
        glutPassiveMotionFunc(handleMouseMovement);
        glutMotionFunc(handleMouseMovement);
        //some com o cursor
        glutSetCursor(GLUT_CURSOR_NONE);
        //vai pra fullscreen
        glutFullScreen();
        glutMainLoop();
        
    //se for o modo de execucao de otimizacao
    }else{
        int c,c2,idCromossomo,*infoGene,menorPerformance=99999999;
        //arquivo de configuracao da melhor performance encontrada
        string confMelhorPerf;
        int *semaforosMelhorPerf;
        //classe de algoritmo genetico
        Ga ObjGa;
        int numConfs;
        //array de performance para cada arquivo de configuracao de semaforos
        int arrPerformance[200];
        //diretorio de output
        ObjGa.setDiretorioDeSaida(dirsaida.c_str());
        //inicia diretorio de leitura com o argumento passado do diretorio das confs dos semaforos
        string diretorio;
        diretorio.assign(direntrada.c_str());
        //roda todas as geracoes
        for(c2=0;c2<numGeracoes+1;c2++){
            //pegaremos todos os arquivos de semaforos para testar um por um
            DIR           *d;
            struct dirent *dir;
            d = opendir(diretorio.c_str());
            //se conseguiu abrir o diretorio
            if(d){
                printf("Geracao %d\n",ObjGa.getGeracao());
                printf("Diretorio sendo processado: %s\n",diretorio.c_str());
                numConfs = 0;
                while ((dir = readdir(d)) != NULL){
	            //so abre arquivos com extensao txt
                    string str,str3,str2(".txt");
                    size_t achou;
                    str.append(dir->d_name);
                    achou = str.find(str2);
                    //se for um arquivo com extensao txt
                    if (achou != string::npos){
                        //le o mapa a as configuracoes de semaforos especificados no arquivo
                        str3.assign(diretorio.c_str());
                        str3.append("/");
                        str3.append(str.c_str());
                        objMapa.init(TEMPOATUALIZACAOMS, mapa.c_str(), str3.c_str(),2);
                        //printf("Configuracao %s... ",str3.c_str());
                        //comeca "pra valer" a partir de quantos segundos
                        int comecaAPartirDe = 0;
                        //simula sem interface grafica
                        for(c=0;c<(((segundosOtimizacao+comecaAPartirDe) * 1000)/TEMPOATUALIZACAOMS);c++){
                            //só começa a contar pontos a partir de X segundos
                            if(c >= ((comecaAPartirDe * 1000)/TEMPOATUALIZACAOMS)){
                                objMapa.render(2,true);
                            }else{
                                objMapa.render(2,false);
                            }
                            
                        }
                        //adiciona cromossomos (configuracoes dos semaforos do mapa) na classe de algoritmos geneticos
                        idCromossomo = ObjGa.newCromossomo();
                        for(c=0;c<objMapa.getNumSemaforos();c++){
                            //pega informacoes para adicionar genes (cada semaforo, com seu tempo de verde, vermlho,quais vias, etc)
                            infoGene = objMapa.getInfoGene(c);
                            ObjGa.newGene(infoGene[0],infoGene[1],infoGene[2],infoGene[3],infoGene[4],idCromossomo);
                            delete[] infoGene;
                        }
                        //mostra resultados na tela
                        //objMapa.mostrarResultados();
                        //salva os resultados desta configuracao
                        arrPerformance[numConfs] = objMapa.getPerformance();
                        //mantem sempre a configuracao com a melhor performance para mostrar no fim
                        if(arrPerformance[numConfs] < menorPerformance){
                            menorPerformance = arrPerformance[numConfs];
                            confMelhorPerf.assign(str3.c_str());
                            semaforosMelhorPerf = objMapa.getMelhorPerformanceSemaforos();
                        }
                        //mostra performance desta configuracao
                        //printf("Performance: %d\n",arrPerformance[numConfs]);
                        //incrementa numero de configuracoes rodadas e da geracao do algoritmo genetico
                        numConfs++;
                    }
                }
                closedir(d);
                //melhor performance da geracao
                printf("Melhor performance: %5.2fs\n",(((float)menorPerformance*TEMPOATUALIZACAOMS)/1000));
                //semaforos da melhor configuracao
                for(int c3=0;c3<objMapa.getNumSemaforos();c3++){
                    printf("Performance semaforo %d: %5.2fs\n",c3,(((float)semaforosMelhorPerf[c3]*TEMPOATUALIZACAOMS)/1000));
                }
                //melhor conf da geracao
                printf("Arquivo da melhor configuracao: %s\n",confMelhorPerf.c_str());
                //faz ranking, crossover e mutacao em todas as geracoes, menos na ultima. porque na ultima so precisamos simular os cromossomos gerados.
                if(c2 < numGeracoes){
                    //cria nova geracao
                    int geracaoGerada = ObjGa.novaGeracao();
                    //cria diretorio de saida desta geracao e ja seta o diretorio pra proxima leitura onde estarao as proximas confs de semaforos geradas pelo GA
                    diretorio.assign(ObjGa.getDiretorioDeSaida(geracaoGerada));
                    //gera ranking baseado no array de performance das configuracoes
                    ObjGa.ranking(numConfs,arrPerformance);
                    //faz crossover e mutacao
                    ObjGa.crossoverEMutacao();
                    //limpa cromossomos
                    ObjGa.limpaCromossomos();
                }
                printf("------------------------------------------\n");
            }else{
                printf("Nao foi possivel abrir o diretorio %s\n",diretorio.c_str());
                exit(1);
            }
        }
        
        printf("Melhor configuracao encontrada\nPerformance: %5.2fs\nArquivo: %s\n",(((float)menorPerformance*TEMPOATUALIZACAOMS)/1000),confMelhorPerf.c_str());
    }
    
    return 0;
}
