#include "TcpServer.h"
#include "BERlength.h"

TcpServer::TcpServer(unsigned int port, client_handler_function handler) : _port(port), _handler_function(handler)
{
}

TcpServer::~TcpServer()
{
	if (_status == status::UP)
		stop();
#ifdef _WIN32
	WSACleanup();
#endif
}

void TcpServer::setHandler(client_handler_function newHandler)
{
	_handler_function = newHandler;
}

unsigned int TcpServer::getPort() const
{
	return _port;
}

void TcpServer::setPort(unsigned int newPort)
{
	_port = newPort;
	restart();
}

TcpServer::status TcpServer::getStatus() const
{
	return _status;
}

#ifdef _WIN32
TcpServer::status TcpServer::start()
{
	WSAStartup(MAKEWORD(2, 2), &_w_data);

	SOCKADDR_IN address;
	address.sin_addr.S_un.S_addr = INADDR_ANY;
	address.sin_port = htons(_port);
	address.sin_family = AF_INET;

	if (static_cast<int>(_serv_socket = socket(AF_INET, SOCK_STREAM, 0)) == SOCKET_ERROR)
		return _status = status::CONNECTION_ERROR;
	if (bind(_serv_socket, (struct sockaddr*)&address, sizeof(address)) == SOCKET_ERROR)
		return _status = status::CONNECTION_ERROR;
	if (listen(_serv_socket, SOMAXCONN) == SOCKET_ERROR)
		return _status = status::CONNECTION_ERROR;
	_status = status::UP;
	_handler_thread = std::thread([this] {handlingLoop(); });
	return _status;
}

#else
TcpServer::status TcpServer::start() {
	struct sockaddr_in server;
	server.sin_addr.s_addr = INADDR_ANY;
	server.sin_port = htons(_port);
	server.sin_family = AF_INET;
	_serv_socket = socket(AF_INET, SOCK_STREAM, 0);

	if (_serv_socket == -1) return _status = status::CONNECTION_ERROR;
	if (bind(_serv_socket, (struct sockaddr*)&server, sizeof(server)) < 0) return _status = status::CONNECTION_ERROR;
	if (listen(_serv_socket, 3) < 0)return _status = status::CONNECTION_ERROR;

	_status = status::UP;
	_handler_thread = std::thread([this] {handlingLoop(); });
	return _status;
}
#endif
void TcpServer::stop()
{
	_status = status::CLOSE;
	joinLoop();
#ifdef _WIN32 
	WSACleanup();
	closesocket(_serv_socket);
#else
	close(_serv_socket);
#endif
	for (std::thread& cl_thr : _client_handler_threads)
		cl_thr.join();
	_client_handler_threads.clear();
	_client_handling_end.clear();
}

TcpServer::status TcpServer::restart()
{
	if (_status == status::UP)
		stop();
	return start();
}

void TcpServer::joinLoop()
{
	_handler_thread.join();

}

#ifdef _WIN32
void TcpServer::handlingLoop()
{
	while (_status == status::UP) {
		SOCKET client_socket;
		SOCKADDR_IN client_addr;
		int addrlen = sizeof(client_addr);
		fd_set serv_set{ 1, _serv_socket };
		TIMEVAL timeout{ 0, 500 };
		int connections_count = select(0, &serv_set, 0, 0, &timeout);
		if (connections_count == 0)
			continue;
		if ((client_socket = accept(_serv_socket, (struct sockaddr*)&client_addr, &addrlen)) != 0 && _status == status::UP) {
			_client_handler_threads.push_back(std::thread([this, &client_socket, &client_addr] {
				_handler_function(ClientHandler(client_socket, client_addr));
				_client_handling_end.push_back(std::this_thread::get_id());
			}));
		}
		if (!_client_handling_end.empty())
			for (std::list<std::thread::id>::iterator id_it = _client_handling_end.begin(); !_client_handling_end.empty(); id_it = _client_handling_end.begin())
				for (std::list<std::thread>::iterator thr_it = _client_handler_threads.begin(); thr_it != _client_handler_threads.end(); ++thr_it)
					if (thr_it->get_id() == *id_it) {
						thr_it->join();
						_client_handler_threads.erase(thr_it);
						_client_handling_end.erase(id_it);
						break;
					}
		std::this_thread::sleep_for(std::chrono::milliseconds(30));
	}
}
#else
void TcpServer::handlingLoop() {
	while (_status == status::UP) {
		int client_socket;
		struct sockaddr_in client_addr;
		fd_set serv_set;
		FD_SET (_serv_socket, &serv_set);
		int addrlen = sizeof(struct sockaddr_in);
		timeval timeout{ 1, 500 };
		//int connections_count = select(0, &serv_set, 0, 0, &timeout);
		//if (connections_count == 0)
		//	continue;
		if ((client_socket = accept(_serv_socket, (struct sockaddr*)&client_addr, (socklen_t*)&addrlen)) >= 0 && _status == status::UP)
			_client_handler_threads.push_back(std::thread([this, &client_socket, &client_addr] {
			_handler_function(ClientHandler(client_socket, client_addr));
			_client_handling_end.push_back(std::this_thread::get_id());
		}));

		if (!_client_handling_end.empty())
			for (std::list<std::thread::id>::iterator id_it = _client_handling_end.begin(); !_client_handling_end.empty(); id_it = _client_handling_end.begin())
				for (std::list<std::thread>::iterator thr_it = _client_handler_threads.begin(); thr_it != _client_handler_threads.end(); ++thr_it)
					if (thr_it->get_id() == *id_it) {
						thr_it->join();
						_client_handler_threads.erase(thr_it);
						_client_handling_end.erase(id_it);
						break;
					}

		std::this_thread::sleep_for(std::chrono::milliseconds(50));
	}
}
#endif

