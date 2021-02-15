#include "ConnectionHandler.h"

#ifdef _WIN32
ConnectionHandler::ConnectionHandler(SOCKET socket, SOCKADDR_IN address) : _socket(socket), _address(address), _bufferPtr(nullptr), _haveData(false), _length(0)
{
}
#else
ConnectionHandler::ConnectionHandler(int socket, struct sockaddr_in address) : _socket(socket), _address(address), _bufferPtr(nullptr), _haveData(false), _length(0)
{
}
#endif

ConnectionHandler::~ConnectionHandler()
{
	shutdown(_socket, 0);
#ifdef _WIN32
	closesocket(_socket);
#else
	close(_socket);
#endif
	delete[] _bufferPtr;
}

int ConnectionHandler::loadData()
{
	if (_bufferPtr != nullptr)
		delete _bufferPtr;
	unsigned char SizeBuffer[128];
	int bytesGet = recv(_socket, (char*)&SizeBuffer[0], 1, 0);
	if (bytesGet != 1)
		throw SocketError();
	if ((SizeBuffer[0] & 0x80) == 0)
	{
		_bufferPtr = new char[(int)SizeBuffer[0] + 1];
		if (_bufferPtr == nullptr)
			throw OutOfMemory();
		int recievedTotal = 0;
		while (recievedTotal < SizeBuffer[0])
		{
			bytesGet = recv(_socket, _bufferPtr + recievedTotal, SizeBuffer[0], 0);
			if (bytesGet == -1 || bytesGet == 0)
				throw SocketError();
			recievedTotal += bytesGet;
		}
		return recievedTotal;
	}
	int recievedTotal = 0;
	while (recievedTotal < SizeBuffer[0] & 0x7F)
	{
		bytesGet = recv(_socket, (char*)&SizeBuffer[1] + recievedTotal, SizeBuffer[0] & 0x7F, 0);
		if (bytesGet == -1 || bytesGet == 0)
			throw SocketError();
		recievedTotal += bytesGet;
	}
	BERlength messageLength((char*)SizeBuffer, 128);
	_bufferPtr = new char[messageLength.getValue()];
	if (_bufferPtr == nullptr)
		throw OutOfMemory();
	recievedTotal = 0;
	while (recievedTotal < SizeBuffer[0] & 0x7F)
	{
		bytesGet = recv(_socket, _bufferPtr + recievedTotal, messageLength.getValue(), 0);
		if (bytesGet == -1 || bytesGet == 0)
			throw SocketError();
		recievedTotal += bytesGet;
	}
	return  messageLength.getValue();
}

const char* ConnectionHandler::getData()
{
	return _bufferPtr;
}

bool ConnectionHandler::sendData(const char* buffer, const size_t size)
{
	BERlength dataSize(size);
	int bytesSentTotal = 0;
	int bytesSentNow = send(_socket, dataSize.getBytes(), dataSize.getLength(), 0);
	if (bytesSentNow != dataSize.getLength())
		throw SocketError();
	while (bytesSentTotal < size)
	{
		bytesSentNow = send(_socket, buffer + bytesSentTotal, size, 0);
		if (bytesSentNow == -1 || bytesSentNow == 0)
			throw SocketError();
		bytesSentTotal += bytesSentNow;
	}
	return true;
}

#ifdef _WIN32
uint32_t ConnectionHandler::getHost() const
{
	return _address.sin_addr.S_un.S_addr;
}
#else
uint32_t ConnectionHandler::getHost() const
{
	return _address.sin_addr.s_addr;
}

#endif

uint16_t ConnectionHandler::getPort() const
{
	return _address.sin_port;
}

bool ConnectionHandler::transmit(int number)
{
	char buffer[sizeof(int) + 1];
	buffer[0] = static_cast<char>(Types::INTEGER);
	*(int*)(&buffer[1]) = number;
	return sendData(buffer, sizeof(buffer));
}

bool ConnectionHandler::transmit(std::string string)
{
	char* buffer = new char[string.size() + 3];
	buffer[0] = static_cast<char>(Types::STRING);
	strcpy(&buffer[1], string.c_str());
	bool result = sendData(buffer, string.size() + 1);
	delete[] buffer;
	return result;
}

bool ConnectionHandler::transmit(double number)
{
	char buffer[sizeof(double) + 1];
	buffer[0] = static_cast<char>(Types::FLOAT);
	*(double*)(&buffer[1]) = number;
	return sendData(buffer, sizeof(buffer));
}

int ConnectionHandler::getInt()
{
	if (!_haveData)
	{
		_length = loadData();
		_haveData = true;
	}
	const char* data = _bufferPtr;
	if (static_cast<Types>(data[0]) != Types::INTEGER)
		throw UnexpectedType();
	if ((int)_length - 1 != sizeof(int))
		throw UnexpectedType();
	int result = *(int*)(&data[1]);
	_haveData = false;
	return result;
}

std::string ConnectionHandler::getString()
{
	if (!_haveData)
	{
		_length = loadData();
		_haveData = true;
	}
	const char* data = _bufferPtr;
	if (static_cast<Types>(data[0]) != Types::STRING)
		throw UnexpectedType();
	std::string result;
	for (int i = 1; i < _length; i++)
		result += data[i];
	_haveData = false;
	return result;
}

double ConnectionHandler::getDouble()
{
	if (!_haveData)
	{
		_length = loadData();
		_haveData = true;
	}
	const char* data = _bufferPtr;
	if (static_cast<Types>(data[0]) != Types::FLOAT)
		throw UnexpectedType();
	if ((int)_length - 1 != sizeof(double))
		throw UnexpectedType();
	double result = *(double*)(&data[1]);
	_haveData = false;
	return result;
}

unsigned long ConnectionHandler::getLength()
{
	char leadingOctet;
	recv(_socket, &leadingOctet, 1, 0);
	if ((leadingOctet & 0x80) == 0)
	{
		return leadingOctet;
	}
	else
	{
		int numberSize = leadingOctet & 0x70;
		if (numberSize > sizeof(unsigned long))
			return 0;
	}
}
