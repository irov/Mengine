#	pragma once

#	include "Kernel/Node.h"

#	include "Kernel/Animatable.h"
#	include "Kernel/Soundable.h"

#   include "ResourceSound.h"

#	include "Interface/SoundSystemInterface.h"

namespace Menge
{
	class SoundEmitter
		: public Node
		, public Animatable
		, public SoundListenerInterface
		, public Soundable
	{
	public:
		SoundEmitter();
		~SoundEmitter();

	protected:
		bool _play( float _time ) override;
		bool _restart( float _time, uint32_t _enumerator ) override;
		void _pause( uint32_t _enumerator ) override;
		void _resume( uint32_t _enumerator ) override;
		void _stop( uint32_t _id ) override;
		void _end( uint32_t _id ) override;
		bool _interrupt( uint32_t _id ) override;
		void _setTiming( float _timing ) override;

    protected:
		void _setVolume( float _volume ) override;
		float _getVolume() const override;
	
	public:
		void updateVolume();

		float getLengthMs() const;

    public:
		void setResourceSound( ResourceSound * _resourceSound );
        ResourceSound * getResourceSound() const;

    protected:
		void onSoundPause( uint32_t _soundId ) override;
		void onSoundStop( uint32_t _soundId ) override;
		
	protected:
		bool _activate() override;
		void _deactivate() override;

		bool _compile() override;
		void _release() override;
		void _setEventListener( const pybind::dict & _listener ) override;

		void _setLoop( bool _value ) override;

	private:
		ResourceHolder<ResourceSound> m_resourceSound;

        SoundBufferInterfacePtr m_soundBuffer;
        
		uint32_t m_sourceID;

		bool m_isHeadMode;

		bool m_onSoundPauseEvent;
		bool m_onSoundStopEvent;

        float m_volume;
	};
};
