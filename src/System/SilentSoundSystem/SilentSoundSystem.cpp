#include "SilentSoundSystem.h"

#include "Config/Config.h"
#include "Kernel/Logger.h"

#include "Kernel/FactoryPool.h"
#include "Kernel/FactoryAssertion.h"

#include <algorithm>
#include <stdio.h>
#include <stdarg.h>

//////////////////////////////////////////////////////////////////////////
SERVICE_FACTORY( SilentSoundSystem, Mengine::SilentSoundSystem );
//////////////////////////////////////////////////////////////////////////
namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    SilentSoundSystem::SilentSoundSystem()
        : m_enumerate( 0 )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    SilentSoundSystem::~SilentSoundSystem()
    {

    }
    //////////////////////////////////////////////////////////////////////////
    bool SilentSoundSystem::_initializeService()
    {
        LOGGER_INFO( "Starting Silent Sound System..." );

        m_factorySilentSoundBuffer = new FactoryPool<SilentSoundBuffer, 32>();
        m_factorySilentSoundSource = new FactoryPool<SilentSoundSource, 32>();

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void SilentSoundSystem::_finalizeService()
    {
        MENGINE_ASSERTION_FACTORY_EMPTY( m_factorySilentSoundBuffer );
        MENGINE_ASSERTION_FACTORY_EMPTY( m_factorySilentSoundSource );

        m_factorySilentSoundBuffer = nullptr;
        m_factorySilentSoundSource = nullptr;
    }
    //////////////////////////////////////////////////////////////////////////
    void SilentSoundSystem::update()
    {
        //Empty
    }
    //////////////////////////////////////////////////////////////////////////
    bool SilentSoundSystem::isSilent() const
    {
        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void SilentSoundSystem::onTurnSound( bool _turn )
    {
        (void)_turn;
    }
    //////////////////////////////////////////////////////////////////////////
    SoundSourceInterfacePtr SilentSoundSystem::createSoundSource( bool _isHeadMode, const SoundBufferInterfacePtr & _buffer )
    {
        SilentSoundSourcePtr soundSource = m_factorySilentSoundSource->createObject();

        soundSource->initialize( this );

        soundSource->setHeadMode( _isHeadMode );
        soundSource->setSoundBuffer( _buffer );

        return soundSource;
    }
    //////////////////////////////////////////////////////////////////////////
    SoundBufferInterfacePtr SilentSoundSystem::createSoundBuffer( const SoundDecoderInterfacePtr & _soundDecoder, bool _streamable )
    {
        SilentSoundBufferPtr buffer = m_factorySilentSoundBuffer->createObject();

        if( buffer->load( _soundDecoder, _streamable ) == false )
        {
            LOGGER_ERROR( "SilentSoundSystem::createSoundBuffer: failed to load sound buffer from decoder"
            );

            return nullptr;
        }

        return buffer;
    }
    //////////////////////////////////////////////////////////////////////////
    uint32_t SilentSoundSystem::genSourceId()
    {
        uint32_t new_id = ++m_enumerate;

        return new_id;
    }
    //////////////////////////////////////////////////////////////////////////
    void SilentSoundSystem::releaseSourceId( uint32_t _sourceId )
    {
        (void)_sourceId;

        //Empty
    }
    //////////////////////////////////////////////////////////////////////////
    uint32_t SilentSoundSystem::genBufferId()
    {
        uint32_t new_id = ++m_enumerate;

        return new_id;
    }
    //////////////////////////////////////////////////////////////////////////
    void SilentSoundSystem::releaseBufferId( uint32_t _sourceId )
    {
        (void)_sourceId;

        //Empty
    }
    //////////////////////////////////////////////////////////////////////////
    void SilentSoundSystem::clearSourceId( uint32_t _sourceId )
    {
        (void)_sourceId;

        //Empty
    }
}
