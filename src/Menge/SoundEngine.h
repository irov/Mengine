#	pragma once

#	include "Holder.h"
#	include	"math/vec3.h"

#	include "Interface/SoundSystemInterface.h"

#	include	<set>
#	include	<string>

#	include "Holder.h"

namespace Menge
{
	class SoundEmitter;
	class ResourceSound;

	class SoundEngine
	{
	public:
		SoundEngine( SoundSystemInterface * _interface );
		~SoundEngine();
	
		void setListenerOrient( const mt::vec3f& _position, const mt::vec3f& _front, const mt::vec3f& top );

		SoundSourceInterface *	createSoundSource(
			bool _isHeadMode, 
			SoundBufferInterface * _sample,
			SoundNodeListenerInterface * _listener);

		SoundBufferInterface * createSoundBuffer();

		void setSoundSourceVolume( float _vol );
		float getSoundSourceVolume() const;

		void setCommonVolume( float _vol );
		float getCommonVolume() const;

		void releaseSoundBuffer( SoundBufferInterface * _soundBuffer );
		void releaseSoundSource( SoundSourceInterface * _node );

		void registerSoundEmitter( SoundEmitter * _emitter );
		void unregisterSoundEmitter( SoundEmitter * _emitter );
	private:

		float m_soundVolume;

		float m_commonVolume;

		SoundSystemInterface * m_interface;

		std::set<SoundEmitter*>	m_soundEmitters;
	};
};