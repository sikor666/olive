#pragma once

#include "INode.hpp"

#include "Protocol.hpp"
#include "Socket.hpp"
#include "Stun.hpp"
#include "State.hpp"

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
        port{ port_ }
    {
        state.addTrigger(Trigger::CloseConnection, [&]() {
            socket.disconnect();
            return std::make_unique<OlivResponse>();
        });

        state.addTrigger(Trigger::ConnectHost, [&]() {
            socket.connect(host.c_str(), port.c_str());
            socket.unblock();
            return std::make_unique<OlivResponse>();
        });

        state.addTrigger(Trigger::SendBuffer, [&]() {
            auto request = createRequest();
            SocketAddress endpoint;
            socket.send_to(request.data(), request.size(), endpoint);
            stat[endpoint].scounter++;
            stat[endpoint].name = name;
            return std::make_unique<OlivResponse>();
        });

        state.addTrigger(Trigger::ReceiveBuffer, [&]() {
            SocketAddress endpoint;
            int n = socket.ready() ? socket.recv_from(buffer, endpoint) : 0;
            if (n)
            {
                OlivHeader header;
                Buffer response{ buffer, buffer + n };
                bufferRead(response, header);
                if (name != header.name)
                {
                    Throw("Host name error");
                }
                stat[endpoint].rcounter++;
            }
            return std::make_unique<OlivResponse>();
        });
    }

    virtual std::unique_ptr<IResponse> poll() override
    {
        return state.changeState();
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

    StateMachine state;

    UDP::Socket socket;
    UDP::Buffer buffer;

    Stats stat;
};
