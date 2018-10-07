#include "ResourceSound.h"

#include "Interface/SoundServiceInterface.h"
#include "Interface/FileSystemInterface.h"
#include "Interface/CodecInterface.h"
#include "Interface/ConverterInterface.h"
#include "Interface/ConfigInterface.h"

#include "Metacode/Metacode.h"

#include "Kernel/Logger.h"

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
    void ResourceSound::setFilePath( const FilePath& _path )
    {
        m_filePath = _path;
    }
    //////////////////////////////////////////////////////////////////////////
    const FilePath & ResourceSound::getFilePath() const
    {
        return m_filePath;
    }
    //////////////////////////////////////////////////////////////////////////
    void ResourceSound::setCodecType( const ConstString& _codec )
    {
        m_codecType = _codec;
    }
    //////////////////////////////////////////////////////////////////////////
    const ConstString & ResourceSound::getCodecType() const
    {
        return m_codecType;
    }
    //////////////////////////////////////////////////////////////////////////
    bool ResourceSound::_loader( const Metabuf::Metadata * _meta )
    {
        const Metacode::Meta_Data::Meta_DataBlock::Meta_ResourceSound * metadata
            = static_cast<const Metacode::Meta_Data::Meta_DataBlock::Meta_ResourceSound *>(_meta);

        m_filePath = metadata->get_File_Path();

        metadata->get_File_Codec( &m_codecType );
        metadata->get_File_Converter( &m_converterType );

        metadata->get_DefaultVolume_Value( &m_defaultVolume );
        metadata->get_IsStreamable_Value( &m_isStreamable );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool ResourceSound::isStreamable() const
    {
        return m_isStreamable;
    }
    //////////////////////////////////////////////////////////////////////////
    bool ResourceSound::_convert()
    {
        bool result = this->convertDefault_( m_converterType, m_filePath, m_filePath, m_codecType );

        return result;
    }
    //////////////////////////////////////////////////////////////////////////
    bool ResourceSound::_compile()
    {
        if( m_isStreamable == false )
        {
            SoundBufferInterfacePtr soundBuffer = this->createSoundBuffer();

            if( soundBuffer == nullptr )
            {
                return false;
            }

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
    void ResourceSound::_debugIncrementReference()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    void ResourceSound::_debugDecrementReference()
    {
    }    
    //////////////////////////////////////////////////////////////////////////
    SoundBufferInterfacePtr ResourceSound::createSoundBuffer() const
    {
        if( m_isStreamable == false && m_soundBufferNoStreamableCache != nullptr )
        {
            return m_soundBufferNoStreamableCache;
        }

        const FileGroupInterfacePtr & fileGroup = this->getFileGroup();

        SoundBufferInterfacePtr soundBuffer = SOUND_SERVICE()
            ->createSoundBufferFromFile( fileGroup, m_filePath, m_codecType, m_isStreamable );

        if( soundBuffer == nullptr )
        {
            LOGGER_ERROR( "ResourceSound::createSoundBuffer: '%s' group '%s' can't load sound '%s'"
                , this->getName().c_str()
                , this->getGroupName().c_str()
                , this->getFilePath().c_str()
            );

            return nullptr;
        }

        return soundBuffer;
    }
    //////////////////////////////////////////////////////////////////////////
    float ResourceSound::getDefaultVolume() const
    {
        return m_defaultVolume;
    }
}
