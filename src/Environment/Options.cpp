#include "Options.h"


namespace fc::Environment
{

bool Options::Init(int argc, char** argv)
{
    try
    {
        mParser.ParseCLI(argc, argv);
    }
    catch (args::Help)
    {
        std::cout << mParser;
        return false;
    }
    catch (args::ParseError e)
    {
        std::cerr << e.what() << std::endl;
        std::cerr << mParser;
        return false;
    }
    catch (args::ValidationError e)
    {
        std::cerr << e.what() << std::endl;
        std::cerr << mParser;
        return false;
    }

    return true;
}

bool Options::IsServer() const
{
    return mListen;
}

bool Options::IsClient() const
{
    return mConnect;
}

std::string Options::GetConnectAddress()
{
    return args::get(mConnect);
}

} // namespace fc::Environment
