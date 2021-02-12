#include "ClientHandler.h"

ClientHandler::ClientHandler(SOCKET socket, SOCKADDR_IN address) : _socket(socket), _address(address), _bufferPtr(nullptr)
{
}

ClientHandler::~ClientHandler()
{
	shutdown(_socket, 0);
	closesocket(_socket);
	delete[] _bufferPtr;
}

int ClientHandler::loadData()
{
	if (_bufferPtr != nullptr)
		delete _bufferPtr;
	char SizeBuffer[128];
	recv(_socket, SizeBuffer, 1, 0);
	if ((SizeBuffer[0] & 0x80) == 0)
	{
		_bufferPtr = new char[SizeBuffer[0]];
		recv(_socket, _bufferPtr, SizeBuffer[0], 0);
		return SizeBuffer[0];
	}
	recv(_socket, &SizeBuffer[1], SizeBuffer[0] & 0x7F, 0);
	BERlength messageLength(SizeBuffer, 128);
	_bufferPtr = new char[messageLength.getValue()];
	recv(_socket, _bufferPtr, messageLength.getValue(), 0);
	return  messageLength.getValue();
}

const char* ClientHandler::getData()
{
	return _bufferPtr;
}

bool ClientHandler::sendData(const char* buffer, const size_t size)
{
	BERlength dataSize(size);
	send(_socket, dataSize.getBytes(), dataSize.getLength(), 0);
	send(_socket, buffer, size, 0);
	return true;
}

uint32_t ClientHandler::getHost() const
{
	return _address.sin_addr.S_un.S_addr;
}

uint16_t ClientHandler::getPort() const
{
	return _address.sin_port;
}

unsigned long ClientHandler::getLength()
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