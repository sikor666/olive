#pragma once

#include <list>

using Nodes = std::list<std::unique_ptr<INode>>;
//using Response = std::unique_ptr<IResponse>;

class Translator
{
public:
    /*void read(Response&& response, Nodes& nodes)
    {
        Origin origin = response->origin();

        switch (origin)
        {
        case Origin::Stun:
            break;
        case Origin::Serv:
            break;
        case Origin::Oliv:
            break;
        }
    }*/
};
