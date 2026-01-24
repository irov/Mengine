#include "SilentSoundBuffer.h"

#include "Interface/SoundCodecInterface.h"

#include "Kernel/TimestampHelper.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    SilentSoundBuffer::SilentSoundBuffer()
        : m_playTime( MENGINE_UINT64_C(0) )
        , m_pauseTime( MENGINE_UINT64_C(0) )
        , m_frequency( 0 )
        , m_channels( 0 )
        , m_duration( 0.f )
        , m_isStereo( false )
        , m_streamable( false )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    SilentSoundBuffer::~SilentSoundBuffer()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool SilentSoundBuffer::acquireSoundBuffer()
    {
        //Empty

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void SilentSoundBuffer::releaseSoundBuffer()
    {
        //Empty
    }
    //////////////////////////////////////////////////////////////////////////
    bool SilentSoundBuffer::updateSoundBuffer()
    {
        float timePos = this->getTimePosition( MENGINE_UINT64_C(0) );

        if( timePos >= m_duration )
        {
            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    const SoundDecoderInterfacePtr & SilentSoundBuffer::getDecoder() const
    {
        return m_soundDecoder;
    }
    //////////////////////////////////////////////////////////////////////////
    bool SilentSoundBuffer::load( const SoundDecoderInterfacePtr & _soundDecoder, bool _streamable )
    {
        m_soundDecoder = _soundDecoder;

        const SoundCodecDataInfo * dataInfo = m_soundDecoder->getCodecDataInfo();

        m_frequency = dataInfo->frequency;
        m_channels = dataInfo->channels;
        m_duration = dataInfo->duration;
        //size_t size = dataInfo->size;

        if( m_channels == 1 )
        {
            m_isStereo = false;
        }
        else
        {
            m_isStereo = true;
        }

        m_streamable = _streamable;

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void SilentSoundBuffer::play( UniqueId _id, bool _looped, float _position )
    {
        MENGINE_UNUSED( _id );
        MENGINE_UNUSED( _looped );
        MENGINE_UNUSED( _position );

        Timestamp time = Helper::getSystemTimestamp();

        m_playTime = time;
        m_pauseTime = MENGINE_UINT64_C(0);
    }
    //////////////////////////////////////////////////////////////////////////
    bool SilentSoundBuffer::resume( UniqueId _id )
    {
        MENGINE_UNUSED( _id );

        Timestamp time = Helper::getSystemTimestamp();

        Timestamp delta = m_pauseTime - m_playTime;

        m_playTime = time - delta;
        m_pauseTime = MENGINE_UINT64_C(0);

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void SilentSoundBuffer::pause( UniqueId _id )
    {
        MENGINE_UNUSED( _id );

        Timestamp time = Helper::getSystemTimestamp();

        m_pauseTime = time;
    }
    //////////////////////////////////////////////////////////////////////////
    void SilentSoundBuffer::stop( UniqueId _id )
    {
        MENGINE_UNUSED( _id );

        m_playTime = MENGINE_UINT64_C(0);
        m_pauseTime = MENGINE_UINT64_C(0);
    }
    //////////////////////////////////////////////////////////////////////////
    float SilentSoundBuffer::getTimePosition( UniqueId _id ) const
    {
        MENGINE_UNUSED( _id );

        if( m_pauseTime > m_playTime )
        {
            uint64_t timePosition = m_pauseTime - m_playTime;

            return (float)timePosition;
        }

        Timestamp time = Helper::getSystemTimestamp();

        Timestamp timePosition = time - m_playTime;

        return float( timePosition );
    }
    //////////////////////////////////////////////////////////////////////////
    float SilentSoundBuffer::getTimeDuration() const
    {
        return m_duration;
    }
    //////////////////////////////////////////////////////////////////////////
}
