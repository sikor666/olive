#pragma once

#include "INode.hpp"

#include "Socket.hpp"
#include "Protocol.hpp"

#include <functional>

/*enum class State : int
{
    Waiting,
    Pending,
    Closing,
};*/

class Node : public INode
{
public:
    Node(const char *host_,
         const char *port_, 
         std::function<Buffer()> fun_,
         std::function<std::unique_ptr<IResponse>(Buffer&&)> call_) :
        host{ host_ },
        port{ port_ },
        fun{ fun_ },
        call{ call_ }
    {
    }

    /*virtual std::unique_ptr<IResponse> poll() override
    {
        switch (state)
        {
        case State::Waiting:
        {
            connect(host, port);
            send(fun());
            state = State::Pending;

            break;
        }
        case State::Pending:
        {
            auto buffer = receive();

            if (!buffer.empty())
            {
                call(std::move(buffer));
                state = State::Closing;
            }

            break;
        }
        case State::Closing:
            break;
        }
    }*/

protected:
    void connect(const char *host, const char *serv)
    {
        socket.connect(host, serv);
    }

    void send(Buffer request)
    {
        socket.send_to(request.data(), request.size());
    }

    Buffer receive()
    {
        std::string endpoint;
        auto n = socket.recv_from(buffer, endpoint);

        return Buffer{ buffer, buffer + n };
    }

    void disconnect()
    {
        socket.disconnect();
    }

private:
    const char *host;
    const char *port;
    std::function<Buffer()> fun;
    std::function<std::unique_ptr<IResponse>(Buffer&&)> call;

    UDP::Socket socket;
    UDP::Buffer buffer;
};