#	include "OggSoundBuffer.h"

#	include <assert.h>

size_t readFunc(void* _ptr, size_t _size, size_t _nmemb, void* _datasource)
{
	OggSoundData* dt   = (OggSoundData *)_datasource;

	if (dt->getPos()>=dt->getSize())
	{
		return 0;
	}

	return (size_t) dt->getPtr (_ptr, _size * _nmemb);
}

int    seekFunc(void* _datasource, ogg_int64_t _offset, int _whence)
{
	OggSoundData * dt = (OggSoundData *)_datasource;

	if ( _whence == SEEK_SET )
		dt->seekAbs ( (int) _offset );
	else
		if ( _whence == SEEK_CUR )
			dt->seekCur ( (int) _offset );
		else
			if ( _whence == SEEK_END )
				dt->seekAbs ( dt->getSize() + (int)_offset );

	return  dt->getPos ();
}

int    closeFunc(void* _datasource)
{
	return 0;
}

long   tellFunc(void* _datasource)
{
	OggSoundData * dt = (OggSoundData *)_datasource;
	return  dt->getPos();
}


OggSoundData::OggSoundData(void * _buffer, size_t _size)
: SoundBufferInterface (_buffer, _size)
{
	ov_callbacks	cb;

	cb.read_func  = readFunc;
	cb.seek_func  = seekFunc;
	cb.close_func = closeFunc;
	cb.tell_func  = tellFunc;

	if ( ov_open_callbacks(this, &mFile, NULL, 0, cb) < 0 )
	{
		assert(!"OggSoundData error");
		return;
	}

	mInfo			= ov_info ( &mFile, -1 );
	setNumChannels(mInfo->channels);
	setFrequency(mInfo->rate);
	setBitsPerSample(16);
}

OggSoundData::~OggSoundData()
{
	ov_clear(&mFile);
}

int		OggSoundData::read(unsigned char* _buffer, int _size)
{
	if (_size < 0)
	{
		_size = getDataSoundSize();
	}

	int	curSection;
	int	bytesRead = 0;

	while (bytesRead < _size)
	{
		int	res = ov_read(&mFile, (char *)(_buffer + bytesRead), _size - bytesRead, 0, 2, 1, &curSection);
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
	if(!ov_time_seek( &mFile, _time ))
	{
		return true;
	}
	return false;
}

int		OggSoundData::getDataSoundSize()  const
{
	return (int)((double)ov_time_total(const_cast <OggVorbis_File *>(&mFile ),-1)+0.5) * getNumChannels() * getFrequency() * 2;
}