#pragma once

enum class Strategy : int
{
    Continue,
    Repeat,
    //Reconnect,
    Disconnect,
    Listen,
};

class IStrategy
{
public:
    virtual Strategy policy() = 0;

    virtual ~IStrategy() = default;
};
