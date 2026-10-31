#include "SoundDecoderMP3.h"

#include "Kernel/Logger.h"
#include "Kernel/Assertion.h"
#include "Kernel/AssertionMemoryPanic.h"
#include "Kernel/AssertionType.h"
#include "Kernel/AllocatorHelper.h"
#include "Kernel/ProfilerHelper.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    namespace Detail
    {
        //////////////////////////////////////////////////////////////////////////
        static size_t readMP3( void * _userData, void * _buffer, size_t _bytesToRead )
        {
            InputStreamInterface * stream = static_cast<InputStreamInterface *>(_userData);

            size_t bytesRead = stream->read( _buffer, _bytesToRead );

            return bytesRead;
        }
        //////////////////////////////////////////////////////////////////////////
        static drmp3_bool32 seekMP3( void * _userData, int _offset, drmp3_seek_origin _origin )
        {
            InputStreamInterface * stream = static_cast<InputStreamInterface *>(_userData);

            int64_t offset = _offset;

            switch( _origin )
            {
            case DRMP3_SEEK_SET:
                break;
            case DRMP3_SEEK_CUR:
                offset += (int64_t)stream->tell();
                break;
            case DRMP3_SEEK_END:
                offset += (int64_t)stream->size();
                break;
            default:
                return DRMP3_FALSE;
            }

            if( offset < 0 )
            {
                return DRMP3_FALSE;
            }

            if( stream->seek( (size_t)offset ) == false )
            {
                return DRMP3_FALSE;
            }

            return DRMP3_TRUE;
        }
        //////////////////////////////////////////////////////////////////////////
        static drmp3_bool32 tellMP3( void * _userData, drmp3_int64 * _cursor )
        {
            InputStreamInterface * stream = static_cast<InputStreamInterface *>(_userData);

            size_t position = stream->tell();

            *_cursor = (drmp3_int64)position;

            return DRMP3_TRUE;
        }
        //////////////////////////////////////////////////////////////////////////
        static void * mallocMP3( size_t _size, void * _userData )
        {
            MENGINE_UNUSED( _userData );

            void * p = Helper::allocateMemory( _size, "mp3" );

            return p;
        }
        //////////////////////////////////////////////////////////////////////////
        static void * reallocMP3( void * _ptr, size_t _size, void * _userData )
        {
            MENGINE_UNUSED( _userData );

            void * p = Helper::reallocateMemory( _ptr, _size, "mp3" );

            return p;
        }
        //////////////////////////////////////////////////////////////////////////
        static void freeMP3( void * _ptr, void * _userData )
        {
            MENGINE_UNUSED( _userData );

            Helper::deallocateMemory( _ptr, "mp3" );
        }
        //////////////////////////////////////////////////////////////////////////
    }
    //////////////////////////////////////////////////////////////////////////
    SoundDecoderMP3::SoundDecoderMP3()
        : m_mp3Initialize( false )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    SoundDecoderMP3::~SoundDecoderMP3()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool SoundDecoderMP3::_initialize()
    {
        //Empty

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void SoundDecoderMP3::_finalize()
    {
        if( m_mp3Initialize == false )
        {
            return;
        }

        m_mp3Initialize = false;

        drmp3_uninit( &m_mp3 );
    }
    //////////////////////////////////////////////////////////////////////////
    bool SoundDecoderMP3::_prepareData()
    {
        drmp3_allocation_callbacks allocationCallbacks;
        allocationCallbacks.pUserData = nullptr;
        allocationCallbacks.onMalloc = &Detail::mallocMP3;
        allocationCallbacks.onRealloc = &Detail::reallocMP3;
        allocationCallbacks.onFree = &Detail::freeMP3;

        const InputStreamInterfacePtr & stream = this->getStream();

        if( drmp3_init( &m_mp3, &Detail::readMP3, &Detail::seekMP3, &Detail::tellMP3, nullptr, stream.get(), &allocationCallbacks ) == DRMP3_FALSE )
        {
            LOGGER_ASSERTION( "invalid drmp3_init" );

            return false;
        }

        m_mp3Initialize = true;

#if defined(MENGINE_SOUND_STEREO_ONLY)
        MENGINE_ASSERTION_FATAL( m_mp3.channels == 2, "invalid channels %u need %u"
            , m_mp3.channels
            , 2
        );
#endif

        drmp3_uint64 pcmFrameCount = drmp3_get_pcm_frame_count( &m_mp3 );

        if( pcmFrameCount == 0 )
        {
            LOGGER_ERROR( "sound decoder MP3 invalid pcm frame count" );

            return false;
        }

        m_dataInfo.size = (size_t)pcmFrameCount * sizeof( drmp3_int16 ) * m_mp3.channels;
        m_dataInfo.channels = m_mp3.channels;
        m_dataInfo.frequency = m_mp3.sampleRate;
        m_dataInfo.bits = sizeof( drmp3_int16 );

        double duration = (double)pcmFrameCount * 1000.0 / (double)m_mp3.sampleRate;

        m_dataInfo.duration = (float)duration;

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool SoundDecoderMP3::_rewind()
    {
        if( drmp3_seek_to_pcm_frame( &m_mp3, 0 ) == DRMP3_FALSE )
        {
            LOGGER_ASSERTION( "invalid rewind" );

            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    size_t SoundDecoderMP3::_decode( const DecoderData * _decoderData )
    {
        MENGINE_ASSERTION_MEMORY_PANIC( _decoderData, "invalid decoder data" );
        MENGINE_ASSERTION_TYPE( _decoderData, const SoundDecoderData *, "invalid decoder data" );

        MENGINE_PROFILER_CATEGORY();

        size_t frameSize = sizeof( drmp3_int16 ) * m_mp3.channels;
        drmp3_uint64 framesToRead = _decoderData->size / frameSize;

        drmp3_int16 * buffer = static_cast<drmp3_int16 *>(_decoderData->buffer);

        drmp3_uint64 framesRead = drmp3_read_pcm_frames_s16( &m_mp3, framesToRead, buffer );

        size_t bytesRead = (size_t)framesRead * frameSize;

        return bytesRead;
    }
    //////////////////////////////////////////////////////////////////////////
    bool SoundDecoderMP3::_seek( float _time )
    {
        if( _time >= m_dataInfo.duration )
        {
            LOGGER_ASSERTION( "time %f > total %f"
                , _time
                , m_dataInfo.duration
            );

            _time = 0.f;
        }

        drmp3_uint64 frameIndex = (drmp3_uint64)((double)_time * 0.001 * (double)m_mp3.sampleRate);

        if( drmp3_seek_to_pcm_frame( &m_mp3, 0 ) == DRMP3_FALSE )
        {
            LOGGER_ASSERTION( "invalid seek to start for time %f"
                , _time
            );

            return false;
        }

        if( drmp3_seek_to_pcm_frame( &m_mp3, frameIndex ) == DRMP3_FALSE )
        {
            LOGGER_ASSERTION( "invalid seek time %f total %f"
                , _time
                , m_dataInfo.duration
            );

            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    float SoundDecoderMP3::_tell() const
    {
        drmp3_uint64 frameIndex = 0;

        if( m_mp3.currentPCMFrame > m_mp3.delayInPCMFrames )
        {
            frameIndex = m_mp3.currentPCMFrame - m_mp3.delayInPCMFrames;
        }

        double position = (double)frameIndex * 1000.0 / (double)m_mp3.sampleRate;

        float floatPosition = (float)position;

        if( floatPosition > m_dataInfo.duration )
        {
            floatPosition = m_dataInfo.duration;
        }

        return floatPosition;
    }
    //////////////////////////////////////////////////////////////////////////
}
