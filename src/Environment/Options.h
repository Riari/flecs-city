#pragma once

#include <args.hxx>

namespace fc::Environment
{

/// @brief Encapsulates launch options.
class Options
{
public:
    /// @brief Initialises options from launch arguments.
    bool Init(int argc, char** argv);

    /// @brief Returns true if the --listen flag was given.
    bool IsServer() const;

    /// @brief Returns true if the --connect option was specified.
    bool IsClient() const;

    /// @brief Returns the address passed to --connect (if specified)
    std::string GetConnectAddress();

private:
    args::ArgumentParser mParser{"Flecs City"};
    args::Flag mListen{mParser, "listen", "Start in dedicated server mode", {'l', "listen"}};
    args::ValueFlag<std::string> mConnect{mParser, "connect", "Start in client mode", {'c', "connect"}};
};

} // namespace fc::Environment
