/*-------------------------------------------------------------------------------------------*\
|                                                                                             |
|               Made by Francisco Matias, ist199936  & João Ambrósio, ist199976               |
|                                                                                             |
\*-------------------------------------------------------------------------------------------*/

#include "cot.h"

#define max(A,B) ((A)>=(B)?(A):(B))

/*
Função main: Função que executa as funcionalidades pedidas no projeto.	
*/
int main(int argc,char* argv[])
{

	//Informação característica do nosso nó.
	char id[3];
    char IP[16];
    char TCP_porto[16];
    char regIp [16];
    char regUdp[16];
	char NET[4];

	char names[100];
	int aux_names[100] ={0}; 

	//Inicialização dos conteúdos 
	content *Lista_ficheiros = (content*) malloc(sizeof(content));

	strcpy(Lista_ficheiros->name, "-1");
	Lista_ficheiros->vazia = 1;
	Lista_ficheiros->next = NULL;

	//Inicialização dos nós vizinhos 
	node_info *ext = (node_info*) malloc(sizeof(node_info));
	node_info *backup = (node_info*) malloc(sizeof(node_info));
	node_info *interno = (node_info*) malloc(sizeof(node_info));

	//////
	ext->next = NULL;
	backup->next = NULL;
	interno->next = NULL; //Guarda os internos (Como pode ter vários , necessita de ter uma lista) 
	strcpy(interno->id ,"-1");
	//////

	//Inicialização da tabela de expedição
	expedicao* Head = (expedicao*) malloc(sizeof(expedicao));
	strcpy(Head->vizinho , " ");
	strcpy(Head->destino , " ");
	Head->next = NULL;


    int fd,newfd, errcode;
    fd_set rfds;
    struct addrinfo hints,*res;

    struct sockaddr addr;
    socklen_t addrlen;

    int i = 0;

    int maxfd,counter, fds_counter, cojt_fds[102];

	//Sigpipe
	struct sigaction act;
	memset(&act, 0 , sizeof (act));
	act.sa_handler = SIG_IGN;
	if(sigaction(SIGPIPE, &act, NULL)==-1)
		exit(1);

    fds_counter = 0;

	//User
    cojt_fds[0] = 0;
    fds_counter++;
    maxfd = 0;


    if(argc <= 3){

		//Por omissão estes parâmetros têm o seguinte valor:
		strcpy (IP,argv[1]);
		strcpy (TCP_porto,argv[2]);
		strcpy(regIp, "193.136.138.142");
		strcpy (regUdp, "59000");
    }
	else if(argc == 5)
	{
		strcpy (IP,argv[1]);
		strcpy (TCP_porto,argv[2]);
		strcpy (regIp ,argv[3]);
		strcpy (regUdp,argv[4]);
	}
	else{
		
		printf("Número inválido de argumentos de entrada da funcao!\n");
		exit(1);
	}


	// TcpServer

    if((fd=socket(AF_INET,SOCK_STREAM,0))==-1) 
	{
		perror("socket");
		exit(1);//error
	}

	memset(&hints,0,sizeof hints);
    hints.ai_family=AF_INET;//IPv4
    hints.ai_socktype=SOCK_STREAM;//TCP socket
    hints.ai_flags=AI_PASSIVE;

	if((errcode=getaddrinfo(NULL,TCP_porto,&hints,&res))!=0) //imagino que aqui tenha de estar o nosso endereço IP e Porto respetivamente
    {
		fprintf(stderr, "getaddrinfo error: %s\n", gai_strerror(errcode));
	}
    if(bind(fd,res->ai_addr,res->ai_addrlen)==-1) //called object ‘bind’ is not a function or function pointer
    {
		perror("bind");
	    exit(1);/*error*/
	}
    
	if(listen(fd,5) == -1) 
	{
		perror("listen");
		exit(1);/*error*/
	}
	
	freeaddrinfo(res);

    cojt_fds[1] = fd;
	maxfd = max(maxfd, cojt_fds[fds_counter]);
	fds_counter++;

    while(1){
        FD_ZERO(&rfds);

        for (i=0 ; i < fds_counter; i++)
            FD_SET(cojt_fds[i], &rfds);


        counter = select(maxfd+1, &rfds, (fd_set*)NULL, (fd_set*)NULL, (struct timeval *)NULL);
        if(counter<=0)/*error*/exit(1);

        for (i=0 ; i < fds_counter; i++){
            if(FD_ISSET(cojt_fds[i],&rfds)){

                FD_CLR(cojt_fds[i],&rfds);
                switch(i){
                    case 0:
						
						//mensagem do user (receber e intrepretar) se a mensagem recebida for "exit" terminar programa com sucesso, caso contrário continuar
						ler_user(NET, &ext,id,&backup,&interno, IP, TCP_porto, regIp, regUdp, &maxfd , &fds_counter, cojt_fds , names, aux_names, &Lista_ficheiros, Head);

					break;

                    case 1:
                    	
						//pedido de acesso de um novo nó da rede
                        addrlen = sizeof(addr);
                        if((newfd=accept(fd,&addr,&addrlen))==-1)
                            exit(1);/*error*/
                        cojt_fds[fds_counter] = newfd;
						maxfd = max(maxfd, cojt_fds[fds_counter]);
						
						fds_counter++;

                        break;

                    default:

						//mensagem tcp de um nó da rede (receber e intrepretar)
						ler_No(cojt_fds[i], &ext,id,&backup,&interno, IP, TCP_porto, regIp, regUdp, &maxfd , &fds_counter, cojt_fds , &Head, &Lista_ficheiros);

						break;

						}//switch(i)
					}
				}
		}//while(1)
}//main


/*
Funcao ler_user: recebe recebe os dados provenientes do utilizador e carrega as palavras por ordem para o conjunto de strings indicadores e invoca a funcao interpretacao, para os intrepretar


*/
void ler_user (char* NET, node_info** ext, char* id, node_info** backup, node_info**interno, char* IP, char* TCP_porto, char* regIp, char* regUdp, int* maxfd, int* fds_counter, int* cojt_fds, char* names, int* aux_names, content **Lista_ficheiros, expedicao* Head)
{
	char* buffer = (char*) malloc (sizeof (char) * 128);
	   if (buffer == NULL){
       printf("Memory allocation error.");
       exit(1);
       }

	char indicadores [6][16];
	int args = 0;

    if (fgets(buffer, 128, stdin) == NULL) {
    	printf("Erro ao ler entrada!\n");
    	//exit(1);
	}

	args = count_strings(buffer);
	
	buffer[strlen(buffer)-1] = '\0';

	sscanf(buffer,"%s %s %s %s %s %s", indicadores[0], indicadores[1], indicadores[2], indicadores[3], indicadores[4], indicadores[5] );

	free(buffer);


    interpretacao (NET, ext,id,backup,interno, indicadores, IP, TCP_porto, regIp, regUdp, maxfd ,fds_counter, cojt_fds, names, aux_names, Lista_ficheiros, Head, args);

}

