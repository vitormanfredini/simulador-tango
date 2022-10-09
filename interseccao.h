#ifndef INTERSECCAO_H
#define INTERSECCAO_H

class Interseccao {

private:
    int idVia1;
    int idVia2;
    float x;
    float z;

public:
    void setIdVia1(int id);
    void setIdVia2(int id);
    void setX(float novoX);
    void setZ(float novoZ);
    int getIdVia1();
    int getIdVia2();
    float getX();
    float getZ();

    Interseccao();
    virtual ~Interseccao();
};

#endif
