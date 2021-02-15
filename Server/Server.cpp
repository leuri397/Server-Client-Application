#include <iostream>
#include <string>
#include "TcpServer.h"

std::string getHostStr(const ConnectionHandler& client);
void serverBehaviour(ConnectionHandler handler);

int main() {
    TcpServer server(8080, serverBehaviour);
    if (server.start() == TcpServer::status::UP) {
        std::cout << "Server is up!" << std::endl;
        std::this_thread::sleep_for(std::chrono::seconds(10));
        server.stop();
    }
    else {
        std::cout << "Server start error! Error code:" << int(server.getStatus()) << std::endl;
        return -1;
    }

}

void serverBehaviour(ConnectionHandler handler)
{
    try
    {
        std::cout << "Connection from: " << getHostStr(handler) << std::endl;
        handler.transmit("Give me some integer");
        int recievedInt = handler.getInt();
        std::cout << "Recieved integer: " << recievedInt << std::endl;
        handler.transmit("Give me some double");
        double recievedDouble = handler.getDouble();
        std::cout << "Recieved double: " << recievedDouble << std::endl;
    }
	catch (const std::exception& error)
	{
        std::cerr << "Error occured during transmition" << error.what() << std::endl;
	}
}

std::string getHostStr(const ConnectionHandler& client) {
    uint32_t ip = client.getHost();
    return std::string() + std::to_string(int(reinterpret_cast<char*>(&ip)[0])) + '.' +
        std::to_string(int(reinterpret_cast<char*>(&ip)[1])) + '.' +
        std::to_string(int(reinterpret_cast<char*>(&ip)[2])) + '.' +
        std::to_string(int(reinterpret_cast<char*>(&ip)[3])) + ':' +
        std::to_string(client.getPort());
}
