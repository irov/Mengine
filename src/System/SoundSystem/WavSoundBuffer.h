#	pragma once

#	include "SoundBuffer.h"

class	CALWavBuffer 
	: public SoundBufferInterface
{
public:
	CALWavBuffer(void * _buffer, size_t _size);
	~CALWavBuffer();

public:
	size_t  getDataSoundSize() const;
	ALsizei	read(unsigned char* _buffer, size_t _size);
	bool	seek(float _time);
	double	getTotalTime();
private:
	size_t m_dataOffset;
	size_t m_dataLength;
};

