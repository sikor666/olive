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

    void add(std::string name, std::string host, std::string port)
    {
        //nodes.push_back(std::make_unique<Oliv>(name, host, port));
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

                auto strategy = node->poll();

                strategy->connect(nodes);
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
