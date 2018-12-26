#pragma once

enum class Origin
{
    Stun,
    Serv,
    Oliv,
};

class IResponse
{
public:
    virtual Origin origin() = 0;

    virtual ~IResponse() = default;
};
