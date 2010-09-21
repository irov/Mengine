#	include "ResourceSound.h"

#	include "ResourceImplement.h"

#	include "Logger/Logger.h"
#	include "SoundEngine.h"
#	include "XmlEngine.h"


namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	RESOURCE_IMPLEMENT( ResourceSound )
	//////////////////////////////////////////////////////////////////////////
	ResourceSound::ResourceSound()
	: m_isStreamable( false )
	, m_interface( 0 )
	, m_defaultVolume( 1.0f )
	{

	}
	//////////////////////////////////////////////////////////////////////////
	void ResourceSound::setFilePath_( const String& _path )
	{
		m_filename = _path;
	}
	//////////////////////////////////////////////////////////////////////////
	void ResourceSound::loader( XmlElement * _xml )
	{
		ResourceReference::loader( _xml );

		XML_SWITCH_NODE(_xml)
		{
			XML_CASE_ATTRIBUTE_NODE_METHOD( "File", "Path", &ResourceSound::setFilePath_ );
			XML_CASE_ATTRIBUTE_NODE( "IsStreamable", "Value",m_isStreamable);
			XML_CASE_ATTRIBUTE_NODE( "DefaultVolume", "Value", m_defaultVolume);
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
		return m_filename;
	}
	//////////////////////////////////////////////////////////////////////////
	bool ResourceSound::_compile()
	{
		const String & category = this->getCategory();

		m_interface = Holder<SoundEngine>::hostage()
			->createSoundBufferFromFile( category, m_filename, m_isStreamable );

		if( m_interface == 0 )
		{
			MENGE_LOG_INFO( "resource sound [%s] can't load sound '%s'\n"
				, getName().c_str() 
				, m_filename.c_str()
				);

			return false;			
		}

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void ResourceSound::_release()
	{
		Holder<SoundEngine>::hostage()->releaseSoundBuffer( m_interface );
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
	//////////////////////////////////////////////////////////////////////////
}
