#include "util.h"

//apontador para o ficheiro onde será escrito o relatório da simulação
FILE *relatorio;

//constantes
int ASCII = 65;

//variáveis globais
int inicio_simulacao, fim_simulacao, num_produtos, quantidade_produtos;
int clientes_fila = 0, empregados = 1, clientes_total = 0, desistencias = 0, esperaram = 0, soma_esperaram = 0;
int corre = 0, pausa = 0, sair = 0;
//array de produtos
int* produtos;

void criar_produtos() 
{
	int i;
	produtos = (int*) malloc(num_produtos * sizeof(int));
	for(i = 0; i < num_produtos; i++)
		produtos[i] = quantidade_produtos;
}

void mostra_estatistica()
{
	printf("\nEstatística da loja\n");
	if(corre && !pausa)
		printf("\n1. Estado atual: Simulação a decorrer.\n");
	else
	{
		if(pausa)
			printf("1. Estado atual: Simulação em pausa.\n");
		else
			printf("1. Estado atual: Simulação terminada.\n");
	}
	printf("2. Clientes na fila: %d\n", clientes_fila);
	printf("3. Empregados ao balcão: %d\n", empregados);
	
	int i, j;
	for(i = 0, j = 4; i < num_produtos; i++, j++) 
		printf("%d. Quantidade Produto %c: %d\n", j, i+ASCII, produtos[i]);
	
	printf("%d. Total de clientes: %d\n", j+1, clientes_total);
	printf("%d. Desistências: %d\n", j+2, desistencias);
	if(esperaram)
		printf("%d. Tempo médio de espera de clientes na fila: %f minutos\n", j+3, (float)soma_esperaram / (float)esperaram);
	else
		printf("%d. Tempo médio de espera de clientes na fila: 0 minutos\n\n", j+3);
}

//função que fica à escuta das mensagens do simulador
void *escuta_comunicacao(void *arg)
{
	int sockfd = *((int *) arg);
	int n, num_comandos;
	int tempo = 0, evento, cod1, cod2, cod3;
	char buffer[BUFFER_SIZE];

	//ciclo que fica à espera de mensagens do simulador
	while(1)
	{
		if((n = recv(sockfd, buffer, sizeof(buffer), 0)) <= 0)
		{
			if(n < 0)
				perror("recv");
		}
		else
		{
           	buffer[n] = '\0';
			//fprintf(relatorio,"%s", buffer);
			num_comandos = sscanf(buffer, "%d %d %d %d %d", &tempo, &evento, &cod1, &cod2, &cod3);

			if(num_comandos > 0) 
			{
				switch(evento)
				{
					case 1:
						num_produtos = cod2;
						quantidade_produtos = cod3;
						criar_produtos();
						if(cod1 == 1) 
						{
							//system("clear");
							printf("%d - A loja abriu aos clientes.\n", tempo);
							sprintf(buffer, "%d - A loja abriu aos clientes.\n", tempo);
							fprintf(relatorio, "%s", buffer);
							inicio_simulacao = tempo;
						}
						else if(cod1 == 0) {
							printf("%d - A loja fechou.\n", tempo);
							sprintf(buffer, "%d - A loja fechou.\n", tempo); 
							fprintf(relatorio, "%s", buffer);
							fim_simulacao = tempo;
							sair = 1;
						}
						break;
					case 2:
						printf("%d - Entrou um cliente na loja. (Cliente nº %d)\n", tempo, cod1);
						sprintf(buffer, "%d - Entrou um cliente na loja. (Cliente nº %d)\n", tempo, cod1);
						fprintf(relatorio, "%s", buffer); 
						clientes_total++;
						break;
					case 3:
						break;
				}
			}
		}
	}
	return NULL;
}

int main(int argc, char **argv)
{	
	struct sockaddr_un serv_addr, cli_addr;
	int sockfd, servlen, newsockfd;
	int n, num_comandos;

	int clilen=sizeof(cli_addr);
	char buffer[BUFFER_SIZE];
	
	//Criacao do socket UNIX
	if((sockfd=socket(AF_UNIX, SOCK_STREAM, 0)) < 0)
		perror("cant open socket stream");
	serv_addr.sun_family=AF_UNIX;
	strcpy(serv_addr.sun_path, UNIXSTR_PATH);
	servlen = strlen(serv_addr.sun_path) + sizeof(serv_addr.sun_family);
	unlink(UNIXSTR_PATH);
	if(bind(sockfd, (struct sockaddr *) &serv_addr, servlen) < 0)
		perror("cant bind local address");
	listen(sockfd, 1);

	if((newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, &clilen)) < 0)
		perror("accept error");
		
	//criacao da tarefa que ira tratar da comunicação com o simulador
	pthread_t thread;
	pthread_create(&thread, NULL, &escuta_comunicacao, &newsockfd);
		
	unlink("relatorio.txt");
	relatorio = fopen("relatorio.txt", "a");
	
	system("clear");
	printf("\nSimulador Loja\n\n");
	printf("Para começar e durante a simulação pode usar as seguintes opções:\n");
	printf("1 - Começar Simulação \n");
	printf("2 - Pausar Simulação \n");
	printf("3 - Retomar Simulação \n");
	printf("4 - Mostrar estatística \n");
	printf("5 - Sair \n\n");
	
	do
	{
		fgets(buffer, sizeof(buffer), stdin);

		/*if(!strcmp (buffer, "sair\n") && pausa)
		{
			char temp_buf[20] = "retomar";
			send(newsockfd, temp_buf, sizeof(temp_buf), 0);
			sleep(1);
		}*/

		if(send(newsockfd, buffer, sizeof(buffer), 0) == -1)
		{
			perror("send");
			exit(1);
		}
	
		switch(atoi(buffer)) 
		{
			case 1:
				corre = 1;
				pausa = 0;
				break;
			case 2:
				pausa = 1;
				printf("Simulacao em pausa - Digite 3 para retomar: ");
				break;
			case 3:
				pausa = 0;
				break;
			case 4:
				mostra_estatistica();
				break;
			case 5:
				sair = 1;
				break;
			default:
				printf("Inseriu uma opção errada.\nOpções: 1-Começar; 2-Pausar; 3-Retomar; 4-Estatística; 5-Sair \n");
		}
	}while(!sair);
	
	corre = 0;
	pausa = 0;

	printf("\nAcabou a simulação.\n\n");
	mostra_estatistica();
	fclose(relatorio);
	close(sockfd);
	return 0;
}
