#	pragma once

#	include "SoundBuffer.h"

class	CALWavBuffer 
	: public SoundBufferInterface
{
public:
	CALWavBuffer(void * _buffer, size_t _size);
	~CALWavBuffer();

public:
	size_t	getDataSoundSize() const;
	size_t	read(unsigned char* _buffer, size_t _size);
	bool	seek(float _time);

private:
	int		mDataOffs;
	int		mDataLength;
};

