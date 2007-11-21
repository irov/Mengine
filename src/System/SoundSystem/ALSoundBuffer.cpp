
#include "stdafx.h"

#include "ALSoundBuffer.h"

ALSoundBuffer::ALSoundBuffer()
{
	alGenBuffers(1, &mBufferName);
}

ALSoundBuffer::~ALSoundBuffer()
{
	//alDeleteBuffers(1, mBufferName);
}

void ALSoundBuffer::unload()
{
	alDeleteBuffers(1, &mBufferName);
}
