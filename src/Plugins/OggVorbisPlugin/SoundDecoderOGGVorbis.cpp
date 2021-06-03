#include "SoundDecoderOGGVorbis.h"

#include "Kernel/Logger.h"
#include "Kernel/AssertionMemoryPanic.h"

#include "Config/StdLib.h"
#include "Config/Abs.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    namespace Detail
    {
        //////////////////////////////////////////////////////////////////////////
        static size_t readOgg( void * _ptr, size_t _size, size_t _nmemb, void * _datasource )
        {
            InputStreamInterface * stream = static_cast<InputStreamInterface *>(_datasource);
            size_t count = stream->read( _ptr, _size * _nmemb );

            return count;
        }
        //////////////////////////////////////////////////////////////////////////
        static int32_t seekOgg( void * _datasource, ogg_int64_t _offset, int _whence )
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
        static long tellOgg( void * _datasource )
        {
            InputStreamInterface * stream = static_cast<InputStreamInterface *>(_datasource);

            size_t pos = stream->tell();

            long long_pos = (long)pos;

            return long_pos;
        }
        //////////////////////////////////////////////////////////////////////////
        static int32_t closeOgg( void * _datasource )
        {
            MENGINE_UNUSED( _datasource );

            return 0;
        }
        //////////////////////////////////////////////////////////////////////////
        static const Char * errorInfo( int32_t _code )
        {
            switch( _code )
            {
            case OV_EREAD: return "A read from media returned an error.";
            case OV_EFAULT: return "Internal logic fault; indicates a bug or heap/stack corruption.";
            case OV_EIMPL: return "OV_EIMPL";
            case OV_EINVAL: return "OV_EINVAL";
            case OV_ENOTVORBIS: return "Bitstream does not contain any Vorbis data.";
            case OV_EBADHEADER: return "Invalid Vorbis bitstream header.";
            case OV_EVERSION: return "Vorbis version mismatch.";
            case OV_ENOTAUDIO: return "OV_ENOTAUDIO";
            case OV_EBADPACKET: return "OV_EBADPACKET";
            case OV_EBADLINK: return "OV_EBADLINK";
            case OV_ENOSEEK: return "OV_ENOSEEK";
            }

            return "UNKNOWN";
        }
    }
    //////////////////////////////////////////////////////////////////////////
    SoundDecoderOGGVorbis::SoundDecoderOGGVorbis()
        : m_oggVorbisFileInitialize( false )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    SoundDecoderOGGVorbis::~SoundDecoderOGGVorbis()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool SoundDecoderOGGVorbis::_initialize()
    {
        //Empty

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
        vorbisCallbacks.read_func = &Detail::readOgg;
        vorbisCallbacks.seek_func = &Detail::seekOgg;
        vorbisCallbacks.tell_func = &Detail::tellOgg;
        vorbisCallbacks.close_func = &Detail::closeOgg;

        const InputStreamInterfacePtr & stream = this->getStream();

        int32_t opcall_err = ov_open_callbacks( stream.get(), &m_oggVorbisFile, nullptr, 0, vorbisCallbacks );

        if( opcall_err < 0 )
        {
            LOGGER_ERROR( "invalid ov_open_callbacks [error: %s (%d)]"
                , Detail::errorInfo( opcall_err )
                , opcall_err
            );

            return false;
        }

        m_oggVorbisFileInitialize = true;

        vorbis_info * vorbisInfo = ov_info( &m_oggVorbisFile, -1 );

        MENGINE_ASSERTION_MEMORY_PANIC( vorbisInfo, "invalid ov_info" );

#ifdef MENGINE_SOUND_STEREO_ONLY
        MENGINE_ASSERTION_FATAL( vorbisInfo->channels == 2, "invalid channels %d need %d"
            , vorbisInfo->channels
            , 2
        );
#endif

        ogg_int64_t pcmTotal = ov_pcm_total( &m_oggVorbisFile, -1 );

        size_t size_pcmTotal = (size_t)pcmTotal;

        m_dataInfo.size = size_pcmTotal * 2 * vorbisInfo->channels;
        m_dataInfo.channels = (uint32_t)vorbisInfo->channels;
        m_dataInfo.frequency = (uint32_t)vorbisInfo->rate;
        m_dataInfo.bits = 2;

        m_dataInfo.size -= (m_dataInfo.size % 4);

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

        const InputStreamInterfacePtr & stream = this->getStream();

        if( stream->seek( 0 ) == false )
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
    size_t SoundDecoderOGGVorbis::_decode( const DecoderData * _data )
    {
        long bytesDone = 0;
        int32_t bytesReading = (int32_t)_data->size;

        for( ;;)
        {
            int32_t current_section = 0;
            char * readBuffer = (char *)_data->buffer + bytesDone;
            long decodeSize = ov_read( &m_oggVorbisFile, readBuffer, bytesReading, 0, 2, 1, &current_section );

            if( decodeSize == OV_HOLE )
            {
                LOGGER_ERROR( "ov_read return OV_HOLE [size %ld]"
                    , decodeSize
                );

                bytesDone = 0;
                break;
            }

            if( decodeSize < 0 )
            {
                LOGGER_ERROR( "invalid ov_read [error: %s (%ld)]"
                    , Detail::errorInfo( (int32_t)decodeSize )
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
        }

        return bytesDone;
    }
    //////////////////////////////////////////////////////////////////////////
    bool SoundDecoderOGGVorbis::_seek( float _time )
    {
        if( _time >= m_dataInfo.length )
        {
            LOGGER_ERROR( "time %f > total %f"
                , _time
                , m_dataInfo.length
            );

            _time = 0.f;
        }

        double al_time = ov_time_tell( &m_oggVorbisFile );
        double al_seek = (double)(_time) * 0.001;

        if( MENGINE_ABS( al_time - al_seek ) < 0.001 )
        {
            return true;
        }

        int32_t seek_err = ov_time_seek( &m_oggVorbisFile, al_seek );

        if( seek_err == OV_EINVAL )
        {
            seek_err = ov_time_seek( &m_oggVorbisFile, 0.0 );
        }

        if( seek_err != 0 )
        {
            //OV_ENOSEEK
            LOGGER_ERROR( "time %f is %f [error: %s (%d)]"
                , _time
                , m_dataInfo.length
                , Detail::errorInfo( seek_err )
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
