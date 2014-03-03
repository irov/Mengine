#	include "ResourceSound.h"

#	include "Kernel/ResourceImplement.h"

#	include "Interface/SoundSystemInterface.h"
#   include "Interface/FileSystemInterface.h"
#   include "Interface/LogSystemInterface.h"
#   include "Interface/CodecInterface.h"
#   include "Interface/ConverterInterface.h"

#	include "Core/File.h"

#   include "Logger/Logger.h"


namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	RESOURCE_IMPLEMENT( ResourceSound );
	//////////////////////////////////////////////////////////////////////////
	ResourceSound::ResourceSound()
		: m_isStreamable(false)
		, m_defaultVolume(1.f)
	{
	}
	//////////////////////////////////////////////////////////////////////////
	void ResourceSound::setPath( const FilePath& _path )
	{
		m_path = _path;
	}
	//////////////////////////////////////////////////////////////////////////
	void ResourceSound::setCodec( const ConstString& _codec )
	{
		m_codec = _codec;
	}
	//////////////////////////////////////////////////////////////////////////
	void ResourceSound::setConverter( const ConstString& _converter )
	{
		m_converter = _converter;
	}
	//////////////////////////////////////////////////////////////////////////
	bool ResourceSound::_loader( const Metabuf::Metadata * _meta )
	{
        const Metacode::Meta_DataBlock::Meta_ResourceSound * metadata 
            = static_cast<const Metacode::Meta_DataBlock::Meta_ResourceSound *>(_meta);

        metadata->swap_File_Path( m_path );
        metadata->swap_File_Codec( m_codec );
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
	const FilePath & ResourceSound::getPath() const
	{
		return m_path;
	}
    //////////////////////////////////////////////////////////////////////////
    bool ResourceSound::_convert()
    {
		bool result = this->convertDefault_( m_converter, m_path, m_path, m_codec );

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
            ->openInputFile( category, m_path );

        if( stream == nullptr )
        {
            LOGGER_ERROR(m_serviceProvider)("ResourceSound::_isValid: %s can't open sound file %s:%s"
                , m_name.c_str()
                , category.c_str()
                , m_path.c_str() 
                );

            return false;
        }

        SoundDecoderInterfacePtr decoder = CODEC_SERVICE(m_serviceProvider)
            ->createDecoderT<SoundDecoderInterfacePtr>( m_codec );

        if( decoder == nullptr )
        {
            LOGGER_ERROR(m_serviceProvider)("SoundEngine::_isValid: %s can't create sound decoder for file %s:%s"
                , m_name.c_str()
                , category.c_str()
                , m_path.c_str() 
                );

            return false;
        }

		if( decoder->initialize( stream ) == false )
		{
			LOGGER_ERROR(m_serviceProvider)("SoundEngine::_isValid: %s can't initialize sound decoder for file %s:%s"
				, m_name.c_str()
				, category.c_str()
				, m_path.c_str() 
				);

			return false;
		}

        const SoundCodecDataInfo * dataInfo = decoder->getCodecDataInfo();

        if( dataInfo->length > 500.f && m_isStreamable == false )
        {
            LOGGER_ERROR(m_serviceProvider)("SoundEngine::_isValid: %s setup to stream (time %.4f > 500.0 ms)\nfile - %s:%s"
                , m_name.c_str()
                , dataInfo->length
                , category.c_str()
                , m_path.c_str() 
                );
        }

        decoder = nullptr;
        stream = nullptr;
		
        SoundBufferInterfacePtr buffer = this->createSoundBufferNoCache();

        if( buffer == nullptr )
        {
            LOGGER_ERROR(m_serviceProvider)("SoundEngine::isValid '%s' can't create buffer '%s'"
                , this->getName().c_str()
                , m_path.c_str()
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
			->createSoundBufferFromFile( category, m_path, m_codec, m_isStreamable );

		if( soundBuffer == nullptr )
		{
			LOGGER_ERROR(m_serviceProvider)("ResourceSound::createSoundBuffer: '%s' can't load sound '%s'"
				, this->getName().c_str() 
				, m_path.c_str()
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