/*
Funcao interpretacao: analisa a primeira palavra recebida pelo o utilizador, atraves desta escolhe que funcao aplicar

*/

void interpretacao (char* NET , node_info** ext, char* id, node_info** backup, node_info** interno, char (*indicadores)[16], char* IP, char* TCP_porto, char* regIp, char* regUdp, int* maxfd, int* fds_counter, int* cojt_fds, char * names, int* aux_names, content **Lista_ficheiros, expedicao* Head, int args)
{
	static int join = 0;
	static int djoin = 0;
	char mensagem[128];
	node_info *aux;
	node_info *prev;
	content *Lista_aux;
	int counter = 0;
	content* AUX;
	expedicao  *root, *root2;
	int fd_query, i;
	char *buffer;

	int correto;
	int max_args;

	if(strcmp("join", indicadores[0]) == 0 )
	{
		correto = 1;
		max_args = 3;

		if(join == 0)
		{
			if(is_number(indicadores[1]) == 0)
				correto = 0;
			if(is_number(indicadores[2]) == 0)
				correto = 0;

			//Se se tiver verificado que a string é numérica, a mesma tem de respeitar algumas condições.
			if (atoi(indicadores[1]) < 0 || atoi(indicadores[1]) >= 1000 ||
			 	atoi(indicadores[2]) < 0 || atoi(indicadores[2]) >= 100 ||
			  	strlen(indicadores[1]) != 3 ||
			   	strlen(indicadores[2]) != 2)
				correto = 0;

			if(args != max_args)
				correto = 0;

			if(correto == 0)
				printf("Introduza um formato de mensagem correto...\n");
			else
			{
				join_node (NET, ext,id, backup, interno, indicadores, IP, TCP_porto,regIp, regUdp, maxfd, fds_counter, cojt_fds, Head);

				sprintf(mensagem, "REG %s %s %s %s", NET, id, IP, TCP_porto);

				buffer = Send_Udp_message(mensagem, regIp, regUdp);
				free(buffer);
				join = 1;
				djoin = 1;
			}
		}
		else
			printf("U already did a Join call\n");
	}
	else if(strcmp("djoin", indicadores[0]) == 0)
	{
		correto = 1;
		max_args = 6;

		if(djoin == 0)
		{
			if(is_number(indicadores[1]) == 0)
				correto = 0;
			if(is_number(indicadores[2]) == 0)
				correto = 0;
			if(is_number(indicadores[3]) == 0)
				correto = 0;
			if(is_number(indicadores[5]) == 0)
				correto = 0;

			// Se se tiver verificado que a string é numérica, a mesma tem de respeitar algumas condições.
			// O argumento do ip vejo a seguir.
			if (atoi(indicadores[1]) < 0 || atoi(indicadores[1]) >= 1000 ||
				atoi(indicadores[2]) < 0 || atoi(indicadores[2]) >= 100 ||
				atoi(indicadores[3]) < 0 || atoi(indicadores[3]) >= 100 ||
				atoi(indicadores[5]) < 0 || atoi(indicadores[5]) >= 65535 ||
				strlen(indicadores[1]) != 3||
				strlen(indicadores[2]) != 2 ||
				strlen(indicadores[3]) != 2 ||
				strlen(indicadores[5]) > 5)
				correto = 0;

			if(is_valid_ip(indicadores[4]) == 0)
				correto = 0;

			if(args != max_args)
				correto = 0;

			if(correto == 0)
				printf("Introduza um formato de mensagem correto...\n");
			else
			{
				djoin_node(NET, ext,id,backup,interno, indicadores, IP, TCP_porto,regIp, regUdp, maxfd, fds_counter, cojt_fds, Head);

				djoin = 1;
				join = 1;
			}
		}
		else
			printf("U already did a dJoin call\n");

	}
	else if(strcmp("create", indicadores[0]) == 0)
	{
		correto = 1;
		max_args = 2;
		
		if(args != max_args)
			correto = 0;

		if(correto == 0)
			printf("Introduza um formato de mensagem correto...\n");
		else
		{
			if ( strcmp((*Lista_ficheiros)->name, "-1") == 0 && (*Lista_ficheiros)->vazia == 1){
				strcpy((*Lista_ficheiros)->name, indicadores[1]);
				(*Lista_ficheiros)->vazia = 0;
			}
			else{
				Lista_aux = (*Lista_ficheiros);
				while(Lista_aux-> next != NULL)
					Lista_aux = Lista_aux->next;


				Lista_aux->next = (content*)malloc(sizeof(content));
				Lista_aux=Lista_aux->next;
				strcpy(Lista_aux->name , indicadores[1]);

				Lista_aux->next = NULL;

			}
		}
	}
	else if(strcmp("delete", indicadores[0]) == 0)
	{
		
		Lista_aux = (*Lista_ficheiros);
		int found = 0;
		correto = 1;
		max_args = 2;

		if(args != max_args)
			correto = 0;

		if(correto == 0)
			printf("Introduza um formato de mensagem correto...\n");
		else
		{
			if(strcmp((*Lista_ficheiros)->name, "-1") != 0 && (*Lista_ficheiros)->vazia == 0)
			{
				if (Lista_aux->next == NULL && (strcmp(Lista_aux->name, indicadores[1])==0))
				{
					strcpy (Lista_aux->name, "-1");
					(*Lista_ficheiros)->vazia = 1;
					found = 1;
				}

				else if(strcmp(Lista_aux->name, indicadores[1])==0){
					(*Lista_ficheiros) = (*Lista_ficheiros)->next;
					Lista_aux->next = NULL;
					free(Lista_aux);
					found = 1;
				}
				else
				{
					while (Lista_aux->next != NULL){
						if(strcmp(Lista_aux->next->name, indicadores[1])==0){
							found = 1;
							AUX = Lista_aux->next;

							Lista_aux->next = Lista_aux->next->next;
							AUX->next = NULL;
							free(AUX);
							break;

						}


						Lista_aux = Lista_aux->next;


					}
				}
			}
			
			if (found == 0)
				printf("Este documento não se encontra nos ficheiros do nó.\n");
		}

	}
	else if(strcmp("get", indicadores[0]) == 0)
	{
		correto = 1;
		max_args = 3;

		if(is_number(indicadores[1]) == 0)
			correto = 0;
		
		if(atoi(indicadores[1]) < 0 || atoi(indicadores[1]) >= 100 || strlen(indicadores[1]) != 2)
			correto = 0;
		
		if(max_args != args)
			correto = 0;

		if(correto == 0)
			printf("Introduza um formato de mensagem correto...\n");
		else
		{

			sprintf(mensagem, "QUERY %s %s %s\n", indicadores[1], id, indicadores[2]);

			for(root = (Head); root != NULL; root = root->next)
					if(strcmp(root->destino, indicadores[1]) == 0)
						break;
				if(root != NULL){ //destinatario encontra-se na tabela de expedicao

					//encontrar o vizinho que tem como destino na lista de expedicao o nó destinatario
					if( strcmp((*ext)->id, root-> vizinho) == 0)
						fd_query = (*ext)->fd;
					else{

						for(aux = (*interno); strcmp(aux->id, root-> vizinho) != 0; aux = aux->next){}
							fd_query = aux-> fd;
					}

					Send_Tcp_message(mensagem, fd_query);


				}else{ //destinatario nao se encontra na tabela, enviar mensagem para todos os vizinhos menos o que mandou a mensagem
					for(i=2 ; i < (*fds_counter); i++)
						Send_Tcp_message(mensagem, cojt_fds[i]);

				}
		}
	}
	else if((strcmp("show", indicadores[0]) == 0 && strcmp(indicadores[1], "topology") == 0) || (strcmp("st", indicadores[0]) == 0))
	{
		correto = 1;
		
		//Verificar de que forma o stor escreveu na linha de comandos
		if(strcmp("show", indicadores[0]) == 0)
		{
			max_args = 2;
			if(args != max_args)
				correto = 0;
		}
		else if(strcmp("st", indicadores[0]) == 0)
		{
			max_args = 1;
			if(args != max_args)
				correto = 0;
		}
		
		if(correto == 0)
			printf("Introduza um formato de mensagem correto...\n");
		else
		{
			printf("Dados do vizinho externo:\n");
			print_data(*ext);
			printf("Dados do vizinho de recuperacao:\n");
			print_data(*backup);
			if (strcmp((*interno)->id , "-1")!=0) {

				counter = 1;


				aux = (*interno); //Está a apontar para a estrutura do nó interno (primeiro)

				while(aux != NULL)
				{
					printf("Dados do vizinho interno %d:\n", counter);
					//Printar todos os ids que estáo na lista.
					print_data(aux);
					aux = aux->next;
					counter++;
				}


			}else{
				printf("Nao existem vizinhos internos\n");
			}
		}
	}
	else if((strcmp("show", indicadores[0]) == 0 && strcmp(indicadores[1], "names") == 0) || (strcmp("sn", indicadores[0]) == 0))
	{
		content* aux;
		correto = 1;
		
		//Verificar de que forma o stor escreveu na linha de comandos
		if(strcmp("show", indicadores[0]) == 0)
		{
			max_args = 2;
			if(args != max_args)
				correto = 0;
		}
		else if(strcmp("sn", indicadores[0]) == 0)
		{
			max_args = 1;
			if(args != max_args)
				correto = 0;
		}
		
		if(correto == 0)
			printf("Introduza um formato de mensagem correto...\n");
		else
		{
			if(strcmp((*Lista_ficheiros)->name, "-1") == 0 && (*Lista_ficheiros)->vazia == 1)
				printf("There is no apps in this node \n");
			else
				printf("We have the following information: \n");

			for(aux = (*Lista_ficheiros); aux != NULL; aux = aux->next)
			{
				if((strcmp(aux->name, "-1") == 0) && aux->vazia == 1)
					continue;

				printf("%s ", aux->name);
			}

			printf("\n");
		}
	}
	else if((strcmp("show", indicadores[0]) == 0 && strcmp(indicadores[1], "routing") == 0) || (strcmp("sr", indicadores[0]) == 0))
	{
		correto = 1;
		
		//Verificar de que forma o stor escreveu na linha de comandos
		if(strcmp("show", indicadores[0]) == 0)
		{
			max_args = 2;
			if(args != max_args)
				correto = 0;
		}
		else if(strcmp("sr", indicadores[0]) == 0)
		{
			max_args = 1;
			if(args != max_args)
				correto = 0;
		}
		
		if(correto == 0)
			printf("Introduza um formato de mensagem correto...\n");
		else
		{
			if(strcmp(Head->destino, " ") != 0 && strcmp(Head->vizinho, " ") != 0)
			{
				printf("The expedition table of the node with the id(%s) is: \n", id);
				for(root = Head; root!=NULL; root = root->next)
				{
					printf("Vizinho -> %s  &  Destino -> %s \n", root->vizinho, root->destino);
				}
			}
			else
				printf("A tabela está, de momento, vazia.\n");
		}	
			
	}
	else if(strcmp("leave", indicadores[0]) == 0)
	{
		correto = 1;
		max_args = 1;

		if(args != max_args)
			correto = 0;

		if(correto == 0)
			printf("Introduza um formato de mensagem correto...\n");
		else
		{
			leave_network (NET, id, regIp,regUdp, fds_counter, cojt_fds);
			//apagar dados internos, tabela expeducao
			prev = (*interno);
			aux = prev->next;
			prev->next = NULL;
			strcpy(prev->id , "-1");

			while(aux!= NULL){
				prev = aux;
				aux =  aux->next;
				prev->next = NULL;
				free(prev);
			}

			root = (Head);
			root2 = root->next;
			root->next = NULL;
			strcpy(root->vizinho , " ");
			strcpy(root->destino , " ");
			while(root2!= NULL){
				root = root2;
				root2 =  root2->next;
				root->next = NULL;
				free(root);
			}

			join = 0;
			djoin = 0;
		}
	}
	else if(strcmp("exit", indicadores[0]) == 0)
	{
		correto = 1;
		max_args = 1;

		if(args != max_args)
			correto = 0;

		if(correto == 0)
			printf("Introduza um formato de mensagem correto...\n");
		else
		{
			free(*ext);
			free(*interno);
			free(*backup);
			free(Head);

			for(Lista_aux = (*Lista_ficheiros); Lista_aux != NULL; Lista_aux = (*Lista_ficheiros) ){
				(*Lista_ficheiros) = (*Lista_ficheiros)->next;
				Lista_aux->next = NULL;
				free(Lista_aux);
			}
			close(cojt_fds[1]);
			exit(1);
		}
	}
	else if((strcmp("clear", indicadores[0]) == 0 && strcmp(indicadores[1], "routing") == 0) || (strcmp("cr", indicadores[0]) == 0))
	{
		correto = 1;

		if(strcmp("clear", indicadores[0]) == 0)
		{
			max_args = 2;
			if(args != max_args)
				correto = 0;
		}
		else if(strcmp("cr", indicadores[0]) == 0)
		{
			max_args = 1;
			if(args != max_args)
				correto = 0;
		}


		if(correto == 0)
			printf("Introduza um formato de mensagem correto...\n");
		else
		{
			root = (Head)->next;
			if(strcmp((Head)->destino, " ") != 0 && strcmp((Head)->vizinho, " ") != 0)
			{
				Head->next = NULL;
				printf("A tabela de expedição do nó %s foi limpa \n", id);

				//Damos free() à tabela de expedição inteira.
				deleteList(root);

				//A Tabela quando está vazia fica desta forma.
				
				strcpy((Head)->destino, " ");
				strcpy((Head)->vizinho, " ");
				

			}
			else
				printf("Neste momento não existe nenhuma tabela de expedição criada com este id (%s).\n", id);
		}
	}
	else
		printf("Introduza um formato de mensagem correto...\n");
}



