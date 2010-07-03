#	include "ResourceSound.h"

#	include "ResourceImplement.h"

#	include "Logger/Logger.h"
#	include "SoundEngine.h"
#	include "XmlEngine.h"

#	include "Utils/Core/File.h"


namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	RESOURCE_IMPLEMENT( ResourceSound )
	//////////////////////////////////////////////////////////////////////////
	ResourceSound::ResourceSound()
	: m_isStreamable( false )
	, m_interface( 0 )
	{

	}
	//////////////////////////////////////////////////////////////////////////
	void ResourceSound::setPath( const String& _path )
	{
		m_path = _path;
	}
	//////////////////////////////////////////////////////////////////////////
	void ResourceSound::setCodec( const ConstString& _codec )
	{
		m_codec = _codec;
	}
	//////////////////////////////////////////////////////////////////////////
	void ResourceSound::loader( XmlElement * _xml )
	{
		ResourceReference::loader( _xml );

		XML_SWITCH_NODE(_xml)
		{
			XML_CASE_ATTRIBUTE_NODE_METHOD( "File", "Path", &ResourceSound::setPath );
			XML_CASE_ATTRIBUTE_NODE_METHOD( "File", "Codec", &ResourceSound::setCodec );
			XML_CASE_ATTRIBUTE_NODE( "IsStreamable", "Value",m_isStreamable);
		}
	}
	//////////////////////////////////////////////////////////////////////////
	bool ResourceSound::isStreamable() const
	{
		return m_isStreamable;
	}
	//////////////////////////////////////////////////////////////////////////
	const String& ResourceSound::getFilename() const
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

			m_codec = codecType;	
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
}
