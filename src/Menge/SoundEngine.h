#	pragma once

#	include "Holder.h"
#	include	"math/vec3.h"

#	include "Interface/SoundSystemInterface.h"

#	include	<map>
#	include	<string>

#	include "Holder.h"

namespace Menge
{
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

		void setVolume( float _vol );
		float getVolume() const;

		void releaseSoundBuffer( SoundBufferInterface * _soundBuffer );
		void releaseSoundSource( SoundSourceInterface * _node );

	private:

		float m_volume;

		SoundSystemInterface * m_interface;
	};
};