/*
Funcao djoin_node: utiliza os parametros provenientes do User ou da funcao join para estabelecer a ligacao entre o nosso No e outro No da rede

*/

int djoin_node(char* NET , node_info** ext, char* id, node_info** backup, node_info** interno, char (*indicadores)[16], char* IP, char* TCP_porto, char* regIp, char* regUdp, int* maxfd, int* fds_counter, int* cojt_fds, expedicao* Head){

	char mensagem[128];

	strcpy(NET, indicadores[1]);

	strcpy((*ext)->id , indicadores [3]);
	strcpy((*ext)->ip , indicadores [4]);
	strcpy((*ext)->port , indicadores [5]);

	strcpy(id , indicadores [2]);

	strcpy((*backup)->id , indicadores [2]);
	strcpy((*backup)->ip , IP);
	strcpy((*backup)->port , TCP_porto);


	if (strcmp(indicadores[2], indicadores [3]) != 0){ //Acho q tem de ser strcmp

		cojt_fds[(*fds_counter)] = open_tcp_socket((*ext)->ip, (*ext)->port);
		if(cojt_fds[(*fds_counter)] == -1)
		 	return -1;

		*maxfd = max(*maxfd ,cojt_fds[(*fds_counter)]);
		(*ext)->fd = cojt_fds[(*fds_counter)];
		(*fds_counter)++;
		sprintf(mensagem, "NEW %s %s %s\n", id, IP, TCP_porto);
		Send_Tcp_message (mensagem, cojt_fds[(*fds_counter)-1]);
		add_routing( Head, indicadores[3], indicadores[3],id);
	}


	return 0;

}


