#ifndef CONVERSAO_H
#define CONVERSAO_H

class Conversao {
    
private:
    int id;
    int idCarro;
    int idViaOrigem;
    int idViaDestino;
    int sentido; // sentido que o carro esta vindo na via de origem. 1 para inicio-fim e 2 para fim-inicio
    int lado; //1 para direita e 2 para esquerda

public:
    void setId(int idNovo);
    void setIdCarro(int id);
    void setIdViaOrigem(int id);
    void setIdViaDestino(int id);
    void setLado(int novoLado);
    int getId();
    int getIdCarro();
    int getIdViaOrigem();
    int getIdViaDestino();
    int getLado();

    Conversao();
    virtual ~Conversao();
};

#endif
