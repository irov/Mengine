#include "SoundIdentity.h"

#include "Kernel/EnumeratorHelper.h"
#include "Kernel/Assertion.h"
#include "Kernel/DocumentHelper.h"
#include "Kernel/PrototypeHelper.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    SoundIdentity::SoundIdentity()
        : m_id( INVALID_UNIQUE_ID )
        , m_workerId( INVALID_UNIQUE_ID )
        , m_timeLeft( 0.f )
        , m_state( ESS_STOP )
        , m_category( ES_SOURCE_CATEGORY_SOUND )
        , m_streamable( false )
        , m_loop( false )
        , m_turn( false )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    SoundIdentity::~SoundIdentity()
    {
        MENGINE_ASSERTION_FATAL( m_source == nullptr );
        MENGINE_ASSERTION_FATAL( m_listener == nullptr );
        MENGINE_ASSERTION_FATAL( m_worker == nullptr );
    }
    //////////////////////////////////////////////////////////////////////////
    bool SoundIdentity::initialize( const SoundSourceInterfacePtr & _source, ESoundSourceCategory _category, bool _streamable, bool _turn )
    {
        UniqueId new_id = Helper::generateUniqueIdentity();

        m_id = new_id;

        m_source = _source;
        m_listener = nullptr;
        m_worker = nullptr;
        m_workerId = INVALID_UNIQUE_ID;

        m_timeLeft = 0.f;

        m_state = ESS_STOP;
        m_category = _category;

        m_streamable = _streamable;
        m_loop = false;
        m_turn = _turn;

        const SoundBufferInterfacePtr & soundBuffer = m_source->getSoundBuffer();

        if( soundBuffer->acquireSoundBuffer() == false )
        {
            return false;
        }

        m_mixerVolume = Helper::generatePrototype( MixerValue::getFactorableType(), ConstString::none(), MENGINE_DOCUMENT_FACTORABLE );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void SoundIdentity::finalize()
    {
        MENGINE_ASSERTION_FATAL( m_worker == nullptr );

        if( m_source != nullptr )
        {
            m_source->stop();

            const SoundBufferInterfacePtr & soundBuffer = m_source->getSoundBuffer();
            soundBuffer->releaseSoundBuffer();
            m_source = nullptr;
        }

        m_listener = nullptr;
        m_worker = nullptr;

        m_state = ESS_STOP;

        m_mixerVolume = nullptr;
    }
    //////////////////////////////////////////////////////////////////////////
    UniqueId SoundIdentity::getId() const
    {
        return m_id;
    }
    //////////////////////////////////////////////////////////////////////////
    const SoundSourceInterfacePtr & SoundIdentity::getSoundSource() const
    {
        return m_source;
    }
    //////////////////////////////////////////////////////////////////////////
    void SoundIdentity::setSoundListener( const SoundListenerInterfacePtr & _listener )
    {
        m_listener = _listener;
    }
    //////////////////////////////////////////////////////////////////////////
    const SoundListenerInterfacePtr & SoundIdentity::getSoundListener() const
    {
        return m_listener;
    }
    //////////////////////////////////////////////////////////////////////////
    SoundListenerInterfacePtr SoundIdentity::popSoundListener()
    {
        SoundListenerInterfacePtr listener = std::move( m_listener );

        return listener;
    }
    //////////////////////////////////////////////////////////////////////////
    void SoundIdentity::setWorkerUpdateBuffer( const ThreadWorkerInterfacePtr & _worker )
    {
        m_worker = _worker;
    }
    //////////////////////////////////////////////////////////////////////////
    const ThreadWorkerInterfacePtr & SoundIdentity::getWorkerUpdateBuffer() const
    {
        return m_worker;
    }
    //////////////////////////////////////////////////////////////////////////
    void SoundIdentity::setWorkerId( UniqueId _id )
    {
        m_workerId = _id;
    }
    //////////////////////////////////////////////////////////////////////////
    UniqueId SoundIdentity::getWorkerId() const
    {
        return m_workerId;
    }
    //////////////////////////////////////////////////////////////////////////
    void SoundIdentity::setStreamable( bool _value )
    {
        m_streamable = _value;
    }
    //////////////////////////////////////////////////////////////////////////
    bool SoundIdentity::getStreamable() const
    {
        return m_streamable;
    }
    //////////////////////////////////////////////////////////////////////////
    void SoundIdentity::setLoop( bool _value )
    {
        m_loop = _value;
    }
    //////////////////////////////////////////////////////////////////////////
    bool SoundIdentity::getLoop() const
    {
        return m_loop;
    }
    //////////////////////////////////////////////////////////////////////////
    void SoundIdentity::setTurn( bool _turn )
    {
        m_turn = _turn;
    }
    //////////////////////////////////////////////////////////////////////////
    bool SoundIdentity::getTurn() const
    {
        return m_turn;
    }
    //////////////////////////////////////////////////////////////////////////
    ESoundSourceCategory SoundIdentity::getCategory() const
    {
        return m_category;
    }
    //////////////////////////////////////////////////////////////////////////
    void SoundIdentity::setState( ESoundSourceState _state )
    {
        m_state = _state;
    }
    //////////////////////////////////////////////////////////////////////////
    ESoundSourceState SoundIdentity::getState() const
    {
        return m_state;
    }
    //////////////////////////////////////////////////////////////////////////
    void SoundIdentity::setTimeLeft( float _time )
    {
        m_timeLeft = _time;
    }
    //////////////////////////////////////////////////////////////////////////
    float SoundIdentity::getTimeLeft() const
    {
        return m_timeLeft;
    }
    //////////////////////////////////////////////////////////////////////////
    const MixerValueInterfacePtr & SoundIdentity::getMixerVolume() const
    {
        return m_mixerVolume;
    }
    //////////////////////////////////////////////////////////////////////////
}