/*
Funcao join_node: utiliza os parametros provenientes do User para estabelecer a ligacao entre o nosso No e outro da rede com recurso ao servidor de NOS

*/

void join_node(char* NET , node_info** ext, char* id, node_info** backup, node_info** interno, char (*indicadores)[16], char* IP, char* TCP_porto, char* regIp, char* regUdp, int* maxfd, int* fds_counter, int* cojt_fds, expedicao* Head){

	char mensagem[40];
	char* delim_lines = "\n";
	char* buffer;
	char buffer_aux[257];
	char *ptr; //Tive q tirar o ptr[60]...pq em C n podemos fazer ptr[60] = strtok ...
	int new[100] = {0}; //ou chars é igual... Temos é de em cada nó já ocupado neste vetor colocar a 1 p.e na posição do mesmo
	int unavailable[100]= {0};
	int i, id_chooser,  counter, n;
	int val;
	char aux_leitura[4];

	fd_set rfds;
	struct timeval timeout;

	timeout.tv_sec = 2;
	timeout.tv_usec = 0;

	sprintf(mensagem, "NODES %s", indicadores[1]);

	buffer = Send_Udp_message(mensagem ,regIp , regUdp);
	strcpy(buffer_aux, buffer);

	ptr = strtok(buffer_aux, delim_lines);
	ptr = strtok(NULL, delim_lines);

	counter = 0;

	srand( (unsigned) time(NULL) );

	//saltar 1ª linha linha;

	while(ptr != NULL)
	{
		//Se o ptr tiver a apontar para o 1º id (03 na imagem do projeto)
		sscanf(ptr,"%s", aux_leitura);
		val = atoi(aux_leitura);
		new[val] = 1;

		//avançar para o próximo id;
		ptr = strtok(NULL, delim_lines);

		counter++;
	}
	if(counter == 0){
		//Primeiro Nó da rede (indicadores[2] = indicadores[3] = id)
		strcpy(indicadores[3], indicadores[2]);
		strcpy(indicadores[4], IP);
		strcpy(indicadores[5], TCP_porto);
		djoin_node(NET, ext , id , backup , interno , indicadores, IP, TCP_porto,regIp, regUdp, maxfd , fds_counter, cojt_fds, Head);

	}else {
		if(new[atoi(indicadores[2])] == 1)
		{
			for(i = 0; i<100; i++)
			{

				if( new[i] == 0) // Assumindo que as posições já preenchidas têm p.e valor 1...
				{
					//Percorrer a linha do nó
					if(i>9)
						sprintf(indicadores[2], "%d", i); //Já encontrámos um novo id para se colocar na rede.
					else
						sprintf(indicadores[2], "0%d", i);

					//Se encontrarmos uma posição sem ter antes de percorrer o vetor todo , break.

					break;
				}

			}
		}
		//escolher um nó da lista a quem nos vamos ligar

		i = 0;
		while(i<counter){

			strcpy(buffer_aux, buffer);
			id_chooser = rand ()% (counter);

			while(unavailable[id_chooser] != 0)
		    	id_chooser = rand ()% (counter);


			ptr = strtok(buffer_aux, delim_lines);
			ptr = strtok(NULL, delim_lines);

			for(i=0; i < id_chooser; i++){
				ptr = strtok(NULL, delim_lines);
			}


			sscanf(ptr,"%s %s %s" ,indicadores[3] , indicadores[4] , indicadores[5] );

			if (djoin_node(NET, ext , id , backup , interno , indicadores, IP, TCP_porto,regIp, regUdp, maxfd , fds_counter, cojt_fds , Head)==-1){
				unavailable[id_chooser] = 1 ;
				i++;
			}else{
				FD_ZERO(&rfds);
				FD_SET(cojt_fds[(*fds_counter)-1], &rfds);
				select(cojt_fds[(*fds_counter)-1]+1,&rfds,(fd_set*)NULL,(fd_set*)NULL, &timeout);


				if(FD_ISSET(cojt_fds[(*fds_counter)-1],&rfds)){

					free(buffer);

					n = read (cojt_fds[(*fds_counter)-1], buffer, 128);

					if(n==-1)
					/*error*/exit(1);
					buffer[n] = '\0';

					//printf("li : %s\n", buffer);

					sscanf(buffer,"%s %s %s %s" , aux_leitura , (*backup)-> id , (*backup)-> ip , (*backup)-> port );



					i = counter+1;


				}else{
					unavailable[id_chooser] = 1 ;
					remove_routing(&Head, indicadores[3]);
					i++;
					close(cojt_fds[(*fds_counter)-1]);
					(*fds_counter)--;
				}
			}
		}
		
		if(counter == i){
			//Primeiro Nó da rede (indicadores[2] = indicadores[3] = id)
			strcpy(indicadores[3], indicadores[2]);
			strcpy(indicadores[4], IP);
			strcpy(indicadores[5], TCP_porto);
			djoin_node(NET, ext , id , backup , interno , indicadores, IP, TCP_porto,regIp, regUdp, maxfd , fds_counter, cojt_fds, Head);
		}
	}

	free(buffer);
	return;

}


