#include "SilentSoundSystem.h"

#include "Interface/EnumeratorServiceInterface.h"

#include "Kernel/Logger.h"
#include "Kernel/Document.h"
#include "Kernel/FactoryPool.h"
#include "Kernel/AssertionFactory.h"

#include <algorithm>

//////////////////////////////////////////////////////////////////////////
SERVICE_FACTORY( SilentSoundSystem, Mengine::SilentSoundSystem );
//////////////////////////////////////////////////////////////////////////
namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    SilentSoundSystem::SilentSoundSystem()
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
        MENGINE_UNUSED( _turn );
    }
    //////////////////////////////////////////////////////////////////////////
    SoundSourceInterfacePtr SilentSoundSystem::createSoundSource( bool _isHeadMode, const SoundBufferInterfacePtr & _buffer )
    {
        SilentSoundSourcePtr soundSource = m_factorySilentSoundSource->createObject( MENGINE_DOCUMENT_FUNCTION );

        soundSource->initialize( this );

        soundSource->setHeadMode( _isHeadMode );
        soundSource->setSoundBuffer( _buffer );

        return soundSource;
    }
    //////////////////////////////////////////////////////////////////////////
    SoundBufferInterfacePtr SilentSoundSystem::createSoundBuffer( const SoundDecoderInterfacePtr & _soundDecoder, bool _streamable )
    {
        SilentSoundBufferPtr buffer = m_factorySilentSoundBuffer->createObject( MENGINE_DOCUMENT_FUNCTION );

        if( buffer->load( _soundDecoder, _streamable ) == false )
        {
            LOGGER_ERROR( "failed to load sound buffer from decoder"
            );

            return nullptr;
        }

        return buffer;
    }
    //////////////////////////////////////////////////////////////////////////
    uint32_t SilentSoundSystem::genSourceId()
    {
        uint32_t new_id = GENERATE_UNIQUE_IDENTITY();

        return new_id;
    }
    //////////////////////////////////////////////////////////////////////////
    void SilentSoundSystem::releaseSourceId( uint32_t _sourceId )
    {
        MENGINE_UNUSED( _sourceId );

        //Empty
    }
    //////////////////////////////////////////////////////////////////////////
    uint32_t SilentSoundSystem::genBufferId()
    {
        uint32_t new_id = GENERATE_UNIQUE_IDENTITY();

        return new_id;
    }
    //////////////////////////////////////////////////////////////////////////
    void SilentSoundSystem::releaseBufferId( uint32_t _bufferId )
    {
        MENGINE_UNUSED( _bufferId );

        //Empty
    }
    //////////////////////////////////////////////////////////////////////////
    void SilentSoundSystem::clearSourceId( uint32_t _sourceId )
    {
        MENGINE_UNUSED( _sourceId );

        //Empty
    }
}
