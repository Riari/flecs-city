#pragma once

#include <vector>

#include <flecs.h>

#include "Environment/Options.h"
#include "Module/Module.h"

namespace fc
{

class Application
{
public:
    static Application& GetInstance()
    {
        static Application instance;
        return instance;
    }

    Application(Application const&) = delete;
    void operator=(Application const&) = delete;

    int Run(fc::Environment::Options& options, std::vector<Module>& modules);

private:
    flecs::world mWorld;

    Application() {}

    int RunAsServer(fc::Environment::Options& options, std::vector<Module>& modules);
    int RunAsClient(fc::Environment::Options& options, std::vector<Module>& modules);
    int RunAsMonolith(fc::Environment::Options& options, std::vector<Module>& modules);
};

}  // namespace fc
