#include "ResourceSound.h"

#include "Interface/SoundSystemInterface.h"
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
    void ResourceSound::setFilePath( const FilePath& _path )
    {
        m_filePath = _path;
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
    const FilePath & ResourceSound::getFilePath() const
    {
        return m_filePath;
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
    bool ResourceSound::_isValid() const
    {
        const FileGroupInterfacePtr & category = this->getCategory();

        InputStreamInterfacePtr stream = FILE_SERVICE()
            ->openInputFile( category, m_filePath, m_isStreamable );

        if( stream == nullptr )
        {
            LOGGER_ERROR( "ResourceSound::_isValid: '%s' group %s can't open sound file '%s:%s'"
                , this->getName().c_str()
                , this->getGroupName().c_str()
                , this->getCategory()->getName().c_str()
                , this->getFilePath().c_str()
            );

            return false;
        }

        SoundDecoderInterfacePtr decoder = CODEC_SERVICE()
            ->createDecoderT<SoundDecoderInterfacePtr>( m_codecType );

        if( decoder == nullptr )
        {
            LOGGER_ERROR( "SoundEngine::_isValid: '%s' group '%s' can't create sound decoder for file '%s:%s'"
                , this->getName().c_str()
                , this->getGroupName().c_str()
                , this->getCategory()->getName().c_str()
                , this->getFilePath().c_str()
            );

            return false;
        }

        if( decoder->prepareData( stream ) == false )
        {
            LOGGER_ERROR( "SoundEngine::_isValid: '%s' group '%s' can't initialize sound decoder for file '%s:%s'"
                , this->getName().c_str()
                , this->getGroupName().c_str()
                , this->getCategory()->getName().c_str()
                , this->getFilePath().c_str()
            );

            return false;
        }

        const SoundCodecDataInfo * dataInfo = decoder->getCodecDataInfo();

        float limitMinimalStreamSoundDuration = CONFIG_VALUE( "Limit", "MinimalStreamSoundDuration", 500.f ); //4kb

        if( (dataInfo->length <= limitMinimalStreamSoundDuration && limitMinimalStreamSoundDuration != 0.f) && m_isStreamable == true )
        {
            LOGGER_ERROR( "SoundEngine::_isValid: '%s' group '%s' remove stream (time %.4f <= %.4f ms)\nfile - '%s:%s'\nAdd <IsStreamable Value=\"0\"/>"
                , this->getName().c_str()
                , this->getGroupName().c_str()
                , dataInfo->length
                , limitMinimalStreamSoundDuration
                , this->getCategory()->getName().c_str()
                , this->getFilePath().c_str()
            );

            return false;
        }

        float limitNoStreamSoundDurationWarning = CONFIG_VALUE( "Limit", "NoStreamSoundDurationWarning", 2000.f ); //4kb

        if( (dataInfo->length > limitNoStreamSoundDurationWarning && limitNoStreamSoundDurationWarning != 0.f) && m_isStreamable == false )
        {
            LOGGER_WARNING( "SoundEngine::_isValid: '%s' group '%s' setup to stream (time %.4f > %.4f ms)\nfile - '%s:%s'\nAdd <IsStreamable Value=\"1\"/>"
                , this->getName().c_str()
                , this->getGroupName().c_str()
                , dataInfo->length
                , limitNoStreamSoundDurationWarning
                , this->getCategory()->getName().c_str()
                , this->getFilePath().c_str()
            );
        }

        float limitNoStreamSoundDurationError = CONFIG_VALUE( "Limit", "NoStreamSoundDurationError", 10000.f ); //4kb

        if( (dataInfo->length > limitNoStreamSoundDurationError && limitNoStreamSoundDurationError != 0.f) && m_isStreamable == false )
        {
            LOGGER_ERROR( "SoundEngine::_isValid: '%s' group '%s' setup to stream (time %.4f > %.4f ms)\nfile - '%s:%s'\nAdd <IsStreamable Value=\"1\"/>"
                , this->getName().c_str()
                , this->getGroupName().c_str()
                , dataInfo->length
                , limitNoStreamSoundDurationError
                , this->getCategory()->getName().c_str()
                , this->getFilePath().c_str()
            );

            return false;
        }

        decoder = nullptr;
        stream = nullptr;

        SoundBufferInterfacePtr buffer = this->createSoundBuffer();

        if( buffer == nullptr )
        {
            LOGGER_ERROR( "SoundEngine::isValid '%s' group '%s' can't create buffer '%s'"
                , this->getName().c_str()
                , this->getGroupName().c_str()
                , this->getFilePath().c_str()
            );

            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    SoundBufferInterfacePtr ResourceSound::createSoundBuffer() const
    {
        if( m_isStreamable == false && m_soundBufferNoStreamableCache != nullptr )
        {
            return m_soundBufferNoStreamableCache;
        }

        const FileGroupInterfacePtr & category = this->getCategory();

        SoundBufferInterfacePtr soundBuffer = SOUND_SERVICE()
            ->createSoundBufferFromFile( category, m_filePath, m_codecType, m_isStreamable );

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
