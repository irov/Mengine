/*
 *	SoundDecoderOGGVorbis.cpp
 *
 *	Created by _Berserk_ on 18.12.2008
 *	Copyright 2008 Menge. All rights reserved.
 *
 */

#	include "SoundDecoderOGGVorbis.h"
#	include "Interface/FileSystemInterface.h"

#	include "DecoderImplement.h"

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	static size_t s_readOgg( void *_ptr, size_t _size, size_t _nmemb, void *_datasource )
	{
		DataStreamInterface* stream = static_cast<DataStreamInterface*>( _datasource );
		std::size_t count = stream->read( _ptr, _size * _nmemb );
		return count;
	}
	//////////////////////////////////////////////////////////////////////////
	static int s_seekOgg( void *_datasource, ogg_int64_t _offset, int _whence )
	{
		DataStreamInterface* stream = static_cast<DataStreamInterface*>( _datasource );
		ogg_int64_t pos = _offset;
		switch( _whence )
		{
		case SEEK_SET: 
			break;
		case SEEK_CUR:
			pos += stream->tell();
			break;
		case SEEK_END:
			pos += stream->size();
			break;
		}
		stream->seek( pos );
		return 0;
	}
	//////////////////////////////////////////////////////////////////////////
	static long s_tellOgg( void *_datasource )
	{
		DataStreamInterface* stream = static_cast<DataStreamInterface*>( _datasource );
		return stream->tell();
	}
	//////////////////////////////////////////////////////////////////////////
	static int s_closeOgg( void *_datasource )
	{
		return 0;
	}
	//////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////
	DECODER_IMPLEMENT( SoundDecoderOGGVorbis );
	//////////////////////////////////////////////////////////////////////////
	SoundDecoderOGGVorbis::SoundDecoderOGGVorbis( DataStreamInterface* _stream, const String& _type )
		: m_stream( _stream )
		, m_valid( false )
		, m_type( _type )
	{
		if( m_stream != NULL )
		{
			m_valid = readHeader_();
		}
	}
	//////////////////////////////////////////////////////////////////////////
	SoundDecoderOGGVorbis::~SoundDecoderOGGVorbis()
	{
		ov_clear( &m_oggVorbisFile );
	}
	//////////////////////////////////////////////////////////////////////////
	void SoundDecoderOGGVorbis::destructor()
	{
		this->~SoundDecoderOGGVorbis();
	}
	//////////////////////////////////////////////////////////////////////////
	void SoundDecoderOGGVorbis::release()
	{
		delete this;
	}
	//////////////////////////////////////////////////////////////////////////
	DataStreamInterface* SoundDecoderOGGVorbis::getStream()
	{
		return m_stream;
	}
	//////////////////////////////////////////////////////////////////////////
	const CodecDataInfo* SoundDecoderOGGVorbis::getCodecDataInfo() const
	{
		if( m_valid == true )
		{
			return &m_dataInfo;
		}

		return NULL;
	}
	//////////////////////////////////////////////////////////////////////////
	unsigned int SoundDecoderOGGVorbis::decode( unsigned char* _buffer, unsigned int _bufferSize )
	{
		if( m_valid == false )
		{
			return 0;
		}

		int current_section;
		long decodeSize = 0;
		//unsigned long samplesNum;
		//short *samples;

		unsigned long bytesDone = 0;
		while( decodeSize = ov_read( &m_oggVorbisFile, (char*)_buffer + bytesDone, _bufferSize - bytesDone, 0, 2, 1, &current_section) )
		{
			bytesDone += decodeSize;

			if ( bytesDone >= _bufferSize )
			{
				break;
			}
		}

		return bytesDone;
	}
	//////////////////////////////////////////////////////////////////////////
	bool SoundDecoderOGGVorbis::readHeader_()
	{
		// callbacks used to read the file
		ov_callbacks vorbisCallbacks;

		// Fill vorbisCallbacks struct
		vorbisCallbacks.read_func  = s_readOgg;
		vorbisCallbacks.seek_func  = s_seekOgg;
		vorbisCallbacks.tell_func  = s_tellOgg;
		vorbisCallbacks.close_func = s_closeOgg;

		if ( ov_open_callbacks( m_stream, &m_oggVorbisFile, NULL, 0, vorbisCallbacks ) < 0 )
		{
			return false;
		}

		vorbis_info* vorbisInfo = ov_info( &m_oggVorbisFile, -1 );
		if( vorbisInfo == NULL )
		{
			return false;
		}

		ogg_int64_t pcmTotal = ov_pcm_total( &m_oggVorbisFile, -1 );	// number of 16bit samples
		m_dataInfo.size = pcmTotal * 2 * vorbisInfo->channels;	// 2 bytes per sample x channels num
		m_dataInfo.channels = vorbisInfo->channels;
		m_dataInfo.frequency = vorbisInfo->rate;
		m_dataInfo.time_total_secs = (float)ov_time_total( &m_oggVorbisFile, -1 );

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	bool SoundDecoderOGGVorbis::seek( float _timing )
	{
		if( m_valid == false )
		{
			return false;
		}

		return ( ov_time_seek( &m_oggVorbisFile, _timing ) == 0 );
	}
	//////////////////////////////////////////////////////////////////////////
	float SoundDecoderOGGVorbis::timeTell()
	{
		if( m_valid == false )
		{
			return 0.0f;
		}

		return ov_time_tell( &m_oggVorbisFile );
	}
	//////////////////////////////////////////////////////////////////////////
}	// namespace Menge
