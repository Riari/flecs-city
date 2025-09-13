#pragma once

#include <cstdint>
#include <string>

#include <args.hxx>

#include "Environment/Constants.h"

namespace fc::Environment
{

struct ConnectAddress
{
    const char* mHost;
    uint32_t mPort;

    std::string mHostString;

    ConnectAddress() : mHost(nullptr), mPort(0) {}

    ConnectAddress(const std::string& hostString, uint32_t port)
        : mHostString(hostString)
        , mPort(port)
    {
        mHost = hostString.c_str();
    }
};

struct ConnectAddressReader
{
    void operator()(const std::string& name, const std::string& value, ConnectAddress& destination)
    {
        size_t colonPos = value.find_last_of(':');

        if (colonPos == std::string::npos)
        {
            if (value.empty())
            {
                throw args::ParseError("Invalid address format for '" + name + "': empty address");
            }
            destination = ConnectAddress(value, DEFAULT_LISTEN_PORT);
            return;
        }

        if (colonPos == 0)
        {
            throw args::ParseError("Invalid address format for '" + name + "': empty host in '" + value + "'");
        }

        std::string host = value.substr(0, colonPos);

        if (colonPos == value.length() - 1)
        {
            destination = ConnectAddress(host, DEFAULT_LISTEN_PORT);
            return;
        }

        std::string portStr = value.substr(colonPos + 1);

        try
        {
            unsigned long portLong = std::stoul(portStr);

            if (portLong == 0 || portLong > 65535)
            {
                throw args::ParseError("Port number for '" + name + "' must be between 1 and 65535, got: " + portStr);
            }

            destination = ConnectAddress(host, static_cast<uint32_t>(portLong));
        }
        catch (const std::invalid_argument&)
        {
            throw args::ParseError("Invalid port number for '" + name + "': '" + portStr + "'");
        }
        catch (const std::out_of_range&)
        {
            throw args::ParseError("Port number for '" + name + "' out of range: '" + portStr + "'");
        }
    }
};

} // namespace fc::Environment
