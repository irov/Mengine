#	pragma once

#	include "SystemDLL.h"
#	include "Holder.h"
#	include	"math/vec3.h"

#	include "SoundSystemInterface.h"

#	include	<map>
#	include	<string>

class	FileDataInterface;

namespace Menge
{
	class ResourceSound;

	class SoundEngine
		: public SystemDLL<SoundSystemInterface>
	{
	public:
		SoundEngine(const std::string& _dllModule);
		~SoundEngine();
	
		void setListenerOrient(const mt::vec3f& _position, const mt::vec3f& _front, const mt::vec3f& top);

		SoundSourceInterface*	createSoundSource(
			bool _isHeadMode, 
			SoundBufferInterface * _sample,
			SoundNodeListenerInterface*	_listener);

		SoundBufferInterface * createSoundBuffer();

		void releaseSoundBuffer(SoundBufferInterface* _soundBuffer);

		void			releaseSoundSource(SoundSourceInterface* _node);		
	};
};