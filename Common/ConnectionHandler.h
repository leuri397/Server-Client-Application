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
#include <string>
#include "BERlength.h"
#include "Exceptions.h"
#define BUFFER_SIZE 4096

enum class Types : char
{
	INTEGER,
	STRING,
	FLOAT,
	BYTE
};

class ConnectionHandler {
public:
	
#ifdef _WIN32
	ConnectionHandler(SOCKET socket, SOCKADDR_IN address);
#else
	ClientHandler(int socket, struct sockaddr_in address);
#endif
	
	~ConnectionHandler();
	
	uint32_t getHost() const;
	uint16_t getPort() const;
	
	bool transmit(int number);
	bool transmit(std::string string);
	bool transmit(double number);

	int getInt();
	std::string getString();
	double getDouble();

private:
	bool _haveData;
	int _length;
	unsigned long getLength();
	int loadData();
	const char* getData();
	bool sendData(const char* buffer, const size_t size);
	char* _bufferPtr;
	
#ifdef _WIN32
	SOCKET _socket;
	SOCKADDR_IN _address;
#else
	int _socket;
	struct sockaddr_in _address;
#endif
	
};
