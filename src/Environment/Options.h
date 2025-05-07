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
    bool IsListenMode() const;

private:
    args::ArgumentParser mParser{"Flecs City"};
    args::Flag mListen{mParser, "listen", "Start in dedicated server mode", {"listen"}};
};

} // namespace fc::Environment
