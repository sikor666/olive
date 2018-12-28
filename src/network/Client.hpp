#pragma once

#include "Stun.hpp"
#include "Serv.hpp"
#include "Oliv.hpp"

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
            std::stringstream stream;
            stream << "\n\n";
            stream << "Public socket : " << PublicSocket << "\n";
            stream << "Host name     : " << HostName << "\n\n";

            for (auto& node : nodes)
            {
                stream << node->print();

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
                                (socket.name, socket.address, socket.port));
                        }
                    }
                }
            }

            static size_t streamSize = 0;

            if (streamSize != stream.str().size())
            {
                std::cout << "\033[2J";
            }

            std::cout << stream.str() << "\033[1;1H";

            streamSize = stream.str().size();
        }
    }

private:
    Nodes nodes;
};
