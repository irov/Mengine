#	include "ResourceSound.h"

#	include "ResourceImplement.h"

#	include "XmlEngine.h"

#	include "SoundEngine.h"

#	include "LogEngine.h"

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	RESOURCE_IMPLEMENT( ResourceSound )
	//////////////////////////////////////////////////////////////////////////
	ResourceSound::ResourceSound( const ResourceFactoryParam & _params )
	: ResourceReference( _params )
	, m_isStreamable( false )
	, m_interface( 0 )
	{

	}
	//////////////////////////////////////////////////////////////////////////
	void ResourceSound::setFilePath( const std::string & _path )
	{
		m_filename = m_params.category + _path;
	}
	//////////////////////////////////////////////////////////////////////////
	void ResourceSound::loader( XmlElement * _xml )
	{
		ResourceReference::loader( _xml );

		XML_SWITCH_NODE(_xml)
		{
			XML_CASE_ATTRIBUTE_NODE_METHOD( "File", "Path", &ResourceSound::setFilePath );
			XML_CASE_ATTRIBUTE_NODE("IsStreamable","Value",m_isStreamable);
		}
	}
	//////////////////////////////////////////////////////////////////////////
	bool ResourceSound::_compile()
	{
		m_interface = Holder<SoundEngine>::hostage()
			->createSoundBufferFromFile( m_filename.c_str(), m_isStreamable );

		if( m_interface == 0 )
		{
			MENGE_LOG("resource sound [%s] can't load sound '%s'\n"
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
}