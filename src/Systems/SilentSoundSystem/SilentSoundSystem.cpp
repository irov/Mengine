#include "SilentSoundSystem.h"

#include "Kernel/EnumeratorHelper.h"
#include "Kernel/Logger.h"
#include "Kernel/DocumentHelper.h"
#include "Kernel/FactoryPool.h"
#include "Kernel/AssertionFactory.h"

#include "Config/Algorithm.h"

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
        m_factorySilentSoundBuffer = Helper::makeFactoryPool<SilentSoundBuffer, 32>( MENGINE_DOCUMENT_FACTORABLE );
        m_factorySilentSoundSource = Helper::makeFactoryPool<SilentSoundSource, 32>( MENGINE_DOCUMENT_FACTORABLE );

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
    SoundSourceInterfacePtr SilentSoundSystem::createSoundSource( bool _isHeadMode, const SoundBufferInterfacePtr & _buffer, const DocumentPtr & _doc )
    {
        SilentSoundSourcePtr soundSource = m_factorySilentSoundSource->createObject( _doc );

        soundSource->initialize( this );

        soundSource->setHeadMode( _isHeadMode );
        soundSource->setSoundBuffer( _buffer );

        return soundSource;
    }
    //////////////////////////////////////////////////////////////////////////
    SoundBufferInterfacePtr SilentSoundSystem::createSoundBuffer( const SoundDecoderInterfacePtr & _soundDecoder, bool _streamable, const DocumentPtr & _doc )
    {
        SilentSoundBufferPtr buffer = m_factorySilentSoundBuffer->createObject( _doc );

        if( buffer->load( _soundDecoder, _streamable ) == false )
        {
            LOGGER_ERROR( "failed to load sound buffer from decoder" );

            return nullptr;
        }

        return buffer;
    }
    //////////////////////////////////////////////////////////////////////////
    UniqueId SilentSoundSystem::genSourceId()
    {
        UniqueId new_id = Helper::generateUniqueIdentity();

        return new_id;
    }
    //////////////////////////////////////////////////////////////////////////
    void SilentSoundSystem::releaseSourceId( UniqueId _sourceId )
    {
        MENGINE_UNUSED( _sourceId );

        //Empty
    }
    //////////////////////////////////////////////////////////////////////////
    UniqueId SilentSoundSystem::genBufferId()
    {
        UniqueId new_id = Helper::generateUniqueIdentity();

        return new_id;
    }
    //////////////////////////////////////////////////////////////////////////
    void SilentSoundSystem::releaseBufferId( UniqueId _bufferId )
    {
        MENGINE_UNUSED( _bufferId );

        //Empty
    }
    //////////////////////////////////////////////////////////////////////////
    void SilentSoundSystem::clearSourceId( UniqueId _sourceId )
    {
        MENGINE_UNUSED( _sourceId );

        //Empty
    }
    //////////////////////////////////////////////////////////////////////////
    uint32_t SilentSoundSystem::getSourcesCount() const
    {
        return 0;
    }
    //////////////////////////////////////////////////////////////////////////
    uint32_t SilentSoundSystem::getBuffersCount() const
    {
        return 0;
    }
    //////////////////////////////////////////////////////////////////////////
}
