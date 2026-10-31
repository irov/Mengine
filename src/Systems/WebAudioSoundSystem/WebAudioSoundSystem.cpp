#include "WebAudioSoundSystem.h"

#include "WebAudioBridge.h"
#include "WebAudioSoundBuffer.h"
#include "WebAudioSoundSource.h"

#include "Kernel/AssertionFactory.h"
#include "Kernel/FactoryPool.h"

//////////////////////////////////////////////////////////////////////////
SERVICE_FACTORY( SoundSystem, Mengine::WebAudioSoundSystem );
//////////////////////////////////////////////////////////////////////////

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    constexpr uint32_t SOUND_BUFFER_FACTORY_SIZE = 32;
    constexpr uint32_t SOUND_SOURCE_FACTORY_SIZE = 32;
    //////////////////////////////////////////////////////////////////////////
    WebAudioSoundSystem::WebAudioSoundSystem()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    WebAudioSoundSystem::~WebAudioSoundSystem()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool WebAudioSoundSystem::_initializeService()
    {
        if( mengineAudioInitialize() == false )
        {
            return false;
        }

        m_factoryBuffer = Helper::makeFactoryPool<WebAudioSoundBuffer, SOUND_BUFFER_FACTORY_SIZE>( MENGINE_DOCUMENT_FACTORABLE );
        m_factorySource = Helper::makeFactoryPool<WebAudioSoundSource, SOUND_SOURCE_FACTORY_SIZE>( MENGINE_DOCUMENT_FACTORABLE );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void WebAudioSoundSystem::_finalizeService()
    {
        MENGINE_ASSERTION_FACTORY_EMPTY( m_factorySource );
        MENGINE_ASSERTION_FACTORY_EMPTY( m_factoryBuffer );
        m_factorySource = nullptr;
        m_factoryBuffer = nullptr;

        mengineAudioFinalize();
    }
    //////////////////////////////////////////////////////////////////////////
    void WebAudioSoundSystem::update()
    {
        //Empty
    }
    //////////////////////////////////////////////////////////////////////////
    bool WebAudioSoundSystem::isSilent() const
    {
        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    SoundBufferInterfacePtr WebAudioSoundSystem::createSoundBuffer( const SoundDecoderInterfacePtr & _decoder, bool _streamable, const DocumentInterfacePtr & _doc )
    {
        WebAudioSoundBufferPtr buffer = m_factoryBuffer->createObject( _doc );

        if( buffer->load( _decoder, _streamable ) == false )
        {
            return nullptr;
        }

        return buffer;
    }
    //////////////////////////////////////////////////////////////////////////
    SoundSourceInterfacePtr WebAudioSoundSystem::createSoundSource( bool _isHeadMode, const SoundBufferInterfacePtr & _buffer, const DocumentInterfacePtr & _doc )
    {
        MENGINE_UNUSED( _isHeadMode );

        WebAudioSoundSourcePtr source = m_factorySource->createObject( _doc );
        source->setSoundBuffer( _buffer );

        return source;
    }
    //////////////////////////////////////////////////////////////////////////
}
