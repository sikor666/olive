#pragma once

#include "Protocol.hpp"

/*enum class Origin : int
{
    Stun,
    Serv,
    Oliv,
};*/

enum class Strategy : int
{
    Continue = 0,
    Repeat = 1,
    Reconnect = 2,
    Disconnect = 3,
};

class IStrategy
{
public:
    //virtual Origin origin() = 0;
    virtual Strategy policy() = 0;
    virtual void connect(Nodes& nodes) = 0;

    virtual ~IStrategy() = default;
};
