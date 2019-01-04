#pragma once

#include "IResponse.hpp"

#include <memory>
#include <string>

class INode
{
public:
    virtual std::unique_ptr<IStrategy> poll() = 0;
    virtual std::string print() = 0;

    virtual ~INode() = default;
};
