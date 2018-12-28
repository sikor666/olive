#pragma once

#include "INode.hpp"

#include "Protocol.hpp"
#include "Socket.hpp"
#include "Stun.hpp"

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
    Oliv(std::string name_, std::string host_, std::string port_) :
        name{ name_ },
        host{ host_ },
        port{ port_ },
        state{ static_cast<State>(0) }
    {
    }

    virtual std::unique_ptr<IResponse> poll() override
    {
        switch (state)
        {
        case State::Unknown:
        {
            socket.connect(host.c_str(), port.c_str());
            socket.unblock();
            state = State::Conn;
            //std::cout << "Oliv connect " << host << ":" << port << std::endl;
            break;
        }
        case State::Conn:
        {
            auto request = createRequest();

            SocketAddress endpoint;
            socket.send_to(request.data(), request.size(), endpoint);
            state = State::Send;
            //std::cout << "Oliv send_to " << endpoint << std::endl;
            stat[endpoint].scounter++;
            stat[endpoint].name = name;
            break;
        }
        case State::Send:
        {
            SocketAddress endpoint;
            int n = socket.ready() ? socket.recv_from(buffer, endpoint) : 0;

            if (n)
            {
                OlivHeader header;
                Buffer request{ buffer, buffer + n };
                bufferRead(request, header);

                if (name != header.name)
                {
                    Throw("Host name error");
                }

                //std::cout << "Oliv recv_from " << endpoint << std::endl;
                stat[endpoint].rcounter++;

                auto response = parseResponse({ buffer, buffer + n });
                state = State::Conn;

                return response;
            }

            break;
        }
        case State::Recv:
        {
            SocketAddress endpoint;
            int n = socket.ready() ? socket.recv_from(buffer, endpoint) : 0;

            if (n)
            {
                //std::cout << "Oliv State::Recv " << endpoint << std::endl;
                stat[endpoint].rcounter++;
            }

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
            stream << "Oliv\t" << s.second.name << "\t"
                   << s.first << "\t"
                   << s.second.scounter << "\t"
                   << s.second.rcounter << "\n";
        }
        return stream.str();
    }

private:
    Buffer createRequest()
    {
        OlivHeader header;
        memcpy(header.name, HostName.c_str(), HostName.size());

        Buffer buffer;
        bufferInsert(buffer, header);

        return buffer;
    }

    std::unique_ptr<OlivResponse> parseResponse(Buffer&& recvline)
    {
        return std::make_unique<OlivResponse>();
    }

private:
    std::string name;
    std::string host;
    std::string port;

    State state;

    UDP::Socket socket;
    UDP::Buffer buffer;

    Stats stat;
};
