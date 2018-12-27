#pragma once

#include "INode.hpp"

#include <list>

struct ServAddress
{
    std::string address;
    std::string port;
};

class ServResponse final : public IResponse
{
public:
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
        state{ static_cast<State>(0) }
    {
    }

    virtual std::unique_ptr<IResponse> poll() override
    {
        switch (state)
        {
        case State::Unknown:
        {
            socket.connect(host, port);
            socket.unblock();
            state = State::Conn;
            std::cout << "Serv connect " << host << ":" << port << std::endl;
            break;
        }
        case State::Conn:
        {
            auto request = createRequest();

            std::string endpoint;
            socket.send_to(request.data(), request.size(), endpoint);
            state = State::Send;
            std::cout << "Serv send_to " << endpoint << std::endl;
            break;
        }
        case State::Send:
        {
            std::string endpoint;
            int n = socket.ready() ? socket.recv_from(buffer, endpoint) : 0;

            if (n)
            {
                std::cout << "Serv recv_from " << endpoint << std::endl;

                auto response = parseResponse({ buffer, buffer + n });
                state = State::Recv;

                return response;
            }

            break;
        }
        case State::Recv:
        {
            std::string endpoint;
            int n = socket.ready() ? socket.recv_from(buffer, endpoint) : 0;

            if (n)
            {
                std::cout << "Serv State::Recv " << endpoint << std::endl;
            }

            break;
        }
        }

        return {};

        /*int n = 0;

        if (state == static_cast<State>(0))
        {
            socket.connect(host, port);
            socket.unblock();

            std::cout << "Serv connect " << host << ":" << port << std::endl;

            auto request = createRequest();
            socket.send_to(request.data(), request.size());

            state = State::Send;
        }
        else if (state == static_cast<State>(1))
        {
            std::string endpoint;
            n = socket.ready() ? socket.recv_from(buffer, endpoint) : 0;

            if (n)
            {
                std::cout << "Serv 1 endpoint " << endpoint << std::endl;

                auto response = parseResponse({ buffer, buffer + n });
                state = State::Recv;

                return response;
            }
        }
        else if (state == static_cast<State>(2))
        {
            std::string endpoint;
            n = socket.ready() ? socket.recv_from(buffer, endpoint) : 0;

            if (n)
            {
                std::cout << "Serv 2 endpoint " << endpoint << std::endl;
            }
        }

        return {};*/
    }

private:
    Buffer createRequest()
    {
        ServHeader header;

        header.type = 1;
        memcpy(header.addr, h.c_str(), h.length());
        //header.addr[h.length()] = 0;
        memcpy(header.port, p.c_str(), p.length());
        //header.port[p.length()] = 0;

        Buffer buffer;
        bufferInsert(buffer, header);

        return buffer;
    }

    std::unique_ptr<ServResponse> parseResponse(Buffer&& recvline)
    {
        auto response = std::make_unique<ServResponse>();

        ServerHeader header;
        bufferRead(recvline, header);

        for (size_t i = 0; i < header.size; i++)
        {
            ServerBody body;
            bufferRead(recvline, body);
            response->sockets.push_back({ body.addr, body.port });
        }

        return response;
        /*std::string endpoint{ recvline.data(), recvline.size() };

        auto found = endpoint.find(":");
        auto addr = endpoint.substr(0, found);
        auto port = endpoint.substr(found + 1);

        auto response = std::make_unique<ServResponse>();

        response->port = port;
        response->address = addr;

        return response;*/
    }

private:
    const char *host;
    const char *port;

    std::string h;
    std::string p;

    State state;

    UDP::Socket socket;
    UDP::Buffer buffer;
};