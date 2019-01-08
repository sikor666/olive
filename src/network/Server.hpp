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
#if defined _WIN32
        std::system("cls");
#else
        std::system("clear");
#endif

        while (true)
        {
            client.run();

            SocketAddress endpoint;
            auto n = socket.ready() ? socket.recv_from(buffer, endpoint) : 0;

            if (n)
            {
                Buffer request{ buffer, buffer + n };
                ServHeader header;
                bufferRead(request, header);

                stuns.push_back(header);
                auto found = endpoint.find(':');
                std::string addr = endpoint.substr(0, found);
                std::string port = endpoint.substr(found + 1);
                bzero(header.addr, sizeof(header.addr));
                bzero(header.port, sizeof(header.port));
                memcpy(header.addr, addr.c_str(), addr.size());
                memcpy(header.port, port.c_str(), port.size());
                stuns.push_back(header);
                //client.add(header.name, header.addr, header.port);

                Buffer response;
                ServerHeader h;
                h.size = stuns.size();
                memcpy(h.name, HostName.c_str(), HostName.size());
                bufferInsert(response, h);
                for (auto s : stuns)
                {
                    ServerBody body;
                    memcpy(body.name, s.name, sizeof(s.name));
                    memcpy(body.addr, s.addr, sizeof(s.addr));
                    memcpy(body.port, s.port, sizeof(s.port));
                    bufferInsert(response, body);
                }

                socket.send_to(response.data(), response.size(), endpoint);
            }

#if defined _WIN32
            Sleep(100);
#else
            usleep(100000);
#endif
        }
    }

private:
    Client client;

    std::list<ServHeader> stuns;

    UDP::Socket socket;
    UDP::Buffer buffer;
};
