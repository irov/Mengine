#	pragma once

#	include "Node.h"

#	include "Animatable.h"

#	include "Soundable.h"

#	include "Interface/SoundSystemInterface.h"

namespace Menge
{
	class ResourceSound;

	class SoundEmitter
		: public Node
		, public Animatable
		, public SoundNodeListenerInterface
		, public Soundable
	{
	public:
		SoundEmitter();
		~SoundEmitter();

	protected:
		bool _play() override;
		bool _restart( size_t _enumerator ) override;
		void _stop( size_t _enumerator ) override;
		void _end( size_t _enumerator ) override;
		bool _interrupt( size_t _enumerator ) override;
		void _setTiming( float _timing ) override;

		void _setVolume( float _volume ) override;
		float _getVolume() const override;
	
	public:
		void updateVolume();

		float getLengthMs() const;

		void setSoundResource( const ConstString& _name );

    protected:
		void listenSoundNodePaused();
		void listenSoundNodeStopped();
		
	protected:
		bool _activate() override;
		void _deactivate() override;

		bool _compile() override;
		void _release() override;
		void _setEventListener( PyObject * _listener ) override;

		void _setLoop( bool _value ) override;

	private:
		ResourceSound * m_resource;
		ConstString m_resourcename;

		unsigned int m_sourceID;

		bool m_isHeadMode;

		bool m_onSoundPauseEvent;
		bool m_onSoundStopEvent;
	};
};
