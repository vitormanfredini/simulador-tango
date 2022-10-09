CC = g++
CFLAGS = -Wall -ansi
PROG = tango

SRCS = main.cpp mouseecamera.cpp tecladoemovimentacao.cpp objeto3d.cpp mapa.cpp via.cpp conversao.cpp semaforo.cpp interseccao.cpp carro.cpp ga.cpp

ifeq ($(shell uname),Darwin)
	LIBS = -framework OpenGL -framework GLUT -framework ApplicationServices
else
	LIBS = -lGL -lGLU -lglut -pthread
endif

all: $(PROG)

$(PROG):	$(SRCS)
	$(CC) $(CFLAGS) -o $(PROG) $(SRCS) $(LIBS)

clean:
	rm -f $(PROG)