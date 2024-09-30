#pragma once

#include <boost/asio.hpp>
#include <string>

class ChatClient {
    boost::asio::io_context _ioContext;
    boost::asio::ip::tcp::socket _clientSocket;
    std::string _userName;
    bool _isRunning = true;
    static const int BUFFER_SIZE;
    static const std::string SERVER_IP;
    static const int SERVER_PORT;

    void getUserName();
    void runReceiver();
    bool connectToServer();
    void sendMessage(const std::string& message);
    void inputMessages();

public:
    ChatClient();
    void runClient();
};

