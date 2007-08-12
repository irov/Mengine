#	include "SoundTextField.h"

#	include "XmlParser/XmlParser.h"

#	include "ObjectImplement.h"

#	include "ResourceSound.h"

#	include "ResourceManager.h"

#	include "SoundEngine.h"

#	include "Interface/SoundSystemInterface.h"

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	OBJECT_IMPLEMENT(SoundTextField)
	//////////////////////////////////////////////////////////////////////////
	SoundTextField::SoundTextField()
	: m_sampleMusic(0)
	, m_soundSourceInterface(0)
	{
	}
	//////////////////////////////////////////////////////////////////////////
	void	SoundTextField::_update(float _timing)
	{
		if(TextField::isPlaying() == false) 
		{
			return;
		}

		if(m_soundSourceInterface->isPlaying() == false)
		{
			const Message * message = getNextMessage();
			_prepareSound(message);
		}
	}
	//////////////////////////////////////////////////////////////////////////
	const Message * SoundTextField::start()
	{
		const Message * message = TextField::start();

		_prepareSound(message);

		return message;
	}
	//////////////////////////////////////////////////////////////////////////
	void	SoundTextField::_prepareSound(const Message * _message)
	{
		Holder<SoundEngine>::hostage()->releaseSoundSource(m_soundSourceInterface);
		Holder<SoundEngine>::hostage()->releaseSoundBuffer(m_sampleMusic);

		m_sampleMusic = Holder<SoundEngine>::hostage()->createSoundBuffer();
		m_sampleMusic->loadFromFile(_message->soundName.c_str(),true);

		m_soundSourceInterface = Holder<SoundEngine>::hostage()->createSoundSource(false,m_sampleMusic,0);

		m_soundSourceInterface->play();
	}
	//////////////////////////////////////////////////////////////////////////
}