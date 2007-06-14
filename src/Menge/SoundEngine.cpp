#	include "SoundEngine.h"

#	include "FileEngine.h"

#	include "SoundSystemInterface.h"
#	include "FileSystemInterface.h"

#	include	<assert.h>

namespace Menge
{
	SoundEngine::SoundEngine(const std::string& _dllModule)
			: SystemDLL<SoundSystemInterface>(_dllModule)
	{
		Holder<SoundEngine>::keep(this);
	}
	//////////////////////////////////////////////////////////////////////////
	SoundEngine::~SoundEngine()
	{
	}
	//////////////////////////////////////////////////////////////////////////
	void	SoundEngine::setListenerOrient(const mt::vec3f& _position, const mt::vec3f& _front, const mt::vec3f& top)
	{
		m_interface->setListenerOrient((float*)_position.m,(float*)_front.m,(float*)top.m);
	}
	//////////////////////////////////////////////////////////////////////////
	SoundSourceInterface*		SoundEngine::loadSoundSource(
		const SoundDataDesc& desc,
		SoundNodeListenerInterface*	_listener)
	{
		SoundSourceInterface* sound = m_interface->loadSoundNode(desc, _listener);
		return	sound;
	}
	//////////////////////////////////////////////////////////////////////////
	void		SoundEngine::releaseSoundSource(SoundSourceInterface* _soundSource)
	{
		m_interface->releaseSoundNode(_soundSource);
	}
}