#include "header.h"

#define FILENAME "simulador_config.conf"

int main(int argc, char **argv)
{ 
	struct config configuracao;
	int i;
	char ficheiro_config[50];

	ficheiro_config[0] = '\0';

	if (argc == 3)
	{
		strcat(ficheiro_config, argv[1]);
		if(atoi(argv[2]) < 1)
		{
			printf("Tempo de simulacao invalido");
			return 0;
		}
		else
			configuracao.minutos_simulacao = atoi(argv[2]);
	}
	else
	{
		printf("Carregando configuracoes predefinidas \n\n");
		configuracao.minutos_simulacao = 1;
		strcat(ficheiro_config, FILENAME);
	}

    ler_config(ficheiro_config, "QUANTIDADE_PRODUTOS", &configuracao.quantidade_produtos);
	ler_config(ficheiro_config, "STOCK", &configuracao.stock);
	ler_config(ficheiro_config, "EMPREGADOS_MAX", &configuracao.empregados_max );
	ler_config(ficheiro_config, "CLIENTES_MAX", &configuracao.clientes_max);
	ler_config(ficheiro_config, "COMPRA_MAX", &configuracao.compra_max);
	ler_config(ficheiro_config, "AFLUENCIA_CLIENTES", &configuracao.afluencia_clientes);
		
	printf("Quantidade de Produtos\t%i\n",configuracao.quantidade_produtos);
	printf("Stock\t\t%i\n",configuracao.stock);     
	printf("Maximo de Empregados\t%i\n",configuracao.empregados_max);
	printf("Maximo de clientes\t%i\n",configuracao.clientes_max);  
	printf("Maximo de compra\t%i\n",configuracao.compra_max);
	printf("Afluencia de clientes\t%i\n",configuracao.afluencia_clientes);
		        
	return 0;
}
