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

                Buffer response{ buffer, buffer + n };
                ServHeader header;
                bufferRead(response, header);

                stuns.push_back(header);

                Buffer b;
                ServerHeader h;
                h.size = stuns.size();
                bufferInsert(b, h);
                for (auto s : stuns)
                {
                    ServerBody body;
                    memcpy(body.addr, s.addr, sizeof(s.addr));
                    memcpy(body.port, s.port, sizeof(s.port));
                    bufferInsert(b, body);
                }

                std::string endpoint;
                socket.send_to(b.data(), b.size(), endpoint);

                std::cout << "Server send_to " << endpoint << std::endl;
            }
        }
    }

private:
    Client client;

    std::list<ServHeader> stuns;

    UDP::Socket socket;
    UDP::Buffer buffer;
};
