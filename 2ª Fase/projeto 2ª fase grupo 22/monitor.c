#include "util.h"

//apontador para o ficheiro onde será escrito o relatório da simulação
FILE *relatorio;

//constantes
int ASCII = 65;

//variáveis globais
int inicio_simulacao, fim_simulacao, num_produtos, quantidade_produtos;
int clientes_fila = 0, clientes_fila_prio = 0, empregados = 1, clientes_total = 0, clientes_total_prio = 0, desistencias = 0, clientes_trocaram = 0, esperaram = 0, esperaram_prio = 0, soma_esperaram = 0, soma_esperaram_prio = 0;
int corre = 0, pausa = 0, sair = 0;
//array de produtos
int* produtos;
char media[20];

void criar_produtos() 
{
	int i;
	produtos = (int*) malloc(num_produtos * sizeof(int));
	for(i = 0; i < num_produtos; i++)
		produtos[i] = quantidade_produtos;
}

//tipo: 1 - média clientes normais, 2 - média clientes prioritários
void getMedia(int tipo) 
{
	int i, aux;
	
	if(tipo == 1) //normais
		aux = soma_esperaram / esperaram;
	else //if(tipo == 2)
		aux = soma_esperaram_prio / esperaram_prio;
	
	for(i = 0; aux > 60; i++, aux-=60);
	
	if(i > 0)
		sprintf(media, "%d:%d minutos.", i, aux);
	else
		sprintf(media, "%d segundos.", aux);
	
	//return buffer;
}

void mostra_estatistica()
{
	printf("\nEstatística da loja\n");
	if(corre && !pausa)
		printf("\n1. Estado atual: Simulação a decorrer.\n");
	else
	{
		if(pausa)
			printf("\n1. Estado atual: Simulação em pausa.\n");
		else
			printf("\n1. Estado atual: Simulação terminada.\n");
	}
	printf("2. Clientes na fila: %d\n", clientes_fila);
	printf("3. Clientes na fila prioritária: %d\n", clientes_fila_prio);
	printf("4. Empregados no balcão: %d\n", empregados);
	
	int i, j;
	for(i = 0, j = 5; i < num_produtos; i++, j++) 
		printf("%d. Quantidade Produto %c: %d\n", j, i+ASCII, produtos[i]);
	
	printf("%d. Total de clientes: %d\n", j+1, clientes_total);
	printf("%d. Total de clientes prioritários: %d\n", j+2, clientes_total_prio);
	printf("%d. Desistências: %d\n", j+3, desistencias);
	printf("%d. Clientes que trocaram de produto: %d\n", j+4, clientes_trocaram);
	
	if(esperaram > 0)
	{
		getMedia(1);
		printf("%d. Tempo médio de espera de clientes na fila: %s\n", j+5, media);
	}
	else
		printf("%d. Tempo médio de espera de clientes na fila: 0 minutos\n", j+5);
	if(esperaram_prio > 0)
	{
		getMedia(2);
		printf("%d. Tempo médio de espera de clientes na fila prioritária: %s\n", j+6, media);
	}
	else 
		printf("%d. Tempo médio de espera de clientes na fila prioritária: 0 minutos\n", j+6);
	
}

