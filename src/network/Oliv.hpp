#pragma once

#include "INode.hpp"

#include "Protocol.hpp"
#include "Socket.hpp"
#include "Stun.hpp"

class Oliv final : public INode
{
public:
    Oliv(const char *host_, const char *port_) :
        host{ host_ },
        port{ port_ },
        state{ static_cast<State>(0) }
    {
    }

    virtual std::unique_ptr<IResponse> poll() override
    {
        int n = 0;

        if (state == static_cast<State>(0))
        {
            socket.connect(host.c_str(), port.c_str());
            socket.unblock();

            std::cout << "Oliv connect " << host << ":" << port << std::endl;

            auto request = createRequest();
            socket.send_to(request.data(), request.size());

            state = State::Send;
        }
        else if (state == static_cast<State>(1))
        {
            std::string endpoint;
            n = socket.ready() ? socket.recv_from(buffer, endpoint) : 0;

            if (n)
            {
                std::cout << "Olive endpoint " << endpoint << std::endl;

                auto response = parseResponse({ buffer, buffer + n });
                state = State::Recv;
                //socket.disconnect();

                return response;
            }
        }
        else if (state == static_cast<State>(2))
        {
            std::string endpoint;
            n = socket.ready() ? socket.recv_from(buffer, endpoint) : 0;

            if (n)
            {
                std::cout << "Oliv 2 endpoint " << endpoint << std::endl;
            }
        }

        return {};
    }

private:
    Buffer createRequest()
    {
        StunHeader header;
        *(&header.type) = htons(0x0001);        //Message Type : 0x0001 (Binding Request)
                          //.... ...0 ...0 .... = Message Class : 0x00 Request(0)
                          //..00 000. 000. 0001 = Message Method : 0x0001 Binding(0x001)
                          //..0. .... .... .... = Message Method Assignment : IETF Review(0x0)

        *(&header.length) = htons(0x0000);      //Message Length : 0
        *(&header.cookie) = htonl(0x2112A442);  //Message Cookie : 2112a442

        *(&header.tid[0]) = htonl(0x00000000);  //Message Transaction ID : 000000000000000000000000
        *(&header.tid[1]) = htonl(0x00000000);  //Random
        *(&header.tid[2]) = htonl(0x00000000);

        Buffer buffer;
        bufferInsert(buffer, header);

        return buffer;
    }

    std::unique_ptr<StunResponse> parseResponse(Buffer&& recvline)
    {
        StunHeader header;

        char variable[UDP::MAX_LINE];

        size_t offset = 0;
        memcpy(reinterpret_cast<void *>(&header), recvline.data(), sizeof(header));
        offset += sizeof(header);

        while (recvline.size() > offset)
        {
            StunAttribute attribute;
            memcpy(reinterpret_cast<char *>(&attribute), recvline.data() + offset, sizeof(attribute));
            offset += sizeof(attribute);

            auto type = static_cast<AttributeType>(ntohs(attribute.type));
            auto length = ntohs(attribute.length);

            memcpy(variable, recvline.data() + offset, length);
            offset += length;
            variable[length] = 0;

            switch (type)
            {
            case AttributeType::MappedAddress:
            case AttributeType::SourceAddress:
            case AttributeType::ChangedAddress:
            case AttributeType::XorMappedAddress:
            case AttributeType::XorMappedAddress2:
            {
                StunAddrVariable addrVariable;
                memcpy(&addrVariable, variable, length);

                std::cout << attributeTypes[type] << std::endl;
                std::cout << "\t family: " << addressFamily[addrVariable.family] << std::endl;

                auto port = type == AttributeType::XorMappedAddress ||
                    type == AttributeType::XorMappedAddress2 ?
                    ntohs(addrVariable.port ^ header.cookie) :
                    ntohs(addrVariable.port);

                std::cout << "\t port: " << port << std::endl;

                if (addrVariable.family == IPv4)
                {
                    struct in_addr addr;
                    addr.s_addr = type == AttributeType::XorMappedAddress ||
                        type == AttributeType::XorMappedAddress2 ?
                        addrVariable.address[0] ^ header.cookie :
                        addrVariable.address[0];

                    static char str[128];

                    if (inet_ntop(AF_INET, &addr, str, sizeof(str)) != nullptr)
                    {
                        std::cout << "\t address: " << str << std::endl;

                        if (type == AttributeType::XorMappedAddress ||
                            type == AttributeType::XorMappedAddress2)
                        {
                            auto response = std::make_unique<StunResponse>();

                            response->port = std::to_string(port);
                            response->address = str;

                            return response;
                        }
                    }
                }

                break;
            }
            default:
                std::cout << attributeTypes[type] << ": " << variable << std::endl;
                break;
            }
        }

        return std::make_unique<StunResponse>();
    }

private:
    std::string host;
    std::string port;

    State state;

    UDP::Socket socket;
    UDP::Buffer buffer;
};
