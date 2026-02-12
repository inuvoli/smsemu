#include <cstddef>
#include <cstdio>
#include "circularbuffer.h"

CircularBuffer::CircularBuffer()
{
	m_nReadPointer = 0;
	m_nWritePointer = 0x100;
	
	m_pBuffer = nullptr;
	m_nSize = 0;
	m_nLenght = 0;

}

CircularBuffer::CircularBuffer(int size)
{
	m_nReadPointer = 0;
	m_nWritePointer = 0x100;
	
	m_pBuffer = new float[size];
	
	for (int i = 0; i < size; i++)
		m_pBuffer[i] = 0.0f;

	m_nSize = size;
}

CircularBuffer::~CircularBuffer()
{
	delete[] m_pBuffer;
}

float CircularBuffer::GetSample()
{
	float fSample;

	mutex.lock();
	if (m_nLenght == 0)
	{
		fSample = 0;
	}
	else
	{
		fSample = m_pBuffer[m_nReadPointer % m_nSize];
		m_nReadPointer++;
		m_nLenght--;
	}
	mutex.unlock();
	
	return fSample;
}

float CircularBuffer::ReadSample()
{
	float fSample;

	mutex.lock();
	fSample = m_pBuffer[m_nReadPointer % m_nSize];
	mutex.unlock();

	return fSample;
}

void CircularBuffer::PutSample(float data)
{
	mutex.lock();
	m_pBuffer[m_nWritePointer % m_nSize] = data;
	m_nWritePointer++;
	m_nLenght++;
	mutex.unlock();

}

