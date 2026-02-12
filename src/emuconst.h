#pragma once

//Define Console Platforms
enum class ConsolePlatform : uint8_t
{
	MASTERSYSTEM = 0x1,
	MEGADRIVE = 0x2
};

//Define Regions
enum class ConsoleRegion : uint8_t
{
	JP = 0,
	US = 1,
	EU = 2
};

//Define Mappers
enum class ConsoleMapper : uint8_t
{
	SEGA = 0x1,
	CODEMASTER = 0x2
};

