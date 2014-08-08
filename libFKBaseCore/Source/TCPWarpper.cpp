//-------------------------------------------------------------------------
#include "../Include/TCPWarpper.h"
//-------------------------------------------------------------------------

#ifdef _WIN32
#pragma comment (lib,"ws2_32.lib")
#endif

//-------------------------------------------------------------------------
int init_socket()
{
	int init_flag = 0;
#ifdef _WIN32
	WSADATA  Ws;
	return  WSAStartup(MAKEWORD(2,2), &Ws);
#endif
	return init_flag;
}
//-------------------------------------------------------------------------
int create_tcp_socket()
{
	return socket(AF_INET, SOCK_STREAM, 0);
}
//-------------------------------------------------------------------------
void cleanup_socket()
{
#ifdef _WIN32
	WSACleanup();
#endif
}
//-------------------------------------------------------------------------
int close_tcp_socket(int sockfd)
{   
#ifdef _WIN32
	return closesocket(sockfd);
#else
	return close(sockfd);
#endif
}
//-------------------------------------------------------------------------
bool isDomain(char *ip)
{
	struct hostent *hp;
	if(!(hp=gethostbyname(ip)))
	{
		return false;
	}

	return true;
}
//-------------------------------------------------------------------------
bool set_socket_addr(struct sockaddr_in *addr, char *ip, int port)
{
	struct hostent *hp;
	struct sockaddr_in local_addr;
	// 支持域名直接解析处理
	if(!(hp=gethostbyname(ip)))
	{
		return false;
	}

	memcpy(&local_addr.sin_addr.s_addr,hp->h_addr,4);
	memset(addr, 0, sizeof(struct sockaddr_in));
	addr->sin_family = AF_INET;
	addr->sin_port = htons(port);
	addr->sin_addr.s_addr = local_addr.sin_addr.s_addr;
	return true;
}
//-------------------------------------------------------------------------
int tcp_bind(int sockfd, char *ip, int port)
{
	struct sockaddr_in addr;
	int addr_len;
	int result = -1;

	if(!set_socket_addr(&addr, ip, port))
	{
		return -1;
	}
	addr_len = sizeof(addr);
	result = bind(sockfd, (struct sockaddr *)&addr, addr_len);
	return result;
}
//-------------------------------------------------------------------------
int listen_sock(int sockfd)
{
	return listen(sockfd, 5);
}
//-------------------------------------------------------------------------
int accept_connection(int sockfd)
{
	struct sockaddr_in client_addr;

#ifdef _WIN32
	int client_len = sizeof(client_addr);
#else
	socklen_t client_len = sizeof(client_addr);
#endif

	memset(&client_addr, 0, client_len);    
	return  accept(sockfd, (struct sockaddr *)(&client_addr), &client_len);
}
//-------------------------------------------------------------------------
int send_mesg(int sock_fd, char *mesg, int mesg_len)
{
	return send(sock_fd, mesg, mesg_len, 0);
}
//-------------------------------------------------------------------------
int recv_mesg(int sockfd, char * buffer, int buf_size)
{
	memset(buffer, 0, buf_size);
	return recv(sockfd, buffer, buf_size, 0);
} 
//-------------------------------------------------------------------------
int connect_server(int sockfd, char *ip, int port)
{
	struct sockaddr_in server_addr;
	if( !set_socket_addr(&server_addr, ip, port))
	{
		return -1;
	} 
	return connect(sockfd, (struct sockaddr *)(&server_addr), sizeof(struct sockaddr));
}
//-------------------------------------------------------------------------