#include "SilentSoundBuffer.h"

#include "Interface/SoundCodecInterface.h"
#include "Interface/TimeSystemInterface.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    SilentSoundBuffer::SilentSoundBuffer()
        : m_playTime( 0UL )
        , m_pauseTime( 0UL )
        , m_frequency( 0 )
        , m_channels( 0 )
        , m_length( 0.f )
        , m_isStereo( false )
        , m_streamable( false )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    SilentSoundBuffer::~SilentSoundBuffer()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool SilentSoundBuffer::update()
    {
        float timePos = this->getTimePos( 0U );

        if( timePos >= m_length )
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
        m_length = dataInfo->length;
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
    void SilentSoundBuffer::play( uint32_t _id, bool _looped, float _pos )
    {
        MENGINE_UNUSED( _id );
        MENGINE_UNUSED( _looped );
        MENGINE_UNUSED( _pos );

        uint64_t time = TIME_SYSTEM()
            ->getTimeMilliseconds();

        m_playTime = time;
        m_pauseTime = 0UL;
    }
    //////////////////////////////////////////////////////////////////////////
    bool SilentSoundBuffer::resume( uint32_t _id )
    {
        MENGINE_UNUSED( _id );

        uint64_t time = TIME_SYSTEM()
            ->getTimeMilliseconds();

        uint64_t deltha = m_pauseTime - m_playTime;

        m_playTime = time - deltha;
        m_pauseTime = 0UL;

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void SilentSoundBuffer::pause( uint32_t _id )
    {
        MENGINE_UNUSED( _id );

        uint64_t time = TIME_SYSTEM()
            ->getTimeMilliseconds();

        m_pauseTime = time;
    }
    //////////////////////////////////////////////////////////////////////////
    void SilentSoundBuffer::stop( uint32_t _id )
    {
        MENGINE_UNUSED( _id );

        m_playTime = 0UL;
        m_pauseTime = 0UL;
    }
    //////////////////////////////////////////////////////////////////////////
    float SilentSoundBuffer::getTimePos( uint32_t _id ) const
    {
        MENGINE_UNUSED( _id );

        if( m_pauseTime > m_playTime )
        {
            uint64_t timePos = m_pauseTime - m_playTime;

            return (float)timePos;
        }

        uint64_t time = TIME_SYSTEM()
            ->getTimeMilliseconds();

        uint64_t timePos = time - m_playTime;

        return float( timePos );
    }
    //////////////////////////////////////////////////////////////////////////
    float SilentSoundBuffer::getTimeTotal() const
    {
        return m_length;
    }
    //////////////////////////////////////////////////////////////////////////
}
