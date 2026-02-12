#include <loguru.hpp>
#include "segaemu.h"

SegaEmu::SegaEmu()
{
	sms = nullptr;
    isRunning = false;

    pWindow = nullptr;
    pScreenSurface = nullptr;
    pOverlay = nullptr;
    pFrameBuffer = nullptr;

    windowWidth = 0;
	windowHeight = 0;

    numGamepads = 0;
    for (int i = 0; i < MAX_GAMEPADS; i++)
    {
        GamepadIDs[i] = -1;
        Gamepad[i] = nullptr;
    }
}

SegaEmu::~SegaEmu()
{
}

bool SegaEmu::InitSystem(int scrWidth, int scrHeight)
{
    //Init Windows Size
	windowWidth = scrWidth;
	windowHeight = scrHeight;

    //Initialize SDL
    if (!SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_GAMEPAD))
    {
        LOG_F(ERROR, "SDL could not initialize! SDL_Error: %s", SDL_GetError());
        return false;
    }
        
    LOG_F(INFO, "SDL Initialized...");

    //Create Window
    pWindow = SDL_CreateWindow("SegaEmu", windowWidth, windowHeight, SDL_WINDOW_RESIZABLE);
    if (pWindow == nullptr)
    {
        LOG_F(ERROR, "Window could not be created! SDL_Error: %s", SDL_GetError());
        return false;
    }

    //Set window minimum size
    SDL_SetWindowMinimumSize(pWindow, MINIMUM_SCREEN_WIDTH, MINIMUM_SCREEN_HEIGHT);

    LOG_F(INFO, "Window created...");
    //Get window surface
    pScreenSurface = SDL_GetWindowSurface(pWindow);
    
    //Init Audio Device Stream - use default audio device for playback
    SDL_AudioSpec spec;
    SDL_memset(&spec, 0, sizeof(spec));
    spec.format = SDL_AUDIO_F32;
    spec.freq = 44100;
    spec.channels = 1;

    LOG_F(INFO, "Initializing Audio Playback Device..");
    activeAudioStream = SDL_OpenAudioDeviceStream(SDL_AUDIO_DEVICE_DEFAULT_PLAYBACK, &spec, NULL, NULL);
    if (activeAudioStream == NULL)
    {
        LOG_F(ERROR, "Failed to initialize Audio Stream! SDL_Error: %s", SDL_GetError());
    }
    else
    {
		const char* audioDeviceName = SDL_GetAudioDeviceName(SDL_GetAudioStreamDevice(activeAudioStream));
        if (audioDeviceName == nullptr)
			LOG_F(ERROR, "Failed to get Audio Device Name! SDL_Error: %s", SDL_GetError());
		else
            LOG_F(INFO, "%s initialized [Freq: %d]", audioDeviceName, spec.freq);
    }
    SDL_ResumeAudioStreamDevice(activeAudioStream);

    //Init Timer for Frame Rate Capping
    tp1 = std::chrono::system_clock::now();
    tp2 = std::chrono::system_clock::now();

    isRunning = true;

    return true;
}

bool SegaEmu::InitEmulator(ConsolePlatform platform)
{
    //Init Platform
    selectedPlatform = platform;
	LOG_F(INFO, "EMU - Selected Platform: %s", selectedPlatform == ConsolePlatform::MASTERSYSTEM ? "Master System" : "Megadrive");

	//Init Game FileName from Command Line
	gameFileName = commandline::getBinFileName();
	LOG_F(INFO, "EMU - Game Filename: %s", gameFileName.c_str());

	//Init Mapper from Command Line
	std::string mapper = commandline::getMapper();
	if (mapper == "SEGA")
		selectedMapper = ConsoleMapper::SEGA;
    if (mapper == "CODEMASTER")
		selectedMapper = ConsoleMapper::CODEMASTER;
	LOG_F(INFO, "EMU - Selected Mapper: %s", selectedMapper == ConsoleMapper::SEGA ? "SEGA" : "CODEMASTER");

	//Init Region from Command Line
	std::string region = commandline::getRegion();
    if (region == "JP")
        selectedRegion = ConsoleRegion::JP;
    if (region == "US")
		selectedRegion = ConsoleRegion::US;
    if (region == "EU")
		selectedRegion = ConsoleRegion::EU;
	LOG_F(INFO, "EMU - Selected Region: %s", selectedRegion == ConsoleRegion::JP ? "Japan" : selectedRegion == ConsoleRegion::US ? "USA" : "Europe");

	//Init Platform Object and FrameBuffer
    switch (selectedPlatform)
    {
    case ConsolePlatform::MASTERSYSTEM:
        
        sms = new SMS(selectedRegion, selectedMapper, gameFileName);
        pFrameBuffer = SDL_CreateSurface(256, 192, SDL_PIXELFORMAT_ARGB8888);
	    frameDuration = sms->GetFrameDuration();
		break;
    
    case ConsolePlatform::MEGADRIVE:
		//TODO - Implement Megadrive Emulator
        break;
    }

    SDL_ClearAudioStream(activeAudioStream);
    return true;
}

