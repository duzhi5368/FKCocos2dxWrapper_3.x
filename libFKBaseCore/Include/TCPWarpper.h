//*************************************************************************
//	创建日期:	2014-8-6   15:27
//	文件名称:	TCPWarpper.h
//  创 建 人:   王宏张 FreeKnight	
//	版权所有:	MIT
//	说    明:	
//*************************************************************************

#pragma once

//-------------------------------------------------------------------------
#ifndef _WIN32
	#include <sys/types.h>
	#include <sys/socket.h>
	#include <netinet/in.h>
	#include <netinet/tcp.h>
	#include <netdb.h>
	#include <stdio.h>
	#include <stdlib.h>
	#include <string.h>
	#include <arpa/inet.h>
	#include <unistd.h>
	#include <errno.h>
	#include <fcntl.h>
	#define  INVALID_SOCKET (-1)
	typedef  int            SOCKET;
	#define HANDLE void*
#endif
//-------------------------------------------------------------------------
#ifdef _WIN32
	#define WIN32_LEAN_AND_MEAN
	#include <Windows.h>
	#include <stdio.h>
	#include <winsock2.h> 
	#include <errno.h>
#endif
//-------------------------------------------------------------------------
extern int init_socket();
extern int create_tcp_socket();
extern void cleanup_socket();
extern int close_tcp_socket(int sockfd);
extern int tcp_bind(int sockfd, char *ip, int port);
extern int listen_sock(int sockfd);
extern int accept_connection(int sockfd);
extern int connect_server(int sockfd, char *ip, int port);
extern int recv_mesg(int sockfd, char *buffer, int buf_size);
extern int send_mesg(int sockfd, char *buffer, int buf_size);
//-------------------------------------------------------------------------