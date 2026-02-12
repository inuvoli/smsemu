#include "framebuffer.h"

FrameBuffer::FrameBuffer()
{
	width = 0;
	height = 0;

	buffer = nullptr;
}

FrameBuffer::FrameBuffer(int xRes, int yRes)
{
	if (buffer != nullptr)
		delete[] buffer;

	width = xRes;
	height = yRes;

	buffer = new uint32_t[(xRes * yRes)];
}

FrameBuffer::~FrameBuffer()
{
	if (buffer != nullptr)
		delete[] buffer;
}

bool FrameBuffer::SetResolution(int xRes, int yRes)
{
	if (buffer != nullptr)
		delete[] buffer;

	width = xRes;
	height = yRes;

	buffer = new uint32_t[(xRes * yRes)];

	return true;
}

bool FrameBuffer::SetPixel(int x, int y, int c)
{
	buffer[x + y * width] = c;
	return false;
}

uint32_t FrameBuffer::GetPixel(int x, int y)
{
	return buffer[x + y * width];
}

uint32_t* FrameBuffer::GetBuffer()
{
	return buffer;
}
