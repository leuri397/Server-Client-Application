#include <iostream>
#include <string>
#include "TcpServer.h"

std::string getHostStr(const ClientHandler& client);
void serverBehaviour(ClientHandler handler);

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

void serverBehaviour(ClientHandler handler)
{
    int stringSize = handler.loadData();
    std::cout << "Connection from: " + getHostStr(handler) + "\nNickname: ";
	for (int i = 0; i < stringSize; i++)
	{
        std::cout << handler.getData()[i];
	}
    std::cout << std::endl;
    handler.sendData("OK", 3);
    handler.loadData();
    std::string filename(handler.getData());
    std::cout << "Requested filename: " << filename << std::endl;
    handler.sendData(filename.c_str(), filename.size());
}

std::string getHostStr(const ClientHandler& client) {
    uint32_t ip = client.getHost();
    return std::string() + std::to_string(int(reinterpret_cast<char*>(&ip)[0])) + '.' +
        std::to_string(int(reinterpret_cast<char*>(&ip)[1])) + '.' +
        std::to_string(int(reinterpret_cast<char*>(&ip)[2])) + '.' +
        std::to_string(int(reinterpret_cast<char*>(&ip)[3])) + ':' +
        std::to_string(client.getPort());
}