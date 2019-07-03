#include "ResourceSound.h"

#include "Interface/SoundServiceInterface.h"
#include "Interface/FileServiceInterface.h"
#include "Interface/CodecInterface.h"
#include "Interface/ConverterInterface.h"
#include "Interface/ConfigServiceInterface.h"

#include "Kernel/Logger.h"
#include "Kernel/AssertionMemoryPanic.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    ResourceSound::ResourceSound()
        : m_isStreamable( false )
        , m_defaultVolume( 1.f )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    ResourceSound::~ResourceSound()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    void ResourceSound::setStreamable( bool _streamable )
    {
        m_isStreamable = _streamable;
    }
    //////////////////////////////////////////////////////////////////////////
    bool ResourceSound::isStreamable() const
    {
        return m_isStreamable;
    }
    //////////////////////////////////////////////////////////////////////////
    void ResourceSound::setDefaultVolume( float _defaultVolume )
    {
        m_defaultVolume = _defaultVolume;
    }
    //////////////////////////////////////////////////////////////////////////
    float ResourceSound::getDefaultVolume() const
    {
        return m_defaultVolume;
    }
    //////////////////////////////////////////////////////////////////////////
    bool ResourceSound::_compile()
    {
        if( m_isStreamable == false )
        {
            SoundBufferInterfacePtr soundBuffer = this->createSoundBuffer();

            MENGINE_ASSERTION_MEMORY_PANIC( soundBuffer, false );

            m_soundBufferNoStreamableCache = soundBuffer;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void ResourceSound::_release()
    {
        m_soundBufferNoStreamableCache = nullptr;
    }
    //////////////////////////////////////////////////////////////////////////
    SoundBufferInterfacePtr ResourceSound::createSoundBuffer() const
    {
        if( m_isStreamable == false && m_soundBufferNoStreamableCache != nullptr )
        {
            return m_soundBufferNoStreamableCache;
        }

        const FileGroupInterfacePtr & fileGroup = this->getFileGroup();
        const FilePath & filePath = this->getFilePath();

        SoundBufferInterfacePtr soundBuffer = SOUND_SERVICE()
            ->createSoundBufferFromFile( fileGroup, filePath, m_codecType, m_isStreamable );

        MENGINE_ASSERTION_MEMORY_PANIC( soundBuffer, nullptr, "sound '%s' group '%s' can't load sound '%s'"
            , this->getName().c_str()
            , this->getGroupName().c_str()
            , this->getFilePath().c_str()
        );

        return soundBuffer;
    }
}
