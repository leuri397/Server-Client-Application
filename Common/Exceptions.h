#include <exception>
class ConnectionError : public std::exception {};
class UnexpectedType : public std::exception {};