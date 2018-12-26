#pragma once

#include "Config.hpp"
#include "Client.hpp"
#include "Socket.hpp"

class Server
{
public:
    Server()
    {
        socket.bind(nullptr, BindPort);
        socket.unblock();
    }

    void run()
    {
        while (true)
        {
            client.run();

            std::string endpoint;
            auto n = socket.ready() ? socket.recv_from(buffer, endpoint) : 0;

            if (n)
            {
                bool stop = 0;
            }
        }
    }

private:
    Client client;

    UDP::Socket socket;
    UDP::Buffer buffer;
};
