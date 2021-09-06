#include "SoundIdentity.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    SoundIdentity::SoundIdentity()
        : m_id( 0 )
        , workerId( 0 )
        , time_left( 0.f )
        , state( ESS_STOP )
        , category( ES_SOURCE_CATEGORY_SOUND )
        , streamable( false )
        , looped( false )
        , turn( false )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    SoundIdentity::~SoundIdentity()
    {
        MENGINE_ASSERTION_FATAL( source == nullptr );
        MENGINE_ASSERTION_FATAL( listener == nullptr );
        MENGINE_ASSERTION_FATAL( worker == nullptr );
    }
    //////////////////////////////////////////////////////////////////////////
    bool SoundIdentity::initialize()
    {
        const SoundBufferInterfacePtr & soundBuffer = source->getSoundBuffer();

        if( soundBuffer->acquireSoundBuffer() == false )
        {
            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void SoundIdentity::finalize()
    {
        MENGINE_ASSERTION_FATAL( worker == nullptr );

        if( source != nullptr )
        {
            source->stop();

            const SoundBufferInterfacePtr & soundBuffer = source->getSoundBuffer();
            soundBuffer->releaseSoundBuffer();
            source = nullptr;
        }

        listener = nullptr;
        worker = nullptr;

        state = ESS_STOP;
    }
    //////////////////////////////////////////////////////////////////////////
    void SoundIdentity::setId( UniqueId _id )
    {
        m_id = _id;
    }
    //////////////////////////////////////////////////////////////////////////
    UniqueId SoundIdentity::getId() const
    {
        return m_id;
    }
    //////////////////////////////////////////////////////////////////////////
    void SoundIdentity::setSoundSource( const SoundSourceInterfacePtr & _source )
    {
        source = _source;
    }
    //////////////////////////////////////////////////////////////////////////
    const SoundSourceInterfacePtr & SoundIdentity::getSoundSource() const
    {
        return source;
    }
    //////////////////////////////////////////////////////////////////////////
    void SoundIdentity::setSoundListener( const SoundListenerInterfacePtr & _listener )
    {
        listener = _listener;
    }
    //////////////////////////////////////////////////////////////////////////
    const SoundListenerInterfacePtr & SoundIdentity::getSoundListener() const
    {
        return listener;
    }
    //////////////////////////////////////////////////////////////////////////
    SoundListenerInterfacePtr SoundIdentity::popSoundListener()
    {
        return std::move( listener );
    }
    //////////////////////////////////////////////////////////////////////////
    bool SoundIdentity::isStreamable() const
    {
        return streamable;
    }
    //////////////////////////////////////////////////////////////////////////
    bool SoundIdentity::getLoop() const
    {
        return looped;
    }
    //////////////////////////////////////////////////////////////////////////
    ESoundSourceCategory SoundIdentity::getCategory() const
    {
        return category;
    }
    //////////////////////////////////////////////////////////////////////////
    ESoundSourceState SoundIdentity::getState() const
    {
        return state;
    }
    //////////////////////////////////////////////////////////////////////////
    const MixerValue & SoundIdentity::getVolume() const
    {
        return volume;
    }
    //////////////////////////////////////////////////////////////////////////
}