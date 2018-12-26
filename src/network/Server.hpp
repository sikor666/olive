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
                std::cout << "Server endpoint " << endpoint << std::endl;

                auto found = endpoint.find(":");
                auto addr = endpoint.substr(0, found);
                auto port = endpoint.substr(found + 1);

                client.add(addr.c_str(), port.c_str());

                Buffer response{ buffer, buffer + n };
                ServHeader header;

                bufferRead(response, header);

                client.add(header.addr, header.port);
                //nodes.push_back(std::make_unique<Oliv>(StunAddr, StunPort));
            }
        }
    }

private:
    Client client;

    UDP::Socket socket;
    UDP::Buffer buffer;
};