/*
Funcao open_tcp_socket: utiliza os dados do utilizador com que se quer estabelecer uma ligacao (Ip e Porto) para estabelecer a mesma, retornando o FD respetivo dessa ligacao, que sera guardado no vetor cojt_fds[]
*/
int open_tcp_socket (char* IP_send_to, char* Port_send_to){

	struct addrinfo hints,*res;
	int fd,n;

	fd=socket(AF_INET,SOCK_STREAM,0);//TCP socket
	if(fd==-1)
		return fd;

	memset(&hints,0,sizeof hints);

	hints.ai_family=AF_INET;//IPv4
	hints.ai_socktype=SOCK_STREAM;//TCP socket

	n =getaddrinfo(IP_send_to,Port_send_to,&hints,&res); //algo me diz que devia ser errcode em vez de n
	if(n!=0)
	{
		fprintf(stderr, "getaddrinfo error: %s\n", gai_strerror(n));
		/*error*/exit(1);
	}
	n=connect(fd,res->ai_addr,res->ai_addrlen);
	if(n==-1)
	{
		perror("connect");
		/*error*/exit(1);
	}

	/* Connected to the node*/
	freeaddrinfo (res);
	return fd;
}



/*
Funcao Send_Tcp_message: envia uma mensagem atraves do protocolo Tcp para outro No. Recebe como parametros a mensagem a ser enviada e o FD do No vizinho a quem se quer enviar mensagem

*/

void Send_Tcp_message (char* mensagem, int fd)
{

	int n;

	n = write (fd, mensagem, strlen(mensagem)); //Vai ter de ficar dentro de um while tbm
	if(n==-1)
	{
		perror("write");
		/*error*/exit(1);
	}

	//printf("escrevi : %s\n", mensagem);

}



/*
Funcao Send_Udp_message: envia uma mensagem atraves do protocolo UDP para o servidor de NOS. 
Recebe  como parametros a mensagem a enviar e os dados do servidor de NOS, necessarios para abrir o socket, que sera fechado no fim da funcao

*/
char* Send_Udp_message(char* mensagem, char* reg_ip, char* reg_udp)
{
	struct addrinfo hints,*res;
	struct sockaddr addr;
	socklen_t addrlen;
	int fd,errcode;
	ssize_t n;

	char* buffer = (char*) malloc (sizeof (char) * 256);
	   if (buffer == NULL){
       printf("Memory allocation error.");
       exit(1);
       }

	int verifier = 0;

	fd_set rfds;
	struct timeval timeout;

	timeout.tv_sec = 2;
	timeout.tv_usec = 0;

	fd = socket(AF_INET,SOCK_DGRAM,0);//UDP socket

	if(fd == -1)
	{
		perror("socket");
		/*error*/exit(1);
	}

	memset(&hints,0,sizeof hints);
	hints.ai_family=AF_INET;//IPv4
	hints.ai_socktype=SOCK_DGRAM;//UDP socket

	errcode = getaddrinfo(reg_ip , reg_udp ,&hints , &res);

	if(errcode!=0)
	{
		fprintf(stderr, "getaddrinfo error: %s\n", gai_strerror(errcode));
		/*error*/exit(1);	
	}

	while(verifier != 1 ){

		n = sendto(fd,mensagem,strlen(mensagem),0,res->ai_addr,res->ai_addrlen); //Aqui passa a ser a mensagem após os strcats...
		if(n == -1)
		{
			perror("send");
			/*error*/exit(1);
		}

		FD_ZERO(&rfds);
		FD_SET(fd, &rfds);
		select(fd+1,&rfds,(fd_set*)NULL,(fd_set*)NULL, &timeout);


		if(FD_ISSET(fd,&rfds)){
			addrlen = sizeof(addr);
			n = recvfrom(fd,buffer,256,0,&addr,&addrlen);
			if(n == -1)
			{
				perror("recv");
				/*error*/exit(1);
			}

			buffer[n] = '\0';
			verifier = 1;
		}

	}

	//printf("%s\n", buffer);

	freeaddrinfo(res);
	close(fd);
	return buffer;

}




/*
Funcao leave_network: tem como funcao fechar todos os FDs correspondentes a comunicacoes com outros NOS, "limpar" o vetor utilizado para guardar FDs 
e retirar os dados do No do servidor de NOS.

*/
void leave_network (char* NET , char* id , char* reg_ip, char* reg_udp, int* fds_counter, int* conjunto_fds)
{
	char mensagem[40];
	char *buffer;

	sprintf(mensagem, "UNREG %s %s", NET, id);

	buffer = Send_Udp_message(mensagem,reg_ip , reg_udp);

	//Recebemos "OKUNREG"

	//fechar as sessoes tcp com outros nos

	int i; // i = 0 e i = 1 não é para fechar.
	for (i = 2; i < (*fds_counter); i++)
	{
		close(conjunto_fds[i]); //Close a todos os fds
	}
	(*fds_counter) = 2;

	free(buffer);

}

/*
Função ler_No: Recebe a mensagem de um ouyto nó, interpreta a mensagem. Se necessário fôr, espalha também a mensagem para os seus vizinhos.

*/


