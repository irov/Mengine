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
		, m_soundBuffer(0)
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
        if( m_path.empty() == true )
        {
            return false;
        }

        if ( m_converter.empty() == false )
        {
            if( this->convert_() == false )
            {
                return false;
            }
        }

        if( m_codec.empty() == true )
        {
            m_codec = CODEC_SERVICE(m_serviceProvider)
                ->findCodecType( m_path );
        }

        if( m_codec.empty() == true )
        {
            LOGGER_ERROR(m_serviceProvider)( "ResourceSound: '%s' you must determine codec for file '%s'"
                , this->getName().c_str()
                , m_path.c_str()
                );

            return false;
        }

        return true;
    }
	//////////////////////////////////////////////////////////////////////////
	bool ResourceSound::_compile()
	{   
		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	bool ResourceSound::convert_()
	{
        const ConstString & category = this->getCategory();

        if( CONVERTER_SERVICE(m_serviceProvider)
            ->convert( m_converter, category, m_path, m_path ) == false )
        {
            LOGGER_ERROR(m_serviceProvider)( "ResourceSound::convert: '%s' can't convert '%s':'%s'"
                , this->getName().c_str() 
                , m_path.c_str()
                , m_converter.c_str()
                );

            return false;
        }

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void ResourceSound::_release()
	{
	}
    //////////////////////////////////////////////////////////////////////////
    bool ResourceSound::_isValid() const
    {
        const ConstString & category = this->getCategory();

        InputStreamInterfacePtr stream = FILE_SERVICE(m_serviceProvider)
            ->openInputFile( category, m_path );

        if( stream == nullptr )
        {
            LOGGER_ERROR(m_serviceProvider)( "ResourceSound::_isValid: %s can't open sound file %s:%s"
                , m_name.c_str()
                , category.c_str()
                , m_path.c_str() 
                );

            return false;
        }

		bool version;
        SoundDecoderInterfacePtr codec = CODEC_SERVICE(m_serviceProvider)
            ->createDecoderT<SoundDecoderInterfacePtr>( m_codec, stream, version );

        if( codec == nullptr )
        {
            LOGGER_ERROR(m_serviceProvider)( "SoundEngine::_isValid: %s can't create sound decoder for file %s:%s"
                , m_name.c_str()
                , category.c_str()
                , m_path.c_str() 
                );

            return false;
        }

        const SoundCodecDataInfo * dataInfo = codec->getCodecDataInfo();

        if( dataInfo->length > 500.f && m_isStreamable == false )
        {
            LOGGER_ERROR(m_serviceProvider)("SoundEngine::_isValid: %s setup to stream (time %.4f > 500.0 ms)\nfile - %s:%s"
                , m_name.c_str()
                , dataInfo->length
                , category.c_str()
                , m_path.c_str() 
                );
        }

        codec = nullptr;
        stream = nullptr;

        SoundBufferInterface * buffer = this->createSoundBuffer();

        if( buffer == nullptr )
        {
            LOGGER_ERROR(m_serviceProvider)( "SoundEngine::isValid '%s' can't create buffer '%s'"
                , this->getName().c_str()
                , m_path.c_str()
                );

            return false;
        }

        buffer->destroy();

        return true;
    }
	//////////////////////////////////////////////////////////////////////////
	SoundBufferInterface * ResourceSound::createSoundBuffer() const
	{
        const ConstString & category = this->getCategory();

        SoundBufferInterface * soundBuffer = SOUND_SERVICE(m_serviceProvider)
            ->createSoundBufferFromFile( category, m_path, m_codec, m_isStreamable );

        if( soundBuffer == nullptr )
        {
            LOGGER_ERROR(m_serviceProvider)( "ResourceSound::createSoundBuffer: '%s' can't load sound '%s'"
                , this->getName().c_str() 
                , m_path.c_str()
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
