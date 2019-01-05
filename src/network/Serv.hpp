#pragma once

#include "Node.hpp"

#include "Oliv.hpp"

#include <list>

struct ServAddress
{
    std::string name;
    std::string address;
    std::string port;
};

class ServStrategy final : public IStrategy
{
public:
    ServStrategy(Strategy strategy_) : strategy{ strategy_ }
    {
    }

    virtual Strategy policy() override
    {
        return strategy;
    }

    virtual void connect(Nodes& nodes) override
    {
        for (auto socket : sockets)
        {
            nodes.push_back(std::make_unique<Oliv>
                (socket.name, socket.address, socket.port));
        }
    }

    std::string name;
    std::list<ServAddress> sockets;

private:
    Strategy strategy;
};

class Serv final : public INode
{
public:
    Serv(const char *host_, const char *port_, std::string h_, std::string p_) :
        host{ host_ },
        port{ port_ },
        h{ h_ },
        p{ p_ }
    {
        state.addTrigger(Trigger::ConnectHost, [&]() {
            socket.connect(host, port);
            socket.unblock();
            return std::make_unique<ServStrategy>(Strategy::Continue);
        });

        state.addTrigger(Trigger::SendBuffer, [&]() {
            auto request = createRequest();
            SocketAddress endpoint;
            socket.send_to(request.data(), request.size(), endpoint);
            stat[endpoint].scounter++;
            return std::make_unique<ServStrategy>(Strategy::Continue);
        });

        state.addTrigger(Trigger::ReceiveBuffer, [&]() {
            SocketAddress endpoint;
            int n = socket.ready() ? socket.recv_from(buffer, endpoint) : 0;
            if (n)
            {
                stat[endpoint].rcounter++;
                auto response = parseResponse({ buffer, buffer + n });
                stat[endpoint].name = response->name;
                return response;
            }
            return std::make_unique<ServStrategy>(Strategy::Repeat);
        });

        state.addTrigger(Trigger::CloseConnection, [&]() {
            socket.disconnect();
            return std::make_unique<ServStrategy>(Strategy::Continue);
        });

        state.addTrigger(Trigger::IdleTransmission, [&]() {
            return std::make_unique<ServStrategy>(Strategy::Continue);
        });
    }

    virtual std::unique_ptr<IStrategy> poll() override
    {
        return state.changeState();;
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

    std::unique_ptr<ServStrategy> parseResponse(Buffer&& recvline)
    {
        auto response = std::make_unique<ServStrategy>(Strategy::Disconnect);

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

    StateMachine state;

    UDP::Socket socket;
    UDP::Buffer buffer;

    Stats stat;
};
