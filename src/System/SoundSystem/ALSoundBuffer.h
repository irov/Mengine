
#pragma once

#include "SoundSystemInterface.h"

class ALSoundBuffer : public SoundBufferInterface
{
public:
	ALSoundBuffer();
	virtual ~ALSoundBuffer();

	virtual void unload();

	virtual bool isStreamed()	const	{ return false; }
	ALuint getBufferName()		const	{ return mBufferName; }
	UINT   getLenghtMs()		const	{ return mLenghtMs; }

	void setLenghtMs(UINT _ms)	{ mLenghtMs = _ms; }

protected:
	ALuint mBufferName;
	UINT   mLenghtMs;
};