#include "SoundDecoderOGGVorbis.h"

#include "Logger/Logger.h"

namespace Mengine
{
	//////////////////////////////////////////////////////////////////////////
	static size_t s_readOgg( void *_ptr, size_t _size, size_t _nmemb, void *_datasource )
	{
		InputStreamInterface * stream = static_cast<InputStreamInterface *>(_datasource);
		size_t count = stream->read( _ptr, _size * _nmemb );

		return count;
	}
	//////////////////////////////////////////////////////////////////////////
	static int s_seekOgg( void *_datasource, ogg_int64_t _offset, int _whence )
	{
		InputStreamInterface * stream = static_cast<InputStreamInterface *>(_datasource);
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
        default:
            return 1;
		}

        size_t int_offset = (size_t)offset;

		stream->seek( int_offset );

		return 0;
	}
	//////////////////////////////////////////////////////////////////////////
	static long s_tellOgg( void * _datasource )
	{
		InputStreamInterface * stream = static_cast<InputStreamInterface *>(_datasource);

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
		: m_oggVorbisFileInitialize(false)
	{
	}
	//////////////////////////////////////////////////////////////////////////
	SoundDecoderOGGVorbis::~SoundDecoderOGGVorbis()
	{
	}
	//////////////////////////////////////////////////////////////////////////
	bool SoundDecoderOGGVorbis::_initialize()
	{
		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void SoundDecoderOGGVorbis::_finalize()
	{
		if( m_oggVorbisFileInitialize == true )
		{
			m_oggVorbisFileInitialize = false;

			ov_clear( &m_oggVorbisFile );
		}		
	}
    //////////////////////////////////////////////////////////////////////////
    bool SoundDecoderOGGVorbis::_prepareData()
    {
        ov_callbacks vorbisCallbacks;
        vorbisCallbacks.read_func = s_readOgg;
        vorbisCallbacks.seek_func = s_seekOgg;
        vorbisCallbacks.tell_func = s_tellOgg;
        vorbisCallbacks.close_func = s_closeOgg;

        int opcall_err = ov_open_callbacks( m_stream.get(), &m_oggVorbisFile, nullptr, 0, vorbisCallbacks );
        
        if( opcall_err < 0 )
        {
            LOGGER_ERROR("SoundDecoderOGGVorbis::_prepareData invalid ov_open_callbacks [%d]"
                , opcall_err
                );

            return false;
        }

		m_oggVorbisFileInitialize = true;

        vorbis_info* vorbisInfo = ov_info( &m_oggVorbisFile, -1 );

        if( vorbisInfo == nullptr )
        {
            LOGGER_ERROR("SoundDecoderOGGVorbis::_prepareData invalid ov_info"
                );

            return false;
        }

        if( vorbisInfo->channels != 2 )
        {
            LOGGER_ERROR("SoundDecoderOGGVorbis::_prepareData invalid channels %d need %d"
                , vorbisInfo->channels
                , 2
                );

            return false;
        }

        ogg_int64_t pcmTotal = ov_pcm_total( &m_oggVorbisFile, -1 );	// number of 16bit samples

        size_t size_pcmTotal = (size_t)pcmTotal;

        //m_dataInfo.size = pcmTotal * 2 * vorbisInfo->channels;	// 2 bytes per sample x channels num
        m_dataInfo.size = size_pcmTotal * 2 * vorbisInfo->channels;	// 2 bytes per sample x channels num
        m_dataInfo.channels = (uint32_t)vorbisInfo->channels;
        m_dataInfo.frequency = (uint32_t)vorbisInfo->rate;
		m_dataInfo.bits = 2;

        double al_total = ov_time_total( &m_oggVorbisFile, -1 );
        float total = (float)(al_total * 1000.0);

        m_dataInfo.length = total;

        return true;
    }
	//////////////////////////////////////////////////////////////////////////
	bool SoundDecoderOGGVorbis::_rewind()
	{
		if( m_oggVorbisFileInitialize == true )
		{
			m_oggVorbisFileInitialize = false;

			ov_clear( &m_oggVorbisFile );
		}

		if( m_stream->seek( 0 ) == false )
		{
			return false;
		}

		if( this->_prepareData() == false )
		{
			return false;
		}

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	size_t SoundDecoderOGGVorbis::_decode( void * _buffer, size_t _bufferSize )
	{
		long bytesDone = 0;
		int bytesReading = (int)_bufferSize;
        
		for(;;)
		{            
            int current_section = 0;
            char * readBuffer = (char *)_buffer + bytesDone;
            long decodeSize = ov_read( &m_oggVorbisFile, readBuffer, bytesReading, 0, 2, 1, &current_section );
			
			if( decodeSize == OV_HOLE )
			{
				LOGGER_CRITICAL("SoundDecoderOGGVorbis::decode ov_read return OV_HOLE"
					, decodeSize
					);

				bytesDone = 0;
				break;
			}

            if( decodeSize < 0 )
            {
                LOGGER_ERROR("SoundDecoderOGGVorbis::decode ov_read return [%d]"
                    , decodeSize
                    );

				bytesDone = 0;
                break;
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
	bool SoundDecoderOGGVorbis::_seek( float _timing )
	{   
        if( _timing >= m_dataInfo.length )
        {
            LOGGER_ERROR("SoundDecoderOGGVorbis::seek timing %f > total %f"
                , _timing
                , m_dataInfo.length
                );

			_timing = 0.f;
        }
		      
        double al_pos = (double)(_timing) * 0.001;
        int seek_err = ov_time_seek( &m_oggVorbisFile, al_pos );

		if( seek_err == OV_EINVAL )
		{
			seek_err = ov_time_seek( &m_oggVorbisFile, 0.0 );
		}

        if( seek_err != 0 )
        {
			//OV_ENOSEEK
            LOGGER_ERROR("SoundDecoderOGGVorbis::seek timing %f is %f error %d"
                , _timing
				, m_dataInfo.length
                , seek_err
                );

            return false;
        }

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	float SoundDecoderOGGVorbis::_tell() const
	{
		double al_pos = ov_time_tell( const_cast<OggVorbis_File *>(&m_oggVorbisFile) );

        float float_pos = (float)(al_pos * 1000.0);

        if( float_pos > m_dataInfo.length )
        {
            float_pos = m_dataInfo.length;
        }
        
		return float_pos;
	}
	//////////////////////////////////////////////////////////////////////////
}	
