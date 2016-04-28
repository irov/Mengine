#	include "ResourceSound.h"

#	include "Interface/SoundSystemInterface.h"
#   include "Interface/FileSystemInterface.h"
#   include "Interface/CodecInterface.h"
#   include "Interface/ConverterInterface.h"
#   include "Interface/ConfigInterface.h"

#	include "Metacode/Metacode.h"

#   include "Logger/Logger.h"

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	ResourceSound::ResourceSound()
		: m_isStreamable(false)
		, m_defaultVolume(1.f)
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
        const Metacode::Meta_DataBlock::Meta_ResourceSound * metadata 
            = static_cast<const Metacode::Meta_DataBlock::Meta_ResourceSound *>(_meta);

        metadata->swap_File_Path( m_filePath );
        metadata->swap_File_Codec( m_codecType );
        metadata->swap_File_Converter( m_converter );

        metadata->get_DefaultVolume_Value( m_defaultVolume );
        metadata->get_IsStreamable_Value( m_isStreamable );

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
		bool result = this->convertDefault_( m_converter, m_filePath, m_filePath, m_codecType );

        return result;
    }
	//////////////////////////////////////////////////////////////////////////
	bool ResourceSound::_compile()
	{   
		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void ResourceSound::_release()
	{
		m_soundBufferCacher.clear();
	}
    //////////////////////////////////////////////////////////////////////////
    bool ResourceSound::_isValid() const
    {
        const ConstString & category = this->getCategory();

        InputStreamInterfacePtr stream = FILE_SERVICE(m_serviceProvider)
            ->openInputFile( category, m_filePath, m_isStreamable );

        if( stream == nullptr )
        {
            LOGGER_ERROR(m_serviceProvider)("ResourceSound::_isValid: '%s' group %s can't open sound file '%s:%s'"
				, this->getName().c_str()
				, this->getGroup().c_str()
                , category.c_str()
                , m_filePath.c_str() 
                );

            return false;
        }

        SoundDecoderInterfacePtr decoder = CODEC_SERVICE(m_serviceProvider)
            ->createDecoderT<SoundDecoderInterfacePtr>( m_codecType );

        if( decoder == nullptr )
        {
            LOGGER_ERROR(m_serviceProvider)("SoundEngine::_isValid: '%s' group '%s' can't create sound decoder for file '%s:%s'"
				, this->getName().c_str()
				, this->getGroup().c_str()
                , category.c_str()
                , m_filePath.c_str() 
                );

            return false;
        }

		if( decoder->prepareData( stream ) == false )
		{
			LOGGER_ERROR( m_serviceProvider )("SoundEngine::_isValid: '%s' group '%s' can't initialize sound decoder for file '%s:%s'"
				, this->getName().c_str()
				, this->getGroup().c_str()
				, category.c_str()
				, m_filePath.c_str() 
				);

			return false;
		}

        const SoundCodecDataInfo * dataInfo = decoder->getCodecDataInfo();

		float limitMinimalStreamSoundDuration = CONFIG_VALUE( m_serviceProvider, "Limit", "MinimalStreamSoundDuration", 500.f ); //4kb

		if( (dataInfo->length <= limitMinimalStreamSoundDuration && limitMinimalStreamSoundDuration != 0.f) && m_isStreamable == true )
		{
			LOGGER_ERROR( m_serviceProvider )("SoundEngine::_isValid: '%s' group '%s' remove stream (time %.4f <= %.4f ms)\nfile - '%s:%s'\nAdd <IsStreamable Value=\"0\"/>"
				, this->getName().c_str()
				, this->getGroup().c_str()
				, dataInfo->length
				, limitMinimalStreamSoundDuration
				, category.c_str()
				, m_filePath.c_str()
				);

			return false;
		}

		float limitNoStreamSoundDurationWarning = CONFIG_VALUE( m_serviceProvider, "Limit", "NoStreamSoundDurationWarning", 2000.f ); //4kb
						
		if( (dataInfo->length > limitNoStreamSoundDurationWarning && limitNoStreamSoundDurationWarning != 0.f) && m_isStreamable == false )
		{
			LOGGER_WARNING( m_serviceProvider )("SoundEngine::_isValid: '%s' group '%s' setup to stream (time %.4f > %.4f ms)\nfile - '%s:%s'\nAdd <IsStreamable Value=\"1\"/>"
				, this->getName().c_str()
				, this->getGroup().c_str()
				, dataInfo->length
				, limitNoStreamSoundDurationWarning
				, category.c_str()
				, m_filePath.c_str()
				);
		}

		float limitNoStreamSoundDurationError = CONFIG_VALUE( m_serviceProvider, "Limit", "NoStreamSoundDurationError", 10000.f ); //4kb
		
		if( (dataInfo->length > limitNoStreamSoundDurationError && limitNoStreamSoundDurationError != 0.f) && m_isStreamable == false )
		{
			LOGGER_ERROR( m_serviceProvider )("SoundEngine::_isValid: '%s' group '%s' setup to stream (time %.4f > %.4f ms)\nfile - '%s:%s'\nAdd <IsStreamable Value=\"1\"/>"
				, this->getName().c_str()
				, this->getGroup().c_str()
				, dataInfo->length
				, limitNoStreamSoundDurationError
				, category.c_str()
				, m_filePath.c_str()
				);

			return false;
		}

        decoder = nullptr;
        stream = nullptr;
		
        SoundBufferInterfacePtr buffer = this->createSoundBufferNoCache();

        if( buffer == nullptr )
        {
            LOGGER_ERROR(m_serviceProvider)("SoundEngine::isValid '%s' group '%s' can't create buffer '%s'"
                , this->getName().c_str()
				, this->getGroup().c_str()
                , m_filePath.c_str()
                );

            return false;
        }
				
        return true;
    }
	//////////////////////////////////////////////////////////////////////////
	SoundBufferInterfacePtr ResourceSound::createSoundBuffer() const
	{
		SoundBufferInterfacePtr cacheSoundBuffer = m_soundBufferCacher.findCache();

		if( cacheSoundBuffer != nullptr )
		{
			const SoundDecoderInterfacePtr & decode = cacheSoundBuffer->getDecoder();

			if( decode != nullptr )
			{
				if( decode->rewind() == false )
				{
					return nullptr;
				}
			}

			return cacheSoundBuffer;
		}

        SoundBufferInterfacePtr soundBuffer = this->createSoundBufferNoCache();

		m_soundBufferCacher.addCache( soundBuffer );

        return soundBuffer;
	}
	//////////////////////////////////////////////////////////////////////////
	SoundBufferInterfacePtr ResourceSound::createSoundBufferNoCache() const
	{
		const ConstString & category = this->getCategory();

		SoundBufferInterfacePtr soundBuffer = SOUND_SERVICE(m_serviceProvider)
			->createSoundBufferFromFile( category, m_filePath, m_codecType, m_isStreamable );

		if( soundBuffer == nullptr )
		{
			LOGGER_ERROR(m_serviceProvider)("ResourceSound::createSoundBuffer: '%s' group '%s' can't load sound '%s'"
				, this->getName().c_str()
				, this->getGroup().c_str()
				, m_filePath.c_str()
				);

			return nullptr;
		}

		return soundBuffer;
	}
	//////////////////////////////////////////////////////////////////////////
	void ResourceSound::destroySoundBuffer( const SoundBufferInterfacePtr & _soundBuffer ) const
	{
		m_soundBufferCacher.removeCache( _soundBuffer );
	}
	//////////////////////////////////////////////////////////////////////////
	void ResourceSound::_cache()
	{
		m_soundBufferCacher.lock();
	}
	//////////////////////////////////////////////////////////////////////////
	void ResourceSound::_uncache()
	{
		m_soundBufferCacher.unlock();
	}
	//////////////////////////////////////////////////////////////////////////
	float ResourceSound::getDefaultVolume() const
	{
		return m_defaultVolume;
	}
}
