#	pragma once

#	include "Holder.h"
#	include	"math/vec3.h"

#	include "Interface/SoundSystemInterface.h"

#	include	<set>
#	include	<string>

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

		SoundBufferInterface *	createSoundBufferFromFile( const char* _filename, bool _isStream ); 
		SoundBufferInterface *	createSoundBufferFromMemory( void* _buffer, int _size, bool _newmem );

		void setSoundSourceVolume( float _volume );
		float getSoundSourceVolume() const;

		void setCommonVolume( float _volume );
		float getCommonVolume() const;

		void releaseSoundBuffer( SoundBufferInterface * _soundBuffer );
		void releaseSoundSource( SoundSourceInterface * _node );

		bool setBlow( bool _active );
		float getBlow();

		void update();

	public:
		void registerSoundEmitter( SoundEmitter * _emitter );
		void unregisterSoundEmitter( SoundEmitter * _emitter );
	protected:
		SoundSystemInterface * m_interface;
	private:

		float m_soundVolume;

		float m_commonVolume;

		typedef std::set<SoundEmitter*> TSetSoundEmitters;
		TSetSoundEmitters	m_soundEmitters;
	};
};