bool SegaEmu::HandleEvents()
{
    //Handle events on queue
    while (SDL_PollEvent(&sdlEvent) != 0)
    {
        switch (sdlEvent.type)
        {
        case SDL_EVENT_QUIT:
            isRunning = false;
            break;

        case SDL_EVENT_WINDOW_RESIZED:
            windowWidth = sdlEvent.window.data1;
            windowHeight = sdlEvent.window.data2;
            windowWidth = windowHeight * 4 / 3;  //Force 4/3 Ratio
            SDL_SetWindowSize(pWindow, windowWidth, windowHeight);
            pScreenSurface = SDL_GetWindowSurface(pWindow);
            break;

        case SDL_EVENT_GAMEPAD_REMOVED:     //Detect Removed Gamepad
            for (int i = 0; i < MAX_GAMEPADS; i++)
            {
                if (GamepadIDs[i] == sdlEvent.gdevice.which)
                {
                    LOG_F(INFO, "EMU - Disconnected Gamepad [%d] - %s", i + 1, SDL_GetGamepadName(Gamepad[i]));
                    SDL_CloseGamepad(Gamepad[i]);
                    Gamepad[i] = nullptr;
                    numGamepads--;
                }
            }
            break;

        case SDL_EVENT_GAMEPAD_ADDED:      //Detect Added Gamepad  
            for (int i = 0; i < MAX_GAMEPADS; i++)
            {
                if (Gamepad[i] == nullptr)
                {
                    Gamepad[i] = SDL_OpenGamepad(sdlEvent.gdevice.which);
                    if (Gamepad[i] != nullptr)
                    {

                        GamepadIDs[i] = sdlEvent.gdevice.which;
                        LOG_F(INFO, "EMU - Connected Gamepad [%d] - %s", i + 1, SDL_GetGamepadName(Gamepad[i]));
                        numGamepads++;
                    }
                    break;
                }
            }
            break;

        case SDL_EVENT_KEY_DOWN:            //Scan Keyboard Pressed
			updateKeyboardButtonsState(sdlEvent.key.key, true);
            break;

        case SDL_EVENT_KEY_UP:            //Scan Keyboard Released
			updateKeyboardButtonsState(sdlEvent.key.key, false);
            break;

        case SDL_EVENT_GAMEPAD_BUTTON_DOWN:     //Scan Gamepad Buttons
        case SDL_EVENT_GAMEPAD_BUTTON_UP:
            updateGamepadsButtonsState(sdlEvent.gbutton.which, sdlEvent.gbutton.button, sdlEvent.gbutton.down);
            break;
        }
    }

    return true;
}

bool SegaEmu::NewFrame()
{
    static float elapsedTime;

    tp2 = std::chrono::system_clock::now();
    std::chrono::duration<float> diff = tp2 - tp1;
    tp1 = tp2;

    elapsedTime += diff.count();

    if (elapsedTime >= frameDuration)
    {
        sms->NewFrame();
        elapsedTime = 0.0f;

        //Get Audio Samples per Frame
        samplePerFrame = sms->psg.GetSamplePerFrame();
		LOG_F(1, "EMU - Audio Samples per Frame: %d", samplePerFrame);
        
        for (int i = 0; i < samplePerFrame; i++)
            audioBuffer[i] = sms->psg.GetSample();

        LOG_F(1, "EMU - Audio Samples Still Queued in Bytes: %d", SDL_GetAudioStreamAvailable(activeAudioStream));

		if(!SDL_PutAudioStreamData(activeAudioStream, audioBuffer, samplePerFrame * 4))
        {
            LOG_F(ERROR, "EMU - Error Queueing Audio Samples: %s", SDL_GetError());
		}
        return true;
    }
    
    return false;
}

