#	include "ResourceSound.h"

#	include "Kernel/ResourceImplement.h"

#	include "Interface/SoundSystemInterface.h"
#   include "Interface/FileSystemInterface.h"
#   include "Interface/LogSystemInterface.h"
#   include "Interface/CodecInterface.h"
#   include "Interface/ConverterInterface.h"

#	include "Metacode.h"

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
            m_codec = this->getCodec_( m_path );
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
	SoundBufferInterface * ResourceSound::createSoundBuffer() const
	{
        const ConstString & category = this->getCategory();

        SoundBufferInterface * soundBuffer = SOUND_SERVICE(m_serviceProvider)
            ->createSoundBufferFromFile( category, m_path, m_codec, m_isStreamable );

        if( soundBuffer == 0 )
        {
            LOGGER_ERROR(m_serviceProvider)( "ResourceSound::createSoundBuffer: '%s' can't load sound '%s'"
                , this->getName().c_str() 
                , m_path.c_str()
                );

            return NULL;			
        }

        return soundBuffer;
	}
    //////////////////////////////////////////////////////////////////////////
    void ResourceSound::releaseSoundBuffer( SoundBufferInterface * _soundBufer ) const
    {
        SOUND_SERVICE(m_serviceProvider)
            ->releaseSoundBuffer( _soundBufer );
    }
	//////////////////////////////////////////////////////////////////////////
	float ResourceSound::getDefaultVolume() const
	{
		return m_defaultVolume;
	}
}
