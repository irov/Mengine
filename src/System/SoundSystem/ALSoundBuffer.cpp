
#include "stdafx.h"

#include "ALSoundBuffer.h"

ALSoundBuffer::ALSoundBuffer()
{
	alGenBuffers(1, &m_bufferName);
}

ALSoundBuffer::~ALSoundBuffer()
{
}

void ALSoundBuffer::unload()
{
	alDeleteBuffers(1, &m_bufferName);
}
