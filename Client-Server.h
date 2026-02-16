#ifndef Proj_INCLUDED
#define Proj_INCLUDED

#ifdef _WIN32
//For Windows
#include <winsock2.h>
#include <ws2tcpip.h>
#include <iphlpapi.h>
#include <ws2def.h>
#pragma comment(lib, "Ws2_32.lib")
#include <windows.h>
#include <io.h>
#else
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#endif

#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/time.h>
#include <string.h>
#include <errno.h>
#include <stdio.h>
#include <time.h>
#include <signal.h>
#include <ctype.h>

typedef struct node_info{
	int fd;
	char id[3];
	char ip[16];
	char port[9];


	struct node_info * next;

} node_info;

typedef struct content{
	char name [101];
	int vazia;

	struct content* next;

} content;

typedef struct expedicao{
	char destino [3];
	char vizinho [3];

	struct expedicao* next;

} expedicao;

void ler_user (char* NET, node_info** ext, char* id, node_info** backup, node_info**interno, char* IP, char* TCP_porto, char* regIp, char* regUdp, int* maxfd, int* fds_counter, int* cojt_fds, char* names, int* aux_names, content **Lista_ficheiros, expedicao* Head);
void interpretacao (char* NET , node_info** ext, char* id, node_info** backup, node_info** interno, char (*indicadores)[16], char* IP, char* TCP_porto, char* regIp, char* regUdp, int* maxfd, int* fds_counter, int* cojt_fds, char * names, int* aux_names, content **Lista_ficheiros, expedicao* Head, int args);
int djoin_node(char* NET , node_info** ext, char* id, node_info** backup, node_info** interno, char (*indicadores)[16], char* IP, char* TCP_porto, char* regIp, char* regUdp, int* maxfd, int* fds_counter, int* cojt_fds, expedicao* Head);
void join_node(char* NET , node_info** ext, char* id, node_info** backup, node_info** interno, char (*indicadores)[16], char* IP, char* TCP_porto, char* regIp, char* regUdp, int* maxfd, int* fds_counter, int* cojt_fds, expedicao* Head);

void Send_Tcp_message (char* mensagem, int fd);
char* Send_Udp_message (char* mensagem, char* reg_ip, char* reg_udp);


void leave_network (char* NET, char* id, char* regIp, char* regUdp,int* fds_counter, int* conjunto_fds);
int open_tcp_socket (char* IP_send_to, char* Port_send_to);
void ler_No (int fd, node_info** ext, char* id, node_info** backup, node_info** interno, char* IP, char* TCP_porto, char* regIp, char* regUdp, int* maxfd, int* fds_counter, int* cojt_fds, expedicao** Head, content** File);
void remove_fd(int fd, int* cojt_fds, int* fds_counter);
void print_data(node_info* Info);
void deleteList(expedicao *head);
void remove_routing ( expedicao **Head, char* id);
void add_routing ( expedicao *Head, char* vizinho, char* destino, char* id);
int is_valid_ip(char *ip_str);
int is_number(char* str);
int count_strings(char *line);

#endif