#pragma once

#include <random>
#include <sstream>

constexpr auto BindPort = "7043";

constexpr auto StunAddr = "216.93.246.18";
constexpr auto StunPort = "3478";

constexpr auto ServAddr = "87.239.90.119";
constexpr auto ServPort = "7043";

//namespace
//{

unsigned char random_char() {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(0, 255);
    return static_cast<unsigned char>(dis(gen));
}

std::string generate_hex(const unsigned int len)
{
    std::stringstream ss;
    for (auto i = 0; i < len; i++) {
        auto rc = random_char();
        std::stringstream hexstream;
        hexstream << std::hex << int(rc);
        auto hex = hexstream.str();
        ss << (hex.length() < 2 ? '0' + hex : hex);
    }
    return ss.str();
}

std::string generate_mac(const unsigned int len)
{
    std::stringstream ss;
    for (auto i = 0; i < len; i++) {
        auto rc = random_char();
        std::stringstream hexstream;
        hexstream << std::hex << int(rc);
        auto hex = hexstream.str();
        ss << (hex.length() < 2 ? '0' + hex : hex);
        ss << ((i < len - 1) ? ":" : "");
    }
    return ss.str();
}

//} // namespace

const auto HostName = generate_mac(3);

using SocketAddress = std::string;
SocketAddress PublicSocket = "0.0.0.0:0.0";
