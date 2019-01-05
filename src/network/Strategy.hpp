#pragma once

#include "Protocol.hpp"

enum class Strategy : int
{
    Continue = 0,
    Repeat = 1,
    Reconnect = 2,
    Disconnect = 3,
    Listen = 4,
};

class IStrategy
{
public:
    virtual Strategy policy() = 0;
    virtual void connect(Nodes& nodes) = 0;

    virtual ~IStrategy() = default;
};
