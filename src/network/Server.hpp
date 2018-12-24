#pragma once

#include "Client.hpp"
#include "Socket.hpp"

constexpr auto BindPort = "7043";

constexpr auto StunAddr = "216.93.246.18";
constexpr auto StunPort = "3478";

constexpr auto ServAddr = "87.239.90.119";
constexpr auto ServPort = "7043";

class Server
{
public:
    Server()
    {
        socket.bind(nullptr, BindPort);
    }

    void run()
    {
        client.connect(StunAddr, StunPort);
        client.send(Action::StunBindingRequest);
        client.receive();
        client.disconnect();

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
