#pragma once
#include "SoundBuffer.h"

class	OggSoundData : public SoundBufferInterface
{
public:
	OggSoundData(void* _buffer, size_t _size);
	~OggSoundData();
	int		getDataSoundSize() const;
	int		read(unsigned char* _buffer, int _size);
	bool	seek(float _time);
private:
	OggVorbis_File	mFile;
	vorbis_info*	mInfo;
};
