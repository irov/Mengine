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
	{}
	//////////////////////////////////////////////////////////////////////////
	void ResourceSound::play()
	{
		return m_interface->play();
	}
	//////////////////////////////////////////////////////////////////////////
	void ResourceSound::pause()
	{
		return m_interface->pause();
	}
	//////////////////////////////////////////////////////////////////////////
	void ResourceSound::stop()
	{
		return m_interface->stop();
	}
	//////////////////////////////////////////////////////////////////////////
	bool ResourceSound::isPlaying() const
	{
		return m_interface->isPlaying();
	}
	//////////////////////////////////////////////////////////////////////////
	void ResourceSound::setVolume(float vol)
	{
		return m_interface->setVolume(vol);
	}
	//////////////////////////////////////////////////////////////////////////
	float ResourceSound::getVolume() const
	{
		return m_interface->getVolume();
	}
	//////////////////////////////////////////////////////////////////////////
	void ResourceSound::setPosition(float x, float y, float z)
	{
		return m_interface->setPosition(x,y,z);
	}
	//////////////////////////////////////////////////////////////////////////
	const float* ResourceSound::getPosition()
	{
		return m_interface->getPosition();
	}
	//////////////////////////////////////////////////////////////////////////
	void ResourceSound::setLoop(bool loop)
	{
		return m_interface->setLoop(loop);
	}
	//////////////////////////////////////////////////////////////////////////
	bool ResourceSound::isLooping() const
	{
		return m_interface->isLooping();
	}
	//////////////////////////////////////////////////////////////////////////
	int ResourceSound::getLengthMS()
	{
		return m_interface->getLengthMS();
	}
	//////////////////////////////////////////////////////////////////////////
	void ResourceSound::loader( TiXmlElement * _xml )
	{
		XML_FOR_EACH_TREE(_xml)
		{
			XML_CHECK_VALUE_NODE("File","Path",m_filename);
		}

		ResourceImpl::loader( _xml );
	}
	//////////////////////////////////////////////////////////////////////////
	bool ResourceSound::_compile()
	{
		/*
			��� ��������� ��� �������:
			-	listener ��� ��������?
			-	filename ���� �� ������ ���� �� ������, �� ��� ����� �������� �����???
			-	� head mode ��� ��������?
		*/
		SoundDataDesc::SOUND_TYPE	sound_type = m_filename.find(".ogg") != std::string::npos ? SoundDataDesc::OGG : SoundDataDesc::WAV;
		SoundDataDesc desc = {sound_type,m_filename,false,false};
		m_interface = Holder<SoundEngine>::hostage()->loadSoundSource(desc,NULL);
		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void ResourceSound::_release()
	{
		Holder<SoundEngine>::hostage()->releaseSoundSource(m_interface);
	}
}