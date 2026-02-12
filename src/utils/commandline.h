#pragma once

#include <string>
#include <algorithm>

class commandline
{
public:
    static commandline& instance()
    {
        static commandline *instance = new commandline();
        return *instance;
    }

    static bool parse(int argc, char* argv[]);
    static std::string getBinFileName();
    static std::string getRegion();
	static std::string getMapper();

private:
    commandline() {}
    
	//Helpers Methods
    bool checkCommand(char** begin, char** end, const std::string &cmd);
    char* getStringValue(char** begin, char** end, const std::string &cmd);
    int getIntValue(char** begin, char** end, const std::string &cmd);

    std::string         binFilename;
    std::string         regionName;
    std::string         mapperName;
};