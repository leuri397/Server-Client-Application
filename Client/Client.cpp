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

int main()
{
#ifdef _WIN32
	WSADATA wsa;
	SOCKET s;
	SOCKADDR_IN server;

	printf("\nInitialising Winsock...");
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
	{
		printf("Failed. Error Code : %d", WSAGetLastError());
		return 1;
	}

	printf("Initialised.\n");


	if ((s = socket(AF_INET, SOCK_STREAM, 0)) == INVALID_SOCKET)
	{
		printf("Could not create socket : %d", WSAGetLastError());
	}

	printf("Socket created.\n");
	server.sin_addr.s_addr = inet_addr("127.0.0.1");
	server.sin_family = AF_INET;
	server.sin_port = htons(8080);

	if (connect(s, (struct sockaddr*)&server, sizeof(server)) < 0)
	{
		puts("connect error");
		return 1;
	}

	puts("Connected");
	ClientHandler transmittionHandler(s, server);
	std::string name("Leonid-2"), file("TestFile.txt");
	transmittionHandler.sendData(name.c_str(), name.size() + 1);
	transmittionHandler.loadData();
	std::cout << transmittionHandler.getData() << std::endl;
	transmittionHandler.sendData(file.c_str(), file.size() + 1);
	transmittionHandler.loadData();
	std::cout << transmittionHandler.getData() << std::endl;
	std::this_thread::sleep_for(std::chrono::seconds(10));
	return 0;
#else
	int s;
	sockaddr_in server;

	printf("\nInitialising Winsock...");
	s = socket(AF_INET, SOCK_STREAM, 0);
	server.sin_addr.s_addr = inet_addr("127.0.0.1");
	server.sin_family = AF_INET;
	server.sin_port = htons(8080);
	connect(s, (struct sockaddr*)&server, sizeof(server)) ;
	ClientHandler transmittionHandler(s, server);
	std::string name("Leonid-2"), file("TestFile.txt");
	transmittionHandler.sendData(name.c_str(), name.size() + 1);
	transmittionHandler.loadData();
	std::cout << transmittionHandler.getData() << std::endl;
	transmittionHandler.sendData(file.c_str(), file.size() + 1);
	transmittionHandler.loadData();
	std::cout << transmittionHandler.getData() << std::endl;
	std::this_thread::sleep_for(std::chrono::seconds(10));
	return 0;
#endif
}


