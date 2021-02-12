#pragma once
#include <WinSock2.h>
#include <functional>
#include <thread>
#include <list>
#pragma comment(lib,"ws2_32.lib") 
#include "ClientHandler.h"

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

	SOCKET _serv_socket = INVALID_SOCKET;
	WSAData _w_data;

	status _status = status::CLOSE;
};

