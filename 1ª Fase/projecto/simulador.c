#include "util.h"

struct sockaddr_un serv_addr;
int sockfd, servlen;

time_t tempo;
int corre = 0, pausa = 0, sair = 0;

//Função que trata dos pedidos vindos do Monitor 
void *recebe_comandos_monitor(void *arg)
{
	struct sockaddr_un cli_addr;
	int done, n, id;

	int sockfd=*((int *) arg), clilen=sizeof(cli_addr);

	char buffer[BUFFER_SIZE];
	
	//Ciclo que fica à espera dos pedidos dos Monitor, para lhe dar resposta adequada
	while(1)
	{
		done = 0;	
		if((n = recv(sockfd, buffer, sizeof(buffer), 0)) <= 0)
		{
			if(n < 0) 
				perror("recv error");
			done = 1;
		}
		buffer[n] = '\0';
		
		switch(atoi(buffer)) {
			case 1: //Começar
				corre = 1;
				break;
			case 2: //Pausar
				pausa = 1;
				break;
			case 3: //Remotar
				pausa = 0;
				break;
			case 5: //Sair
				corre = 0;
				exit(1);
			default:
				break;
		}
	}
	return NULL;
}

int main(int argc, char **argv)
{
	struct configuracao configs;
	
	if(argc == 3)
	{
		if(atoi(argv[1]) > 0)
		{
			printf("Insira o ficheiro de configuração seguindo do tempo de simulação.\n");
			return 0;
		}
		else if(atoi(argv[2]) < 1)
		{
			printf("O tempo de simulação tem que ser maior que 1.\n");
			return 0;
		}
		else
			configs.tempo_simulacao = atoi(argv[2]);
	}
	else
	{
		printf("Insiriu mal os argumentos.\n");
		return 0;
	}
	
	//interpretacao do ficheiro de configuraçao
	int *conf = ler_config(argv[1]);
	configs.tempo_medio_chegada_clientes 		= conf[0];
	configs.numero_produtos						= conf[1];
	configs.stock_inicial_produtos				= conf[2];
	configs.tempo_produto_um					= conf[3];
	configs.tempo_produto_dois					= conf[4];
	configs.max_clientes_fila 					= conf[5];
	configs.min_stock_produto					= conf[6];
	configs.prob_cliente_desistir_aborrecido 	= conf[7];
	configs.prob_cliente_alterar_produto 		= conf[8];
	
	//printf("Tempo de simulação:  %d\n", configs.tempo_simulacao);
	//printf("Tempo médio de chegada de um cliente:  %d\n", configs.tempo_medio_chegada_clientes);     
	//printf("Tempo produto um: %d\n", configs.tempo_produto_um);
	//printf("Tempo produto dois: %d\n", configs.tempo_produto_dois);
	//printf("Máximo de clientes por fila:  %d\n", configs.max_clientes_fila);
	//printf("Mínimo de stock por produto: %d\n", configs.min_stock_produto);
	//printf("Probabilidade de um cliente desistir aborrecido:  %d\n", configs.prob_cliente_desistir_aborrecido);  
	//printf("Probabilidade de um cliente alterar o produto:  %d\n", configs.prob_cliente_alterar_produto);
	
	//criacao do socket e ligação
	if((sockfd = socket(AF_UNIX, SOCK_STREAM, 0)) < 0)
		perror("Simulador: cant open socket stream");
	bzero((char *) &serv_addr, sizeof(serv_addr));
	serv_addr.sun_family = AF_UNIX;
	strcpy(serv_addr.sun_path, UNIXSTR_PATH);
	servlen = strlen(serv_addr.sun_path) + sizeof(serv_addr.sun_family);
	if(connect(sockfd, (struct sockaddr *) &serv_addr, servlen) < 0)
		perror("connect error");
			
	//Criacao da tarefa que ira tratar dos pedidos enviados pelo Monitor
	pthread_t thread;
	pthread_create(&thread, NULL, &recebe_comandos_monitor, &sockfd);
		
	while(!corre);
	
	char buffer[BUFFER_SIZE];
	char input[200]; //para apagar depois da 2ª fase
	tempo = time(0);
	sprintf(buffer, "%d 1 1 %d %d\n", (int)tempo, configs.numero_produtos, configs.stock_inicial_produtos);
	send(sockfd, buffer, sizeof(buffer), 0);
		
	configs.tempo_simulacao *= 60;
	
	do
	{
		fgets(input, sizeof(input), stdin);
		sprintf(buffer, "%d %s", (int)tempo, input);
		send(sockfd, buffer, sizeof(buffer), 0);
	}while(sair != 1);
	
	close(sockfd);
	
	return 0;
} 
