#pragma once
#include "SoundBuffer.h"

class	CALWavBuffer : public SoundBufferInterface
{
public:
	CALWavBuffer(void * _buffer, size_t _size);
	~CALWavBuffer();
	int		getDataSoundSize() const;
	int		read(unsigned char* _buffer, int _size);
	bool	seek(float _time);
private:
	int		mDataOffs;
	int		mDataLength;
};