void ler_No (int fd, node_info** ext, char* id, node_info** backup, node_info** interno, char* IP, char* TCP_porto, char* regIp, char* regUdp, int* maxfd, int* fds_counter, int* cojt_fds, expedicao** Head, content** Lista_ficheiros){

	/*inicializar todas as variáveis, até os pointers a NULL*/

	char buffer[129];
	char Mensagem_Tcp [4][16];
	char id_emissor[3];
	int counter = 0;
	node_info *aux, *aux2;
	char mensagem[129] = "\0";
	int i,j, fd_query;
	expedicao* roots = (*Head);
	content* file = (*Lista_ficheiros);
	int n[5] , n_aux = 1;
	int completo = 0;
	char final[5][129];
	char final_aux[40];
	char *position_ptr ;
	char *buffer_aux;
	int position;
	int position_lixo;
	
    for(i = 0 ; i < 5; i++)
       strcpy(final[i],"\0");
   
	//Tratamento de mensagens enviadas juntas / concatenadas ou mal formatadas
	while(completo!= 1){
	   
	   
        if ((n_aux = read (fd, buffer, 128)) == 0){
			n[counter] = 0;
			counter++;
			break;
		}
		buffer[n_aux] = '\0';
		

        
        position = 0;
		position_lixo =0;
        position_ptr = strchr(buffer, '\n');
        buffer_aux = buffer;
        while(position_ptr != NULL){
            position  = position_ptr - buffer_aux;
            buffer_aux[position] = '\0';
			n[counter] = 1;
			strcpy(final_aux, final[counter]);
            sprintf(final[counter],"%s%s", final_aux ,buffer_aux);
            
            counter++;
            if((position + position_lixo) == n_aux-1)
                completo = 1;
            else{
                buffer_aux = &buffer_aux[position+1];
				position_lixo = position_lixo + position +1;
			}
            position_ptr = strchr(buffer_aux, '\n');
        }
        if(completo != 1){
			strcpy(final_aux, final[counter]);
            sprintf(final[counter],"%s%s", final_aux ,buffer_aux);
		}	
			
    }

	//Se n[j] = 0 , um nó vizinho saiu da rede -> id_emissor.
	for( j = 0; j < counter; j++ ){
		if(n[j]==-1)
			/*error*/exit(1);
		else if (n[j]==0){

			//descobrir o id de quem nos mandou mensagem atraves dos nossos dados guardados

			//Se um vizinho nos tiver como externo.
			if((*ext)->fd == fd)
				strcpy(id_emissor, (*ext)->id);
			else{
				//Se um vizinho nos tiver como interno.
				for(aux = (*interno); aux->fd != fd ; aux = aux->next){}
					strcpy(id_emissor, aux->id);
			}



			remove_fd(fd , cojt_fds, fds_counter);//para já so temos de remover o fd "parasita" mais tarde, entrar para a funcao de manutenção
			sprintf(mensagem , "WITHDRAW %s\n", id_emissor);
			remove_routing(Head , id_emissor);

			for(i=2 ; i < (*fds_counter); i++)
				Send_Tcp_message(mensagem, cojt_fds[i]);


										/*Manutenção da topologia*/

			//Só se termina ligações com o vizinho externo ou com o vizinho interno

			if((*ext)->fd == fd)  // O que saiu é meu externo.
			{

				if(strcmp((*backup)->id, id) == 0) //Tu es ancora
				{

					//Tem vizinho interno (É âncora)

					aux = (*interno);


					//Vamos escolher um dos nossos internos (o primeiro que aparecer.)
					if(strcmp(aux->id,"-1")!=0) //Se a lista dos internos nao estiver vazia...
					{
						strcpy((*ext)->id, aux->id); //O nosso externo modificou.
						strcpy((*ext)->ip, aux->ip);
						strcpy((*ext)->port, aux->port);
						(*ext)->fd = aux->fd;
						//Agr o aux->id deixa de pertencer aos nossos internos já que o mesmo é externo.

						for(aux = (*interno); aux!=NULL; aux = aux->next)
						{

							//Envia Extern aos seus vizinhos internos todos c exceção de um
							sprintf(mensagem, "EXTERN %s %s %s\n", (*ext)->id, (*ext)->ip, (*ext)->port);
							Send_Tcp_message(mensagem, aux->fd);
						}

						//Remover o aux->id dos internos
						aux = (*interno);
						if((*interno)->next !=NULL){
							(*interno) = (*interno)->next;
							aux->next = NULL;
							free(aux);
						}
						else
							strcpy((*interno)->id, "-1");

					}
					else //Nó ficou sozinho.
					{
						strcpy((*ext)->id, id);
						strcpy((*ext)->ip, IP);
						strcpy((*ext)->port, TCP_porto);
					}
				}
				else if(strcmp((*backup)->id, id) != 0) //tu nao es ancora
				{
					add_routing( (*Head), (*backup)->id, (*backup)->id, id);

					strcpy((*ext)->id, (*backup)->id);
					strcpy((*ext)->ip, (*backup)->ip);
					strcpy((*ext)->port, (*backup)->port) ;

					cojt_fds[(*fds_counter)] = open_tcp_socket((*ext)->ip, (*ext)->port);
					*maxfd = max(*maxfd ,cojt_fds[(*fds_counter)]);
					(*ext)->fd = cojt_fds[(*fds_counter)];
					(*fds_counter)++;
					sprintf(mensagem, "NEW %s %s %s\n", id, IP, TCP_porto);
					Send_Tcp_message (mensagem, cojt_fds[(*fds_counter)-1]);



					for(aux = (*interno); aux!=NULL; aux = aux->next)
					{
						if(strcmp(aux->id,"-1")!=0){
							sprintf(mensagem, "EXTERN %s %s %s\n", (*ext)->id, (*ext)->ip, (*ext)->port);
							Send_Tcp_message(mensagem, aux->fd);
						}
					}

				}

			}
			else{
				if((*interno)->fd == fd){

					aux = (*interno);
					if((*interno)->next !=NULL){
						(*interno) = (*interno)->next;
						aux->next = NULL;
						free(aux);
					}
					else
						strcpy((*interno)->id, "-1");
				}
				else{
					for(aux = (*interno); aux->next->fd != fd; aux = aux->next){}


					aux2 = aux->next;
					aux->next = aux->next->next;
					aux2->next = NULL;
					free(aux2);
					// Remover o nó.
				}


			}
			return;
		}


		//printf("li : %s\n", final[j]);

		sscanf(final[j],"%s %s %s %s", Mensagem_Tcp[0] , Mensagem_Tcp[1] , Mensagem_Tcp[2] , Mensagem_Tcp[3]);


		if(strcmp(Mensagem_Tcp[0],"NEW") == 0){  // NEW id IP TCP<LF> (dois casos possiveis ou es ancora sozinho, ou nao)

			add_routing( *Head , Mensagem_Tcp[1], Mensagem_Tcp[1],id);



			if(strcmp((*ext)->id,id)==0){

				strcpy((*ext)->id , Mensagem_Tcp[1]);
				strcpy((*ext)->ip , Mensagem_Tcp[2]);
				strcpy((*ext)->port , Mensagem_Tcp[3]);
				(*ext)->fd = fd;



			}else{
					if(strcmp((*interno)->id, "-1")== 0){  //primeiro interno a dar entrada
						strcpy((*interno)->id , Mensagem_Tcp[1]);
						strcpy((*interno)->ip , Mensagem_Tcp[2]);
						strcpy((*interno)->port , Mensagem_Tcp[3]);
						(*interno)->fd = fd;



							//A mensagem terá de terminar com \n  -> <LF>

					}else{

						aux = (*interno);
						while(aux->next != NULL)
							aux = aux->next;
						aux->next = (node_info*) malloc  (sizeof(node_info));

						aux = aux->next;

						strcpy( aux->id , Mensagem_Tcp[1]);
						strcpy(aux->ip , Mensagem_Tcp[2]);
						strcpy(aux->port , Mensagem_Tcp[3]);
						aux->fd = fd;
						aux->next = NULL;




					}
			}


			sprintf(mensagem, "EXTERN %s %s %s\n", (*ext)->id, (*ext)->ip, (*ext)->port);
			Send_Tcp_message( mensagem, fd);
			return;
		}
		else if(strcmp(Mensagem_Tcp[0],"EXTERN") == 0){

			//O backup do nó que recebe a mensagem altera.
			strcpy((*backup)->id, Mensagem_Tcp[1]);
			strcpy((*backup)->ip, Mensagem_Tcp[2]);
			strcpy((*backup)->port, Mensagem_Tcp[3]);

			add_routing(*Head , (*ext)->id, (*backup)->id, id);

		}
		else if(strcmp(Mensagem_Tcp[0],"WITHDRAW") == 0){

			remove_routing (Head , Mensagem_Tcp[1]);
			//Enviar a mensagem aos vizinhos.

			sprintf(mensagem, "%s %s\n", Mensagem_Tcp[0], Mensagem_Tcp[1]);

			//Mensagem é enviada a todos os seus vizinhos à exceção de quem lhe enviou a mensagem
			for(i=2 ; i < (*fds_counter); i++)
				if(fd != cojt_fds[i]){

					
					Send_Tcp_message(mensagem, cojt_fds[i]);
				}
		}
		else if(strcmp(Mensagem_Tcp[0],"QUERY") == 0)
		{
			if((*ext)->fd == fd)
				strcpy(id_emissor, (*ext)->id);
			else{

				for(aux = (*interno); aux->fd != fd ; aux = aux->next){}
				strcpy(id_emissor, aux->id);
			}
			//Adicionar a lista de expedicao o caminho entre o vizinho e no de origem da mensagem Mensagem_Tcp[2]
			add_routing(*Head, id_emissor, Mensagem_Tcp[2], id);

			if(strcmp(id, Mensagem_Tcp[1]) == 0 ) //...
			{
				
				//Percorremos a lista que contém os conteúdos.
				for(file = (*Lista_ficheiros); file!= NULL ; file = file->next)
					if(strcmp(file->name,  Mensagem_Tcp[3]) == 0)
						break;

				if(file != NULL)
					{
						sprintf(mensagem, "CONTENT %s %s %s\n", Mensagem_Tcp[2],Mensagem_Tcp[1] , Mensagem_Tcp[3]);
						Send_Tcp_message(mensagem, fd);


					}
				else
					{
						sprintf(mensagem, "NOCONTENT %s %s %s\n", Mensagem_Tcp[2], Mensagem_Tcp[1], Mensagem_Tcp[3]);
						Send_Tcp_message(mensagem,fd) ;
					}


			}

			else
			{

				for(roots = (*Head); roots != NULL; roots = roots->next)
					if(strcmp(roots->destino, Mensagem_Tcp[1]) == 0)
						break;
				if(roots != NULL){ //destinatario encontra-se na tabela de expedicao

					//encontrar o vizinho que tem como destino na lista de expedicao o nó destinatario
					if( strcmp((*ext)->id, roots-> vizinho) == 0)
						fd_query = (*ext)->fd;
					else{

						for(aux = (*interno); strcmp(aux->id, roots-> vizinho) != 0; aux = aux->next){}
							fd_query = aux-> fd;
					}
					if(fd != fd_query){
						sprintf(mensagem, "%s %s %s %s\n", Mensagem_Tcp[0], Mensagem_Tcp[1], Mensagem_Tcp[2], Mensagem_Tcp[3]);
						Send_Tcp_message(mensagem, fd_query);
					}
					


				}else{ //destinatario nao se encontra na tabela, enviar mensagem para todos os vizinhos menos o que mandou a mensagem
					
					sprintf(mensagem, "%s %s %s %s\n", Mensagem_Tcp[0], Mensagem_Tcp[1], Mensagem_Tcp[2], Mensagem_Tcp[3]);

					for(i=2 ; i < (*fds_counter); i++)
						if(fd != cojt_fds[i]){

							
							Send_Tcp_message(mensagem, cojt_fds[i]);

						}
							
				}
			}

		}
		else if(strcmp(Mensagem_Tcp[0],"CONTENT") == 0){

			if((*ext)->fd == fd)
				strcpy(id_emissor, (*ext)->id);
			else{

			for(aux = (*interno); aux->fd != fd ; aux = aux->next){}
			strcpy(id_emissor, aux->id);
			}

			// adicionar a expedicao o id do fd que enviou e o id de origem Mensagem_Tcp[2]
			add_routing(*Head, id_emissor, Mensagem_Tcp[2], id);



			if(strcmp(Mensagem_Tcp[1], id ) == 0) //sou o no destinatario da mensagem

				printf("Ficheiro encontrado \n Conteudo : %s\n", Mensagem_Tcp[3]);

			else{

				for(roots = (*Head); roots != NULL; roots = roots->next)
					if(strcmp(roots->destino, Mensagem_Tcp[1]) == 0)
						break;
				if(roots != NULL){ //destinatario encontra-se na tabela de expedicao

					//encontrar o vizinho que tem como destino na lista de expedicao o nó destinatario
					if( strcmp((*ext)->id, roots-> vizinho) == 0)
						fd_query = (*ext)->fd;
					else{

						for(aux = (*interno); strcmp(aux->id, roots-> vizinho) != 0; aux = aux->next){}
							fd_query = aux-> fd;
					}

					if(fd != fd_query){
						sprintf(mensagem, "%s %s %s %s\n", Mensagem_Tcp[0], Mensagem_Tcp[1], Mensagem_Tcp[2], Mensagem_Tcp[3]);
						Send_Tcp_message(mensagem, fd_query);
					}
					


				}else{ //destinatario nao se encontra na tabela, enviar mensagem para todos os vizinhos menos o que mandou a mensagem
					
					sprintf(mensagem, "%s %s %s %s\n", Mensagem_Tcp[0], Mensagem_Tcp[1], Mensagem_Tcp[2], Mensagem_Tcp[3]);
					
					
					for(i=2 ; i < (*fds_counter); i++)
						if(fd != cojt_fds[i]){

						
						Send_Tcp_message(mensagem, fd_query);
						}
							

				}
			}
		}
		else if(strcmp(Mensagem_Tcp[0],"NOCONTENT") == 0){

			if((*ext)->fd == fd)
				strcpy(id_emissor, (*ext)->id);
			else{

				for(aux = (*interno); aux->fd != fd ; aux = aux->next){} //Quando aux->fd == fd e desocbriu-se o id que queríamos.
				strcpy(id_emissor, aux->id);
			}

			// adicionar a expedicao o id do fd que enviou e o id de origem Mensagem_Tcp[2]
			add_routing(*Head, id_emissor, Mensagem_Tcp[2], id);

			if(strcmp(Mensagem_Tcp[1], id ) == 0) //sou o no destinatario da mensagem

				printf("Ficheiro não encontrado \n");

			else{

				for(roots = (*Head); roots != NULL; roots = roots->next)
					if(strcmp(roots->destino, Mensagem_Tcp[1]) == 0)
						break;
				if(roots != NULL){ //destinatario encontra-se na tabela de expedicao

					//encontrar o vizinho que tem como destino na lista de expedicao o nó destinatario
					if( strcmp((*ext)->id, roots-> vizinho) == 0)
						fd_query = (*ext)->fd;
					else{

						for(aux = (*interno); strcmp(aux->id, roots-> vizinho) != 0; aux = aux->next){}
							fd_query = aux-> fd;
					}
					if(fd != fd_query){
						sprintf(mensagem, "%s %s %s %s\n", Mensagem_Tcp[0], Mensagem_Tcp[1], Mensagem_Tcp[2], Mensagem_Tcp[3]);
						Send_Tcp_message(mensagem, fd_query);
					}

				}else{ //destinatario nao se encontra na tabela, enviar mensagem para todos os vizinhos menos o que mandou a mensagem
					
					sprintf(mensagem, "%s %s %s %s\n", Mensagem_Tcp[0], Mensagem_Tcp[1], Mensagem_Tcp[2], Mensagem_Tcp[3]);
					
					for(i=2 ; i < (*fds_counter); i++)
						if(fd != cojt_fds[i])
							Send_Tcp_message(mensagem, cojt_fds[i]);

				}
			}


		}
		else{}//que mensagem é que aparece se um nó fechar a ligacao fd que tem comigo?
	}
}

