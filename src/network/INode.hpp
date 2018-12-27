#pragma once

#include "IResponse.hpp"

#include <memory>

enum class State : int
{
    Unknown = 0,
    Conn,
    Send,
    Recv,
};

class INode
{
public:
    virtual std::unique_ptr<IResponse> poll() = 0;

    virtual ~INode() = default;
};
