### Simulador Tango

Código do meu TCC, feito em 2010.
É um simulador de trânsito que usa um algoritmo genético para decidir a melhor combinação de tempos para seus semáforos.

Instalação
```
sudo apt-get install freeglut3-dev libdevil-dev
make
```

Para rodar em modo otimização (procurar os melhores tempos):

`./tango -otimizacao mapas/mapa5.txt -dir-entrada exemplosentrada/mapa5/ -dir-saida saida/ -s 120 -g 10`

Para visualizar graficamente uma configuração de semáforos funcionando:

`./tango -simulacao mapas/mapa5.txt -semaforos saida/10/conf00061.txt`