/*
função remove_fd: Ao termos read = 0 na função ler_no invocamos esta função para removermos o fd em causa no vizinho a quem enviamos a mensagem, 
	porque o fd tem de ser removido dos dois lados. Para um bom funcionamento do nosso vetor de fds (cojt_fds[102]) 
	manuseamos o mesmo aqui também para o preparar para a saída de um fd.
*/
void remove_fd(int fd, int* cojt_fds, int* fds_counter){

	int i = 0;
	int j;

	while(fd != cojt_fds[i])
		i++;
	if(i != (*fds_counter)-1)
			for(j = i+1; j != (*fds_counter); i++, j++)
				cojt_fds[i] = cojt_fds[j];

	close(fd);
	(*fds_counter)--;

	return;

}

/*
função print_data: Informação sobre os nós vizinhos (interno, externo e backup).
*/
void print_data(node_info* Info){

	printf("ID : %s\n", Info->id);
	printf("IP : %s\n", Info->ip);
	printf("Porto : %s\n", Info->port);

	return;
}

/*
Função deleteList: Função usada especificamente para apagar todos os elementos da tabela de expedição.
*/
void deleteList(expedicao *head){
   expedicao *curr = head;
   expedicao *next;

   while (curr != NULL)
   {
      next = curr->next;
	  curr->next = NULL;
      free(curr);
      curr = next;
   }
}

