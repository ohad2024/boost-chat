#include "client.hpp"
#include <iostream>
#include <thread>

const std::string ChatClient::SERVER_IP = "127.0.0.1";
const int ChatClient::SERVER_PORT = 54000;
const int ChatClient::BUFFER_SIZE = 1024;

ChatClient::ChatClient()
    : _clientSocket(_ioContext) {}

void ChatClient::getUserName() {
    std::cout << "Enter your username: ";
    std::getline(std::cin, _userName);
    while (_userName.empty()) {
        std::cout << "Username cannot be empty. Please enter your username: ";
        std::getline(std::cin, _userName);
    }
}

void ChatClient::runReceiver() {
    try {
        while (_isRunning) {
            char buffer[BUFFER_SIZE];
            boost::system::error_code error;
            int length = _clientSocket.read_some(boost::asio::buffer(buffer), error);

            if (error == boost::asio::error::eof) {
                _isRunning = false;
            } else if (error) {
                throw boost::system::system_error(error);
            }
            std::cout.write(buffer, length);
            std::cout << std::endl;
        }
    } catch (std::exception& e) {
        std::cerr << "Error receiving message: " << e.what() << std::endl;
    }
}

bool ChatClient::connectToServer() {
    try {
        boost::asio::ip::tcp::resolver resolver(_ioContext);
        auto endpoints = resolver.resolve(SERVER_IP, std::to_string(SERVER_PORT));
        boost::asio::connect(_clientSocket, endpoints);
        return true;
    } catch (std::exception& e) {
        std::cerr << "Connection failed: " << e.what() << std::endl;
        return false;
    }
}

void ChatClient::sendMessage(const std::string& message) {
    boost::asio::write(_clientSocket, boost::asio::buffer(message));
}

void ChatClient::inputMessages() {
    std::string message;
    while (_isRunning) {
        std::getline(std::cin, message);
        if (message == "exit") {
            _isRunning = false;
            break;
        }
        sendMessage(_userName + ": " + message);
    }

    _clientSocket.close();
}

void ChatClient::runClient() {
    getUserName();
    if (connectToServer()) {
        std::thread receiver(&ChatClient::runReceiver, this);
        inputMessages();
        receiver.join();
    }
}
