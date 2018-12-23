#pragma once

#include "Client.hpp"
#include "Socket.hpp"

constexpr auto ServPort = "7043";

class Server
{
public:
    Server()
    {
        socket.bind(nullptr, ServPort);
    }

    void run()
    {
        client.send(Action::StunBindingRequest);
        client.receive();

        while (true)
        {
            std::string endpoint;
            auto n = socket.recv_from(buffer, endpoint);
        }
    }

private:
    Client client;

    UDP::Socket socket;
    UDP::Buffer buffer;
};