/*
Função add_routing: Adicionamos elementos à tabela de expedição.
*/
void add_routing ( expedicao *Head, char* vizinho, char* destino, char* id){

	if(strcmp(vizinho,id)!= 0 && strcmp(destino,id)!= 0){
		expedicao *aux, *prev;
		if(strcmp(Head->destino, " ") == 0 || strcmp(Head->vizinho, " ") == 0){
			strcpy(Head->destino, destino);
			strcpy(Head->vizinho, vizinho);
		}
		else{
			for (aux = Head;aux != NULL; aux = aux->next){
				if(strcmp(aux->destino, destino) == 0 && strcmp(aux->vizinho, vizinho) == 0) //Não queremos adicionar duas vezs o mesmo id...
					break;
				prev = aux;
			}
			if(aux == NULL){
				prev->next = (expedicao*) malloc (sizeof(expedicao));
				prev = prev->next;
				strcpy(prev->destino, destino);
				strcpy(prev->vizinho, vizinho);
				prev -> next = NULL;
			}
		}
	}
}

/*
Função remove_routing: Removemos elementos à tabela de expedição.
*/
void remove_routing ( expedicao **Head, char* id){

	expedicao *aux, *aux2;

		for (aux = (*Head);	aux->next != NULL; ){
			if(strcmp(aux->next->destino, id) == 0 || strcmp(aux->next->vizinho, id) == 0){
				aux2 = aux->next;
				aux->next = aux->next->next;
				aux2->next = NULL;
				free(aux2);

			}
			else
				aux = aux->next;
		}

	if((strcmp((*Head)->destino, id) == 0 || strcmp((*Head)->vizinho, id) == 0) && (*Head)->next == NULL){
		//Quando está vazia , fica desta forma...
		strcpy((*Head)->destino, " ");
		strcpy((*Head)->vizinho, " ");
	}
	else if(strcmp((*Head)->destino, id) == 0 || strcmp((*Head)->vizinho, id) == 0){ //Head->next !=NULL (Sabendo já à partida que só faltava verificar a cabeça, pelo que heas->next é um id que já foi verificado)
		aux = (*Head);
		(*Head) = (*Head)->next;
		aux->next = NULL;
		free(aux);
	}

}

/*
Função is_number: Usado para verificações sobre os inputs que o user escreve na linha de comandos. 
	
	(p.e: O nosso número do grupo: 091. Impedimos que o user faça algo do género "olá").
*/
int is_number(char* str)
{
	int correto = 1;
	int i;
	for (i = 0; str[i] != '\0'; i++)
	{
      	if (!isdigit(str[i])) // verifica se o caractere não é um dígito numérico
		{
			correto = 0;
			break;
		}
   	}

	return correto;
}

/*
Função is_valid_ip: Função usada para verificar quando o user escreve na consola o conando djoin , se um ip é ou não um ip válido.
*/
int is_valid_ip(char *ip_str) {
    int num, dots = 0;
    char *ptr;
	char aux[16];
	strcpy(aux, ip_str);

    if (ip_str == NULL) {
        return 0;
    }

    ptr = strtok(aux, ".");

    if (ptr == NULL) {
        return 0;
    }

    while (ptr) {
        if (!isdigit(*ptr)) {
            return 0;
        }

        num = atoi(ptr);

        if (num < 0 || num > 255) {
            return 0;
        }

        ptr = strtok(NULL, ".");

        if (ptr != NULL) {
            ++dots;
        }
    }

    if (dots != 3) {
        return 0;
    }

    return 1;
}

/*
Função count_strings: Conta o número de strings invocadas na linha de comandos pelo user, 
para não permitir que o mesmo escreva mais argumentos do que os necessários ao invocar
o comnando join.

	(p.e: join 091 02 -> args = 3)
*/
int count_strings(char *line)
{
    int count = 0;
    int in_word = 0;
    char *ptr = line;

	while (*ptr != '\0' && *ptr != '\n')
	{
        if (*ptr != ' ' && !in_word) {
            count++;
            in_word = 1;
        } else if (*ptr == ' ') {
            in_word = 0;
        }
        ptr++;
    }

    return count;
}