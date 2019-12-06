#include "SoundIdentity.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    SoundIdentity::SoundIdentity()
        : m_id( 0 )
        , bufferId( 0 )
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
    }
    //////////////////////////////////////////////////////////////////////////
    bool SoundIdentity::initialize()
    {
        const SoundBufferInterfacePtr & soundBuffer = source->getSoundBuffer();

        if( soundBuffer->acquire() == false )
        {
            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void SoundIdentity::finalize()
    {
        if( source != nullptr )
        {
            const SoundBufferInterfacePtr & soundBuffer = source->getSoundBuffer();
            soundBuffer->release();
            source = nullptr;
        }

        listener = nullptr;
        worker = nullptr;
    }
    //////////////////////////////////////////////////////////////////////////
    void SoundIdentity::setId( uint32_t _id )
    {
        m_id = _id;
    }
    //////////////////////////////////////////////////////////////////////////
    uint32_t SoundIdentity::getId() const
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
}