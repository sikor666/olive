#pragma once

#include <cstring>
#include <ctime>
#include <string>
#include <vector>
#include <list>
#include <map>

enum class NodeAction : int
{
    Send,
    Recv,
};

struct ActionDetails
{
    unsigned long scounter = 0;
    unsigned long rcounter = 0;
};

using SocketAddress = std::string;
using Byte = char;
using Buffer = std::vector<Byte>;
using Nodes = std::list<std::unique_ptr<INode>>;
using Stats = std::map<SocketAddress, ActionDetails>;

#pragma pack (push, 1)
struct ServerHeader
{
    char type = 0;
    char size = 0;
};

struct ServerBody
{
    char addr[16] = { 0 };
    char port[6] = { 0 };
};

struct ServHeader
{
    char type = 1;
    char addr[16] = { 0 };
    char port[6] = { 0 };
};

struct OlivHeader
{
    char type = 2;
    char name[4] = { 'o', 'l', 'i', 'v' };
};
#pragma pack (pop)

template<typename B, typename D>
void bufferInsert(B& buffer, D& data)
{
    buffer.insert(buffer.begin(),
        reinterpret_cast<Byte *>(&data),
        reinterpret_cast<Byte *>(&data) + sizeof(data));
}

template<typename B, typename D>
void bufferRead(B& buffer, D& data)
{
    memcpy(reinterpret_cast<Byte *>(&data),
        buffer.data() + buffer.size() - sizeof(data),
        sizeof(data));

    buffer.resize(buffer.size() - sizeof(data));
}
