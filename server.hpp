#pragma once

#include <boost/asio.hpp>
#include <vector>
#include <memory>

class ChatServer {
    boost::asio::io_context _ioContext;
    boost::asio::ip::tcp::acceptor _acceptor;
    std::vector<std::shared_ptr<boost::asio::ip::tcp::socket>> _clients;
    bool _isRunning = true;
    static const int SERVER_PORT;
    static const int BUFFER_SIZE;

    void handleClient(std::shared_ptr<boost::asio::ip::tcp::socket> clientSocket);
    void broadcastMessage(const char* buffer, int length, std::shared_ptr<boost::asio::ip::tcp::socket> senderSocket);
    void removeClient(std::shared_ptr<boost::asio::ip::tcp::socket> clientSocket);
    void acceptClients();

public:
    ChatServer();
    void runServer();
};

