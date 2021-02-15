#include <exception>
class ConnectionError : public std::exception {};
class SocketError : public std::exception {};
class SocketClosed : public std::exception {};
class UnexpectedType : public std::exception {};
class OutOfMemory : public std::exception {};