//função que fica à escuta das mensagens do simulador
void *escuta_comunicacao(void *arg)
{
	int sockfd = *((int*) arg);
	int n, num_comandos;
	int horas, minutos, segundos, evento, cod1, cod2, cod3, cod4;
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
			num_comandos = sscanf(buffer, "%d %d %d %d %d %d %d %d", &horas, &minutos, &segundos, &evento, &cod1, &cod2, &cod3, &cod4);
			
			if(num_comandos > 0) 
			{
				switch(evento)
				{
					case 1:
						if(cod1 == 1) 
						{
							num_produtos = cod2;
							quantidade_produtos = cod3;
							criar_produtos();
							printf("%d:%d:%d - A loja abriu aos clientes.\n", horas, minutos, segundos);
							sprintf(buffer, "%d:%d:%d - A loja abriu aos clientes.\n", horas, minutos, segundos);
							inicio_simulacao = getTempo(horas, minutos, segundos);
						}
						else if(cod1 == 0) 
						{
							printf("%d:%d:%d - A loja fechou.\n", horas, minutos, segundos);
							sprintf(buffer, "%d:%d:%d - A loja fechou.\n", horas, minutos, segundos); 
							fim_simulacao = getTempo(horas, minutos, segundos);
							sair = 1;
						}
						break;
					case 2:
						printf("%d:%d:%d - Entrou um cliente na loja. (Cliente nº %d)\n", horas, minutos, segundos, cod1);
						sprintf(buffer, "%d:%d:%d - Entrou um cliente na loja. (Cliente nº %d)\n", horas, minutos, segundos, cod1);
						if(cod2 == 1)
							clientes_fila++;
						else //if(cod2 == 2)
						{
							clientes_fila_prio++;
							clientes_total_prio++;
						}
						clientes_total++;
						break;
					case 3:
						printf("%d:%d:%d - O cliente %d escolheu o produto %c e está a ser atendido.\n", horas, minutos, segundos, cod1, cod2+ASCII);
						sprintf(buffer, "%d:%d:%d - O cliente %d escolheu o produto %c e está a ser atendido.\n", horas, minutos, segundos, cod1, cod2+ASCII);
						produtos[cod2]--;
						if(cod3 == 1)
						{
							esperaram++;
							soma_esperaram += cod4;
						}
						else //if(cod3 == 2)
						{
							esperaram_prio++;
							soma_esperaram_prio += cod4;
						}
						break;
					case 4:
						printf("%d:%d:%d - O cliente %d foi atendido e saiu da loja.\n", horas, minutos, segundos, cod1);
						sprintf(buffer, "%d:%d:%d - O cliente %d foi atendido e saiu da loja.\n", horas, minutos, segundos, cod1);
						if(cod2 == 1)
							clientes_fila--;
						else //if(cod2 == 2)
							clientes_fila_prio--;
						break;
					case 5:
						printf("%d:%d:%d - O cliente %d desistiu por aborrecimento.\n", horas, minutos, segundos, cod1);
						sprintf(buffer, "%d:%d:%d - O cliente %d desistiu por aborrecimento.\n", horas, minutos, segundos, cod1);
						if(cod2 == 1)
							clientes_fila--;
						else //if(cod2 == 2)
							clientes_fila_prio--;
						desistencias++;
						break;
					case 6:
						printf("%d:%d:%d - O cliente %d decidiu trocar o produto %d pelo produto %d.\n", horas, minutos, segundos, cod1, cod2+ASCII, cod3+ASCII);
						sprintf(buffer, "%d:%d:%d - O cliente %d decidiu trocar o produto %d pelo produto %d.\n", horas, minutos, segundos, cod1, cod2+ASCII, cod3+ASCII);
						produtos[cod2]++;
						produtos[cod3]--;
						clientes_trocaram++;
						break;
					case 7:
						printf("%d:%d:%d - O gerente chamou um novo empregado para o balcão.\n", horas, minutos, segundos);
						sprintf(buffer, "%d:%d:%d - O gerente chamou um novo empregado para o balcão.\n", horas, minutos, segundos);
						empregados++;
						break;
					case 8:
						printf("%d:%d:%d - Foi reposto o produto %d.\n", horas, minutos, segundos, cod1);
						sprintf(buffer, "%d:%d:%d - Foi reposto o produto %d para o stock inicial.\n", horas, minutos, segundos, cod1);
						produtos[cod1] += cod2;
						break;
					case 9:
						printf("%d:%d:%d - O gerente retirou um emprego do balcão.\n", horas, minutos, segundos);
						sprintf(buffer, "%d:%d:%d - O gerente retirou um emprego do balcão.\n", horas, minutos, segundos);
						empregados--;
						break;
					default:
						break;
				}
				fprintf(relatorio, "%s", buffer);
			}
		}
	}
	return NULL;
}

int getTempo(int horas, int minutos, int segundos) 
{
	char tempo[6];
	if(minutos < 10) horas*=10;
	if(segundos < 10) minutos*=10;
	sprintf(tempo, "%d%d%d", horas, minutos, segundos);
	return atoi(tempo);
}

int main(int argc, char **argv)
{	
	struct sockaddr_un serv_addr, cli_addr;
	int sockfd, servlen, newsockfd;
	int n, num_comandos;

	int clilen = sizeof(cli_addr);
	char* opcao;
	
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
		fgets(opcao, sizeof(opcao), stdin);

		/*if(!strcmp (buffer, "sair\n") && pausa)
		{
			char temp_buf[20] = "retomar";
			send(newsockfd, temp_buf, sizeof(temp_buf), 0);
			sleep(1);
		}*/

		if(send(newsockfd, opcao, sizeof(opcao), 0) == -1)
		{
			perror("send");
			exit(1);
		}
	
		switch(atoi(opcao)) 
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

	printf("\nAcabou a simulação.\n");
	mostra_estatistica();
	fclose(relatorio);
	close(sockfd);
	return 0;
}
