#pragma once
#include <cstdint>
#include <mutex>

class CircularBuffer
{
public:
	CircularBuffer();
	CircularBuffer(int size);
	~CircularBuffer();

	float GetSample();
	float ReadSample();
	void PutSample(float data);

private:
	std::mutex mutex;

	float * m_pBuffer;
	int	m_nSize;
	int m_nLenght;

public:
	int m_nReadPointer;
	int m_nWritePointer;
};

