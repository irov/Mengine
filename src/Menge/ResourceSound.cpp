#	include "ResourceSound.h"
#	include "ResourceImplement.h"

#	include "SoundSystemInterface.h"

#	include "XmlParser.h"
#	include "SoundEngine.h"
#	include "FileEngine.h"

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	RESOURCE_IMPLEMENT( ResourceSound )
	//////////////////////////////////////////////////////////////////////////
	ResourceSound::ResourceSound( const std::string & _name )
	: ResourceImpl(_name)
	, m_isStreamable(false)
	{}
	//////////////////////////////////////////////////////////////////////////
	void ResourceSound::loader( TiXmlElement * _xml )
	{
		XML_FOR_EACH_TREE(_xml)
		{
			XML_CHECK_VALUE_NODE("File","Path",m_filename);
			XML_CHECK_VALUE_NODE("IsStreamable","Value",m_isStreamable);
		}

		ResourceImpl::loader( _xml );
	}
	//////////////////////////////////////////////////////////////////////////
	bool ResourceSound::_compile()
	{
		m_interface = Holder<SoundEngine>::hostage()->createSoundBuffer();
		m_interface->loadFromFile(m_filename.c_str(),m_isStreamable);
		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void ResourceSound::_release()
	{
		Holder<SoundEngine>::hostage()->releaseSoundBuffer(m_interface);
	}
	//////////////////////////////////////////////////////////////////////////
	SoundBufferInterface * ResourceSound::get()
	{
		return m_interface;
	}
}