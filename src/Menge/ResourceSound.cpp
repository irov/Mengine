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
	ResourceSound::ResourceSound( const std::string & _name )
	: ResourceReference( _name )
	, m_isStreamable( false )
	, m_interface( 0 )
	{}
	//////////////////////////////////////////////////////////////////////////
	void ResourceSound::loader( XmlElement * _xml )
	{
		ResourceReference::loader( _xml );

		XML_SWITCH_NODE(_xml)
		{
			XML_CASE_ATTRIBUTE_NODE("File","Path",m_filename);
			XML_CASE_ATTRIBUTE_NODE("IsStreamable","Value",m_isStreamable);
		}
	}
	//////////////////////////////////////////////////////////////////////////
	bool ResourceSound::_compile()
	{
		m_interface = Holder<SoundEngine>::hostage()->createSoundBufferFromFile( m_filename.c_str(), m_isStreamable );

		if( m_interface == 0 )
		{
			MENGE_LOG("resource sound [%s] can't load sound '%s'\n"
				, m_name.c_str() 
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