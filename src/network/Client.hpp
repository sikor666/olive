#pragma once

#include "Socket.hpp"
#include "Stun.hpp"

enum class Action : int
{
    StunBindingRequest
};

class Client
{
public:
    void connect(const char *host, const char *serv)
    {
        socket.connect(host, serv);
    }

    void send(Action action)
    {
        if (action == Action::StunBindingRequest)
        {
            auto request = stun.createRequest();
            socket.send_to(request.data(), request.size());
        }
    }

    void receive()
    {
        std::string endpoint;
        auto n = socket.recv_from(buffer, endpoint);

        auto response = stun.parseResponse(buffer, n);
    }

    void disconnect()
    {
        socket.disconnect();
    }

private:
    Stun stun;

    UDP::Socket socket;
    UDP::Buffer buffer;
};