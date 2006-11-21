#	pragma once

#	include "SoundBuffer.h"

#	include "includes/Vorbis/vorbisfile.h"



class	OggSoundData 
	: public SoundBufferInterface
{ 
public:
	OggSoundData(void* _buffer, size_t _size);
	~OggSoundData();

public:
	size_t	getDataSoundSize() const;
	size_t	read(unsigned char* _buffer, size_t _size);
	bool	seek(float _time);

private:
	OggVorbis_File	mFile;
	vorbis_info*	mInfo;
};
