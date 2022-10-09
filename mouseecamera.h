#ifndef MOUSEECAMERA_H
#define MOUSEECAMERA_H

#ifdef __APPLE__
#include <OpenGL/OpenGL.h>
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

class MouseECamera {
private:
        int centerWidth;							// Centro da camera na horizontal
	int centerHeight;							// Centro da camera na vertical
        
        int totalWidth;                                                         // Centro da camera na horizontal
	int totalHeight;							// Centro da camera na vertical

        int xAnterior;                                                          // Ultima posição do cursor em x
	int yAnterior;                                                          // Ultima posicao do cursor em y

        float rotateHorizontal;                                                 // Rotacao Horizontal em graus gerada pelo movimento do mouse
	float rotateVertical;                                                   // Rotacao Vertical em graus gerada pelo movimento do mouse
public:
	void amoveTo(int x, int y);						// Movimenta camera absolutamente

        void setCameraLimits(int x, int y);                                     // Seta os limites da camera (tamanho da tela)
        void Init(int x, int y);                                                // Inicializa camera

        float getAnguloRotateHorizontal();                                      // Retorna o angulo para rotate em eixo y que o ultimo movimento de mouse gerou
        float getAnguloRotateVertical();                                        // Retorna o angulo para rotate em eixo x que o ultimo movimento de mouse gerou
        int getCenterWidth();                                                   // Retorna o meio da tela na horizontal
        int getCenterHeight();                                                  // Retorna o meio da tela na vertical
        void poeMouseCentro();                                                  // Poe mouse no centro da tela


	MouseECamera();								// Constructor
	virtual ~MouseECamera();						// Destructor
};

#endif
