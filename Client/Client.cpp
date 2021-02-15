#include <iostream>
#include <thread>
#include "BERlength.h"
#include "ConnectionHandler.h"

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

ConnectionHandler getConnectionHandler(std::string address, int port);

int main()
{
	try
	{
		ConnectionHandler transmission = getConnectionHandler("127.0.0.1", 8080);
		std::string answer;
		int intToSend = 42;
		double doubleToSend = 0.87876765;
		answer = transmission.getString();
		transmission.transmit(intToSend);
		std::cout << "Recieved message: " << answer << std::endl << "Transmitted number: " << intToSend << std::endl;
		answer = transmission.getString();
		transmission.transmit(doubleToSend);
		std::cout << "Recieved message: " << answer << std::endl << "Transmitted number: " << doubleToSend << std::endl;
	}
	catch (const std::exception& err)
	{
		std::cerr << err.what() << std::endl;
	}
}

#ifdef _WIN32
ConnectionHandler getConnectionHandler(std::string address, int port)
{
	WSADATA wsa;
	SOCKET s;
	SOCKADDR_IN server;
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
		throw ConnectionError();
	s = socket(AF_INET, SOCK_STREAM, 0);
	if (s == INVALID_SOCKET)
		throw ConnectionError();
	server.sin_addr.s_addr = inet_addr(address.c_str());
	server.sin_family = AF_INET;
	server.sin_port = htons(8080);
	if (connect(s, (struct sockaddr*)&server, sizeof(server)) != 0)
		throw ConnectionError();
	return ConnectionHandler(s, server);
}
#else
ConnectionHandler getConnectionHandler(std::string address, int port)
{
	int s;
	sockaddr_in server;
	server.sin_addr.s_addr = inet_addr(address.c_str());
	server.sin_family = AF_INET;
	server.sin_port = htons(port);
	s = socket(AF_INET, SOCK_STREAM, 0);
	if (s == -1)
		throw ConnectionError();
	if (connect(s, (sockaddr*)&server, sizeof(server)) != 0)
		throw ConnectionError();
	return ConnectionHandler(s, server);
}
#endif
