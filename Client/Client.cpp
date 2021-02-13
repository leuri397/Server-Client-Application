#include <iostream>
#include <thread>
#include "BERlength.h"
#include "ClientHandler.h"

#ifdef _WIN32
#pragma comment(lib,"ws2_32.lib") 
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include <WinSock2.h>
#include <ws2tcpip.h>
#else
	#include <sys/types.h>
	#include <sys/socket.h>
	#include <netinet/in.h>
	#include <unistd.h>
	#include <stdio.h>
	#include <stdlib.h>
	#include <arpa/inet.h>
#endif

ClientHandler getConnectionHandler(std::string address, int port);

int main()
{
	ClientHandler transmission = getConnectionHandler("127.0.0.1", 8080);

}

#ifdef _WIN32
ClientHandler getConnectionHandler(std::string address, int port)
{
	WSADATA wsa;
	SOCKET s;
	SOCKADDR_IN server;
	WSAStartup(MAKEWORD(2, 2), &wsa);
	printf("Initialised.\n");
	s = socket(AF_INET, SOCK_STREAM, 0);
	server.sin_addr.s_addr = inet_addr(address.c_str());
	server.sin_family = AF_INET;
	server.sin_port = htons(8080);
	connect(s, (struct sockaddr*)&server, sizeof(server));
	return ClientHandler(s, server);
}
#else
ClientHandler getConnectionHandler(std::string address, int port)
{
	int s;
	sockaddr_in server;
	s = socket(AF_INET, SOCK_STREAM, 0);
	server.sin_addr.s_addr = inet_addr(address.c_str());
	server.sin_family = AF_INET;
	server.sin_port = htons(port);
	connect(s, (struct sockaddr*)&server, sizeof(server));
	return ClientHandler(s, server);
}
#endif
