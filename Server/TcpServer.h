#pragma once
#include <functional>
#include <thread>
#include <list>
#include "ClientHandler.h"
#ifdef _WIN32
	#include <WinSock2.h>
	#pragma comment(lib,"ws2_32.lib")
#else
	#include <sys/types.h>
	#include <sys/socket.h>
	#include <netinet/in.h>
	#include <unistd.h>
	#include <stdio.h>
	#include <stdlib.h>
	#include <poll.h>
#endif

class TcpServer
{
	typedef std::function<void(ClientHandler)> client_handler_function;

public:
	enum class status
	{
		UP,
		CLOSE,
		CONNECTION_ERROR
	};
	TcpServer(unsigned int port, client_handler_function handler);
	~TcpServer();

	void setHandler(client_handler_function newHandler);
	unsigned int getPort() const;
	void setPort(unsigned int newPort);
	status getStatus() const;

	status start();
	void stop();
	status restart();

	void joinLoop();
	void handlingLoop();

private:
	unsigned int _port;
	client_handler_function _handler_function;

	std::thread _handler_thread;
	std::list<std::thread> _client_handler_threads;
	std::list<std::thread::id> _client_handling_end;

	status _status = status::CLOSE;
#ifdef _WIN32
	SOCKET _serv_socket = INVALID_SOCKET;
	WSAData _w_data;
#else
	int _serv_socket;
#endif
};

