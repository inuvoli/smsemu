#pragma once
#include <cstdint>

class FrameBuffer
{
public:
	FrameBuffer();
	FrameBuffer(int xRes, int yRes);
	~FrameBuffer();

public:
	bool SetResolution(int xRes, int yRes);
	bool SetPixel(int x, int y, int c);
	uint32_t GetPixel(int x, int y);
	uint32_t* GetBuffer();

private:
	int width;
	int height;

	uint32_t* buffer = nullptr;
};

