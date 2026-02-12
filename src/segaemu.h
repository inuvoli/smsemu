#pragma once

#include <cstdint>
#include <cstdio>
#include <chrono>
#include <SDL3/SDL.h>

#include "emuconst.h"
#include "sms.h"

constexpr auto MINIMUM_SCREEN_WIDTH = 640;
constexpr auto MINIMUM_SCREEN_HEIGHT = 480;
constexpr auto MAX_GAMEPADS = 2;

//Emulator Class Definition
class SegaEmu
{
public:
	SegaEmu();
	~SegaEmu();

	SMS *sms;

	bool InitSystem(int wndWidth, int wndHeight);
	bool InitEmulator(ConsolePlatform platform);
	bool HandleEvents();
	bool NewFrame();
	bool RenderFrame();
	void Close();
	void updateGamepadsButtonsState(SDL_JoystickID id, uint32_t button, bool pressed);
	void updateKeyboardButtonsState(uint32_t key, bool pressed);

public:
	bool						isRunning;

private:
	ConsolePlatform				selectedPlatform;
	ConsoleRegion				selectedRegion;
	ConsoleMapper				selectedMapper;
	std::string					gameFileName;

	SDL_Event					sdlEvent;
	SDL_Window*					pWindow;
	SDL_AudioStream*            activeAudioStream;		//Audio Stream used to queue audio samples to the active audio device	
	SDL_Surface*				pScreenSurface;
	SDL_Surface*				pFrameBuffer;
	SDL_Surface*				pOverlay;

	//Windows Size
	int							windowWidth;
	int	     					windowHeight;	

	//Gamepads
	int							numGamepads;
	SDL_JoystickID				GamepadIDs[MAX_GAMEPADS];
	SDL_Gamepad*				Gamepad[MAX_GAMEPADS];

	//Video Frame Timing
	float						frameDuration;
	std::chrono::time_point<std::chrono::system_clock> tp1, tp2;
	
	//Audio Buffer and Timing
	int							samplePerFrame;
	float						audioBuffer[1024];
};

