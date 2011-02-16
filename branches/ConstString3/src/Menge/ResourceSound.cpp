#	include "ResourceSound.h"

#	include "ResourceImplement.h"

#	include "Logger/Logger.h"
#	include "SoundEngine.h"
#	include "BinParser.h"

#	include "Utils/Core/File.h"


namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	RESOURCE_IMPLEMENT( ResourceSound );
	//////////////////////////////////////////////////////////////////////////
	ResourceSound::ResourceSound()
		: m_isStreamable(false)
		, m_interface(0)
		, m_defaultVolume(1.f)
	{
	}
	//////////////////////////////////////////////////////////////////////////
	void ResourceSound::setPath( const ConstString& _path )
	{
		m_path = _path;
	}
	//////////////////////////////////////////////////////////////////////////
	void ResourceSound::setCodec( const ConstString& _codec )
	{
		m_codec = _codec;
	}
	//////////////////////////////////////////////////////////////////////////
	void ResourceSound::loader( BinParser * _parser )
	{
		ResourceReference::loader( _parser );

		BIN_SWITCH_ID(_parser)
		{
			BIN_CASE_ATTRIBUTE_METHOD( Protocol::File_Path, &ResourceSound::setPath );
			BIN_CASE_ATTRIBUTE_METHOD( Protocol::File_Codec, &ResourceSound::setCodec );
			BIN_CASE_ATTRIBUTE( Protocol::DefaultVolume_Value, m_defaultVolume);

			BIN_CASE_ATTRIBUTE( Protocol::IsStreamable_Value, m_isStreamable);
		}
	}
	//////////////////////////////////////////////////////////////////////////
	bool ResourceSound::isStreamable() const
	{
		return m_isStreamable;
	}
	//////////////////////////////////////////////////////////////////////////
	const ConstString& ResourceSound::getFilename() const
	{
		return m_path;
	}
	//////////////////////////////////////////////////////////////////////////
	bool ResourceSound::_compile()
	{
		const ConstString & category = this->getCategory();

		if( m_codec.empty() == true )
		{
			String codecType;
			Utils::getFileExt( codecType, m_path );
			codecType += "Sound";

			m_codec = ConstString(codecType);	
		}

		m_interface = SoundEngine::get()
			->createSoundBufferFromFile( category, m_path, m_codec, m_isStreamable );

		if( m_interface == 0 )
		{
			MENGE_LOG_INFO( "resource sound [%s] can't load sound '%s'\n"
				, getName().c_str() 
				, m_path.c_str()
				);

			return false;			
		}

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void ResourceSound::_release()
	{
		SoundEngine::get()
			->releaseSoundBuffer( m_interface );
	}
	//////////////////////////////////////////////////////////////////////////
	SoundBufferInterface * ResourceSound::get()
	{
		return m_interface;
	}
	//////////////////////////////////////////////////////////////////////////
	float ResourceSound::getDefaultVolume() const
	{
		return m_defaultVolume;
	}
}
