#include "server.hpp"
#include <iostream>
#include <thread>

const int ChatServer::SERVER_PORT = 54000;
const int ChatServer::BUFFER_SIZE = 1024;

ChatServer::ChatServer()
    : _acceptor(_ioContext, boost::asio::ip::tcp::endpoint(boost::asio::ip::tcp::v4(), SERVER_PORT)) {}

void ChatServer::handleClient(std::shared_ptr<boost::asio::ip::tcp::socket> clientSocket) {
    try {
        char buffer[BUFFER_SIZE];
        while (_isRunning) {
            boost::system::error_code error;
            int length = clientSocket->read_some(boost::asio::buffer(buffer), error);

            if (error == boost::asio::error::eof) {
                std::cout << "Client disconnected" << std::endl;
                break;
            } else if (error) {
                throw boost::system::system_error(error);
            }

            broadcastMessage(buffer, length, clientSocket);
        }
    } catch (std::exception& e) {
        std::cerr << "Error handling client: " << e.what() << std::endl;
    }

    removeClient(clientSocket);
}

void ChatServer::broadcastMessage(const char* buffer, int length, std::shared_ptr<boost::asio::ip::tcp::socket> senderSocket) {
    boost::system::error_code error;
    for (int i = 0; i < _clients.size(); ++i) {
        if (_clients[i] != senderSocket) {
            boost::asio::write(*_clients[i], boost::asio::buffer(buffer, length), error);
            if (error) {
                std::cerr << "Error sending message to client: " << error.message() << std::endl;
            }
        }
    }
}

void ChatServer::removeClient(std::shared_ptr<boost::asio::ip::tcp::socket> clientSocket) {
    _clients.erase(std::find(_clients.begin(), _clients.end(), clientSocket));
}

void ChatServer::acceptClients() {
    while (_isRunning) {
        auto clientSocket = std::make_shared<boost::asio::ip::tcp::socket>(_ioContext);
        _acceptor.accept(*clientSocket);
        _clients.push_back(clientSocket);
        std::cout << "New client connected" << std::endl;

        std::thread clientThread(&ChatServer::handleClient, this, clientSocket);
        clientThread.detach();
    }
}

void ChatServer::runServer() {
    std::cout << "Server is working" << std::endl;
    acceptClients();
}
