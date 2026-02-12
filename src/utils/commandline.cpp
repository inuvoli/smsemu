#include <loguru.hpp>
#include "commandline.h"

bool commandline::parse(int argc, char* argv[])
{
    auto& r = instance();  // Singleton Alias

    if (argc <= 1)
        return false;

    //Parse all command line parameters
    if (r.checkCommand(argv, argv + argc, "--help"))
    {
        printf("usage: smsemu [--version] [--help]\n");
        printf("              [--bin <bin filename>]\n");
        printf("              [--reg <Region: JP, US, EU>]\n");
		printf("              [--map <Mapper: SEGA, CODEMASTER>]\n");
        return false;
    }

    if (r.checkCommand(argv, argv + argc, "--version"))
    {
        printf("smsemu version 0.2 (alpha)\n");
        return false;
    }

    if (r.checkCommand(argv, argv + argc, "--bin"))
    {
        char *filename = r.getStringValue(argv, argv + argc, "--bin");
        if (filename != nullptr)
        {
            r.binFilename = std::string(filename);
        }
        else
        {
            printf("ERROR - Incorrect Bin filename parameter!\n");
            return false;
        }      
    }

    if (r.checkCommand(argv, argv + argc, "--reg"))
    {
        char* region = r.getStringValue(argv, argv + argc, "--reg");
        if (region != nullptr)
        {
            r.regionName = std::string(region);
        }
        else
        {
            printf("ERROR - Incorrect Region parameter!\n");
            return false;
        }
    }

    if (r.checkCommand(argv, argv + argc, "--map"))
    {
        char* mapper = r.getStringValue(argv, argv + argc, "--map");
        if (mapper != nullptr)
        {
            r.mapperName = std::string(mapper);
        }
        else
        {
            printf("ERROR - Incorrect Mapper parameter!\n");
            return false;
        }
    }
    
    return true;
}

std::string commandline::getBinFileName()
{
    auto& r = instance();  // Singleton Alias
    return r.binFilename;
}

std::string commandline::getRegion()
{
    auto& r = instance();  // Singleton Alias
    return r.regionName;
}

std::string commandline::getMapper()
{
    auto& r = instance();  // Singleton Alias
    return r.mapperName;
}

//-----------------------------------------------------------------------------
//
// Private Helpers Methods
//
//-----------------------------------------------------------------------------
bool commandline::checkCommand(char** begin, char** end, const std::string& cmd)
{
    return std::find(begin, end, cmd) != end;
}

char* commandline::getStringValue(char** begin, char** end, const std::string& cmd)
{
    char** p = std::find(begin, end, cmd);

    if (p != end && p++ != end)
    {
        return *p;
    }

    return nullptr;
}

int commandline::getIntValue(char** begin, char** end, const std::string& cmd)
{
    return 0;
}
