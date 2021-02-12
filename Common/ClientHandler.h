#ifdef _WIN32
	#include <WinSock2.h>
#else
	#include <sys/types.h>
	#include <sys/socket.h>
	#include <netinet/in.h>
	#include <unistd.h>
	#include <stdio.h>
	#include <stdlib.h>
	#pragma stdlib_stddef_h
#endif
#include <functional>
#include "BERlength.h"
#define BUFFER_SIZE 4096

class ClientHandler {
public:
	
#ifdef _WIN32
	ClientHandler(SOCKET socket, SOCKADDR_IN address);
#else
	ClientHandler(int socket, struct sockaddr_in address);
#endif
	
	~ClientHandler();
	int loadData();
	const char* getData();
	bool sendData(const char* buffer, const size_t size);
	uint32_t getHost() const;
	uint16_t getPort() const;
private:
	unsigned long getLength();
	char* _bufferPtr;
	
#ifdef _WIN32
	SOCKET _socket;
	SOCKADDR_IN _address;
#else
	int _socket;
	struct sockaddr_in _address;
#endif
	
};
