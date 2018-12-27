#pragma once

#include "INode.hpp"

#include "Protocol.hpp"
#include "Socket.hpp"
#include "Stun.hpp"

#pragma pack (push, 1)
struct OlivHeader
{
    char type = 1;
    char name[4] = {'o', 'l', 'i', 'v'};
};
#pragma pack (pop)

class OlivResponse final : public IResponse
{
public:
    std::string name;
    std::string port;
    std::string address;

    virtual Origin origin() override
    {
        return Origin::Oliv;
    }
};

class Oliv final : public INode
{
public:
    Oliv(const char *host_, const char *port_) :
        host{ host_ },
        port{ port_ },
        state{ static_cast<State>(0) }
    {
    }

    virtual std::unique_ptr<IResponse> poll() override
    {
        int n = 0;

        if (state == static_cast<State>(0))
        {
            socket.connect(host.c_str(), port.c_str());
            socket.unblock();

            std::cout << "Oliv connect " << host << ":" << port << std::endl;

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
                std::cout << "Oliv 1 endpoint " << endpoint << std::endl;

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
                std::cout << "Oliv 2 endpoint " << endpoint << std::endl;
            }
        }

        return {};
    }

private:
    Buffer createRequest()
    {
        OlivHeader header;

        Buffer buffer;
        bufferInsert(buffer, header);

        return buffer;
    }

    std::unique_ptr<OlivResponse> parseResponse(Buffer&& recvline)
    {
        return std::make_unique<OlivResponse>();
    }

private:
    std::string host;
    std::string port;

    State state;

    UDP::Socket socket;
    UDP::Buffer buffer;
};
