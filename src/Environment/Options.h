#pragma once

#include <cstdint>
#include <unordered_map>

#include <args.hxx>

#include "Environment/Args/ConnectAddress.h"
#include "Environment/Constants.h"

namespace fc::Environment
{

enum RunMode
{
    Monolith,   // Run the client and server code monolithically with no networking.
    Server,     // Run as a dedicated server.
    Client      // Run as a client. Must connect to a server.
};

/// @brief Encapsulates launch options.
class Options
{
public:
    /// @brief Initialises options from launch arguments.
    bool Init(int argc, char** argv);

    /// @brief Returns true if --mode is Monolith.
    bool IsMonolith();

    /// @brief Returns true if --mode is Server.
    bool IsServer();

    /// @brief Returns true if --mode is Client.
    bool IsClient();

    /// @brief Returns the port passed to --listen (if specified).
    uint32_t GetListenPort();

    /// @brief Returns the address passed to --connect (if specified).
    ConnectAddress GetConnectAddress();

private:
    std::unordered_map<std::string, RunMode> mModeMap{
        {"monolith", RunMode::Monolith},
        {"server", RunMode::Server},
        {"client", RunMode::Client}
    };

    args::ArgumentParser mParser{"Flecs City"};
    args::MapFlag<std::string, RunMode> mMode{mParser, "mode", "Mode to run in (monolith|server|client). Defaults to monolith.", {'m', "mode"}, mModeMap};
    args::ValueFlag<uint32_t> mListen{mParser, "listen", "Port to listen on (if mode is Server). Defaults to 6942.", {'l', "listen"}, DEFAULT_LISTEN_PORT};
    args::ValueFlag<ConnectAddress, ConnectAddressReader> mConnect{mParser, "connect", "Address to connect to (if mode is Client). Defaults to 127.0.0.1:6942.", {'c', "connect"}};
};

} // namespace fc::Environment
