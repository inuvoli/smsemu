# SegaEmu

A Sega Master System emulator project built with C++ and SDL3.

## Features
- Master System emulation (work in progress)
- SDL3 windowing, input, and audio
- Gamepad and keyboard input support
- Audio streaming via SDL3 audio stream API

## Build
This project uses CMake (minimum 3.31) and builds with Ninja.

## Usage
```
smsemu [--version] [--help]
       [--bin <rom file>]
       [--reg <Region: JP, US, EU>]
       [--map <Mapper: SEGA, CODEMASTER>]
```

### Examples
```
smsemu --bin Sonic.sms --reg EU --map SEGA
```

## Notes
- Only the Master System platform is currently implemented.
- Megadrive support is not yet implemented.
