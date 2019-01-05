#pragma once

#include <string>

class INode
{
public:
    virtual void poll() = 0;
    virtual std::string print() = 0;

    virtual ~INode() = default;
};
