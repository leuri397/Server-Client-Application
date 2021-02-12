#include <WinSock2.h>
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
	
#ifdef _WIN32
	SOCKET _socket;
	SOCKADDR_IN _address;
	char* _bufferPtr;
#else
	int socket;
	struct sockaddr_in address;
#endif
	
};