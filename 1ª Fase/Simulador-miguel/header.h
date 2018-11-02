#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct config
{
   int quantidade_produtos;
   int stock;
   int empregados_max;
   int clientes_max;
   int compra_max;
   int afluencia_clientes;
   int minutos_simulacao;
};

void ler_config(char *aFilename, char *aParametro, int* retorno);
