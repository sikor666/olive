#pragma once

#include "Stun.hpp"
#include "Serv.hpp"
#include "Oliv.hpp"
#include "Translator.hpp"

#include <queue>
#include <list>
#include <memory>

class Client
{
public:
    Client()
    {
        nodes.push_back(std::make_unique<Stun>(StunAddr, StunPort));
    }

    void run()
    {
        if (!nodes.empty())
        {
            for (auto& node : nodes)
            {
                auto response = node->poll();

                if (response)
                {
                    if (response->origin() == Origin::Stun)
                    {
                        auto stunr = dynamic_cast<StunResponse *>(response.get());

                        nodes.push_back(std::make_unique<Serv>
                            (ServAddr, ServPort, stunr->address, stunr->port));
                    }
                    else if (response->origin() == Origin::Serv)
                    {
                        auto servr = dynamic_cast<ServResponse *>(response.get());

                        for (auto socket : servr->sockets)
                        {
                            nodes.push_back(std::make_unique<Oliv>
                                (socket.address.c_str(), socket.port.c_str()));
                        }
                    }
                }
            }
        }
    }

private:
    Translator translator;

    Nodes nodes;
};
