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
		InputStreamInterface* stream = static_cast<InputStreamInterface*>(_datasource);
		size_t count = stream->read( _ptr, _size * _nmemb );

		return count;
	}
	//////////////////////////////////////////////////////////////////////////
	static int s_seekOgg( void *_datasource, ogg_int64_t _offset, int _whence )
	{
		InputStreamInterface* stream = static_cast<InputStreamInterface*>(_datasource);
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

        size_t int_offset = (size_t)offset;

		stream->seek( int_offset );

		return 0;
	}
	//////////////////////////////////////////////////////////////////////////
	static long s_tellOgg( void * _datasource )
	{
		InputStreamInterface* stream = static_cast<InputStreamInterface*>(_datasource);
        size_t pos = stream->tell();

        long long_pos = (long)pos;

		return long_pos;
	}
	//////////////////////////////////////////////////////////////////////////
	static int s_closeOgg( void * _datasource )
	{
        (void)_datasource;

		return 0;
	}
	//////////////////////////////////////////////////////////////////////////
	SoundDecoderOGGVorbis::SoundDecoderOGGVorbis()		
	{
	}
	//////////////////////////////////////////////////////////////////////////
	SoundDecoderOGGVorbis::~SoundDecoderOGGVorbis()
	{
		ov_clear( &m_oggVorbisFile );
	}
    //////////////////////////////////////////////////////////////////////////
    bool SoundDecoderOGGVorbis::_initialize()
    {
        // callbacks used to read the file
        ov_callbacks vorbisCallbacks;

        // Fill vorbisCallbacks struct
        vorbisCallbacks.read_func = s_readOgg;
        vorbisCallbacks.seek_func = s_seekOgg;
        vorbisCallbacks.tell_func = s_tellOgg;
        vorbisCallbacks.close_func = s_closeOgg;

        //MENGE_LOG_INFO( "SoundDecoderOGGVorbis::readHeader_ 1" );
        int opcall_err = ov_open_callbacks( m_stream.get(), &m_oggVorbisFile, NULL, 0, vorbisCallbacks );
        
        if( opcall_err < 0 )
        {
            LOGGER_ERROR(m_serviceProvider)("SoundDecoderOGGVorbis::_initialize invalid ov_open_callbacks [%d]"
                , opcall_err
                );

            return false;
        }

        //MENGE_LOG_INFO( "SoundDecoderOGGVorbis::readHeader_ 2" );
        vorbis_info* vorbisInfo = ov_info( &m_oggVorbisFile, -1 );
        if( vorbisInfo == NULL )
        {
            LOGGER_ERROR(m_serviceProvider)("SoundDecoderOGGVorbis::_initialize invalid ov_info"
                );

            return false;
        }

        if( vorbisInfo->channels != 2 )
        {
            LOGGER_ERROR(m_serviceProvider)("SoundDecoderOGGVorbis::_initialize invalid channels %d need %d"
                , vorbisInfo->channels
                , 2
                );

            return false;
        }

        //MENGE_LOG_INFO( "SoundDecoderOGGVorbis::readHeader_ 3" );
        ogg_int64_t pcmTotal = ov_pcm_total( &m_oggVorbisFile, -1 );	// number of 16bit samples

        size_t size_pcmTotal = (size_t)pcmTotal;

        //m_dataInfo.size = pcmTotal * 2 * vorbisInfo->channels;	// 2 bytes per sample x channels num
        m_dataInfo.size = size_pcmTotal * 2 * vorbisInfo->channels;	// 2 bytes per sample x channels num
        m_dataInfo.channels = vorbisInfo->channels;
        m_dataInfo.frequency = vorbisInfo->rate;

        double al_total = ov_time_total( &m_oggVorbisFile, -1 );
        float total = (float)(al_total * 1000.0);

        m_dataInfo.length = total;

        return true;
    }
	//////////////////////////////////////////////////////////////////////////
	unsigned int SoundDecoderOGGVorbis::decode( unsigned char* _buffer, unsigned int _bufferSize )
	{
		unsigned long bytesDone = 0;
        unsigned long bytesReading = _bufferSize;
        
		while( true, true )
		{            
            int current_section = 0;
            unsigned char * readBuffer = _buffer + bytesDone;
            long decodeSize = ov_read( &m_oggVorbisFile, (char *)readBuffer, bytesReading, 0, 2, 1, &current_section );

            if( decodeSize < 0 )
            {
                LOGGER_ERROR(m_serviceProvider)("SoundDecoderOGGVorbis::decode ov_read return [%d]"
                    , decodeSize
                    );

                return 0;
            }

            bytesReading -= decodeSize;
            bytesDone += decodeSize;

            if( decodeSize == 0 )
            {
                break;
            }

            if( bytesReading == 0 )
            {
                break;
            }

            //if( bytesDone >= _bufferSize )
            //{
            //    break;
            //}
		}

		return bytesDone;
	}
	//////////////////////////////////////////////////////////////////////////
	bool SoundDecoderOGGVorbis::seek( float _timing )
	{         
        if( _timing > m_dataInfo.length )
        {
            LOGGER_ERROR(m_serviceProvider)("SoundDecoderOGGVorbis::seek timing %f > total %f"
                , _timing
                , m_dataInfo.length
                );

            return false;
        }

        double al_pos = (double)(_timing * 0.001f);

        double al_total = ov_time_total( &m_oggVorbisFile, -1 );

        if( al_pos > al_total )
        {
            al_pos = al_total - 0.0001;
        }

        int seek_err = ov_time_seek( &m_oggVorbisFile, al_pos );

        if( seek_err != 0 )
        {
            LOGGER_ERROR(m_serviceProvider)("SoundDecoderOGGVorbis::seek timing %f error %d"
                , _timing
                , seek_err
                );

            return false;
        }

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	float SoundDecoderOGGVorbis::timeTell()
	{
        //double al_total = ov_time_total( &m_oggVorbisFile, -1 );
		double al_pos = ov_time_tell( &m_oggVorbisFile );
        
        float float_pos = (float)(al_pos * 1000.f);

		return float_pos;
	}
	//////////////////////////////////////////////////////////////////////////
}	// namespace Menge
