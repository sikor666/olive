#pragma once

#include "INode.hpp"

#include <list>

struct ServAddress
{
    std::string name;
    std::string address;
    std::string port;
};

class ServResponse final : public IResponse
{
public:
    std::string name;
    std::list<ServAddress> sockets;

    virtual Origin origin() override
    {
        return Origin::Serv;
    }
};

class Serv final : public INode
{
public:
    Serv(const char *host_, const char *port_, std::string h_, std::string p_) :
        host{ host_ },
        port{ port_ },
        h{ h_ },
        p{ p_ },
        state{ State::Close }
    {
    }

    virtual std::unique_ptr<IResponse> poll() override
    {
        switch (state)
        {
        case State::Close:
        {
            socket.connect(host, port);
            socket.unblock();
            state = State::Connect;
            //std::cout << "Serv connect " << host << ":" << port << std::endl;
            break;
        }
        case State::Connect:
        {
            auto request = createRequest();

            SocketAddress endpoint;
            socket.send_to(request.data(), request.size(), endpoint);
            state = State::Send;
            //std::cout << "Serv send_to " << endpoint << std::endl;
            stat[endpoint].scounter++;
            break;
        }
        case State::Send:
        {
            SocketAddress endpoint;
            int n = socket.ready() ? socket.recv_from(buffer, endpoint) : 0;

            if (n)
            {
                //std::cout << "Serv recv_from " << endpoint << std::endl;
                stat[endpoint].rcounter++;

                auto response = parseResponse({ buffer, buffer + n });
                state = State::Receive;

                stat[endpoint].name = response->name;

                return response;
            }

            break;
        }
        case State::Receive:
        {
            /*SocketAddress endpoint;
            int n = socket.ready() ? socket.recv_from(buffer, endpoint) : 0;

            if (n)
            {
                //std::cout << "Serv State::Recv " << endpoint << std::endl;
                stat[endpoint].rcounter++;
            }*/

            break;
        }
        }

        return {};
    }

    virtual std::string print() override
    {
        std::stringstream stream;
        for (auto s : stat)
        {
            stream << "Serv\t" << s.second.name << "\t"
                << s.first << "\t"
                << s.second.scounter << "\t"
                << s.second.rcounter << "\n";
        }
        return stream.str();
    }

private:
    Buffer createRequest()
    {
        ServHeader header;

        header.type = 1;
        memcpy(header.name, HostName.c_str(), sizeof(header.name));
        memcpy(header.addr, h.c_str(), h.length());
        memcpy(header.port, p.c_str(), p.length());

        Buffer buffer;
        bufferInsert(buffer, header);

        return buffer;
    }

    std::unique_ptr<ServResponse> parseResponse(Buffer&& recvline)
    {
        auto response = std::make_unique<ServResponse>();

        ServerHeader header;
        bufferRead(recvline, header);

        response->name = header.name;

        for (size_t i = 0; i < header.size; i++)
        {
            ServerBody body;
            bufferRead(recvline, body);
            response->sockets.push_back({ body.name, body.addr, body.port });
        }

        return response;
    }

private:
    const char *host;
    const char *port;

    std::string h;
    std::string p;

    State state;

    UDP::Socket socket;
    UDP::Buffer buffer;

    Stats stat;
};