/*
 *	SoundDecoderOGGVorbis.cpp
 *
 *	Created by _Berserk_ on 18.12.2008
 *	Copyright 2008 Menge. All rights reserved.
 *
 */

#	include "SoundDecoderOGGVorbis.h"

#	include "Interface/FileSystemInterface.h"

#	include "Utils/Logger/Logger.h"

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	static size_t s_readOgg( void *_ptr, size_t _size, size_t _nmemb, void *_datasource )
	{
		FileInputStreamInterface* stream = static_cast<FileInputStreamInterface*>( _datasource );
		int count = stream->read( _ptr, _size * _nmemb );
		return static_cast<size_t>( count );
	}
	//////////////////////////////////////////////////////////////////////////
	static int s_seekOgg( void *_datasource, ogg_int64_t _offset, int _whence )
	{
		FileInputStreamInterface* stream = static_cast<FileInputStreamInterface*>( _datasource );
		ogg_int64_t offset = _offset;
		switch( _whence )
		{
		case SEEK_SET: 
			break;
		case SEEK_CUR:
			offset += stream->tell();
			break;
		case SEEK_END:
			offset += stream->size();
			break;
		}
		stream->seek( offset );
		return 0;
	}
	//////////////////////////////////////////////////////////////////////////
	static long s_tellOgg( void *_datasource )
	{
		FileInputStreamInterface* stream = static_cast<FileInputStreamInterface*>( _datasource );
		return stream->tell();
	}
	//////////////////////////////////////////////////////////////////////////
	static int s_closeOgg( void *_datasource )
	{
		return 0;
	}
	//////////////////////////////////////////////////////////////////////////
	SoundDecoderOGGVorbis::SoundDecoderOGGVorbis( CodecServiceInterface * _service, InputStreamInterface * _stream, LogSystemInterface * _logSystem )
		: SoundDecoder(_service, _stream)
		, m_logSystem(_logSystem)
	{
	}
	//////////////////////////////////////////////////////////////////////////
	SoundDecoderOGGVorbis::~SoundDecoderOGGVorbis()
	{
		ov_clear( &m_oggVorbisFile );
	}
	//////////////////////////////////////////////////////////////////////////
	unsigned int SoundDecoderOGGVorbis::decode( unsigned char* _buffer, unsigned int _bufferSize )
	{
		int current_section;
		long decodeSize = 0;
		//unsigned long samplesNum;
		//short *samples;

		unsigned long bytesDone = 0;
		//MENGE_LOG_INFO( "SoundDecoderOGGVorbis::readHeader_ 5" );

		while( decodeSize = ov_read( &m_oggVorbisFile, (char*)_buffer + bytesDone, _bufferSize - bytesDone, 0, 2, 1, &current_section) )
		{
			bytesDone += decodeSize;

			if ( bytesDone >= _bufferSize )
			{
				break;
			}
		}
		//MENGE_LOG_INFO( "SoundDecoderOGGVorbis::readHeader_ 6" );

		return bytesDone;
	}
	//////////////////////////////////////////////////////////////////////////
	bool SoundDecoderOGGVorbis::initialize()
	{
		// callbacks used to read the file
		ov_callbacks vorbisCallbacks;

		// Fill vorbisCallbacks struct
		vorbisCallbacks.read_func  = s_readOgg;
		vorbisCallbacks.seek_func  = s_seekOgg;
		vorbisCallbacks.tell_func  = s_tellOgg;
		vorbisCallbacks.close_func = s_closeOgg;

		//MENGE_LOG_INFO( "SoundDecoderOGGVorbis::readHeader_ 1" );
		if ( ov_open_callbacks( m_stream, &m_oggVorbisFile, NULL, 0, vorbisCallbacks ) < 0 )
		{
			return false;
		}

		//MENGE_LOG_INFO( "SoundDecoderOGGVorbis::readHeader_ 2" );
		vorbis_info* vorbisInfo = ov_info( &m_oggVorbisFile, -1 );
		if( vorbisInfo == NULL )
		{
			return false;
		}

		//MENGE_LOG_INFO( "SoundDecoderOGGVorbis::readHeader_ 3" );
		ogg_int64_t pcmTotal = ov_pcm_total( &m_oggVorbisFile, -1 );	// number of 16bit samples
		m_dataInfo.size = pcmTotal * 2 * vorbisInfo->channels;	// 2 bytes per sample x channels num
		m_dataInfo.channels = vorbisInfo->channels;
		m_dataInfo.frequency = vorbisInfo->rate;
		m_dataInfo.time_total_secs = (float)ov_time_total( &m_oggVorbisFile, -1 );
		//MENGE_LOG_INFO( "SoundDecoderOGGVorbis::readHeader_ 4" );

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	bool SoundDecoderOGGVorbis::seek( float _timing )
	{
		return ( ov_time_seek( &m_oggVorbisFile, _timing ) == 0 );
	}
	//////////////////////////////////////////////////////////////////////////
	float SoundDecoderOGGVorbis::timeTell()
	{
		return ov_time_tell( &m_oggVorbisFile );
	}
	//////////////////////////////////////////////////////////////////////////
}	// namespace Menge
