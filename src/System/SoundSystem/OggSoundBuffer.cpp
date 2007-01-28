#	include "OggSoundBuffer.h"

#	include <assert.h>

size_t readFunc(void* _ptr, size_t _size, size_t _nmemb, void* _datasource)
{
	OggSoundData* dt   = (OggSoundData *)_datasource;

	size_t pos = dt->getPos();
	size_t length = dt->getLength();

	if ( pos >= length )
	{
		return 0;
	}

	size_t ptr = dt->getPtr (_ptr, _size * _nmemb);

	return ptr;
}

int    seekFunc(void* _datasource, ogg_int64_t _offset, int _whence)
{
	OggSoundData * dt = (OggSoundData *)_datasource;

	int _offset_i = (int)_offset;

	switch( _whence )
	{		
	case SEEK_SET: 	return (int)dt->seekAbs(_offset_i);
	case SEEK_CUR:	return (int)dt->seekCur(_offset_i);
	case SEEK_END:	return (int)dt->seekAbs( dt->getLength() + _offset_i);
	default: return (int)dt->getPos();
	}
}

int    closeFunc(void* _datasource)
{
	return 0;
}

long   tellFunc(void* _datasource)
{
	OggSoundData * dt = (OggSoundData *)_datasource;
	return  (long)dt->getPos();
}


OggSoundData::OggSoundData(void * _buffer, size_t _size)
: SoundBufferInterface(_buffer, _size)
{
	ov_callbacks cb;

	cb.read_func = readFunc;
	cb.seek_func = seekFunc;
	cb.close_func = closeFunc;
	cb.tell_func = tellFunc;

	if ( ov_open_callbacks(this, &m_oggFile, NULL, 0, cb) < 0 )
	{
		assert(!"OggSoundData error");
		return;
	}

	m_vorbisInfo = ov_info ( &m_oggFile, -1 );
	setNumChannels(m_vorbisInfo->channels);
	setFrequency(m_vorbisInfo->rate);
	setBitsPerSample(16);
}

OggSoundData::~OggSoundData()
{
	printf("Buffer Cleared \n");
	ov_clear(&m_oggFile);
}

double	OggSoundData::getTotalTime()
{
	return ov_time_total(&m_oggFile,-1);
}

ALsizei		OggSoundData::read(unsigned char* _buffer, size_t _size)
{
	if (_size < 0)
	{
		_size = getDataSoundSize();
	}

	int	curSection;
	ALsizei bytesRead = 0;

	while (bytesRead < _size)
	{
		char * ch = (char *)(_buffer + bytesRead);
		int size = (int)(_size) - bytesRead;

		int	res = ov_read(&m_oggFile, ch, size, 0, 2, 1, &curSection);
		if (res <= 0)
		{
			break;
		}
		bytesRead += res;
	}
	return bytesRead;
}

bool	OggSoundData::seek(float _time)
{
	if(!ov_time_seek( &m_oggFile, _time ))
	{
		return true;
	}
	return false;
}

size_t	OggSoundData::getDataSoundSize()  const
{
	double	totalTime = ov_time_total ( const_cast <OggVorbis_File *> ( &m_oggFile ), -1 );

	return size_t( totalTime + 0.5 ) * getNumChannels() * getFrequency() * 2;
}