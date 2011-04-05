#	pragma once

#	include "Node.h"

#	include "Interface/SoundSystemInterface.h"

namespace Menge
{
	class	ResourceSound;

	class	SoundEmitter
		: public Node
		, public SoundNodeListenerInterface
	{
	public:
		SoundEmitter();
		~SoundEmitter();

	public:
		void play();
		void pause();
		void stop();

		bool isPlaying();

		void setVolume( float _volume );
		float getVolume();
		void updateVolume();

		void setLoop( bool _loop );
		bool getLoop();

		float getLengthMs();

		void setSoundResource( const ConstString& _name );

		void listenPaused();
		void listenStopped();

	public:
		void loader( BinParser * _parser ) override;

	protected:
		bool _activate() override;
		void _deactivate() override;

		bool _compile() override;
		void _release() override;
		void _setEventListener( PyObject * _listener ) override;

	private:
		ResourceSound * m_resource;
		ConstString m_resourcename;

		unsigned int m_sourceID;

		bool m_isHeadMode;
		bool m_loop;
		bool m_playing;

		bool m_onSoundPauseEvent;
		bool m_onSoundStopEvent;
	};
};
