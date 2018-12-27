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

            SocketAddress endpoint;
            auto n = socket.ready() ? socket.recv_from(buffer, endpoint) : 0;

            if (n)
            {
                std::cout << "Server endpoint " << endpoint << std::endl;

                Buffer request{ buffer, buffer + n };
                ServHeader header;
                bufferRead(request, header);

                stuns.push_back(header);

                Buffer response;
                ServerHeader h;
                h.size = stuns.size();
                bufferInsert(response, h);
                for (auto s : stuns)
                {
                    ServerBody body;
                    memcpy(body.addr, s.addr, sizeof(s.addr));
                    memcpy(body.port, s.port, sizeof(s.port));
                    bufferInsert(response, body);
                }

                socket.send_to(response.data(), response.size(), endpoint);
                std::cout << "Server send_to " << endpoint << std::endl;
            }

            std::cout << "----------------------------------" << std::endl;
#if defined _WIN32
            Sleep(1000);
#else
            sleep(1);
#endif
        }
    }

private:
    Client client;

    std::list<ServHeader> stuns;

    UDP::Socket socket;
    UDP::Buffer buffer;
};