bool SegaEmu::RenderFrame()
{
    //Copy FrameBuffer to ScreenSurface
    if (!SDL_LockSurface(pFrameBuffer))
    {
        LOG_F(ERROR, "EMU - Error while locking ScreenSurface: %s", SDL_GetError());
        return false;
    }
    
    SDL_memcpy(
        pFrameBuffer->pixels,
        sms->vdp.GetScreen(),
        sms->vdp.GetScreenWidth() * sms->vdp.GetScreenHeight() * sizeof(uint32_t));
    SDL_UnlockSurface(pFrameBuffer);
    
    SDL_Rect rect;
    rect.x = 0;
    rect.y = 0;

    if (!SDL_GetWindowSize(pWindow, &rect.w, &rect.h))
    {
        LOG_F(ERROR, "EMU - Error while getting Window Size: %s", SDL_GetError());
        return false;
    }

    if (!SDL_BlitSurfaceScaled(pFrameBuffer, NULL, pScreenSurface, &rect, SDL_SCALEMODE_NEAREST))
    {
        LOG_F(ERROR, "EMU - Error while blitting Surface: %s", SDL_GetError());
        return false;
    }

    //Update the Windows
    if (!SDL_UpdateWindowSurface(pWindow))
    {
        LOG_F(ERROR, "EMU - Error while updating Window Surface: %s", SDL_GetError());
        return false;
    }

    return true;
}

void SegaEmu::Close()
{
    //Close Controllers
    SDL_CloseGamepad(Gamepad[0]);
    SDL_CloseGamepad(Gamepad[1]);

    //Free Additional Surface;
    SDL_DestroySurface(pOverlay);

    //Destroy window
    SDL_DestroyWindow(pWindow);

    //Close Audio Stream
	SDL_DestroyAudioStream(activeAudioStream);

    //Quit SDL subsystems
    SDL_Quit();
}

void SegaEmu::updateKeyboardButtonsState(uint32_t key, bool pressed)
{
    switch (key)
    {
    case SDLK_UP:
		sms->cnt.setButtonState(0, ControllerButton::UP, pressed);
        break;
    case SDLK_DOWN:
        sms->cnt.setButtonState(0, ControllerButton::DOWN, pressed);
        break;
    case SDLK_LEFT:
        sms->cnt.setButtonState(0, ControllerButton::LEFT, pressed);
        break;
    case SDLK_RIGHT:
        sms->cnt.setButtonState(0, ControllerButton::RIGHT, pressed);
        break;
    case SDLK_Z:
        sms->cnt.setButtonState(0, ControllerButton::S1, pressed);
        break;
    case SDLK_X:
        sms->cnt.setButtonState(0, ControllerButton::S2, pressed);
        break;
	}
}

void SegaEmu::updateGamepadsButtonsState(SDL_JoystickID id, uint32_t button, bool pressed)
{
    //Identify Gamepad Index
    int gamepadIndex = -1;
    for (int i = 0; i < MAX_GAMEPADS; i++)
    {
        if (GamepadIDs[i] == id)
        {
            gamepadIndex = i;
            break;
        }
    }

    switch (button)
    {
    case SDL_GAMEPAD_BUTTON_SOUTH:
		sms->cnt.setButtonState(gamepadIndex, ControllerButton::S2, pressed);
        break;
    case SDL_GAMEPAD_BUTTON_WEST:
		sms->cnt.setButtonState(gamepadIndex, ControllerButton::S1, pressed);
        break;
    case SDL_GAMEPAD_BUTTON_DPAD_UP:
		sms->cnt.setButtonState(gamepadIndex, ControllerButton::UP, pressed);
        break;
    case SDL_GAMEPAD_BUTTON_DPAD_DOWN:
		sms->cnt.setButtonState(gamepadIndex, ControllerButton::DOWN, pressed);
        break;
    case SDL_GAMEPAD_BUTTON_DPAD_LEFT:
        sms->cnt.setButtonState(gamepadIndex, ControllerButton::LEFT, pressed);
        break;
    case SDL_GAMEPAD_BUTTON_DPAD_RIGHT:
        sms->cnt.setButtonState(gamepadIndex, ControllerButton::RIGHT, pressed);
        break;
    }
}