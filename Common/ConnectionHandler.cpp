#include "ConnectionHandler.h"

#ifdef _WIN32
ConnectionHandler::ConnectionHandler(SOCKET socket, SOCKADDR_IN address) : _socket(socket), _address(address), _bufferPtr(nullptr), _haveData(false)
{
}
#else
ConnectionHandler::ConnectionHandler(int socket, struct sockaddr_in address) : _socket(socket), _address(address), _bufferPtr(nullptr)
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
	char SizeBuffer[128];
	int bytesGet = recv(_socket, SizeBuffer, 1, 0);
	if ((SizeBuffer[0] & 0x80) == 0)
	{
		_bufferPtr = new char[SizeBuffer[0]];
		bytesGet = recv(_socket, _bufferPtr, SizeBuffer[0], 0);
		return SizeBuffer[0];
	}
	bytesGet = recv(_socket, &SizeBuffer[1], SizeBuffer[0] & 0x7F, 0);
	BERlength messageLength(SizeBuffer, 128);
	_bufferPtr = new char[messageLength.getValue()];
	bytesGet = recv(_socket, _bufferPtr, messageLength.getValue(), 0);
	return  messageLength.getValue();
}

const char* ConnectionHandler::getData()
{
	return _bufferPtr;
}

bool ConnectionHandler::sendData(const char* buffer, const size_t size)
{
	BERlength dataSize(size);
	int result = send(_socket, dataSize.getBytes(), dataSize.getLength(), 0);
	result = send(_socket, buffer, size, 0);
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
	char* buffer = new char[string.size() + 2];
	buffer[0] = static_cast<char>(Types::STRING);
	strcpy(&buffer[1], string.c_str());
	bool result = sendData(buffer, string.size() + 1);
	delete[] buffer;
	return result;
}

bool ConnectionHandler::transmit(double number)
{
	char buffer[sizeof(double) + 1];
	buffer[0] = static_cast<char>(Types::INTEGER);
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
	if (_length != sizeof(int))
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
	if (_length != sizeof(double))
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
