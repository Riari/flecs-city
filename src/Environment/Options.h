#pragma once

#include <cstdint>

#include <args.hxx>

namespace fc::Environment
{

constexpr uint32_t DEFAULT_LISTEN_PORT = 6420;

/// @brief Encapsulates launch options.
class Options
{
public:
    /// @brief Initialises options from launch arguments.
    bool Init(int argc, char** argv);

    /// @brief Returns true if the --listen option was specified.
    bool IsServer() const;

    /// @brief Returns true if the --connect option was specified.
    bool IsClient() const;

    /// @brief Returns the port passed to --listen (if specified).
    uint32_t GetListenPort();

    /// @brief Returns the address passed to --connect (if specified).
    std::string GetConnectAddress();

private:
    args::ArgumentParser mParser{"Flecs City"};
    args::ValueFlag<uint32_t> mListen{mParser, "listen", "Start in dedicated server mode", {'l', "listen"}, DEFAULT_LISTEN_PORT};
    args::ValueFlag<std::string> mConnect{mParser, "connect", "Start in client mode", {'c', "connect"}};
};

} // namespace fc::Environment
