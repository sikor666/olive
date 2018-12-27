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
        std::string address;
        std::string port;

        while (true)
        {
            client.run(address, port);

            std::string endpoint;
            auto n = socket.ready() ? socket.recv_from(buffer, endpoint) : 0;

            if (n)
            {
                std::cout << "Server endpoint " << endpoint << std::endl;

                std::string text = address + ":" + port;
                socket.send_to(text.c_str(), text.length());

                Buffer response{ buffer, buffer + n };
                ServHeader header;

                bufferRead(response, header);

                client.add(header.addr, header.port);

                auto found = endpoint.find(":");
                auto addr = endpoint.substr(0, found);
                auto port = endpoint.substr(found + 1);

                client.add(addr.c_str(), port.c_str());
            }
        }
    }

private:
    Client client;

    UDP::Socket socket;
    UDP::Buffer buffer;
};
