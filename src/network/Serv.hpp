#pragma once

#include "INode.hpp"

#pragma pack (push, 1)
struct ServHeader
{
    char type = 0;
    char addr[16] = { 0 };
    char port[6] = { 0 };
};
#pragma pack (pop)

class ServResponse final : public IResponse
{
public:
    std::string port;
    std::string address;

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
        int n = 0;

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

        return {};
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
        std::string endpoint{ recvline.data(), recvline.size() };

        auto found = endpoint.find(":");
        auto addr = endpoint.substr(0, found);
        auto port = endpoint.substr(found + 1);

        auto response = std::make_unique<ServResponse>();

        response->port = port;
        response->address = addr;

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
};