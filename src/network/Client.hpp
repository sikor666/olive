#pragma once

#include "Socket.hpp"
#include "Stun.hpp"

enum class Action : int
{
    StunBindingRequest
};

constexpr auto StunAddr = "216.93.246.18";
constexpr auto StunPort = "3478";

class Client
{
public:
    Client()
    {
        socket.connect(StunAddr, StunPort);
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

        stun.parseResponse(buffer, n);
    }

private:
    Stun stun;

    UDP::Socket socket;
    UDP::Buffer buffer;
};