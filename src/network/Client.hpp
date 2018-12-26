#pragma once

#include "Stun.hpp"
#include "Serv.hpp"
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
            auto node = nodes.front().get();
            auto response = node->poll();

            if (response)
            {
                if (response->origin() == Origin::Stun)
                {
                    auto stunr = dynamic_cast<StunResponse *>(response.get());

                    nodes.push_back(std::make_unique<Serv>
                        (ServAddr, ServPort, stunr->address, stunr->port));

                    //nodes.pop_front();
                }
            }
        }
    }


private:
    Translator translator;

    Nodes nodes;
};
