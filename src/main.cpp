#include <loguru.hpp>
#include "segaemu.h"
#include "commandline.h"

constexpr auto DEFAULT_SCREEN_WIDTH = 1024;
constexpr auto DEFAULT_SCREEN_HEIGHT = 768;

int main(int argc, char* argv[])
{
    //Parse command line parameters
    if (!commandline::parse(argc, argv))
        return 0;   

    //Init Log Library
    loguru::init(argc, argv);
    loguru::g_stderr_verbosity = loguru::Verbosity_INFO;
    loguru::add_file("debug.log", loguru::Truncate, 2);
    
    //Init Emulator Object
    SegaEmu emu;

    //Init SDL Platform
    if (!emu.InitSystem(DEFAULT_SCREEN_WIDTH, DEFAULT_SCREEN_HEIGHT))
    {
		LOG_F(ERROR, "Failed to Initialize SDL!");
        return 0;
    }

    //Init Emulator
    if (!emu.InitEmulator(ConsolePlatform::MASTERSYSTEM))
    {
		LOG_F(ERROR, "Failed to Initialize Emulator!");
        return 0;
    }

    while (emu.isRunning)
    {
        //Manage SDL Events
        emu.HandleEvents();

        //Render NewFrame at given Frame Rate
        if (emu.NewFrame())
            emu.RenderFrame();
    }

    emu.Close();
    
    return 0;
}