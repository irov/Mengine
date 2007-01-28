#	pragma once

#	include "SoundBuffer.h"

#	include "includes/Vorbis/vorbisfile.h"

class	OggSoundData : public SoundBufferInterface
{
public:
	OggSoundData(void* _buffer, size_t _size);
	~OggSoundData();

public:
	size_t	getDataSoundSize() const;
	ALsizei	read(unsigned char* _buffer, size_t _size);
	bool	seek(float _time);
	double	getTotalTime();

private:
	OggVorbis_File m_oggFile;
	vorbis_info* m_vorbisInfo;
};
