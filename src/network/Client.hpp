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

    /*void add(const char *host_, const char *port_)
    {
        nodes.push_back(std::make_unique<Oliv>(host_, port_));
    }*/

    void run()//std::string& address, std::string& port)
    {
        if (!nodes.empty())
        {
            //auto node = nodes.front().get();

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

                        //address = stunr->address;
                        //port = stunr->port;
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
                    /*else if (response->origin() == Origin::Serv)
                    {
                        auto servr = dynamic_cast<ServResponse *>(response.get());

                        nodes.push_back(std::make_unique<Oliv>
                            (servr->address.c_str(), servr->port.c_str()));
                    }
                    else if (response->origin() == Origin::Oliv)
                    {
                        auto olivr = dynamic_cast<OlivResponse *>(response.get());

                        if (olivr->name == "stun")
                        {
                            nodes.push_back(std::make_unique<Oliv>
                                (olivr->address.c_str(), olivr->port.c_str()));
                        }
                    }*/
                }
            }
        }
    }


private:
    Translator translator;

    Nodes nodes;
};
